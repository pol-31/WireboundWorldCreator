#include "Game.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>

#include <glm/gtc/type_ptr.hpp>

#include "Layers.h"
#include "../io/Camera.h"
#include "../io/Window.h"

#include "../common/models/DirectedLight.h"

#include "../common/Callbacks.h"

#include "ContactListenerImpl.h"

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

Game::Game()
  : terrain_renderer_(&(mdl_loader_.GetScene()->materials)),
    world_manager_(mdl_loader_.GetScene(), &camera_, player_,
    characters_, weapons_, dir_lights_),
    renderer_(mdl_loader_.GetScene(), &camera_, &player_, world_manager_.GetCulledData()){
  Init();
}

void Game::RenderInterface() {
  float target_size = 32.0f;
  float half_target_size = target_size / 2.0f;
  if (player_->GetBody()->IsAiming()) {
    renderer_.AddSprite("GoldenCircle",
    glm::vec2(800.0f, 450.0f) - half_target_size,
    glm::vec2(target_size), glm::vec4(1.0f));
  }
  const glm::vec2 start_hp_pos = glm::vec2(10.0f, 45.0f);
  glm::vec2 hp_pos = start_hp_pos;
  glm::vec2 hp_size = glm::vec2(16.0f);
  for (int i = 0; i < 10; ++i) {
    hp_pos.x += hp_size.x;
    renderer_.AddSprite("FlowerWhite", hp_pos,
      hp_size, glm::vec4(1.0f));
  }
  hp_pos = start_hp_pos;
  hp_pos.y -= hp_size.y;
  for (int i = 0; i < 10; ++i) {
    hp_pos.x += hp_size.x;
    renderer_.AddSprite("StaminaPoint", hp_pos,
      hp_size, glm::vec4(1.0f));
  }
  hp_pos = glm::vec2(1380.0f, 25.0f);
  for (int i = 0; i < 7; ++i) {
    hp_pos.x += hp_size.x;
    renderer_.AddSprite("HealthPoint", hp_pos,
      hp_size * 2.0f, glm::vec4(1.0f));
  }
  renderer_.AddSprite("Gear", glm::vec2(1531.0f, 5.0f),
    hp_size * 4.0f, glm::vec4(1.0f));
}


void UpdateDirLightFrustum(DirectedLight& light, const Camera* camera) {
  constexpr float fovx = glm::radians(120.0f);
  constexpr float fovy = glm::radians(75.0f);
  constexpr float near_distance = 0.1f;

  auto world_up = ToJph(camera->GetDirectionWorldUp());
  auto camera_forward = ToJph(camera->GetDirectionFront());
  camera_forward = (camera_forward - world_up * camera_forward.Dot(world_up));
  camera_forward = camera_forward.NormalizedOr(JPH::Vec3(0, 0, -1));

  auto camera_pos = ToJph(camera->GetPosition());
  const JPH::Vec3 sun_position =
      camera_pos + JPH::Vec3(0.0f, 1.0f, 0.0f) +
      camera_forward * 4.0f;

  JPH::Vec3 sun_dir = JPH::Vec3(light.dir.x, light.dir.y, light.dir.z).Normalized();
  if (abs(sun_dir.Dot(world_up)) > 0.99f) {
    world_up = JPH::Vec3(1.0f, 0.0f, 0.0f);
  }

  light.frustum = Frustum(sun_position, sun_dir, world_up,fovx, fovy, near_distance);

  constexpr float near_plane = 1.0f;
  constexpr float far_plane = 7.5f;

  glm::mat4 lightProjection =
      glm::ortho(
          -10.0f, 10.0f,
          -10.0f, 10.0f,
          near_plane,
          far_plane);

  glm::vec3 sun_pos_glm(sun_position.GetX(), sun_position.GetY(), sun_position.GetZ());
  glm::vec3 sun_dir_glm(sun_dir.GetX(), sun_dir.GetY(), sun_dir.GetZ());
  glm::vec3 target = sun_pos_glm + sun_dir_glm;
  glm::mat4 lightView = glm::lookAt(sun_pos_glm, target,glm::vec3(0.0f, 1.0f, 0.0f));
  light.lightSpaceMatrix = lightProjection * lightView;
}

