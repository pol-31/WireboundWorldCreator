#include "Game.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <glad/glad.h>
#include <stb_image_write.h>

#include <glm/gtc/type_ptr.hpp>

#include "../common/models/DirectedLight.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../ui/DebugUI.h"
#include "ContactListenerImpl.h"
#include "Layers.h"
#include "UiScene.h"

JPH_SUPPRESS_WARNINGS_STD_BEGIN
#include <fstream>
JPH_SUPPRESS_WARNINGS_STD_END

JPH_GCC_SUPPRESS_WARNING("-Wswitch")

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------
static constexpr GLuint cNumBodies = 10240;
static constexpr GLuint cNumBodyMutexes = 0;  // Autodetect
static constexpr GLuint cMaxBodyPairs = 65536;
static constexpr GLuint cMaxContactConstraints = 20480;
static const JPH::Vec3 cCharacterVelocity(0, 0, 2);

static const float cCollisionTolerance = 0.05f;

namespace JPH {
class JobSystem;
class TempAllocator;
};  // namespace JPH

inline JPH::Mat44 CreateOrthoMatrix(float left, float right, float bottom,
                                    float top, float near_plane,
                                    float far_plane) {
  JPH::Mat44 mat = JPH::Mat44::sZero();
  mat.SetColumn4(0, JPH::Vec4(2.0f / (right - left), 0.0f, 0.0f, 0.0f));
  mat.SetColumn4(1, JPH::Vec4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f));
  mat.SetColumn4(2,
                 JPH::Vec4(0.0f, 0.0f, -2.0f / (far_plane - near_plane), 0.0f));
  mat.SetColumn4(
      3, JPH::Vec4(-(right + left) / (right - left),
                   -(top + bottom) / (top - bottom),
                   -(far_plane + near_plane) / (far_plane - near_plane), 1.0f));
  return mat;
}

void UpdateDirLightFrustum(DirectedLight &light, const Camera *camera) {
  auto camera_pos = ToJph(camera->GetPosition());

  // Focus the shadow map center slightly ahead of the player
  auto camera_forward = ToJph(camera->GetDirectionFront());
  JPH::Vec3 focus_pt = camera_pos + camera_forward * 5.0f;

  // Pull shadow camera BACK along -sun_dir so higher structures (roofs) are
  // captured
  float shadow_distance = 30.0f;
  JPH::Vec3 sun_position = focus_pt - light.dir * shadow_distance;

  JPH::Vec3 world_up(0.0f, 1.0f, 0.0f);
  if (std::abs(light.dir.Dot(world_up)) > 0.99f) {
    world_up = JPH::Vec3(1.0f, 0.0f, 0.0f);
  }

  float ortho_size = 15.0f;
  float near_plane = 1.0f;
  float far_plane = shadow_distance + 20.0f;

  JPH::Mat44 lightProjection = CreateOrthoMatrix(
      -ortho_size, ortho_size, -ortho_size, ortho_size, near_plane, far_plane);
  JPH::Mat44 lightView = JPH::Mat44::sLookAt(sun_position, focus_pt, world_up);
  light.lightSpaceMatrix = lightProjection * lightView;
}

struct PrebakeCameraDir {
  float yaw;
  float pitch;
};

PrebakeCameraDir gPrebakeCameraDirs[] = {
    {0.0f, 0.0f},     {180.0f, 0.0f}, {-90.0f, 90.0f},
    {-90.0f, -90.0f}, {90.0f, 0.0f},  {-90.0f, 0.0f},
};

void Game::PrebakeEnvMaps() {
  JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();
  gWindowWidth = 512;
  gWindowHeight = 512;
  glfwSetWindowSize(gWindow, gWindowWidth, gWindowHeight);

  stbi_flip_vertically_on_write(false);

  // --- 1. PROCESS EACH ROOM / ZONE ---
  for (const SceneZone *zone :
       mdl_loader_.GetScene()->scene_data_.zones) {
    auto ibl_name = renderer_.GetIblBakeFilename(zone->scene_node->name);
    if (ibl_name.empty()) {
      continue;
    }
    cubemap_.LoadDefaultCubemap();
    auto camera_pos = zone->scene_node->global_bounds.GetCenter();

    // Step A: Render the 6 environment faces to raw .hdr files
    for (int i = 0; i < std::size(gPrebakeCameraDirs); ++i) {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      auto camera_dirs = gPrebakeCameraDirs[i];
      camera_.UpdateBakeEnv(camera_pos, camera_dirs.yaw, camera_dirs.pitch);

      for (auto &l : dir_lights_) {
        UpdateDirLightFrustum(l, &camera_);
      }
      // terrain_renderer_.Update(&camera_);
      world_manager_.UpdatePlayerZone(camera_pos, &bi);
      world_manager_.Cull(true, false, false, false,
                          render_settings_.objects_num);

      glDisable(GL_BLEND);
      renderer_.UpdateBuffer();
      // renderer_.DrawShadowPass();
      renderer_.DrawGeometryPass(terrain_renderer_.GetRenderData(),
                                 render_settings_.triplanar_level, true);
      renderer_.DrawSsaoPass();
      renderer_.DrawLightPass(cubemap_.GetRenderData(), true, false, false,
                              false, false, 0);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer_.GetLightingFbo());
      glReadBuffer(GL_COLOR_ATTACHMENT0);

      std::vector<float> pixels(gWindowWidth * gWindowHeight * 3);
      glReadPixels(0, 0, gWindowWidth, gWindowHeight, GL_RGB, GL_FLOAT,
                   pixels.data());

      for (int y = 0; y < gWindowHeight; ++y) {
        float *row = pixels.data() + y * gWindowWidth * 3;
        for (int x = 0; x < gWindowWidth / 2; ++x) {
          float *a = row + x * 3;
          float *b = row + (gWindowWidth - 1 - x) * 3;
          std::swap(a[0], b[0]);
          std::swap(a[1], b[1]);
          std::swap(a[2], b[2]);
        }
      }

      // not necessary, but good for dbg
      std::string filename =
          "../assets/ibl/" + ibl_name + "_face_" + std::to_string(i) + ".hdr";
      stbi_write_hdr(filename.c_str(), gWindowWidth, gWindowHeight, 3,
                     pixels.data());
    }
    //
    // // Step B: Load raw faces, bake Irradiance & Prefiltered maps, and save
    // to binary .ibl
    cubemap_.LoadZone(zone);  // we need this only to load to OpenGL to bake ibl
    renderer_.IblBakeRadiance(cubemap_.GetRenderData());
    renderer_.IblBakePrefiltered(cubemap_.GetRenderData());

    std::string filepath = "../assets/ibl/" + ibl_name + ".ibl";
    renderer_.SaveZoneIbl(filepath);
  }
  cubemap_.LoadDefaultCubemap();
  renderer_.IblBakeLut();
  renderer_.SaveIblLut("../assets/ibl/brdf_lut.hdr");

  // Restore viewport and window resolution
  camera_.SetDefaultFovy();
  auto default_res = GetWindowResolution(GetDefaultWindowResolutionId());
  gWindowWidth = default_res.width;
  gWindowHeight = default_res.height;
  glfwSetWindowSize(gWindow, gWindowWidth, gWindowHeight);

  const SceneZone *cur_zone = world_manager_.GetCurrentZone();
  cubemap_.LoadZone(cur_zone);
  // renderer_.LoadZoneIbl(cur_zone->name);
  cubemap_.LoadDefaultCubemap();
  renderer_.ReLoadIblZones();
}

Game::Game()
    : terrain_renderer_(&(mdl_loader_.GetScene()->materials)),
      world_manager_(mdl_loader_.GetScene(), &camera_, player_, characters_,
                     weapons_, dir_lights_),
      renderer_(mdl_loader_.GetScene(), &camera_, &player_,
                world_manager_.GetCulledData()) {
  Init();
}

Game::~Game() { DeInit(); }

