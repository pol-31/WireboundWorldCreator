#include "Renderer.h"

#include <fstream>
#include <iostream>

#include "../common/models/DirectedLight.h"
#include "../common/models/EnemyController.h"
#include "../common/models/PlayerController.h"
#include "../common/models/PointLight.h"
#include "../common/models/Scene.h"
#include "../common/models/StaticObject.h"
#include "../common/models/Weapon.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//TODO: there's thing in lightning/shadows when obj appears/disappears
//      and some thrash shadows appears and disappears... so probably
//      due to huge ssbo instance buffer modification

// const GLuint Renderer::cShadowMapSize = 256;
const int Renderer::cMaxInstances = 10000;
const int Renderer::cMaxInstancesRigged = 10;
const int Renderer::cMaxLines = 100 * 3;

const int Renderer::cMaxDirectedLights = 4;
const GLuint Renderer::cShadowCubeMapSize = 256;
const int Renderer::cMaxPointLights = 32;
const GLuint Renderer::cShadowMapSize = 256;

GLuint CreateShadowMap() {
  GLuint depth_map = 0;
  glGenTextures(1, &depth_map);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    Renderer::cShadowMapSize, Renderer::cShadowMapSize, 0,
    GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  return depth_map;
}

GLuint CreateShadowCubeMapArray() {
  uint32_t max_lights = 1; // TODO; careful
  GLuint depth_cubemap_array = 0;
  glGenTextures(1, &depth_cubemap_array);
  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depth_cubemap_array);

  // Allocate storage for max_lights * 6 cubemap faces
  glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24,
                 Renderer::cShadowMapSize, Renderer::cShadowMapSize,
                 max_lights * 6);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
  return depth_cubemap_array;
}

void BindMaterial(const Material& material) {
  glActiveTexture(GL_TEXTURE0);
  material.albedo.BindSampler(0);
  glActiveTexture(GL_TEXTURE0);
  material.rough_metal_ao.BindSampler(1);
  glActiveTexture(GL_TEXTURE0);
  material.normal.BindSampler(2);
}

void BindAnimatedRenderData(const AnimatedRenderData& data) {
  glUniformMatrix4fv(2, 1, false,
    reinterpret_cast<const float*>(&data.transform));
  glUniform4fv(3, 1, JPH::Vec4::sOne().mF32);
  glUniform1i(4, data.bones_offset);
  glBindVertexArray(data.vao);
  BindMaterial(*data.material);
}

void RenderAnimatedPrimitive(const Scene::Primitive& p) {
  glDrawElementsBaseVertex(
          GL_TRIANGLES,
          p.index_count,
          p.index_type,
          reinterpret_cast<void*>(p.index_byte_offset),
          static_cast<GLint>(p.base_vertex)
      );
}

void RenderAnimated(const AnimatedRenderData& data) {
  BindAnimatedRenderData(data);
  for (int i = 0; i < data.meshes->size(); ++i) {
    const Scene::Mesh& m = (*data.meshes)[i];
    for (const auto& p : m.primitives) {
      RenderAnimatedPrimitive(p);
    }
  }
}

// not used: sh_shadow_dir_apply_
// not used: sh_shadow_point_apply_

// geometry into shadowmaps (more obj that geometry pass - outside the view)
// Shadow Pass (before / parallel geometry) : sh_shadow_dir_
// Shadow Pass : sh_shadow_point_
// Light Pass (final) : sh_deferred_shading_
// Light Pass light emitters : sh_light_emitter_


// SO to capture shadows we capture it with default params, while
// apply it with params only in Light pass, so we should discard our
// custom params from Shadow pass and bear them out to Light pass

Renderer::Renderer(const Scene* scene,
  const Camera* camera,
  const std::unique_ptr<PlayerController>* player,
  const WorldManager::ZoneCulledData* culled_data)
: scene_(scene),
camera_(camera),
culled_data_(culled_data),
player_(player),

