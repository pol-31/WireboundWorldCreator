#include "Renderer.h"

#include <iostream>
#include <fstream>

#include "../io/Window.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../common/models/Scene.h"

const GLuint Renderer::cShadowMapSize = 256;
const int Renderer::cMaxInstances = 10000;
const int Renderer::cMaxInstancesRigged = 10;
const int Renderer::cMaxLines = 100 * 3;

void Renderer::AddInstance(DirLight* dir_light, InstanceInfo info) {
  if (!dir_lights_.empty()) return;
  dir_lights_.push_back({dir_light, {}});
}

void Renderer::AddInstance(const PointLight* point_light, InstanceInfo info) {
  // if (point_lights_.size() > 8) return;
  point_lights_.push_back({point_light, info, {}});
}

void Renderer::AddCharacter(InstanceInfoRigged info) {
  characters_.push_back(info);
}

void Renderer::AddInstance(InstanceInfo info) {
  objects_.push_back(info);
}

Renderer::Renderer()
: sh_lines_("../shaders/Line.vert", "../shaders/Line.frag", {}),
sh_shadow_dir_apply_("../shaders/TriangleNormalDir.vert",
                          "../shaders/TriangleNormalDir.frag", {1}),
sh_shadow_point_apply_("../shaders/TriangleNormalPoint.vert",
                            "../shaders/TriangleNormalPoint.frag", {1}),
sh_shadow_dir_("../shaders/TriangleShadowDir.vert",
                        "../shaders/TriangleShadowDir.frag", {}),
sh_shadow_point_("../shaders/TriangleShadowPoint.vert",
                          "../shaders/TriangleShadowPoint.geom",
                          "../shaders/TriangleShadowPoint.frag", {}),
sh_geometry_("../shaders/TriangleGeometry.vert",
                      "../shaders/TriangleGeometry.frag", {0, 1}),
sh_geometry5_("../shaders/TriangleGeometry5.vert",
                      "../shaders/TriangleGeometry.frag", {0, 1}),
sh_deferred_shading_("../shaders/DeferredShading.vert",
                              "../shaders/DeferredShading.frag", {0, 1, 2}),
sh_bloom_("../shaders/DeferredShading.vert", "../shaders/Bloom.frag", {0}),
sh_gauss_("../shaders/DeferredShading.vert", "../shaders/Gauss.frag", {0}),
sh_composite_("../shaders/DeferredShading.vert", "../shaders/FinalComposite.frag", {0, 1}),
sh_light_emitter_("../shaders/LightEmitter.vert",
                           "../shaders/LightEmitter.frag", {}) {
  Init();
}

void Renderer::UpdateVboBuffer(Frustum frustum_camera) {
  // if (!scene_) {
    // return;
  // }
  SsboDataMeshMap instances_gpu_camera;

  // map per light source - mesh:vector<InstanceGpu>
  std::vector<SsboDataMeshMap> instances_gpu_dir_light(dir_lights_.size());
  std::vector<SsboDataMeshMap> instances_gpu_point_light(point_lights_.size());

  // |camera|dir_light1|dir_light2|dir_lightN|point_light1|point_light2|...
  for (int k = 0; k < objects_.size(); ++k) {
    const auto& src_instance = objects_[k];
    /// camera frustum
    if (frustum_camera.Overlaps(src_instance.bounds)) {
      instances_gpu_camera[src_instance.mesh_id].instances.push_back(
        InstanceGpu{src_instance.matrix, src_instance.color.ToVec4()});
    }

    /// dir light frustums
    for (int l = 0; l < dir_lights_.size(); ++l) {
      if (false) { // todo; overlaps
        instances_gpu_dir_light[l][src_instance.mesh_id].instances.push_back(
          InstanceGpu{src_instance.matrix, src_instance.color.ToVec4()});
      }
    }

    /// point light frustums
    for (int l = 0; l < point_lights_.size(); ++l) {
      if (l != 1 && l != 5 && l != 7 && l != 10) {
        continue;
      }
      auto jph_pos = point_lights_[l].info.bounds.GetCenter();
      if (!frustum_camera.Overlaps(JPH::AABox(jph_pos, point_lights_[l].source->radius_))) {
        continue;
      }
      if (src_instance.bounds.Overlaps(
            JPH::AABox(jph_pos, point_lights_[l].source->radius_)) &&
            frustum_camera.Overlaps(src_instance.bounds)) {
        instances_gpu_point_light[l][src_instance.mesh_id].instances.push_back(
          InstanceGpu{src_instance.matrix, src_instance.color.ToVec4()});
            }
    }
  }

  /// linealization - SsboOffsetData to SsboOffset (add offset:num)
  std::vector<InstanceGpu> ssbo_data;
  for (const auto& instance : instances_gpu_camera) {
    int mesh_id = instance.first;
    const auto& mesh_data = instance.second.instances;
    camera_data_.objects.push_back(SsboOffset{
      static_cast<int>(ssbo_data.size()),
      static_cast<int>(mesh_data.size()),
      mesh_id});
    ssbo_data.insert(ssbo_data.end(), mesh_data.begin(), mesh_data.end());
  }

  /// now per each light need SsboOffset (instances offset, num, mesh_id)
  for (int i = 0; i < instances_gpu_dir_light.size(); ++i) {
    for (const auto& instance : instances_gpu_dir_light[i]) {
      int mesh_id = instance.first;
      const auto& mesh_data = instance.second.instances;
      dir_lights_[i].objects.push_back(SsboOffset{
        static_cast<int>(ssbo_data.size()),
        static_cast<int>(mesh_data.size()),
        mesh_id});
      ssbo_data.insert(ssbo_data.end(), mesh_data.begin(), mesh_data.end());
    }
  }
  for (int i = 0; i < instances_gpu_point_light.size(); ++i) {
    for (const auto& instance : instances_gpu_point_light[i]) {
      int mesh_id = instance.first;
      const auto& mesh_data = instance.second.instances;
      point_lights_[i].objects.push_back(SsboOffset{
        static_cast<int>(ssbo_data.size()),
        static_cast<int>(mesh_data.size()),
        mesh_id});
      ssbo_data.insert(ssbo_data.end(), mesh_data.begin(), mesh_data.end());
    }
  }
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_);
  if (ssbo_data.size() > 0) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    ssbo_data.size() * sizeof(InstanceGpu),
                    ssbo_data.data());
  }
  std::vector<InstanceGpuRigged> ssbo_data_rigged;
  character_offset = SsboOffset{
    static_cast<int>(0),
    static_cast<int>(characters_.size()),
    0};
  for (const auto& c : characters_) {
    ssbo_data_rigged.push_back({c.matrix, c.color.ToVec4(),
      (uint32_t)c.bones_offset, 0, 0, 0});
  }
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_rigged_);
  if (ssbo_data_rigged.size() > 0) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    ssbo_data_rigged.size() * sizeof(InstanceGpuRigged),
                    ssbo_data_rigged.data());
  }
}