void Game::RenderInterface() {
  float target_size = 16.0f;
  float half_target_size = target_size / 2.0f;
  if (player_->GetBody()->IsAiming()) {
    ui_renderer_.AddSprite(
        "GoldenCircle",
        glm::vec2(gWindowWidth, gWindowHeight) - half_target_size,
        glm::vec2(target_size), glm::vec4(1.0f));
  }
  const glm::vec2 start_hp_pos = glm::vec2(10.0f, 45.0f);
  glm::vec2 hp_pos = start_hp_pos;
  glm::vec2 hp_size = glm::vec2(16.0f);
  for (int i = 0; i < 10; ++i) {
    hp_pos.x += hp_size.x;
    ui_renderer_.AddSprite("FlowerWhite", hp_pos, hp_size, glm::vec4(1.0f));
  }
  hp_pos = start_hp_pos;
  hp_pos.y -= hp_size.y;
  for (int i = 0; i < 10; ++i) {
    hp_pos.x += hp_size.x;
    ui_renderer_.AddSprite("StaminaPoint", hp_pos, hp_size, glm::vec4(1.0f));
  }
  hp_pos = glm::vec2(gWindowWidth * 0.8f, 25.0f);
  for (int i = 0; i < 7; ++i) {
    hp_pos.x += hp_size.x;
    ui_renderer_.AddSprite("HealthPoint", hp_pos, hp_size * 2.0f,
                           glm::vec4(1.0f));
  }
  ui_renderer_.AddSprite("Gear", glm::vec2(gWindowWidth * 0.9f, 5.0f),
                         hp_size * 4.0f, glm::vec4(1.0f));

  // ---

  int characters_left = 0;
  for (const auto &c : characters_) {
    if (!c->GetBody()->IsDead()) {
      ++characters_left;
    }
  }
  if (characters_left == 0) {
    for (auto &c : characters_) {
      c->GetBody()->Revive();
    }
    characters_left = characters_.size();
  }
  std::string charactersLeftText =
      "enemies left: " + std::to_string(characters_left);
  ui_renderer_.AddText(charactersLeftText, glm::vec2(100.0f, 5.0f),
                       glm::vec2(255.0f), glm::vec4(255.0f));
}

void Game::RenderFps() {
  std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps_));
  ui_renderer_.AddText(fpsText, glm::vec2(10.0f, 5.0f), glm::vec2(1.0f),
                       glm::vec4(255.0f));
}

void Game::Run() {
  while (true) {
    if (state_ == State::Game) {
      // glEnable(GL_STENCIL_TEST);
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      camera_.HideCursor();
      glfwSetScrollCallback(gWindow, GameScrollCallback);
      glfwSetMouseButtonCallback(gWindow, GameMouseButtonCallback);
      glfwSetKeyCallback(gWindow, GameKeyCallback);
      glfwSetCursorPosCallback(gWindow, GameCursorPosCallback);
      RunGameLoop();
    } else if (state_ == State::Menu) {
      // glDisable(GL_STENCIL_TEST);
      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      camera_.ShowCursor();
      glfwSetScrollCallback(gWindow, MenuScrollCallback);
      glfwSetMouseButtonCallback(gWindow, MenuMouseButtonCallback);
      glfwSetKeyCallback(gWindow, MenuKeyCallback);
      glfwSetCursorPosCallback(gWindow, MenuCursorPosCallback);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      RunMenuLoop();
    } else if (state_ == State::Exit) {
      break;
    }
  }
}

void Game::UpdateHoveredObject() {
  JPH::RVec3 hit_pos;
  JPH::BodyID hit_id;
  float hit_fraction;

  // Reset hovered object every frame
  hovered_object_ = nullptr;

  if (player_->IsDragging()) {
    return;
  }

  auto camera_pos = ToJph(camera_.GetPosition());
  auto camera_forward = ToJph(camera_.GetDirectionFront());

  // Cast a short ray from the camera
  float interact_dist = 2.5f;
  if (character_shared_data_->CastProbe(
          camera_pos, camera_forward, interact_dist, hit_fraction, hit_pos,
          hit_id, player_->GetBody()->GetJphCharacter()->GetInnerBodyID())) {
    uint64_t user_data = mPhysicsSystem->GetBodyInterface().GetUserData(hit_id);
    if (user_data != 0) {
      GameObject *obj = reinterpret_cast<GameObject *>(user_data);
      if (!obj->GetInteractPrompt(player_->GetBody()).prompt.empty()) {
        hovered_object_ = obj;
      }
    }
  }
}

void Game::RenderHoveredObjectPrompt() {
  if (hovered_object_) {
    auto projection_matrix = camera_.GetProjMatrix();
    auto view_matrix = camera_.GetViewMatrix();
    auto prompt_data = hovered_object_->GetInteractPrompt(player_->GetBody());
    JPH::Vec3 worldPos_jph = prompt_data.position;
    glm::vec3 worldPos = glm::vec3(worldPos_jph.GetX(), worldPos_jph.GetY(),
                                   worldPos_jph.GetZ());
    glm::vec4 clipPos =
        projection_matrix * view_matrix * glm::vec4(worldPos, 1.0f);
    glm::vec2 ndc(clipPos.x, clipPos.y);
    if (clipPos.w > 0.0f) {
      ndc /= clipPos.w;
    } else {
      ndc = -ndc;
      if (glm::length(ndc) > 0.0001f) {
        ndc = glm::normalize(ndc);
        ndc /= std::max(std::abs(ndc.x), std::abs(ndc.y));
      }
    }
    float screenX = (ndc.x + 1.0f) * 0.5f * static_cast<float>(gWindowWidth);
    float screenY = (ndc.y + 1.0f) * 0.5f * static_cast<float>(gWindowHeight);
    float uiY = static_cast<float>(gWindowHeight) - screenY;
    std::string hoveredObjectText = prompt_data.prompt;
    glm::vec2 textSize = ui_renderer_.MeasureText(hoveredObjectText);
    float finalX = screenX - textSize.x / 2.0f;
    float finalY = uiY - textSize.y;
    const float padding = 10.0f;
    finalX =
        std::clamp(finalX, padding,
                   static_cast<float>(gWindowWidth) - textSize.x - padding);
    finalY =
        std::clamp(finalY, padding,
                   static_cast<float>(gWindowHeight) - textSize.y - padding);
    ui_renderer_.AddText(hoveredObjectText, glm::vec2(finalX, finalY),
                         glm::vec2(1.0f),
                         glm::vec4(120.0f, 155.0f, 0.0f, 255.0f));
  }
}

void Game::UpdatePlayerZone() {
  JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();
  auto player_pos = player_->GetBody()->GetPosition();
  // std::cout << player_pos.GetX() << ' ' << player_pos.GetZ() << std::endl;
  bool new_zone_entered = world_manager_.UpdatePlayerZone(player_pos, &bi);
  // cubemap_.LoadZone(cur_zone);
  std::unordered_map<IblZoneType, float> ibl_types;
  auto &zones = mdl_loader_.GetScene()->scene_data_.zones;
  const auto *cur_zone = world_manager_.GetCurrentZone();
  int cur_zone_id = world_manager_.GetCurrentZoneId();

  ScenePortal *closest_portal = nullptr;
  float dist_to_closest_portal = -std::numeric_limits<float>::lowest();
  for (const auto &portal : cur_zone->portals) {
    float dist =
        (portal->scene_node->global_bounds.GetCenter() - player_pos).Length();
    if (dist < dist_to_closest_portal) {
      dist_to_closest_portal = dist;
      closest_portal = portal;
    }
  }
  if (closest_portal->zone_1 == cur_zone_id) {
    renderer_.LoadIblZones(zones[closest_portal->zone_1]->properties,
                           zones[closest_portal->zone_2]->properties,
                           dist_to_closest_portal);
  } else {
    renderer_.LoadIblZones(zones[closest_portal->zone_2]->properties,
                           zones[closest_portal->zone_1]->properties,
                           dist_to_closest_portal);
  }
}