sh_cubemap_("../shaders/Skybox.vert", "../shaders/Skybox.frag", {0}),
sh_terrain_(
      "../shaders/Terrain.vert",
      "../shaders/Terrain.tesc",
      "../shaders/Terrain.tese",
      "../shaders/Terrain.frag",
{0, 1, 2, 3}),
sh_lines_("../shaders/Line.vert", "../shaders/Line.frag", {}),
// sh_shadow_dir_apply_("../shaders/TriangleNormalDir.vert",
//                           "../shaders/TriangleNormalDir.frag", {1}),
// sh_shadow_point_apply_("../shaders/TriangleNormalPoint.vert",
//                             "../shaders/TriangleNormalPoint.frag", {1}),
sh_shadow_dir_("../shaders/TriangleShadowDir.vert",
                        "../shaders/TriangleShadowDir.frag", {}),
sh_shadow_point_("../shaders/TriangleShadowPoint.vert",
                          "../shaders/TriangleShadowPoint.frag", {}),
sh_shadow_point5_("../shaders/TriangleShadowPoint5.vert",
                          "../shaders/TriangleShadowPoint.frag", {}),
sh_geometry_("../shaders/TriangleGeometry.vert",
                      "../shaders/TriangleGeometry.frag", {0, 1, 2}),
sh_geometry5_("../shaders/TriangleGeometry5.vert",
                      "../shaders/TriangleGeometry5.frag", {0, 1}),
sh_deferred_shading_("../shaders/DeferredShading.vert",
                              "../shaders/DeferredShading.frag", {0, 1, 2}),
sh_bloom_("../shaders/DeferredShading.vert", "../shaders/Bloom.frag", {0}),
sh_gauss_("../shaders/DeferredShading.vert", "../shaders/Gauss.frag", {0}),
sh_composite_("../shaders/DeferredShading.vert", "../shaders/FinalComposite.frag", {0, 1}),
sh_light_emitter_("../shaders/LightEmitter.vert",
                           "../shaders/LightEmitter.frag", {}),
sh_wall_("../shaders/Mazanka.vert", "../shaders/Mazanka.frag", {}) {
  Init();
}

//TODO: shadow maps / cubemaps.. we can't just create 10, or at scene init;
// we need to act smarter.. chaches and so on
// we need to act smarter.. chaches and so on
// we need to act smarter.. chaches and so on
// we need to act smarter.. chaches and so on

// WE CAN'T CONNECT OPENGL_ID TO A LIGHT SOURCE!!!

void Renderer::DrawDirectionalLightShadowPass() {
  int num_to_add = culled_data_->dir_lights.size() - shadow_maps_.size();
  for (int i = 0; i < num_to_add; ++i) {
    shadow_maps_.push_back(CreateShadowMap());
  }
  if (culled_data_->dir_lights.empty()) {
    return;
  }
  glViewport(0, 0, cShadowMapSize, cShadowMapSize);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map_);
  glClear(GL_DEPTH_BUFFER_BIT);
  sh_shadow_dir_.DebugUpdate();
  sh_shadow_dir_.Bind();
  glBindVertexArray(scene_->scene_data_.vao);
  for (int i = 0; i < culled_data_->dir_lights.size(); ++i) {
    const auto& l = culled_data_->dir_lights[i];
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_2D,shadow_maps_[i], 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(l.source->lightSpaceMatrix));
    for (const auto& o : l.objects) {
      const auto& m = scene_->scene_data_.meshes[o.mesh_id];
      for (const auto& p : m.primitives) {
        glDrawElementsInstancedBaseVertexBaseInstance(
            GL_TRIANGLES,
            p.index_count,
            p.index_type,
            (void*)p.index_byte_offset,
            o.instances_num,
            p.base_vertex,
            o.instance_offset
        );
      }
    }
  }
}

struct FaceTarget {
  glm::vec3 dir;
  glm::vec3 up;
};