void Game::RunRenderLoop() {
  camera_.Reset();

  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  bool disable_animator = false;

  while (!glfwWindowShouldClose(gWindow)) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilMask(0xFF);
    glClearColor(0.2f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    UpdateDeltaTime();
    UpdateFPS(gDeltaTime);

    int characters_left = 0;
    for (const auto& c : characters_) {
      if (!c->GetBody()->IsDead()) {
        ++characters_left;
      }
    }
    if (characters_left == 0) {
      for (auto& c : characters_) {
        c->GetBody()->Revive();
      }
      characters_left = characters_.size();
    }
    std::string charactersLeftText = "enemies left: " + std::to_string(characters_left);
    renderer_.AddText(charactersLeftText, glm::vec2(100.0f, 20.0f),
      glm::vec2(1.0f), glm::vec4(1.0f));


    // Reinitialize the job system if the concurrency setting changed
    if (mMaxConcurrentJobs != mJobSystem->GetMaxConcurrency())
      static_cast<JPH::JobSystemThreadPool *>(mJobSystem)
          ->SetNumThreads(mMaxConcurrentJobs - 1);
    JPH::BodyInterface &bi = mPhysicsSystem->GetBodyInterface();

    for (auto& l : dir_lights_) {
      UpdateDirLightFrustum(l, &camera_);
    }


    /// pre physics update
    for (auto& c : characters_) {
      c->UpdateLogic(mPhysicsSystem, gDeltaTimePhysics);
      c->GetBody()->PrePhysicsUpdate(mPhysicsSystem, mTempAllocator, gDeltaTimePhysics);
    }

    player_->UpdateObjectDragging();
    player_->UpdateCoverState();
    player_->HandleCoverMovement(glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS);
    player_->UpdateView();
    player_->GetBody()->PrePhysicsUpdate(mPhysicsSystem, mTempAllocator, gDeltaTimePhysics);

    /// physics update
    mPhysicsSystem->Update(gDeltaTimePhysics, 1, mTempAllocator, mJobSystem);
    const JPH::BodyLockInterface& bli = mPhysicsSystem->GetBodyLockInterface();
    mdl_loader_.GetScene()->UpdateRenderTransform(
      bli, mdl_loader_.GetScene()->scene_data_.tiles);


    /// post physics update
    for (auto& c : characters_) {
      c->GetBody()->PostPhysicsUpdate(mPhysicsSystem->GetGravity(), gDeltaTimePhysics);
    }
    player_->GetBody()->PostPhysicsUpdate(mPhysicsSystem->GetGravity(), gDeltaTimePhysics);

    animator_.Update(disable_animator);
    if (!disable_animator) {
      disable_animator = true;
    }

    camera_.Update(player_->GetBody()->GetCameraBoneMatrix());
    terrain_renderer_.Update(&camera_);

    bool first_face_mode = camera_.IsFirstFaceMode();

    world_manager_.UpdatePlayerZone(player_->GetBody()->GetPosition(), &bi);
    world_manager_.Cull();
    if (render_physics_only_) {
      renderer_.RenderDebug();
    } else {
      renderer_.DrawShadowPass();
      renderer_.DrawGeometryPass();
      renderer_.DrawLightPass(terrain_renderer_.GetRenderData(), cubemap_.GetRenderData());
    }
    RenderInterface();

    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glfwPollEvents();
    glfwSwapBuffers(gWindow);
  }
}

void Game::UpdateDeltaTime() {
  auto current_frame = static_cast<float>(glfwGetTime());
  gDeltaTime = current_frame - last_frame;
  last_frame = current_frame;
  gDeltaTimePhysics = std::clamp(gDeltaTime, 0.001f, 1.0f / 30.0f);
}

void Game::UpdateFPS(float delta_time) {
  frameCount_++;
  elapsedTime_ += delta_time;

  if (elapsedTime_ >= 1.0f) {
    fps_ = frameCount_ / elapsedTime_;
    frameCount_ = 0;
    elapsedTime_ -= 1.0f;
  }

  std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps_));
  renderer_.AddText(fpsText, glm::vec2(10.0f, 20.0f),
    glm::vec2(1.0f), glm::vec4(1.0f));
}

JPH::Ref<JPH::Shape> CreateMeshShape(const Scene::Mesh& mesh) {
  JPH::Ref<JPH::Shape> local_shape;
  if (mesh.collision_type == Scene::CollisionType::Sphere) {
    float radius = (mesh.max[0] - mesh.min[0]) * 0.5f;
    JPH::SphereShapeSettings sphere_settings(radius);
    // Safely create via Jolt reference counting
    local_shape = sphere_settings.Create().Get();
  } else {
    auto half_extend_glm = (mesh.max - mesh.min) / 2.0f;
    JPH::Vec3 half_extend(half_extend_glm.x, half_extend_glm.y, half_extend_glm.z);
    JPH::BoxShapeSettings box_settings(half_extend);
    local_shape = box_settings.Create().Get();
  }
  auto center_glm = (mesh.max + mesh.min) / 2.0f;
  JPH::Vec3 local_center(center_glm.x, center_glm.y, center_glm.z);
  if (!local_center.IsNearZero()) {
    JPH::RotatedTranslatedShapeSettings offset_settings(local_center, JPH::Quat::sIdentity(), local_shape);
    local_shape = offset_settings.Create().Get();
  }
  return local_shape;
}