void Game::RunGameLoop() {
  while (true) {
    if (glfwWindowShouldClose(gWindow)) {
      state_ = State::Exit;
      break;
    }
    if (state_ != State::Game) {
      break;
    }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glStencilMask(0xFF);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
    // GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    UpdateDeltaTime();

    if (render_settings_.update_physics) {
      // Reinitialize the job system if the concurrency setting changed
      if (mMaxConcurrentJobs != mJobSystem->GetMaxConcurrency())
        static_cast<JPH::JobSystemThreadPool *>(mJobSystem)
            ->SetNumThreads(mMaxConcurrentJobs - 1);

      for (auto &l : dir_lights_) {
        UpdateDirLightFrustum(l, &camera_);
      }

      /// pre physics update
      for (auto &c : characters_) {
        c->UpdateLogic(mPhysicsSystem, gDeltaTimePhysics);
        c->GetBody()->PrePhysicsUpdate(mPhysicsSystem, mTempAllocator,
                                       gDeltaTimePhysics);
      }

      player_->UpdateObjectDragging();
      player_->UpdateCoverState();
      player_->HandleCoverMovement(glfwGetKey(gWindow, GLFW_KEY_A) ==
                                   GLFW_PRESS);
      player_->UpdateView();
      player_->GetBody()->PrePhysicsUpdate(mPhysicsSystem, mTempAllocator,
                                           gDeltaTimePhysics);

      /// physics update
      mPhysicsSystem->Update(gDeltaTimePhysics, 1, mTempAllocator, mJobSystem);
      const JPH::BodyLockInterface &bli =
          mPhysicsSystem->GetBodyLockInterface();
      mdl_loader_.GetScene()->UpdateRenderTransform(bli);

      /// post physics update
      for (auto &c : characters_) {
        c->GetBody()->PostPhysicsUpdate(mPhysicsSystem->GetGravity(),
                                        gDeltaTimePhysics);
      }
      player_->GetBody()->PostPhysicsUpdate(mPhysicsSystem->GetGravity(),
                                            gDeltaTimePhysics);
    }

    for (auto& d : doors_) {
      d->Update(mBodyInterface, gDeltaTimePhysics);
    }
    for (auto& w : weapons_) {
      w->Update(gDeltaTimePhysics);
    }

    auto player_pos = player_->GetBody()->GetPosition();
    auto player_dir = player_->GetBody()->GetLookDirection();
    ma_engine_listener_set_position(&audio_engine_, 0, player_pos.GetX(),
                                    player_pos.GetY(), player_pos.GetZ());
    ma_engine_listener_set_direction(&audio_engine_, 0, player_dir.GetX(),
                                     player_dir.GetY(), player_dir.GetZ());

    animator_.Update(!render_settings_.animate_animated);

    camera_.Update(player_->GetBody()->GetCameraBoneMatrix());
    // terrain_renderer_.Update(&camera_);

    UpdatePlayerZone();

    if (render_settings_.cull) {
      world_manager_.Cull(false, false, render_settings_.render_animated,
                          render_physics_only_, render_settings_.objects_num);
    }

    glDisable(GL_BLEND);

    if (render_settings_.update_buffer) {
      renderer_.UpdateBuffer();
    }

    if (render_physics_only_) {
      renderer_.RenderDebug();
    } else {
      if (render_settings_.render_shadows) {
        renderer_.DrawShadowPass();
      }
      renderer_.DrawGeometryPass(terrain_renderer_.GetRenderData(),
                                 render_settings_.triplanar_level,
                                 render_settings_.geometry_pass);
      if (render_settings_.ssao) {
        renderer_.DrawSsaoPass();
      }
      renderer_.DrawLightPass(
          cubemap_.GetRenderData(), render_settings_.light_pass,
          render_settings_.apply_shadows, render_settings_.light_sources,
          render_settings_.bloom, render_settings_.ibl,
          render_settings_.cubemap_type);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderInterface();

    ui_renderer_.Render();

    if (!render_physics_only_) {
      renderer_.RenderToTheScreen();
    }

    ui_menu_scene_->MouseCancel();
    RenderFps();

    UpdateHoveredObject();

    RenderHoveredObjectPrompt();

    ui_renderer_.Render();

    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void Game::RunMenuLoop() {
  while (true) {
    if (glfwWindowShouldClose(gWindow)) {
      state_ = State::Exit;
      break;
    }
    if (state_ != State::Menu) {
      break;
    }
    glClear(GL_COLOR_BUFFER_BIT);

    UpdateDeltaTime();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer_.RenderToTheScreen();

    RenderFps();

    ui_menu_scene_->Update(gDeltaTime);
    ui_menu_scene_->Draw();

    ui_renderer_.Render();

    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void Game::UpdateDeltaTime() {
  auto current_frame = static_cast<float>(glfwGetTime());
  gDeltaTime = current_frame - last_frame;
  last_frame = current_frame;
  gDeltaTimePhysics = std::clamp(gDeltaTime, 0.001f, 1.0f / 30.0f);

  frameCount_++;
  elapsedTime_ += gDeltaTime;

  if (elapsedTime_ >= 1.0f) {
    fps_ = frameCount_ / elapsedTime_;
    frameCount_ = 0;
    elapsedTime_ -= 1.0f;
  }
}

JPH::Ref<JPH::Shape> CreateMeshShape(
    const SceneMesh &mesh, AssetProperties::CollisionType collision_type) {
  JPH::Ref<JPH::Shape> local_shape;
  if (collision_type == AssetProperties::CollisionType::Sphere) {
    float radius = (mesh.max[0] - mesh.min[0]) * 0.5f;
    JPH::SphereShapeSettings sphere_settings(radius);
    // Safely create via Jolt reference counting
    local_shape = sphere_settings.Create().Get();
  } else {
    auto half_extend_glm = (mesh.max - mesh.min) / 2.0f;
    JPH::Vec3 half_extend(half_extend_glm.x, half_extend_glm.y,
                          half_extend_glm.z);
    JPH::BoxShapeSettings box_settings(half_extend);
    local_shape = box_settings.Create().Get();
  }
  auto center_glm = (mesh.max + mesh.min) / 2.0f;
  JPH::Vec3 local_center(center_glm.x, center_glm.y, center_glm.z);
  if (!local_center.IsNearZero()) {
    JPH::RotatedTranslatedShapeSettings offset_settings(
        local_center, JPH::Quat::sIdentity(), local_shape);
    local_shape = offset_settings.Create().Get();
  }
  return local_shape;
}

void InitSceneGlobalTransforms(Scene* scene) {
  std::function<void(SceneNode *, const JPH::Mat44 &)> dfs =
      [&](SceneNode *node, const JPH::Mat44 &parent) {
        if (!node) {
          return;
        }
        auto local = node->local_transform.Matrix();
        auto global_transform = parent * local;
        node->global_transform = global_transform;
        for (auto child : node->children) {
          dfs(child, global_transform);
        }
  };
  for (auto node : scene->scene_data_.characters) {
    dfs(node, JPH::Mat44::sIdentity());
  }
  for (auto node : scene->scene_data_.object_nodes) {
    dfs(node, JPH::Mat44::sIdentity());
  }
}

/* int GetShapeDataId(
  const Game::ShapeToGeometryMap& shapeToGeometry,
  const JPH::Shape* shape, JPH::Vec3& out_scale) {
  while (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass ||
           shape->GetSubType() == JPH::EShapeSubType::RotatedTranslated) {
    if (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass) {
      shape = static_cast<const
JPH::OffsetCenterOfMassShape*>(shape)->GetInnerShape(); } else { shape =
static_cast<const JPH::RotatedTranslatedShape*>(shape)->GetInnerShape();
    }
           }
  out_scale = JPH::Vec3::sReplicate(1.0f);
  if (shape->GetSubType() == JPH::EShapeSubType::Box) {
    out_scale = static_cast<const JPH::BoxShape*>(shape)->GetHalfExtent();
  } else if (shape->GetSubType() == JPH::EShapeSubType::Sphere) {
    out_scale = JPH::Vec3::sReplicate(static_cast<const
JPH::SphereShape*>(shape)->GetRadius());
  }
  return shapeToGeometry.at(shape->GetSubType());
} */

// TODO: not used yet
JPH::Color DefineColor(JPH::EMotionType body_type, JPH::BodyID body_id) {
  JPH::Color color;
  switch (body_type) {
    case JPH::EMotionType::Static:
      color = JPH::Color::sGrey;
      break;
    case JPH::EMotionType::Kinematic:
      color = JPH::Color::sGreen;
      break;
    case JPH::EMotionType::Dynamic:
      color = JPH::Color::sGetDistinctColor(body_id.GetIndex());
      break;
    default:
      JPH_ASSERT(false);
      color = JPH::Color::sBlack;
      break;
  }
  return color;
}

JPH::Ref<JPH::Shape> CreateContainer(const SceneNode *node) {
  // std::cout << "+1 container" << std::endl;
  JPH::StaticCompoundShapeSettings compoundSettings;
  JPH::AABox aabb = node->local_bounds;
  JPH::Vec3 center = aabb.GetCenter();
  JPH::Vec3 extent = aabb.GetExtent();
  float halfW = 0.5f * (extent.GetX() + extent.GetZ());
  float halfH = extent.GetY();
  float wallThickness = std::min(0.05f, halfW * 0.2f);
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY() - halfH + wallThickness * 0.5f,
                center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShape(JPH::Vec3(halfW, wallThickness * 0.5f, halfW)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY() + wallThickness * 0.5f,
                center.GetZ() + halfW - wallThickness * 0.5f),
      JPH::Quat::sIdentity(),
      new JPH::BoxShape(JPH::Vec3(halfW - wallThickness,
                                  halfH - wallThickness * 0.5f,
                                  wallThickness * 0.5f)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY() + wallThickness * 0.5f,
                center.GetZ() - halfW + wallThickness * 0.5f),
      JPH::Quat::sIdentity(),
      new JPH::BoxShape(JPH::Vec3(halfW - wallThickness,
                                  halfH - wallThickness * 0.5f,
                                  wallThickness * 0.5f)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX() + halfW - wallThickness * 0.5f,
                center.GetY() + wallThickness * 0.5f, center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShape(JPH::Vec3(wallThickness * 0.5f,
                                  halfH - wallThickness * 0.5f,
                                  halfW - wallThickness)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX() - halfW + wallThickness * 0.5f,
                center.GetY() + wallThickness * 0.5f, center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShape(JPH::Vec3(wallThickness * 0.5f,
                                  halfH - wallThickness * 0.5f,
                                  halfW - wallThickness)));
  return compoundSettings.Create().Get();
}

JPH::Ref<JPH::Shape> CreateCabinet(const SceneNode *node) {
  // std::cout << "+1 cabinet" << std::endl;
  JPH::StaticCompoundShapeSettings compoundSettings;
  JPH::AABox aabb = node->local_bounds;
  JPH::Vec3 center = aabb.GetCenter();
  JPH::Vec3 extent = aabb.GetExtent();
  float halfW = extent.GetX();
  float halfH = extent.GetY();
  float halfD = extent.GetZ();
  float wallThickness = std::min(0.05f, std::min(halfW, halfD) * 0.1f);
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY() - halfH + wallThickness * 0.5f,
                center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShapeSettings(JPH::Vec3(halfW, wallThickness * 0.5f, halfD)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY() + halfH - wallThickness * 0.5f,
                center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShapeSettings(JPH::Vec3(halfW, wallThickness * 0.5f, halfD)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX(), center.GetY(),
                center.GetZ() - halfD + wallThickness * 0.5f),
      JPH::Quat::sIdentity(),
      new JPH::BoxShapeSettings(JPH::Vec3(
          halfW - wallThickness, halfH - wallThickness, wallThickness * 0.5f)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX() - halfW + wallThickness * 0.5f, center.GetY(),
                center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShapeSettings(
          JPH::Vec3(wallThickness * 0.5f, halfH - wallThickness, halfD)));
  compoundSettings.AddShape(
      JPH::Vec3(center.GetX() + halfW - wallThickness * 0.5f, center.GetY(),
                center.GetZ()),
      JPH::Quat::sIdentity(),
      new JPH::BoxShapeSettings(
          JPH::Vec3(wallThickness * 0.5f, halfH - wallThickness, halfD)));
  float innerBottomY = center.GetY() - halfH + wallThickness;
  float innerTopY = center.GetY() + halfH - wallThickness;
  float innerHeight = innerTopY - innerBottomY;
  for (int i = 1; i <= 2; ++i) {
    float shelfY = innerBottomY + innerHeight * (static_cast<float>(i) / 3.0f);
    compoundSettings.AddShape(JPH::Vec3(center.GetX(), shelfY, center.GetZ()),
                              JPH::Quat::sIdentity(),
                              new JPH::BoxShapeSettings(JPH::Vec3(
                                  halfW - wallThickness, wallThickness * 0.5f,
                                  halfD - wallThickness)));
  }
  return compoundSettings.Create().Get();
}

// zone might be as well nullptr in case of character TODO: bear it out
void Game::CreateBodyForNode(SceneNode *node) {
  if (!node->mesh) {
    for (SceneNode *child : node->children) {
      CreateBodyForNode(child);
    }
    return;
  }
  const auto scene = mdl_loader_.GetScene();
  const auto &mesh = *node->mesh;
  auto mesh_properties = GetAssetDefaultProperties(mesh.name);
  auto mesh_collision_type = mesh_properties.collision_type;
  auto mesh_type = mesh_properties.type;
  if (mesh_type == AssetProperties::Type::Portal ||
      mesh_type == AssetProperties::Type::Zone) {
    for (SceneNode *child : node->children) {
      CreateBodyForNode(child);
    }
    return;  // we do it in a separate call
  }
  std::vector<SceneZone *> zones;
  for (SceneZone *z : scene->scene_data_.zones) {
    if (z->scene_node->global_bounds.Overlaps(node->global_bounds)) {
      zones.push_back(z);
    }
  }
  if (zones.empty()) {
    std::cerr << "Game::CreateBodyForNode no zone" << std::endl;
    return;
  }
  JPH::Ref<JPH::Shape> local_shape;
  if (mesh_collision_type == AssetProperties::CollisionType::Container) {
    local_shape = CreateContainer(node);
  } else if (mesh_collision_type == AssetProperties::CollisionType::Cabinet) {
    local_shape = CreateCabinet(node);
  } else {
    if (mesh_type == AssetProperties::Type::Tile) {
      const Scene::TerrainData *terrain_data = nullptr;
      for (const auto &t : scene->terrain_datas) {
        if (t.name == mesh.name) {
          terrain_data = &t;
        }
      }
      if (!terrain_data) {
        std::cerr << "but tile no mesh found" << std::endl;
        return;
      }
      JPH::MeshShapeSettings settings(terrain_data->jolt_vertices,
                                      terrain_data->jolt_triangles);
      settings.Sanitize();
      JPH::MeshShapeSettings::ShapeResult shape_result = settings.Create();
      if (shape_result.HasError()) {
        throw std::runtime_error("Failed to create Jolt MeshShape: " +
                                 std::string(shape_result.GetError()));
      }
      local_shape = shape_result.Get();
    } else {
      local_shape = CreateMeshShape(mesh, mesh_collision_type);
    }
  }
  JPH::EMotionType motion_type = JPH::EMotionType::Static;
  JPH::ObjectLayer object_layer = Layers::NON_MOVING;
  JPH::Ref<JPH::Shape> shape_settings = local_shape;
  JPH::EActivation activation_state = JPH::EActivation::DontActivate;
  uint64_t jph_user_data = 0;
  if (mesh_type == AssetProperties::Type::Dynamic ||
      mesh_type == AssetProperties::Type::HingeMoving) {
    motion_type = JPH::EMotionType::Dynamic;
    object_layer = Layers::MOVING;
    node->can_be_activated = true;
    // activation_state = JPH::EActivation::Activate;
    for (auto zone : zones) {
      zone->static_objects_.push_back(std::make_unique<StaticObject>(node));
    }
    jph_user_data = reinterpret_cast<uint64_t>(
        static_cast<GameObject *>(zones[0]->static_objects_.back().get()));
  } else if (mesh_type == AssetProperties::Type::Hinge) {
    motion_type = JPH::EMotionType::Kinematic;
    object_layer = Layers::MOVING;
    for (auto zone : zones) {
      zone->static_objects_.push_back(std::make_unique<StaticObject>(node));
    }
    hinges_.emplace_back(std::make_unique<Hinge>(mPhysicsSystem, node));
    jph_user_data = reinterpret_cast<uint64_t>(hinges_.back().get());
  } else if (mesh_type == AssetProperties::Type::Character) {
    characters_.push_back(std::make_unique<EnemyController>(
        character_shared_data_.get(), node, &scene->character_skins_[0],
        &scene->character_rig_, &audio_engine_, &sfx_group_));
    weapons_.push_back(std::make_unique<Weapon>(
      &scene->weapons_[0], mPhysicsSystem, &animator_, &audio_engine_, &sfx_group_));
    characters_.back()->GetBody()->EquipWeapon(weapons_.back().get());
    characters_.back()->GetBody()->SetPositionRotation(
        node->local_transform.t, node->local_transform.r.Normalized());
    return;
  } else if (mesh_type == AssetProperties::Type::PointLight) {
    std::vector<JPH::Vec4> colors;
    colors.push_back(JPH::Color::sGreen.ToVec4());
    colors.push_back(JPH::Color::sWhite.ToVec4());
    colors.push_back(JPH::Color::sRed.ToVec4());
    colors.push_back(JPH::Color::sBlue.ToVec4());
    colors.push_back(JPH::Color::sYellow.ToVec4());
    colors.push_back(JPH::Color::sBlack.ToVec4());
    int color_id = node->node_id % colors.size();
    for (auto zone : zones) {
      zone->point_lights_.push_back(std::make_unique<PointLight>(node));
      zone->point_lights_.back()->radius_ = 5.0f;
      zone->point_lights_.back()->color_ =
          5.0f * JPH::Vec3(colors[color_id].GetX(), colors[color_id].GetY(),
                           colors[color_id].GetZ());
    }

    // jph_user_data = reinterpret_cast<uint64_t>(
        // static_cast<GameObject *>(&zones[0]->point_lights_.back()));
    return;
  } else if (mesh_type == AssetProperties::Type::PointLightFake) {
    for (auto zone : zones) {
      zone->point_lights_fake_.push_back(std::make_unique<StaticObject>(node));
    }
    // jph_user_data = reinterpret_cast<uint64_t>(
        // static_cast<GameObject *>(&zones[0]->point_lights_fake_.back()));
    return;
  } else if (mesh_type == AssetProperties::Type::Portal ||
             mesh_type == AssetProperties::Type::None ||
             mesh_type == AssetProperties::Type::HingeBase) {
    for (auto zone : zones) {
      zone->static_objects_.push_back(std::make_unique<StaticObject>(node));
    }
    // jph_user_data = reinterpret_cast<uint64_t>(
        // static_cast<GameObject *>(zones[0]->static_objects_.back().get()));
    for (SceneNode *child : node->children) {
      CreateBodyForNode(child);
    }
    // non-physics bodies
    return;
  } else if (mesh_type == AssetProperties::Type::Tile) {
    for (auto zone : zones) {
      zone->static_objects_.push_back(std::make_unique<StaticObject>(node));
    }
    jph_user_data = reinterpret_cast<uint64_t>(
        static_cast<GameObject *>(zones[0]->static_objects_.back().get()));
    motion_type = JPH::EMotionType::Static;
    object_layer = Layers::NON_MOVING;
    activation_state = JPH::EActivation::DontActivate;
  } else {
    for (auto zone : zones) {
      zone->static_objects_.push_back(std::make_unique<StaticObject>(node));
    }
    jph_user_data = reinterpret_cast<uint64_t>(
        static_cast<GameObject *>(zones[0]->static_objects_.back().get()));
  }

  if (!node->body_id.IsInvalid()) {
    // std::cout << "obj already exists" << std::endl;
    return;  // already exists (for objects exist in few rooms like portals)
  }

  // no scale component, so safe
  JPH::Vec3 translation = node->global_transform.GetTranslation();
  JPH::Quat rotation =
      node->global_transform.GetRotation().GetQuaternion().Normalized();
  JPH::BodyCreationSettings obj_settings(shape_settings, translation, rotation,
                                         motion_type, object_layer);
  obj_settings.mOverrideMassProperties =
      JPH::EOverrideMassProperties::CalculateInertia;
  obj_settings.mMassPropertiesOverride.mMass = 10.0f;
  if (jph_user_data != 0) {
    obj_settings.mUserData = jph_user_data;
  }
  node->body_id =
      mBodyInterface->CreateAndAddBody(obj_settings, activation_state);
  node->shape = local_shape;
  for (SceneNode *child : node->children) {
    CreateBodyForNode(child);
  }
}

void Game::Init() {
  // glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  // jph

  // Allocate temp memory
  mMaxConcurrentJobs = std::thread::hardware_concurrency();

  mTempAllocator = new JPH::TempAllocatorImpl(32 * 1024 * 1024);
  mJobSystem = new JPH::JobSystemThreadPool(
      JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, mMaxConcurrentJobs - 1);
  mJobSystemValidating = new JPH::JobSystemSingleThreaded(JPH::cMaxPhysicsJobs);

  JPH::Vec3 old_gravity = mPhysicsSystem != nullptr
                              ? mPhysicsSystem->GetGravity()
                              : JPH::Vec3(0, -9.81f, 0);

  // Create physics system
  mPhysicsSystem = new JPH::PhysicsSystem();
  mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs,
                       cMaxContactConstraints, mBroadPhaseLayerInterface,
                       mObjectVsBroadPhaseLayerFilter,
                       mObjectVsObjectLayerFilter);
  mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

  // Restore gravity
  mPhysicsSystem->SetGravity(old_gravity);

  mBodyInterface = &mPhysicsSystem->GetBodyInterface();
  mContactListener = new ContactListenerImpl;
  mContactListener->SetNextListener(this);
  mPhysicsSystem->SetContactListener(mContactListener);

  mPhysicsSystem->OptimizeBroadPhase();

  camera_.Reset();
  camera_.SetOriginDist(10.0f);

  InitAudio();

  LoadScenes();
  InitScenesBodies();

  DirectedLight sun = {};
  sun.dir = JPH::Vec3(0.8f, -1.0f, -0.3f).Normalized();
  sun.color = JPH::Vec3::sReplicate(20.0f);
  dir_lights_.push_back(sun);

  // terrain_renderer_.LoadHmap(&bi,
  // scene->scene_data_.tiles[0]->properties.hmap_path);
  // terrain_renderer_.LoadDefaultSurface(mBodyInterface);
  // auto scene = mdl_loader_.GetScene();
  // terrain_renderer_.LoadFromScene(
  // mBodyInterface, scene->jolt_vertices, scene->jolt_triangles);

  InitUi();
  // cubemap_.LoadZone(world_manager_.GetCurrentZone());
  cubemap_.LoadDefaultCubemap();

  global_data_.renderer = &renderer_;
  global_data_.ui_manager = mDebugUI.get();
  global_data_.game = this;
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void *>(&global_data_));
}

void Game::DeInit() {
  // mCharacter->RemoveFromPhysicsSystem();
  ma_sound_group_uninit(&sfx_group_);
  for (auto& s : collision_sounds_) {
    ma_sound_uninit(s.get());
  }
  ma_sound_uninit(&ambient_music_);
  ma_engine_uninit(&audio_engine_);
}

void GameScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  game->camera_.ZoomOriginDist(yoffset);
}

void GameMouseButtonCallback(GLFWwindow *window, int button, int action,
                             int mods) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  // bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  // bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (game->player_->IsDragging()) {
      game->player_->ReleaseObjectDragging(50.0f);
    } else {
      game->player_->Shoot();
    }
  } else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_MIDDLE) {
    game->player_->SetAiming(true);
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_MIDDLE) {
    game->player_->SetAiming(false);
  } else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
    game->player_->TryEnterCover();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
    game->player_->ExitCover();
  }
}

void GameKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      if (mod_shift) {
        glfwSetWindowShouldClose(window, true);
      }
    } else if (key == GLFW_KEY_TAB) {
      game->state_ = Game::State::Menu;
    } else if (key == GLFW_KEY_F1) {
      game->camera_.SwitchFaceMode();
    } else if (key == GLFW_KEY_F2) {
      game->render_physics_only_ = !game->render_physics_only_;
    } else if (key == GLFW_KEY_E) {
      if (game->hovered_object_) {
        game->hovered_object_->Interact(game->player_->GetBody());
      } else {
        game->player_->StartObjectDragging();
      }
    }
  } else if (action == GLFW_RELEASE && key == GLFW_KEY_E) {
    game->player_->ReleaseObjectDragging(1.0f);
  }
  game->player_->ProcessMovement(key, action);
}

void GameCursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  // std::cout << xpos << ' ' << ypos << std::endl;

  lastX = xpos;
  lastY = ypos;

  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;

  game->camera_.MoveRotateView(xoffset, yoffset);
  // game->camera_.MoveRotateViewOrigin(xoffset, yoffset);
}

void MenuScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
}

void MenuMouseButtonCallback(GLFWwindow *window, int button, int action,
                             int mods) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    game->ui_menu_scene_->MouseDown(x, y);
    std::cout << x << ' ' << y << std::endl;
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    game->ui_menu_scene_->MouseUp(x, y);
  }
}

void MenuKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      if (mod_shift) {
        glfwSetWindowShouldClose(window, true);
      } else {
        game->mDebugUI->ToggleVisibility();
      }
    } else if (key == GLFW_KEY_TAB) {
      game->state_ = Game::State::Game;
    }
  } else if (action == GLFW_RELEASE && key == GLFW_KEY_E) {
  }
}

void MenuCursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  auto context =
      reinterpret_cast<GlfwContext *>(glfwGetWindowUserPointer(window));
  auto game = context->game;
  game->ui_menu_scene_->MouseMove(xpos, ypos);
}

/// ---

void Game::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                          const JPH::ContactManifold &inManifold,
                          JPH::ContactSettings &ioSettings) {
  auto* data1 = reinterpret_cast<GameObject*>(inBody1.GetUserData());
  auto* data2 = reinterpret_cast<GameObject*>(inBody2.GetUserData());
  if (!data1 || !data2) return;
  auto currentTime = glfwGetTime();
  if (currentTime - data1->lastCollisionSoundTime < 0.15f) {
    return;
  }
  data1->lastCollisionSoundTime = currentTime;
  data2->lastCollisionSoundTime = currentTime;
  JPH::Vec3 relVel = inBody1.GetLinearVelocity() - inBody2.GetLinearVelocity();
  float collisionSpeed = relVel.Length();
  if (collisionSpeed < 2.0f) return;
  // std::cout << collisionSpeed << std::endl;
  Scene* scene = mdl_loader_.GetScene();
  const auto& propsA = data1->GetAssetProperties();
  const auto& propsB = data2->GetAssetProperties();
  PlayImpactSound(static_cast<int>(propsA.audio_material),
    static_cast<int>(propsB.audio_material),
    inManifold.GetWorldSpaceContactPointOn1(0));
}