// Modern OpenGL cubemap face sampling order (+X, -X, +Y, -Y, +Z, -Z)
static const FaceTarget kCubeFaces[6] = {
  { glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // +X
  { glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // -X
  { glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f) }, // +Y
  { glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f) }, // -Y
  { glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f) }, // +Z
  { glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f) }  // -Z
};

void GetLightFaceViewProj(const WorldManager::FinalPointLight& l, int face) {
  auto jph_pos = l.source->object_->global_bounds.GetCenter();
  auto radius = l.source->radius_;
  auto pos = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
  float aspect = 1.0f;
  float near_plane = 0.1f;
  glm::mat4 proj =
    glm::perspective(glm::radians(90.0f), aspect, near_plane, radius);
  glm::mat4 view = glm::lookAt(pos, pos + kCubeFaces[face].dir, kCubeFaces[face].up);
  auto viev_proj = proj * view;
  glUniformMatrix4fv(7, 1, GL_FALSE, glm::value_ptr(viev_proj));
  glUniform3fv(5, 1, glm::value_ptr(pos));
  glUniform1f(6, radius);
}

void Renderer::DrawPointLightShadowPass() {
  int num_to_add = culled_data_->point_lights.size() - shadow_cubemaps_.size();
  for (int i = 0; i < num_to_add; ++i) {
    shadow_cubemaps_.push_back(CreateShadowCubeMapArray());
  }
  if (culled_data_->point_lights.empty()) {
    return;
  }
  glViewport(0, 0, cShadowCubeMapSize, cShadowCubeMapSize);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_cubemap_);
  sh_shadow_point_.DebugUpdate();
  sh_shadow_point_.Bind();
  glBindVertexArray(scene_->scene_data_.vao);
  for (int i = 0; i < culled_data_->point_lights.size(); ++i) {
    const auto& l = culled_data_->point_lights[i];
    for (int face = 0; face < 6; ++face) {
      GLint layer = static_cast<GLint>(0 * 6 + face); // layer = 0; num_light...
      glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               shadow_cubemaps_[i], 0, layer);
      // need to clear it first
      glClear(GL_DEPTH_BUFFER_BIT);
      if (l.objects[face].empty()) {
        continue;
      }
      GetLightFaceViewProj(l, face);

      for (const auto& o : l.objects[face]) {
        const auto& m = scene_->scene_data_.meshes[o.mesh_id];
        for (const auto& p : m.primitives) {
          glDrawElementsInstancedBaseVertexBaseInstance(
              GL_TRIANGLES,
              p.index_count,
              p.index_type,
              (void*)p.index_byte_offset,
              o.instances_num,
              p.base_vertex,
              o.instance_offset
          );
        }
      }
    }
  }
  sh_shadow_point5_.DebugUpdate();
  sh_shadow_point5_.Bind();
  for (int i = 0; i < culled_data_->point_lights.size(); ++i) {
    const auto& l = culled_data_->point_lights[i];
    for (int face = 0; face < 6; ++face) {
      if (l.object_animated[face].empty()) {
        continue;
      }
      GetLightFaceViewProj(l, face);
      GLint layer = static_cast<GLint>(0 * 6 + face); // layer = 0; num_light...
      glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               shadow_cubemaps_[i], 0, layer);
      auto render_data = (*player_)->GetBody()->GetAnimatedRenderData();
      RenderAnimated(render_data);
      for (const auto& data : l.object_animated[face]) {
        RenderAnimated(data);
      }
    }
  }
}

void Renderer::DrawShadowPass() {
  glCullFace(GL_FRONT);
  DrawDirectionalLightShadowPass();
  DrawPointLightShadowPass();
}

void RenderPlayer(const AnimatedRenderData& data) {
  BindAnimatedRenderData(data);
  for (int i = 0; i < data.meshes->size(); ++i) {
    const Scene::Mesh& m = (*data.meshes)[i];
    if (i == 2) {
      continue; // for character only;
    }
    for (const auto& p : m.primitives) {
      RenderAnimatedPrimitive(p);
    }
  }
}