void InitSceneGlobalTransforms(
    std::vector<Scene::Tile*>& tiles) {
  std::function<void(SceneNode*, const JPH::Mat44&)> dfs =
      [&](SceneNode* node, const JPH::Mat44& parent) {
        auto local = node->local_transform.Matrix();
        auto global_transform = parent * local;
        node->global_transform = global_transform;
        for (auto child : node->children) {
          dfs(child, global_transform);
        }
  };
  for (auto& tile : tiles) {
    for (auto node : tile->portals) {
      dfs(node.render, JPH::Mat44::sIdentity());
      dfs(node.portal, JPH::Mat44::sIdentity());
    }
    for (auto node : tile->characters) {
      dfs(node, JPH::Mat44::sIdentity());
    }
    for (auto& zone : tile->zones) {
      for (auto node : zone->object_nodes) {
        dfs(node, JPH::Mat44::sIdentity());
      }
    }
  }
}

/* int GetShapeDataId(
  const Game::ShapeToGeometryMap& shapeToGeometry,
  const JPH::Shape* shape, JPH::Vec3& out_scale) {
  while (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass ||
           shape->GetSubType() == JPH::EShapeSubType::RotatedTranslated) {
    if (shape->GetSubType() == JPH::EShapeSubType::OffsetCenterOfMass) {
      shape = static_cast<const JPH::OffsetCenterOfMassShape*>(shape)->GetInnerShape();
    } else {
      shape = static_cast<const JPH::RotatedTranslatedShape*>(shape)->GetInnerShape();
    }
           }
  out_scale = JPH::Vec3::sReplicate(1.0f);
  if (shape->GetSubType() == JPH::EShapeSubType::Box) {
    out_scale = static_cast<const JPH::BoxShape*>(shape)->GetHalfExtent();
  } else if (shape->GetSubType() == JPH::EShapeSubType::Sphere) {
    out_scale = JPH::Vec3::sReplicate(static_cast<const JPH::SphereShape*>(shape)->GetRadius());
  }
  return shapeToGeometry.at(shape->GetSubType());
} */

//TODO: not used yet
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

// zone might be as well nullptr in case of character TODO: bear it out
void Game::CreateBodyForNode(SceneNode* node, Scene::Zone* zone) {
  //TODO: should we do smt with it?... probably there shouldn't be those
  if (node->mesh_index == -1) return;
  const auto scene = mdl_loader_.GetScene();
    const auto& mesh = scene->scene_data_.meshes[node->mesh_index];
    JPH::Ref<JPH::Shape> local_shape = CreateMeshShape(mesh);
    JPH::EMotionType motion_type = JPH::EMotionType::Static;
    JPH::ObjectLayer object_layer = Layers::NON_MOVING;
    JPH::Ref<JPH::Shape> shape_settings = local_shape;
    JPH::EActivation activation_state = JPH::EActivation::DontActivate;
    if (mesh.type == Scene::Type::Dynamic) {
      motion_type = JPH::EMotionType::Dynamic;
      object_layer = Layers::MOVING;
      node->can_be_activated = true;
      //activation_state = JPH::EActivation::Activate;
      zone->static_objects_.emplace_back(node);
    } else if (mesh.type == Scene::Type::Character) {
      // return;
      characters_.push_back(std::make_unique<EnemyController>(
        character_shared_data_.get(), node,
        &scene->character_skins_[0], &scene->character_rig_));
      weapons_.push_back(std::make_unique<Weapon>(
        &scene->weapons_[0], mPhysicsSystem, &animator_));
      characters_.back()->GetBody()->EquipWeapon(weapons_.back().get());
      characters_.back()->GetBody()->SetPositionRotation(
        node->global_transform.GetTranslation(),
        node->global_transform.GetRotation().GetQuaternion().Normalized());
    } else {
      // no point light (it's not a ModelNode)
      if (mesh.type == Scene::Type::PointLight) {
        zone->point_lights_.emplace_back(node);
      } else if (mesh.type == Scene::Type::None) {
        zone->static_objects_.push_back(node);
        //TODO: non-physics bodies
        return;
      } else {
        // no difference for hinge now, it just static and have constraint later
        zone->static_objects_.push_back(node);
      }
    }
    // no scale component, so safe
    JPH::Vec3 translation = node->global_transform.GetTranslation();
    JPH::Quat rotation = node->global_transform.GetRotation().GetQuaternion().Normalized();
    JPH::BodyCreationSettings obj_settings(
          shape_settings,
          translation,
          rotation,
          motion_type,
          object_layer);
    obj_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
    obj_settings.mMassPropertiesOverride.mMass = 10.0f;
    node->body_id = mBodyInterface->CreateAndAddBody(obj_settings, activation_state);
    node->shape = local_shape;
}