std::string_view gCollisionSounds[] = {
  "../assets/audio/sound_metal1.mp3",
  "../assets/audio/sound_wood.mp3",
  "../assets/audio/sound_glass.mp3",
  "../assets/audio/sound_coin.mp3",
  "../assets/audio/sound_footstep.mp3",
};

void Game::InitAudio() {
  ma_result result;
  result = ma_engine_init(nullptr, &audio_engine_);
  if (result != MA_SUCCESS) {
    std::cerr << "Failed to initialize audio engine.\n";
    throw -1;
  }
  ma_sound_init_from_file(&audio_engine_, "../assets/audio/ambient_forest.mp3",
                          MA_SOUND_FLAG_STREAM, NULL, NULL, &ambient_music_);
  if (result != MA_SUCCESS) {
    std::cerr << "Failed to load music file.\n";
    ma_engine_uninit(&audio_engine_);
    throw -1;
  }
  ma_sound_set_looping(&ambient_music_, MA_TRUE);
  ma_sound_set_spatialization_enabled(&ambient_music_, MA_FALSE);
  ma_sound_set_volume(&ambient_music_, music_level_);
  ma_sound_start(&ambient_music_);

  ma_sound_group_init(&audio_engine_, 0, NULL, &sfx_group_);
  constexpr size_t poolSize = 8; // Size of your sound pool
  collision_sounds_.reserve(poolSize);
  for (size_t i = 0; i < poolSize; ++i) {
    auto sound = std::make_unique<ma_sound>();
    if (ma_sound_init_from_file(&audio_engine_, gCollisionSounds[0].data(), 0, &sfx_group_, NULL, sound.get()) == MA_SUCCESS) {
      ma_sound_set_spatialization_enabled(sound.get(), MA_TRUE);
      collision_sounds_.push_back(std::move(sound));
    }
  }
  ma_sound_group_set_volume(&sfx_group_, sounds_level_);
}

void Game::PlayImpactSound(int material_1, int material_2, JPH::Vec3 position) {
    int collision_sound_type = material_1;

    // Bounds check to prevent out-of-range crashes
    if (collision_sound_type < 0 || collision_sound_type >= static_cast<int>(std::size(gCollisionSounds))) {
        collision_sound_type = 0;
    }

    ma_sound* targetSound = nullptr;

    // 1. Find the first sound instance in the pool that isn't currently playing
    for (std::unique_ptr<ma_sound>& src : collision_sounds_) {
        if (!ma_sound_is_playing(src.get())) {
            targetSound = src.get();
            break;
        }
    }

    // 2. Fallback: If all slots are busy, steal the first slot
    if (!targetSound && !collision_sounds_.empty()) {
        targetSound = collision_sounds_[0].get();
        ma_sound_stop(targetSound);
    }

    // 3. Re-target and play the sound
    if (targetSound) {
        ma_sound_uninit(targetSound); // Safe to call; miniaudio's resource manager caches the underlying data

        if (ma_sound_init_from_file(&audio_engine_, gCollisionSounds[collision_sound_type].data(), 0, &sfx_group_, NULL, targetSound) == MA_SUCCESS) {
            ma_sound_set_spatialization_enabled(targetSound, MA_TRUE);
            ma_sound_set_position(targetSound, position.GetX(), position.GetY(), position.GetZ());
            ma_sound_start(targetSound);
        }
    }
}

void Game::OnContactPersisted(const JPH::Body &inBody1,
                              const JPH::Body &inBody2,
                              const JPH::ContactManifold &inManifold,
                              JPH::ContactSettings &ioSettings) {
  // Same behavior as contact added
  OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void Game::OnAdjustBodyVelocity(const JPH::CharacterVirtual *inCharacter,
                                const JPH::Body &inBody2,
                                JPH::Vec3 &ioLinearVelocity,
                                JPH::Vec3 &ioAngularVelocity) {
  // Apply artificial velocity to the character when standing on the conveyor
  // belt
  // if (inBody2.GetID() == mConveyorBeltBody) ioLinearVelocity += JPH::Vec3(0,
  // 0, 2);
}

void Game::OnContactCommon(const JPH::CharacterVirtual *inCharacter,
                           const JPH::BodyID &inBodyID2,
                           const JPH::SubShapeID &inSubShapeID2,
                           JPH::RVec3Arg inContactPosition,
                           JPH::Vec3Arg inContactNormal,
                           JPH::CharacterContactSettings &ioSettings) {
  // Draw a box around the character when it enters the sensor
  // if (inBodyID2 == mSensorBody) {
  //   JPH::AABox box = inCharacter->GetShape()->GetWorldSpaceBounds(
  //       inCharacter->GetCenterOfMassTransform(), JPH::Vec3::sOne());
  //   // mDebugRenderer->DrawBox(box, JPH::Color::sGreen,
  //   JPH::DebugRenderer::ECastShadow::Off,
  //   //                         JPH::DebugRenderer::EDrawMode::Wireframe);
  // }

  // Dynamic boxes on the ramp go through all permutations
  // JPH::Array<JPH::BodyID>::const_iterator i =
  //     std::find(mRampBlocks.begin(), mRampBlocks.end(), inBodyID2);
  // if (i != mRampBlocks.end()) {
  //   size_t index = i - mRampBlocks.begin();
  //   ioSettings.mCanPushCharacter = (index & 1) != 0;
  //   ioSettings.mCanReceiveImpulses = (index & 2) != 0;
  // }

  // If we encounter an object that can push the player, enable sliding
  if (inCharacter == player_->GetBody()->GetJphCharacter() &&
      ioSettings.mCanPushCharacter &&
      mPhysicsSystem->GetBodyInterface().GetMotionType(inBodyID2) !=
          JPH::EMotionType::Static)
    player_->GetBody()->AllowSliding(true);
}

void Game::OnContactAdded(const JPH::CharacterVirtual *inCharacter,
                          const JPH::BodyID &inBodyID2,
                          const JPH::SubShapeID &inSubShapeID2,
                          JPH::RVec3Arg inContactPosition,
                          JPH::Vec3Arg inContactNormal,
                          JPH::CharacterContactSettings &ioSettings) {
  OnContactCommon(inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
                  inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact added with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    JPH::CharacterVirtual::ContactKey c(inBodyID2, inSubShapeID2);
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(), c) !=
        mActiveContacts.end())
      throw std::runtime_error(
          "Got an add contact that should have been a persisted contact");
    mActiveContacts.push_back(c);
  }
}

void Game::OnContactPersisted(const JPH::CharacterVirtual *inCharacter,
                              const JPH::BodyID &inBodyID2,
                              const JPH::SubShapeID &inSubShapeID2,
                              JPH::RVec3Arg inContactPosition,
                              JPH::Vec3Arg inContactNormal,
                              JPH::CharacterContactSettings &ioSettings) {
  OnContactCommon(inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
                  inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact persisted with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(
                      inBodyID2, inSubShapeID2)) == mActiveContacts.end())
      throw std::runtime_error(
          "Got a persisted contact that should have been an add contact");
  }
}

void Game::OnContactRemoved(const JPH::CharacterVirtual *inCharacter,
                            const JPH::BodyID &inBodyID2,
                            const JPH::SubShapeID &inSubShapeID2) {
  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact removed with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    ContactSet::iterator it =
        std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inBodyID2, inSubShapeID2));
    if (it == mActiveContacts.end())
      throw std::runtime_error("Got a remove contact that has not been added");
    mActiveContacts.erase(it);
  }
}

void Game::OnCharacterContactCommon(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
  // Characters can only be pushed in their own update
  ioSettings.mCanPushCharacter =
      inOtherCharacter == player_->GetBody()->GetJphCharacter();
  // If the player can be pushed by the other virtual character, we allow
  // sliding
  if (inCharacter == player_->GetBody()->GetJphCharacter() &&
      ioSettings.mCanPushCharacter)
    player_->GetBody()->AllowSliding(true);
}

void Game::OnCharacterContactAdded(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
  OnCharacterContactCommon(inCharacter, inOtherCharacter, inSubShapeID2,
                           inContactPosition, inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact added with character %08x, sub shape %08x",
          inOtherCharacter->GetID().GetValue(), inSubShapeID2.GetValue());
#endif
    JPH::CharacterVirtual::ContactKey c(inOtherCharacter->GetID(),
                                        inSubShapeID2);
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(), c) !=
        mActiveContacts.end())
      throw std::runtime_error(
          "Got an add contact that should have been a persisted contact");
    mActiveContacts.push_back(c);
  }
}