void Renderer::DrawGeometryPass(TerrainRenderData terrain) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_);
  if (!culled_data_->ssbo_data.empty()) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
    culled_data_->ssbo_data.size() * sizeof(WorldManager::InstanceGpu),
    culled_data_->ssbo_data.data());
  }

  glBindVertexArray(scene_->scene_data_.vao);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
  sh_geometry_.Bind();

  glBindTextureUnit(0, scene_->materials.albedo);
  glBindTextureUnit(1, scene_->materials.normal);
  glBindTextureUnit(2, scene_->materials.rough_metal_ao);

  for (const auto& o : culled_data_->camera.objects) {
    const auto& m = scene_->scene_data_.meshes[o.mesh_id];
    for (const auto& p : m.primitives) {
      glDrawElementsInstancedBaseVertexBaseInstance(
          GL_TRIANGLES,
          p.index_count,
          p.index_type,
          (void*)p.index_byte_offset,
          o.instances_num,
          p.base_vertex,
          o.instance_offset
      );
    }
  }

  //TODO: to cast shadows we should also render them in ShadowPass...
  //TODO: to cast shadows we should also render them in ShadowPass...
  //TODO: to cast shadows we should also render them in ShadowPass...

  // we need the shader.. probably we have both files

  RenderWalls();
  RenderTerrain(terrain);
  sh_geometry5_.Bind();
  auto render_data = (*player_)->GetBody()->GetAnimatedRenderData();
  RenderPlayer(render_data);
  for (const auto& data : culled_data_->camera.object_animated) {
    RenderAnimated(data);
  }
}

int GetDbgShapeId(
  const std::vector<Scene::Mesh>& dbg_meshes, Scene::CollisionType type) {
  //TODO: we probably can do this just once
  for (int i = 0; i < dbg_meshes.size(); i++) {
    if (dbg_meshes[i].collision_type == type) {
      return i;
    }
  }
  return 0;
}

void Renderer::RenderDebug() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_);
  if (!culled_data_->ssbo_data.empty()) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
    culled_data_->ssbo_data.size() * sizeof(WorldManager::InstanceGpu),
    culled_data_->ssbo_data.data());
  }

  glBindVertexArray(scene_->scene_dbg_shapes_.vao);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);

  sh_geometry_.Bind(); //TODO: probably different shader

  glBindTextureUnit(0, scene_->materials.albedo);
  glBindTextureUnit(1, scene_->materials.normal);
  glBindTextureUnit(2, scene_->materials.rough_metal_ao);

  for (const auto& o : culled_data_->camera.objects) {
    const auto& m = scene_->scene_data_.meshes[o.mesh_id];
    int dbg_shape_id = GetDbgShapeId(
      scene_->scene_dbg_shapes_.meshes, m.collision_type);
    const auto& m_dbg = scene_->scene_dbg_shapes_.meshes[dbg_shape_id];
    for (const auto& p : m_dbg.primitives) {
      glDrawElementsInstancedBaseVertexBaseInstance(
          GL_TRIANGLES,
          p.index_count,
          p.index_type,
          (void*)p.index_byte_offset,
          o.instances_num,
          p.base_vertex,
          o.instance_offset
      );
    }
  }
  // auto cam_frustum = camera_->GetFrustum();
  // for (const auto& w : culled_data_->camera.weapons) {
  //   (*w)->Render(cam_frustum); // non-animated, rendered only if no owner_
  // }
  //
  // sh_geometry5_.Bind();
  // for (const auto& c : culled_data_->camera.characters) {
  //   (*c)->GetBody()->RenderWithWeapon(cam_frustum); // both animated
  // }
}