void Game::Init() {
  glfwSetWindowUserPointer(gWindow, reinterpret_cast<void *>(this));

  // global_data_.camera = &camera_;
  // global_data_.tile_renderer = &tile_renderer_;
  // global_data_.picking_fbo = &picking_fbo_;
  // ui_renderer_.SetupGlobalData();

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glClearColor(0.2f, 0.7f, 0.1f, 1.0f);

  camera_.HideCursor();

  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetCursorPosCallback(gWindow, CursorPosCallback);

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

  /// CharacterBaseTest

  mdl_loader_.LoadDebugShapes("C:\\Users\\Pavlo\\Desktop\\assets\\DebugShapes.gltf");
  mdl_loader_.LoadScene( "C:\\Users\\Pavlo\\Desktop\\assets\\SceneBackyard.gltf");
  mdl_loader_.LoadCharacters(
        "C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf",
        {"C:\\Users\\Pavlo\\Desktop\\assets\\Human.gltf"});
  //TODO: create CharacterSkeleton.gltf
  // mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\nagan.gltf"});
  mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\MauserC96.gltf"});
  // mdl_loader_.LoadWeapon({"C:\\Users\\Pavlo\\Desktop\\assets\\Nagant.gltf"});
  const auto scene = mdl_loader_.GetScene();

  character_shared_data_ = std::make_unique<CharacterSharedData>(
    mPhysicsSystem, mTempAllocator, this, &animator_, &characters_);

  const JPH::BodyLockInterface& bli = mPhysicsSystem->GetBodyLockInterface();


  JPH::BodyInterface& bi = mPhysicsSystem->GetBodyInterface();

  InitSceneGlobalTransforms(scene->scene_data_.tiles);
  mdl_loader_.GetScene()->UpdateRenderTransform(
    bli, mdl_loader_.GetScene()->scene_data_.tiles);
  mdl_loader_.GetScene()->ConnectZonesWithPortals(0);

  for (auto& tile : scene->scene_data_.tiles) {
    terrain_renderer_.InitializeBody(mBodyInterface);
    std::cout << "tile added" << std::endl;
    for (auto node : tile->portals) {
      //TODO: portals.. idk
      //CreateBodyForNode(node.render);
    }
    for (auto node : tile->characters) {
      CreateBodyForNode(node, nullptr);
    }
    for (Scene::Zone* zone : tile->zones) {
      // world_manager_.AddZone(node);
      std::cout << "zone added" << std::endl;
      for (auto node : zone->object_nodes) {
        CreateBodyForNode(node, zone);
      }
      //break; // only first zone for now
    }
    //break; // only one tile for now
  }
  auto player_node = scene->scene_data_.player_node;
  player_ = std::make_unique<PlayerController>(
    &camera_, character_shared_data_.get(), player_node,
    &scene->character_skins_[0], &scene->character_rig_);
  weapons_.push_back(std::make_unique<Weapon>(
    &scene->weapons_[0], mPhysicsSystem, &animator_));
  player_->GetBody()->EquipWeapon(weapons_.back().get());

  world_manager_.UpdatePlayerZone(player_->GetBody()->GetPosition(), &bi);
}

void Game::DeInit() {
  // mCharacter->RemoveFromPhysicsSystem();
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
  game->camera_.ZoomOriginDist(yoffset);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
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

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, true);
    } else if (key == GLFW_KEY_F1) {
      game->camera_.SwitchFaceMode();
    } else if (key == GLFW_KEY_F2) {
      game->render_physics_only_ = !game->render_physics_only_;
    } else if (key == GLFW_KEY_E) {
      game->player_->StartObjectDragging();
    }
  } else if (action == GLFW_RELEASE && key == GLFW_KEY_E) {
    game->player_->ReleaseObjectDragging(1.0f);
  }
  game->player_->ProcessMovement(key, action);
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  // std::cout << xpos << ' ' << ypos << std::endl;

  lastX = xpos;
  lastY = ypos;

  auto game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));

  game->camera_.MoveRotateView(xoffset, yoffset);
  // game->camera_.MoveRotateViewOrigin(xoffset, yoffset);
}