void Game::OnCharacterContactPersisted(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
  OnCharacterContactCommon(inCharacter, inOtherCharacter, inSubShapeID2,
                           inContactPosition, inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact persisted with character %08x, sub shape %08x",
          inOtherCharacter->GetID().GetValue(), inSubShapeID2.GetValue());
#endif
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inOtherCharacter->GetID(),
                                                    inSubShapeID2)) ==
        mActiveContacts.end())
      throw std::runtime_error(
          "Got a persisted contact that should have been an add contact");
  }
}

void Game::OnCharacterContactRemoved(const JPH::CharacterVirtual *inCharacter,
                                     const JPH::CharacterID &inOtherCharacterID,
                                     const JPH::SubShapeID &inSubShapeID2) {
  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact removed with character %08x, sub shape %08x",
          inOtherCharacterID.GetValue(), inSubShapeID2.GetValue());
#endif
    ContactSet::iterator it = std::find(
        mActiveContacts.begin(), mActiveContacts.end(),
        JPH::CharacterVirtual::ContactKey(inOtherCharacterID, inSubShapeID2));
    if (it == mActiveContacts.end())
      throw std::runtime_error("Got a remove contact that has not been added");
    mActiveContacts.erase(it);
  }
}

void Game::OnContactSolve(
    const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::Vec3Arg inContactVelocity,
    const JPH::PhysicsMaterial *inContactMaterial,
    JPH::Vec3Arg inCharacterVelocity, JPH::Vec3 &ioNewCharacterVelocity) {
  // Ignore callbacks for other characters than the player
  if (inCharacter != player_->GetBody()->GetJphCharacter()) return;

  // Don't allow the player to slide down static not-too-steep surfaces when not
  // actively moving and when not on a moving platform
  if (!player_->GetBody()->GetAllowSliding() &&
      inContactVelocity.IsNearZero() &&
      !inCharacter->IsSlopeTooSteep(inContactNormal))
    ioNewCharacterVelocity = JPH::Vec3::sZero();
}

void Game::LoadScenes() {
  mdl_loader_.LoadDebugShapes(
      "C:\\Users\\Pavlo\\Desktop\\assets\\DebugShapes.gltf");
  mdl_loader_.LoadScene(
      "C:\\Users\\Pavlo\\Desktop\\assets\\SceneBackyard.gltf",
      {"C:\\Users\\Pavlo\\Desktop\\assets\\OutdoorStuff.gltf",
       "C:\\Users\\Pavlo\\Desktop\\assets\\DownloadedSkyrim.gltf",
       "C:\\Users\\Pavlo\\Desktop\\assets\\House.gltf"});
  mdl_loader_.LoadSceneMaterials(
      "C:\\Users\\Pavlo\\Desktop\\assets\\Materials.gltf");
  // mdl_loader_.LoadEmbroideryTextures("C:\\Users\\Pavlo\\Desktop\\assets\\Embroidery");
  mdl_loader_.LoadEmbroideryTextures(
      "C:\\Users\\Pavlo\\Desktop\\assets\\Embroidery\\tablecloth",
      "C:\\Users\\Pavlo\\Desktop\\assets\\Embroidery\\ryadno",
      "C:\\Users\\Pavlo\\Desktop\\assets\\Embroidery\\ryshnuk",
      "C:\\Users\\Pavlo\\Desktop\\assets\\Embroidery\\ribbons");
  mdl_loader_.LoadSkyrimMaterials(
      "C:\\Users\\Pavlo\\Desktop\\assets\\SkyrimTextures");
  mdl_loader_.LoadCharacters("C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf",
                             {"C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf"});
  // TODO: create CharacterSkeleton.gltf
  //  mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\nagan.gltf"});
  mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\MauserC96.gltf"});
  // mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\Nagant.gltf"});
}

void Game::InitScenesBodies() {
  const auto scene = mdl_loader_.GetScene();

  character_shared_data_ = std::make_unique<CharacterSharedData>(
      mPhysicsSystem, mTempAllocator, this, &animator_, &characters_);

  const JPH::BodyLockInterface &bli = mPhysicsSystem->GetBodyLockInterface();

  JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();

  InitSceneGlobalTransforms(scene);
  mdl_loader_.GetScene()->UpdateRenderTransform(bli);
  mdl_loader_.GetScene()->ConnectZonesWithPortals(0);

  // terrain_renderer_.InitializeBody(mBodyInterface);
  //  std::cout << "tile added" << std::endl;
  for (auto node : scene->scene_data_.characters) {
    CreateBodyForNode(node);
  }
  for (auto node : scene->scene_data_.object_nodes) {
    CreateBodyForNode(node);
  }
  for (auto portal : scene->scene_data_.portals) {
    // DESK, NOT FRAME, because frame doesn't have a jph body
    if (!portal.frame) {
      continue;
    }
    for (SceneNode *desk : portal.desks) {
      doors_.emplace_back(std::make_unique<Door>());
      for (SceneNode *door_hinge : portal.hinges) {
        for (const auto &h : hinges_) {
          if (h->GetSceneNode()->body_id == door_hinge->body_id) {
            auto hinge_aabox = JPH::AABox(
                h->GetSceneNode()->global_bounds.GetCenter(), 0.1f);
            if (desk->global_bounds.Overlaps(hinge_aabox)) {
              // std::cout << "connected" << std::endl;
              doors_.back()->AddHinge(h.get());
              break;
            }
          }
        }
      }
      doors_.back()->Initialize(mPhysicsSystem, desk, portal.frame->body_id,
        &audio_engine_, &sfx_group_);
    }
  }
  CreateBodyForNode(scene->scene_data_.tiles[0]->scene_node);

  auto player_node = scene->scene_data_.player_node;
  player_ = std::make_unique<PlayerController>(
      &camera_, character_shared_data_.get(), player_node,
      &scene->character_skins_[0], &scene->character_rig_, &audio_engine_, &sfx_group_);
  weapons_.push_back(std::make_unique<Weapon>(
    &scene->weapons_[0], mPhysicsSystem, &animator_, &audio_engine_, &sfx_group_));
  player_->GetBody()->EquipWeapon(weapons_.back().get());
  player_->GetBody()->SetPositionRotation(
      player_node->local_transform.t,
      player_node->local_transform.r.Normalized());

  world_manager_.UpdatePlayerZone(player_->GetBody()->GetPosition(), &bi);

  /// clearance.. probably TOOD: more
  scene->terrain_datas.clear();
}