void Renderer::RenderWalls() {
  sh_wall_.DebugUpdate();
  sh_wall_.Bind();

  glDisable(GL_CULL_FACE);
  glBindVertexArray(vao_wall_);

  std::vector<JPH::AABox> wall_bounds;
  for (int zone_id : culled_data_->active_zones) {
    const auto* zone = scene_->scene_data_.tiles[0]->zones[zone_id];
    wall_bounds.push_back(zone->bounds);
  }

  for (const JPH::AABox& aabb : wall_bounds) {
    JPH::Vec3 jph_center = (aabb.mMax + aabb.mMin) * 0.5f;
    JPH::Vec3 jph_size = aabb.mMax - aabb.mMin;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(jph_center.GetX(), jph_center.GetY(), jph_center.GetZ()));
    model = glm::scale(model, glm::vec3(jph_size.GetX(), jph_size.GetY(), jph_size.GetZ()));

    //glUniformMatrix4fv(0, 1, false, reinterpret_cast<const float*>(model));
    glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
    //JPH::Vec3 light_dir = JPH::Vec3(0.2f, 0.4f, 0.5f).Normalized();
    //glUniform3fv(1, 1, light_dir.mF32);
    glDrawArrays(GL_TRIANGLES, 0, 24);
  }
  glEnable(GL_CULL_FACE);

  glBindVertexArray(0);
}

void Renderer::RenderTerrain(TerrainRenderData terrain) {
  sh_terrain_.DebugUpdate();
  if (glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glEnable(GL_CULL_FACE);
  sh_terrain_.Bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrain.hmap_id);
  glBindTextureUnit(1, scene_->materials.albedo);
  glBindTextureUnit(2, scene_->materials.normal);
  glBindTextureUnit(3, scene_->materials.rough_metal_ao);
  glUniform1i(4, terrain.material_id);

  glBindVertexArray(terrain.vao);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, terrain.patch_num);

  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: ???wtf comment