void Renderer::DrawDirectionalLightShadowPass() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map_);
  glClear(GL_DEPTH_BUFFER_BIT);
  sh_shadow_dir_.Bind();
  float near_plane = 1.0f, far_plane = 7.5f;
  glm::mat4 lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  for (int i = 0; i < dir_lights_.size(); ++i) {
    auto& l = dir_lights_[i];
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_maps_[i], 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 lightView =
        glm::lookAt(l.source->dir_, l.source->dir_ - glm::vec3(-2.0f, -4.0f, 1.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    l.source->lightSpaceMatrix_ = lightProjection * lightView;
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(l.source->lightSpaceMatrix_));
    for (const auto& o : l.objects) {
      const auto& p = scene_->meshes[o.rename__id];
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

void Renderer::DrawPointLightShadowPass() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_cubemap_);
  glClear(GL_DEPTH_BUFFER_BIT);
  sh_shadow_point_.Bind();
  float aspect = 1.0f;
  float near_plane = 1.0f;
  for (int i = 0; i < point_lights_.size(); ++i) {
    auto& l = point_lights_[i];  // ::not_visible optimization modified
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         depth_cubemaps_[i], 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 shadowProj =
        glm::perspective(glm::radians(90.0f), aspect, near_plane, l.source->far_plane_);
    auto jph_pos = l.info.bounds.GetCenter();
    auto pos = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0),
                                           glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0),
                                           glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0),
                                           glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0),
                                           glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0),
                                           glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0),
                                           glm::vec3(0.0, -1.0, 0.0)));
    glUniformMatrix4fv(2, shadowTransforms.size(), GL_FALSE,
                       glm::value_ptr(shadowTransforms[0]));
    glUniform3fv(0, 1, glm::value_ptr(pos));
    glUniform1f(1, l.source->far_plane_);
    for (const auto& o : l.objects) {
      const auto& p = scene_->meshes[o.rename__id];
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

void Renderer::DrawShadowPass(Frustum frustum_camera) {
  UpdateVboBuffer(frustum_camera);

  glCullFace(GL_FRONT);
  glViewport(0, 0, cShadowMapSize, cShadowMapSize);
  glBindVertexArray(scene_->vao);

  DrawDirectionalLightShadowPass();
  DrawPointLightShadowPass();
}

void Renderer::DrawGeometryPass() {
  glBindVertexArray(scene_->vao);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);
  sh_geometry_.Bind();
  for (const auto& o : camera_data_.objects) {
    const auto& p = scene_->meshes[o.rename__id];
    glActiveTexture(GL_TEXTURE0);
    scene_->materials[p.material].albedo.BindSampler(0);
    glActiveTexture(GL_TEXTURE1);
    scene_->materials[p.material].normal.BindSampler(1);
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
  glBindVertexArray(scene_->vao_rigged);
  sh_geometry5_.Bind();
  const auto& p = scene_->meshes_rigged[character_offset.rename__id];
  glActiveTexture(GL_TEXTURE0);
  scene_->materials_rigged[p.material].albedo.BindSampler(0);
  glActiveTexture(GL_TEXTURE1);
  scene_->materials_rigged[p.material].normal.BindSampler(1);
  glDrawElementsInstancedBaseVertexBaseInstance(
        GL_TRIANGLES,
        p.index_count,
        p.index_type,
        (void*)p.index_byte_offset,
        character_offset.instances_num,
        p.base_vertex,
        character_offset.instance_offset
    );
}

void Renderer::DrawLightPass() {
  sh_deferred_shading_.DebugUpdate();
  sh_light_emitter_.DebugUpdate();
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_hdr_scene_);
  // glDisable(GL_CULL_FACE);

  // render fbo quad
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sh_deferred_shading_.Bind();
  glBindVertexArray(vao_ui_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_position_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_normal_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_albedo_spec_);

  glUniform1i(3, static_cast<int>(point_lights_.size()));
  for (unsigned int i = 0; i < point_lights_.size(); i++) {
    auto jph_pos = point_lights_[i].info.bounds.GetCenter();
    auto pos = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
    glUniform3fv(4 + i * 2, 1, glm::value_ptr(pos));
    glUniform3fv(4 + i * 2 + 1, 1, glm::value_ptr(point_lights_[i].source->color_));
  }
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // blit DEPTH to bloom fbo
  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_hdr_scene_);
  glBlitFramebuffer(0, 0, gWindowWidth, gWindowHeight, 0, 0, gWindowWidth,
                    gWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_hdr_scene_);

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  sh_shadow_dir_apply_.DebugUpdate();
  sh_shadow_point_apply_.DebugUpdate();
  glBindVertexArray(scene_->vao);
  // dir light
  sh_shadow_dir_apply_.Bind();
  for (int i = 0; i < dir_lights_.size(); ++i) {
    const auto& l = dir_lights_[i];
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(l.source->lightSpaceMatrix_));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_maps_[i]);
    glUniform3fv(2, 1, glm::value_ptr(l.source->dir_));
    for (const auto& o : l.objects) {
      const auto& p = scene_->meshes[o.rename__id];
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
  // point light
  sh_shadow_point_apply_.Bind();
  for (int i = 0; i < point_lights_.size(); ++i) {
    // if (i != 0) continue;
    const auto& l = point_lights_[i];
    glUniform1f(0, l.source->far_plane_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemaps_[i]);
    auto jph_pos = point_lights_[i].info.bounds.GetCenter();
    auto pos = glm::vec3(jph_pos.GetX(), jph_pos.GetY(), jph_pos.GetZ());
    glUniform3fv(2, 1, glm::value_ptr(pos));
    for (const auto& o : l.objects) {
      const auto& p = scene_->meshes[o.rename__id];
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /// light sources (point light)
  glBindVertexArray(scene_->vao);
  sh_light_emitter_.Bind();
  for (const auto& l : point_lights_) {
    const auto& p = scene_->meshes[l.info.mesh_id];
    glUniformMatrix4fv(0, 1, GL_FALSE, reinterpret_cast<const float*>(&l.info.matrix));
    JPH::Vec4 colorVec = l.info.color.ToVec4();
    glUniform4fv(1, 1, &colorVec.mF32[0]);
    glDrawElementsBaseVertex(
            GL_TRIANGLES,
            p.index_count,
            p.index_type,
            (void*)p.index_byte_offset,
            p.base_vertex
        );
  }

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_bloom_);
  glClear(GL_COLOR_BUFFER_BIT);
  sh_bloom_.Bind();
  glBindVertexArray(vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_hdr_scene_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  bool horizontal = true;
  bool first_iteration = true;
  int amount = 10;
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

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  sh_composite_.Bind();
  glBindVertexArray(vao_ui_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_hdr_scene_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, buffer_ping_pong_[!horizontal]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisable(GL_DEPTH_TEST);
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
  characters_.clear();
  objects_.clear();
  camera_data_.objects.clear();
  dir_lights_.clear();
  point_lights_.clear();
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

  /// dir lights: shadowmap (sun)
  glGenTextures(cMaxDirLights, depth_maps_.data());
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  for (GLuint tex : depth_maps_) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cShadowMapSize,
                 cShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  }
  glGenFramebuffers(1, &fbo_depth_map_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_map_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth_maps_[0], 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// point lights: shadow cubemap
  glGenTextures(cMaxPointLights, depth_cubemaps_.data());
  for (GLuint tex : depth_cubemaps_) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    for (unsigned int i = 0; i < 6; ++i)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                   cShadowMapSize, cShadowMapSize, 0, GL_DEPTH_COMPONENT,
                   GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  glGenFramebuffers(1, &fbo_depth_cubemap_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth_cubemap_);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemaps_[0],
                       0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// instanced ssbo

  glGenBuffers(1, &ssbo_instanced_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_instanced_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, cMaxInstances * sizeof(InstanceGpu),
               nullptr, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &ssbo_instanced_rigged_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_instanced_rigged_);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, ssbo_instanced_rigged_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, cMaxInstancesRigged * sizeof(InstanceGpuRigged),
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
}

void Renderer::DeInit() {}

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