void Game::InitUi() {
  ui_menu_scene_ = std::make_unique<UiScene>(&ui_renderer_);
  mDebugUI = std::make_unique<DebugUI>(ui_menu_scene_.get(), nullptr);
  {
    // Disable allocation checking

    // Create UI
    UIElement *main_menu = mDebugUI->CreateMenu();
    mDebugUI->CreateTextButton(main_menu, "remove hinges", [this]() {
      std::cout << "removing hinges" << std::endl;
      for (auto &hinge : hinges_) {
        hinge->Interact(nullptr);
      }
    });
    mDebugUI->CreateSlider(main_menu, "music level",
                           music_level_, 0.0f, 1.0f, 0.01f,
                           [this](float inValue) {
                             music_level_ = inValue;
                             ma_sound_set_volume(&ambient_music_, music_level_);
    });
    mDebugUI->CreateSlider(main_menu, "sounds level",
                           sounds_level_, 0.0f, 1.0f, 0.01f,
                           [this](float inValue) {
                             sounds_level_ = inValue;
                             ma_sound_group_set_volume(&sfx_group_, sounds_level_);
                           });
    mDebugUI->CreateTextButton(main_menu, "Physics Settings", [this]() {
      UIElement *phys_settings = mDebugUI->CreateMenu();
      mDebugUI->CreateSlider(
          phys_settings, "Max Concurrent Jobs", float(mMaxConcurrentJobs), 1,
          float(std::thread::hardware_concurrency()), 1,
          [this](float inValue) { mMaxConcurrentJobs = (int)inValue; });
      mDebugUI->CreateSlider(
          phys_settings, "Gravity (m/s^2)",
          -mPhysicsSystem->GetGravity().GetY(), 0.0f, 20.0f, 1.0f,
          [this](float inValue) {
            mPhysicsSystem->SetGravity(JPH::Vec3(0, -inValue, 0));
          });
      mDebugUI->CreateSlider(
          phys_settings, "Num Velocity Steps",
          float(mPhysicsSettings.mNumVelocitySteps), 0, 30, 1,
          [this](float inValue) {
            mPhysicsSettings.mNumVelocitySteps = int(round(inValue));
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateSlider(
          phys_settings, "Num Position Steps",
          float(mPhysicsSettings.mNumPositionSteps), 0, 30, 1,
          [this](float inValue) {
            mPhysicsSettings.mNumPositionSteps = int(round(inValue));
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateSlider(
          phys_settings, "Baumgarte Stabilization Factor",
          mPhysicsSettings.mBaumgarte, 0.01f, 1.0f, 0.05f,
          [this](float inValue) {
            mPhysicsSettings.mBaumgarte = inValue;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateSlider(phys_settings, "Speculative Contact Distance (m)",
                             mPhysicsSettings.mSpeculativeContactDistance, 0.0f,
                             0.1f, 0.005f, [this](float inValue) {
                               mPhysicsSettings.mSpeculativeContactDistance =
                                   inValue;
                             });
      mDebugUI->CreateSlider(phys_settings, "Penetration Slop (m)",
                             mPhysicsSettings.mPenetrationSlop, 0.0f, 0.1f,
                             0.005f, [this](float inValue) {
                               mPhysicsSettings.mPenetrationSlop = inValue;
                             });
      mDebugUI->CreateSlider(phys_settings, "Linear Cast Threshold",
                             mPhysicsSettings.mLinearCastThreshold, 0.0f, 1.0f,
                             0.05f, [this](float inValue) {
                               mPhysicsSettings.mLinearCastThreshold = inValue;
                             });
      mDebugUI->CreateSlider(
          phys_settings, "Min Velocity For Restitution (m/s)",
          mPhysicsSettings.mMinVelocityForRestitution, 0.0f, 10.0f, 0.1f,
          [this](float inValue) {
            mPhysicsSettings.mMinVelocityForRestitution = inValue;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateSlider(
          phys_settings, "Time Before Sleep (s)",
          mPhysicsSettings.mTimeBeforeSleep, 0.1f, 1.0f, 0.1f,
          [this](float inValue) {
            mPhysicsSettings.mTimeBeforeSleep = inValue;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateSlider(
          phys_settings, "Point Velocity Sleep Threshold (m/s)",
          mPhysicsSettings.mPointVelocitySleepThreshold, 0.01f, 1.0f, 0.01f,
          [this](float inValue) {
            mPhysicsSettings.mPointVelocitySleepThreshold = inValue;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
#ifdef JPH_CUSTOM_MEMORY_HOOK_ENABLED
      mDebugUI->CreateCheckBox(
          phys_settings, "Enable Checking Memory Hook",
          IsCustomMemoryHookEnabled(), [](UICheckBox::EState inState) {
            EnableCustomMemoryHook(inState == UICheckBox::STATE_CHECKED);
          });
#endif
      mDebugUI->CreateCheckBox(
          phys_settings, "Deterministic Simulation",
          mPhysicsSettings.mDeterministicSimulation,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mDeterministicSimulation =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Constraint Warm Starting",
          mPhysicsSettings.mConstraintWarmStart,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mConstraintWarmStart =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Use Body Pair Contact Cache",
          mPhysicsSettings.mUseBodyPairContactCache,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mUseBodyPairContactCache =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Contact Manifold Reduction",
          mPhysicsSettings.mUseManifoldReduction,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mUseManifoldReduction =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Use Large Island Splitter",
          mPhysicsSettings.mUseLargeIslandSplitter,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mUseLargeIslandSplitter =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Allow Sleeping", mPhysicsSettings.mAllowSleeping,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mAllowSleeping =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->CreateCheckBox(
          phys_settings, "Check Active Triangle Edges",
          mPhysicsSettings.mCheckActiveEdges,
          [this](UICheckBox::EState inState) {
            mPhysicsSettings.mCheckActiveEdges =
                inState == UICheckBox::STATE_CHECKED;
            mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);
          });
      mDebugUI->ShowMenu(phys_settings);
    });
    mDebugUI->CreateTextButton(main_menu, "Graphics Settings", [this]() {
      UIElement *graphic_settings = mDebugUI->CreateMenu();
       mDebugUI->CreateCheckBox(graphic_settings, "ssao", render_settings_.ssao,
                             [this](UICheckBox::EState state) {
                               render_settings_.ssao =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "render shadows",
                             render_settings_.render_shadows,
                             [this](UICheckBox::EState state) {
                               render_settings_.render_shadows =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "apply shadows",
                             render_settings_.apply_shadows,
                             [this](UICheckBox::EState state) {
                               render_settings_.apply_shadows =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "bloom", render_settings_.bloom,
                             [this](UICheckBox::EState state) {
                               render_settings_.bloom =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "cull", render_settings_.cull,
                             [this](UICheckBox::EState state) {
                               render_settings_.cull =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "light sources",
                             render_settings_.light_sources,
                             [this](UICheckBox::EState state) {
                               render_settings_.light_sources =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "render animated",
                             render_settings_.render_animated,
                             [this](UICheckBox::EState state) {
                               render_settings_.render_animated =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "animate animated",
                             render_settings_.animate_animated,
                             [this](UICheckBox::EState state) {
                               render_settings_.animate_animated =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "geometry pass",
                             render_settings_.geometry_pass,
                             [this](UICheckBox::EState state) {
                               render_settings_.geometry_pass =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "light pass",
                             render_settings_.light_pass,
                             [this](UICheckBox::EState state) {
                               render_settings_.light_pass =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "update physics",
                             render_settings_.update_physics,
                             [this](UICheckBox::EState state) {
                               render_settings_.update_physics =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "update buffer",
                             render_settings_.update_buffer,
                             [this](UICheckBox::EState state) {
                               render_settings_.update_buffer =
                                   state == UICheckBox::EState::STATE_CHECKED;
                             });
    mDebugUI->CreateCheckBox(graphic_settings, "ibl", render_settings_.ibl,
                             [this](UICheckBox::EState state) {
                               render_settings_.ibl =
                                   state == UICheckBox::EState::STATE_CHECKED;
                               std::cout << render_settings_.ibl << std::endl;
                             });
    mDebugUI->CreateSlider(
        graphic_settings, "light distance", float(5.0f), 1, 25, 1,
        [this](float inValue) {
          for (auto zone :
               mdl_loader_.GetScene()->scene_data_.zones) {
            for (auto &l : zone->point_lights_) {
              l->radius_ = float(inValue);
            }
          }
        });
    mDebugUI->CreateTextButton(graphic_settings, "prebake env maps",
                               [this] { this->PrebakeEnvMaps(); });
    auto window_resolutions_strings = GenWindowResolutionStrings();
    mDebugUI->CreateComboBox(
        graphic_settings, "resolution", window_resolutions_strings,
        GetDefaultWindowResolutionId(), [](int id) {
          auto res = GetWindowResolution(id);
          gWindowWidth = res.width;
          gWindowHeight = res.height;
          glfwSetWindowSize(gWindow, gWindowWidth, gWindowHeight);
        });
    mDebugUI->CreateCheckBox(
        graphic_settings, "fullscreen", false, [this](UICheckBox::EState state) {
          SetFullscreen(state == UICheckBox::EState::STATE_CHECKED);
        });
    mDebugUI->CreateSlider(
        graphic_settings, "objects num", float(render_settings_.objects_num), 1, 500,
        1,
        [this](float inValue) { render_settings_.objects_num = int(inValue); });
    mDebugUI->CreateSlider(graphic_settings, "triplanar level",
                           float(render_settings_.triplanar_level), 1, 3, 1,
                           [this](float inValue) {
                             render_settings_.triplanar_level = (int)inValue;
                           });
    mDebugUI->CreateSlider(graphic_settings, "triplanar level",
                           float(render_settings_.cubemap_type), 0, 2, 1,
                           [this](float inValue) {
                             render_settings_.cubemap_type = (int)inValue;
                           });
      mDebugUI->ShowMenu(graphic_settings);
    });
    mDebugUI->CreateTextButton(main_menu, "Help", [this]() {
      UIElement *help = mDebugUI->CreateMenu();
      mDebugUI->CreateStaticText(
          help,
          "ESC: Back to previous menu.\n"
          "WASD + Mouse: Fly around. Hold Shift to speed up, Ctrl to slow "
          "down.\n"
          "Space: Hold to pick up and drag a physics object under the "
          "crosshair.\n"
          "P: Pause / unpause simulation.\n"
          "O: Single step the simulation.\n"
          ",: Step back (only when Physics Settings / Record State for "
          "Playback is on).\n"
          ".: Step forward (only when Physics Settings / Record State for "
          "Playback is on).\n"
          "Shift + ,: Play reverse (only when Physics Settings / Record State "
          "for Playback is on).\n"
          "Shift + .: Replay forward (only when Physics Settings / Record "
          "State for Playback is on).\n"
          "T: Dump frame timing information to profile_*.html (when "
          "JPH_PROFILE_ENABLED defined).");
      mDebugUI->ShowMenu(help);
    });
    mDebugUI->ShowMenu(main_menu);
  }
}