// we've set GL_LEQUAL by default for the whole app
void Renderer::RenderCubemap(CubemapRenderData cubemap) {
  sh_cubemap_.Bind();
  glBindVertexArray(cubemap.vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.texture_id);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void Renderer::DrawLightPass(CubemapRenderData cubemap) {
  sh_deferred_shading_.DebugUpdate();
  sh_light_emitter_.DebugUpdate();
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_hdr_scene_);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sh_deferred_shading_.Bind();
  glBindVertexArray(vao_ui_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_position_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_normal_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_albedo_spec_);

  // --- Deferred Render Pass Execution ---
  sh_deferred_shading_.Bind();

  const uint32_t num_lights = static_cast<uint32_t>(culled_data_->point_lights.size());
  glUniform1i(3, num_lights);

  if (num_lights > 0) {
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    std::vector<float>     farPlanes;

    lightPositions.reserve(num_lights);
    lightColors.reserve(num_lights);
    farPlanes.reserve(num_lights);

    for (int i = 0; i < culled_data_->point_lights.size(); ++i) {
      const auto& light = culled_data_->point_lights[i];
      glActiveTexture(GL_TEXTURE3 + i);
      glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadow_cubemaps_[i]);
      auto jph_pos = light.source->object_->global_bounds.GetCenter();

      auto pos = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
      glUniform3fv(4 + i * 2, 1, glm::value_ptr(pos));
      glUniform3fv(4 + i * 2 + 1, 1, glm::value_ptr(glm::vec3(1.0f)));

      //TODO:
      std::string samplerName = "shadowMaps[" + std::to_string(i) + "]";
      glUniform1i(glGetUniformLocation(sh_deferred_shading_.GetId(),
        samplerName.c_str()), 3 + i);
      std::string farPlaneName = "farPlanes[" + std::to_string(i) + "]";
      glUniform1f(glGetUniformLocation(sh_deferred_shading_.GetId(),
        farPlaneName.c_str()), culled_data_->point_lights[i].source->radius_);
    }
  }
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


  // blit DEPTH to bloom fbo
  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_hdr_scene_);
  glBlitFramebuffer(0, 0, gWindowWidth, gWindowHeight, 0, 0, gWindowWidth,
                    gWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_hdr_scene_);
  glEnable(GL_DEPTH_TEST);

  glDepthFunc(GL_LEQUAL);
  RenderCubemap(cubemap);
  glDepthFunc(GL_LESS);

  /// light sources (point light)
  glBindVertexArray(scene_->scene_data_.vao);
  sh_light_emitter_.Bind();
  for (const auto& l : culled_data_->point_lights) {
    const auto& m = scene_->scene_data_.meshes[l.source->object_->mesh_index];
    glUniformMatrix4fv(0, 1, GL_FALSE,
      reinterpret_cast<const float*>(&l.source->object_->global_transform));
    // JPH::Vec4 colorVec = l.object_->color.ToVec4();
    JPH::Vec4 colorVec = JPH::Vec4::sOne();
    glUniform4fv(1, 1, &colorVec.mF32[0]);
    for (const auto& p : m.primitives) {
      glDrawElementsBaseVertex(
            GL_TRIANGLES,
            p.index_count,
            p.index_type,
            (void*)p.index_byte_offset,
            p.base_vertex
        );
    }
  }
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_bloom_);
  glClear(GL_COLOR_BUFFER_BIT);
  sh_bloom_.Bind();
  glBindVertexArray(vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_hdr_scene_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  bool horizontal = true;
  bool first_iteration = true;
  int amount = 0;
  sh_gauss_.Bind();
  for (unsigned int i = 0; i < amount; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_ping_pong_[horizontal]);
    glUniform1i(1, horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,
      first_iteration ? bloom_tex_ : buffer_ping_pong_[!horizontal]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sh_composite_.Bind();
  glBindVertexArray(vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_hdr_scene_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, buffer_ping_pong_[!horizontal]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  text_renderer_.Render();
  glEnable(GL_DEPTH_TEST);

  /// lines
  if (!mLines.empty()) {
    sh_lines_.Bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo_lines_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mLines.size(), mLines.data());
    glBindVertexArray(vao_lines_);
    glDrawArrays(GL_LINES, 0, mLines.size());
  }

  Clear();
}

void Renderer::AddText(std::string_view text,
  glm::vec2 position, glm::vec2 scale, glm::vec4 color) {
  text_renderer_.AddText(text, position, scale, color);
}

void Renderer::AddSprite(const std::string& name,
  glm::vec2 position, glm::vec2 scale, glm::vec4 color) {
  text_renderer_.AddSprite(name, position, scale, color);
}

void Renderer::Clear() {
  mLines.clear();
}

void Renderer::InitializeProceduralWall() {
    // 36 vertices. Each row is: Position (X, Y, Z), Normal (NX, NY, NZ)
    // The cube is centered at 0,0,0 with bounds from -0.5 to 0.5
    float cube_vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &vao_wall_);
    glGenBuffers(1, &vbo_wall_);

    glBindVertexArray(vao_wall_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_wall_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);

    // Location 0: aPos (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Location 1: aNormal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void Renderer::Init() {
  /// ui
  float quadVertices[] = {
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  glGenVertexArrays(1, &vao_ui_);
  glGenBuffers(1, &vbo_ui_);
  glBindVertexArray(vao_ui_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));

  /// g-buffer
  glGenFramebuffers(1, &g_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);

  glGenTextures(1, &g_position_);
  glBindTexture(GL_TEXTURE_2D, g_position_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gWindowWidth, gWindowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         g_position_, 0);

  glGenTextures(1, &g_normal_);
  glBindTexture(GL_TEXTURE_2D, g_normal_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gWindowWidth, gWindowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         g_normal_, 0);

  glGenTextures(1, &g_albedo_spec_);
  glBindTexture(GL_TEXTURE_2D, g_albedo_spec_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gWindowWidth, gWindowHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         g_albedo_spec_, 0);

  unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                 GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);

  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWindowWidth,
                        gWindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// bloom / post-light effects
  glGenFramebuffers(1, &fbo_bloom_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_bloom_);

  glGenTextures(1, &bloom_tex_);
  glBindTexture(GL_TEXTURE_2D, bloom_tex_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gWindowWidth, gWindowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         bloom_tex_, 0);
  unsigned int bloom_attachment[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, bloom_attachment);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// bloom / post-light effects
  glGenFramebuffers(1, &fbo_hdr_scene_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_hdr_scene_);

  glGenTextures(1, &tex_hdr_scene_);
  glBindTexture(GL_TEXTURE_2D, tex_hdr_scene_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gWindowWidth, gWindowHeight, 0,
               GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex_hdr_scene_, 0);
  unsigned int hdr_attachment[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, hdr_attachment);
  unsigned int hdrDepth;
  glGenRenderbuffers(1, &hdrDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, hdrDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWindowWidth,
                        gWindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, hdrDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// gauss
  glGenFramebuffers(2, fbo_ping_pong_);
  glGenTextures(2, buffer_ping_pong_);
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_ping_pong_[i]);
    glBindTexture(GL_TEXTURE_2D, buffer_ping_pong_[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gWindowWidth,
      gWindowHeight, 0, GL_RGBA, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_ping_pong_[i], 0
    );
  }

  /// dir lights: shadowmap (sun)  glGenFramebuffers(1, &fbo_depth_map_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// point lights: shadow cubemap
  glGenFramebuffers(1, &fbo_depth_cubemap_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_cubemap_);
  // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemaps_[0], 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// instanced ssbo

  glGenBuffers(1, &ssbo_instanced_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_instanced_);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
    cMaxInstances * sizeof(WorldManager::InstanceGpu),
               nullptr, GL_DYNAMIC_DRAW);

  glGenVertexArrays(1, &vao_lines_);
  glBindVertexArray(vao_lines_);

  glGenBuffers(1, &vbo_lines_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_lines_);
  glBufferData(GL_ARRAY_BUFFER, cMaxLines * sizeof(Line) / 2, nullptr,
               GL_DYNAMIC_DRAW);

  auto stride = sizeof(JPH::Float3) + sizeof(JPH::Color);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, stride, GL_TRUE,
                        (void*)(sizeof(JPH::Float3)));
  glEnableVertexAttribArray(1);

  // ... (all your setup code) ...
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //
  InitializeProceduralWall();
}

