#ifndef WIREBOUNDWORLDCREATOR_RENDER_RENDERER_H
#define WIREBOUNDWORLDCREATOR_RENDER_RENDERER_H

#include "../common/models/Aabb3D.h"
#include "Frustum.h"
#include "Shader.h"
#include "Texture.h"

/// render to shadow map (cubemap depth buffer)
struct ShadowPass {
  static const GLuint gSizeShadowMap;
  Shader shader_shadow_pass_;
  Texture tex_shadow_map_;
  GLuint fbo_shadow_ = 0;
  void Init();
  void Start();
};

/// render to g-buffer
struct GeometryPass {
  Shader shader_geometry_;
  Texture tex_positions_;
  Texture tex_normals_;
  Texture tex_albedo_spec_;
  Texture tex_ids_;
  GLuint rbo_depth_ = 0;
  GLuint fbo_geometry_ = 0;
  void Init();
  void Start();
};

/// render from shadow map and g-buffer to screen quad
struct LightPass {
  Shader shader_lightning_;
  GLuint fbo_light_ = 0;
  Texture tex_screen_;
  std::vector<Frustum> lights_all_directed_;  // sun, projectors
  std::vector<Aabb3D> lights_directed_;
  Frustum frustum_sun_;

  void Init();
  void Process();
};

/// render screen quad to default window framebuffer
struct PostProcessPass {
  Shader shader_post_process_;  // just render quad
  void Process();
};

class Renderer {
 public:
  Renderer() { Init(); }

  ~Renderer() { DeInit(); }

  void Render(float delta_time);  // let's merge for now

  // void BeginFrame();
  // void EndShadowPass();
  // void EndFrame();

  void OnWindowResize();

 private:
  void Init();

  void DeInit();

  void UpdateCameraFrustum();

  void UpdateReflectionFrustum();

  Frustum camera_frustum_;
  Frustum camera_frustum_reflection_;

  ShadowPass shadow_;
  GeometryPass geometry_;
  LightPass light_;
  PostProcessPass post_process_;

  UiRenderer__Deprecated ui_;
  TextRenderer text_;
  ModelRenderer
      models_;  // TODO: merged with ModelManager/ModelLoader/JPH::PhysicSys
};

#endif  // WIREBOUNDWORLDCREATOR_RENDER_RENDERER_H