void Game::OnContactAdded(const JPH::Body &inBody1,
                                       const JPH::Body &inBody2,
                                       const JPH::ContactManifold &inManifold,
                                       JPH::ContactSettings &ioSettings) {
  // Draw a box around the character when it enters the sensor
  // if (inBody1.GetID() == mSensorBody)
  //   mDebugRenderer->DrawBox(inBody2.GetWorldSpaceBounds(), JPH::Color::sGreen,
  //                           JPH::DebugRenderer::ECastShadow::Off,
  //                           JPH::DebugRenderer::EDrawMode::Wireframe);
  // else if (inBody2.GetID() == mSensorBody)
  //   mDebugRenderer->DrawBox(inBody1.GetWorldSpaceBounds(), JPH::Color::sGreen,
  //                           JPH::DebugRenderer::ECastShadow::Off,
  //                           JPH::DebugRenderer::EDrawMode::Wireframe);
}

void Game::OnContactPersisted(
    const JPH::Body &inBody1, const JPH::Body &inBody2,
    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) {
  // Same behavior as contact added
  OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void Game::OnAdjustBodyVelocity(
    const JPH::CharacterVirtual *inCharacter, const JPH::Body &inBody2,
    JPH::Vec3 &ioLinearVelocity, JPH::Vec3 &ioAngularVelocity) {
  // Apply artificial velocity to the character when standing on the conveyor
  // belt
  // if (inBody2.GetID() == mConveyorBeltBody) ioLinearVelocity += JPH::Vec3(0, 0, 2);
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
  //   // mDebugRenderer->DrawBox(box, JPH::Color::sGreen, JPH::DebugRenderer::ECastShadow::Off,
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
  if (inCharacter == player_->GetBody()->GetJphCharacter() && ioSettings.mCanPushCharacter &&
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

void Game::OnContactPersisted(
    const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2,
    const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition,
    JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
  OnContactCommon(inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
                  inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact persisted with body %08x, sub shape %08x",
          inBodyID2.GetIndexAndSequenceNumber(), inSubShapeID2.GetValue());
#endif
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(),
                  JPH::CharacterVirtual::ContactKey(inBodyID2, inSubShapeID2)) ==
        mActiveContacts.end())
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
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
  // Characters can only be pushed in their own update
  ioSettings.mCanPushCharacter = inOtherCharacter == player_->GetBody()->GetJphCharacter();
  // If the player can be pushed by the other virtual character, we allow
  // sliding
  if (inCharacter == player_->GetBody()->GetJphCharacter() && ioSettings.mCanPushCharacter)
    player_->GetBody()->AllowSliding(true);
}

void Game::OnCharacterContactAdded(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
  OnCharacterContactCommon(inCharacter, inOtherCharacter, inSubShapeID2,
                           inContactPosition, inContactNormal, ioSettings);

  if (inCharacter == player_->GetBody()->GetJphCharacter()) {
#ifdef CHARACTER_TRACE_CONTACTS
    Trace("Contact added with character %08x, sub shape %08x",
          inOtherCharacter->GetID().GetValue(), inSubShapeID2.GetValue());
#endif
    JPH::CharacterVirtual::ContactKey c(inOtherCharacter->GetID(), inSubShapeID2);
    if (std::find(mActiveContacts.begin(), mActiveContacts.end(), c) !=
        mActiveContacts.end())
      throw std::runtime_error(
          "Got an add contact that should have been a persisted contact");
    mActiveContacts.push_back(c);
  }
}

void Game::OnCharacterContactPersisted(
    const JPH::CharacterVirtual *inCharacter,
    const JPH::CharacterVirtual *inOtherCharacter, const JPH::SubShapeID &inSubShapeID2,
    JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
    JPH::CharacterContactSettings &ioSettings) {
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

void Game::OnCharacterContactRemoved(
    const JPH::CharacterVirtual *inCharacter, const JPH::CharacterID &inOtherCharacterID,
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
    const JPH::PhysicsMaterial *inContactMaterial, JPH::Vec3Arg inCharacterVelocity,
    JPH::Vec3 &ioNewCharacterVelocity) {
  // Ignore callbacks for other characters than the player
  if (inCharacter != player_->GetBody()->GetJphCharacter()) return;

  // Don't allow the player to slide down static not-too-steep surfaces when not
  // actively moving and when not on a moving platform
  if (!player_->GetBody()->GetAllowSliding() && inContactVelocity.IsNearZero() &&
      !inCharacter->IsSlopeTooSteep(inContactNormal))
    ioNewCharacterVelocity = JPH::Vec3::sZero();
}