void Renderer::DeInit() {
  glDeleteTextures(shadow_maps_.size(), shadow_maps_.data());
  glDeleteTextures(shadow_cubemaps_.size(), shadow_cubemaps_.data());
}

// void Renderer::DrawLine(RVec3Arg inFrom, RVec3Arg inTo,
//                                 ColorArg inColor) {
//   RVec3 offset = RVec3{0.0f, 0.0f, 0.0f};
//   Line line;
//   Vec3(inFrom - offset).StoreFloat3(&line.mFrom);
//   line.mFromColor = inColor;
//   Vec3(inTo - offset).StoreFloat3(&line.mTo);
//   line.mToColor = inColor;
//   mLines.push_back(line);
// }
//
// void Renderer::DrawTriangle(
//     RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor) {
//   RVec3 offset = RVec3(0.0f, 0.0f, 0.0f);
//
//   Vec3 v1(inV1 - offset);
//   Vec3 v2(inV2 - offset);
//   Vec3 v3(inV3 - offset);
//
//   // Set alpha to zero if we don't want to cast shadows to notify the pixel
//   // shader
//   Color color(inColor, 0xff);
//
//   // Construct triangle in separate buffer and then copy it to the target
//   memory
//   // block (may be uncached memory)
//   Triangle triangle(v1, v2, v3, color);
//   *(Triangle *)mLockedVertices = triangle;
//   mLockedVertices += 3;
//
//   // Update bounding box
//   mLockedPrimitiveBounds.Encapsulate(v1);
//   mLockedPrimitiveBounds.Encapsulate(v2);
//   mLockedPrimitiveBounds.Encapsulate(v3);
// }
