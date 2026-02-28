#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../io/Window.h"

const GLuint ShadowPass::gSizeShadowMap = 1024;

void ShadowPass::Init() {
  GLuint shado_map_id = 0;
  glGenTextures(1, &shado_map_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, shado_map_id);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 gSizeShadowMap, gSizeShadowMap, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  tex_shadow_map_ = Texture(shado_map_id, gSizeShadowMap, gSizeShadowMap,
                            GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadow_);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                       tex_shadow_map_.GetId(), 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  float aspect = (float)gSizeShadowMap / (float)gSizeShadowMap;
  float near = 1.0f;
  float far = 25.0f;
  // TODO: need Frustrum (so CPU aabb-test included)
  glm::mat4 shadowProj =
      glm::perspective(glm::radians(90.0f), aspect, near, far);
  std::vector<glm::mat4> shadowTransforms;

  glm::vec3 lightPos(0.0f);

  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0),
                               glm::vec3(0.0, 0.0, 1.0)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0),
                               glm::vec3(0.0, 0.0, -1.0)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0),
                               glm::vec3(0.0, -1.0, 0.0)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0),
                               glm::vec3(0.0, -1.0, 0.0)));
}

void GeometryPass::Init() {
  glGenFramebuffers(1, &fbo_geometry_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_geometry_);
  tex_positions_ = Texture16F(Texture::Type::WindowRGBA16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex_positions_.GetId(), 0);
  tex_normals_ = Texture16F(Texture::Type::WindowRGBA16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         tex_normals_.GetId(), 0);
  tex_albedo_spec_ = Texture(Texture::Type::WindowRGBA8);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         tex_albedo_spec_.GetId(), 0);
  tex_ids_ =
      TextureUi(Texture::Type::WindowR32UI, GL_NEAREST, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                         tex_ids_.GetId(), 0);

  unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                 GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
  glDrawBuffers(4, attachments);

  glGenRenderbuffers(1, &rbo_depth_);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gWindowWidth,
                        gWindowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rbo_depth_);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightPass::Init() {
  glGenFramebuffers(1, &fbo_light_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_light_);
  tex_screen_ = Texture16F(Texture::Type::WindowRGBA8);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex_screen_.GetId(), 0);
  // TODO: specify NOT_DEPTH

  JPH::Vec3 sun_pos = JPH::Vec3(100, 100, 100);
  JPH::Vec3 sun_tgt = JPH::Vec3::sZero();
  JPH::Vec3 sun_up = JPH::Vec3(0, 1, 0);
  JPH::Vec3 sun_fwd = (sun_tgt - sun_pos).Normalized();
  float sun_fov = JPH::DegreesToRadians(20.0f);
  float sun_near = 1.0f;
  frustum_sun_ = Frustum(sun_pos, sun_fwd, sun_up, sun_fov, sun_fov, sun_near);
}

void Renderer::Init() {
  shadow_.Init();
  geometry_.Init();
  light_.Init();
}

void Renderer::DeInit() {
  // TODO: rbo, fbo.. we can actually RAII it...
}

void Renderer::OnWindowResize() {
  DeInit();
  Init();
}

void ShadowPass::Start() {
  glViewport(0, 0, gSizeShadowMap, gSizeShadowMap);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadow_);
  glClear(GL_DEPTH_BUFFER_BIT);
  simpleDepthShader.use();
  for (unsigned int i = 0; i < 6; ++i)
    simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]",
                              shadowTransforms[i]);
  simpleDepthShader.setFloat("far_plane", far_plane);
  simpleDepthShader.setVec3("lightPos", lightPos);
}

void GeometryPass::Start() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  tex_positions_.BindSampler(0);
  tex_normals_.BindSampler(1);
  tex_albedo_spec_.BindSampler(2);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_shadow_map_.GetId());
  gBufferShader.use();
}

void LightPass::Process() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  lightingPassShader.use();
  BindAllGBufferTextures();
  SetLightingUniforms();
  RenderQuad();
}

void PostProcessPass::Process() {}

void Renderer::Render(float delta_time) {
  /// check fructum IsOverlaps per each light source
  shadow_.Start();
  RenderScene(light_.frustum_sun_);
  for (const auto& light : light_.lights_directed_) {
    RenderScene(light);
  }
  for (const auto& light : light_.lights_all_directed_) {
    RenderScene(light);
  }
  glViewport(0, 0, gWindowWidth, gWindowHeight);  // restore

  /// check fructum IsOverlaps per each camera view
  UpdateCameraFrustum();
  geometry_.Start(camera_frustum_);
  RenderScene(camera_frustum_);
  UpdateCameraFrustum();
  geometry_.Start(camera_frustum_reflection_);
  RenderScene(camera_frustum_reflection_);

  light_.Process();
  post_process_.Process();

  ui_renderer_.Render();
  text_renderer_.Render();
}

void Renderer::UpdateCameraFrustum() {
  JPH::Vec3 cam_pos = JPH::Vec3(inCamera.mPos);
  float camera_fovy = inCamera.mFOVY;
  float camera_aspect = static_cast<float>(gWindowWidth) / gWindowHeight;
  float camera_fovx =
      2.0f * JPH::ATan(camera_aspect * JPH::Tan(0.5f * camera_fovy));
  float camera_near = 0.01f * inWorldScale;
  camera_frustum_ = Frustum(cam_pos, inCamera.mForward, inCamera.mUp,
                            camera_fovx, camera_fovy, camera_near);
}

void Renderer::UpdateReflectionFrustum() {
  JPH::Vec3 cam_pos = JPH::Vec3(inCamera.mPos);
  float camera_fovy = inCamera.mFOVY;
  float camera_aspect = static_cast<float>(gWindowWidth) / gWindowHeight;
  float camera_fovx =
      2.0f * JPH::ATan(camera_aspect * JPH::Tan(0.5f * camera_fovy));
  float camera_near = 0.01f * inWorldScale;
  camera_frustum_reflection_ =
      Frustum(cam_pos, inCamera.mForward_reflected, inCamera.mUp, camera_fovx,
              camera_fovy, camera_near);
}