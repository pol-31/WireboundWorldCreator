#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Math/Float2.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

#include <glm/glm.hpp>
#include <map>

#include "../common/models/WorldManager.h"
#include "../render/Shader.h"
#include "../render/Texture.h"
#include "Cubemap.h"
#include "Frustum.h"
#include "TerrainRenderer.h"

class Camera;
struct Scene;
class PlayerController;
class EnemyController;
class StaticObject;
class DirectedLight;
class PointLight;
class Weapon;

/// Implementation of DebugRenderer
class Renderer {
 public:
  Renderer(const Scene* scene,
  const Camera* camera,
  const std::unique_ptr<PlayerController>* player,
  const WorldManager::ZoneCulledData* culled_data);

  ~Renderer() { DeInit(); }

  void RenderDebug();

  void UpdateBuffer();

  /// we have culled objects from WorldManager, so we just
  /// DrawShadowPass() -> DrawGeometryPass() -> DrawLightPass()
  /// OR just RenderDebug();
  void DrawShadowPass();

  void DrawGeometryPass(TerrainRenderData terrain);

  void DrawBloom();

  void DrawSsaoPass();

  void DrawLightPass(CubemapRenderData cubemap);

  void RenderToTheScreen();

  /// Clear all primitives (to be called after drawing)
  void Clear();

  /// DBG Unused func for composite draw (picked apart by lines/triangles)
  // void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor);
  // void DrawTriangle(
  // RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor);

  // static const GLuint cShadowMapSize;
  static const int cMaxInstances;
  static const int cMaxInstancesRigged;
  static const int cMaxLines;


  static const int cMaxDirectedLights;
  static const GLuint cShadowCubeMapSize;
  static const int cMaxPointLights;
  static const GLuint cShadowMapSize;

 private:
  void Init();
  void DeInit();

  void DrawDirectionalLightShadowPass();
  void DrawPointLightShadowPass();

  void InitSsaoNoise();

  void InitSsaoFbo();

  Texture ssao_tex_noise_;
  GLuint ssao_fbo_ = 0;
  Texture ssao_tex_color_;
  Shader sh_ssao_;
  std::vector<glm::vec3> ssao_kernel_;

  void InitInstancedBuffer();
  GLuint ssbo_instanced_ = 0;

  const Scene* scene_;
  const Camera* camera_; // to know is it 1st/3rd AND frustum
  const std::unique_ptr<PlayerController>* player_;

  const WorldManager::ZoneCulledData* culled_data_ = nullptr;

  std::vector<GLuint> shadow_maps_; // point light
  std::vector<GLuint> shadow_cubemaps_; // dir light

  void InitLights();

  Shader sh_shadow_dir_;
  Shader sh_shadow_dir_apply_;
  GLuint fbo_depth_map_ = 0;

  Shader sh_shadow_point_;
  Shader sh_shadow_point5_;
  Shader sh_shadow_point_apply_;
  GLuint fbo_depth_cubemap_ = 0;

  /// deferred lighting
  Shader sh_geometry_;
  Shader sh_geometry5_;
  Shader sh_deferred_shading_;
  Shader sh_light_emitter_;

  void InitGBuffer();

  GLuint g_buffer_ = 0;
  GLuint g_position_ = 0;
  GLuint g_normal_ = 0;
  GLuint g_albedo_spec_ = 0;

  void InitBloom();

  /// bloom / post-light effects
  GLuint fbo_bloom_ = 0;
  GLuint bloom_tex_ = 0;
  Shader sh_bloom_;

  void InitScene();

  GLuint fbo_scene_ = 0;
  GLuint tex_scene_ = 0;

  void InitGauss();

  GLuint fbo_ping_pong_[2];
  GLuint buffer_ping_pong_[2];
  Shader sh_gauss_;

  void InitCompositeFbo();

  Shader sh_composite_;
  Shader sh_pass_through_;
  GLuint fbo_composite_ = 0;
  Texture tex_composite_;

  void RenderTerrain(TerrainRenderData terrain);
  void RenderCubemap(CubemapRenderData cubemap);

  void InitializeProceduralWall();
  void RenderWalls();

  Shader sh_terrain_;
  Shader sh_cubemap_;

  void InitUi();

  /// ui
  GLuint vao_ui_ = 0;
  GLuint vbo_ui_ = 0;

  /// lines
  struct Line {
    JPH::Float3 mFrom;
    JPH::Color mFromColor;
    JPH::Float3 mTo;
    JPH::Color mToColor;
  };

  GLuint vao_lines_ = 0;
  GLuint vbo_lines_ = 0;

  Shader sh_lines_;
  JPH::Array<Line> mLines;

  Shader sh_wall_;
  GLuint vao_wall_ = 0;
  GLuint vbo_wall_ = 0;
};
