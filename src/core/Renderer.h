#pragma once

#include <unordered_set>

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
#include "WindowResizeListener.h"

class Camera;
struct Scene;
class PlayerController;
class EnemyController;
class StaticObject;
class DirectedLight;
class PointLight;
class Weapon;

/// Implementation of DebugRenderer
class Renderer : public WindowResizeListener {
 public:
  Renderer(const Scene* scene, const Camera* camera,
           const std::unique_ptr<PlayerController>* player,
           const WorldManager::ZoneCulledData* culled_data);

  void OnWindowResize() override;

  ~Renderer() { DeInit(); }

  void RenderDebug();

  void UpdateBuffer();

  /// we have culled objects from WorldManager, so we just
  /// DrawShadowPass() -> DrawGeometryPass() -> DrawLightPass()
  /// OR just RenderDebug();
  void DrawShadowPass();

  void DrawGeometryPass(TerrainRenderData terrain, int triplanar_level,
                        bool render_geometry);

  // void DrawBloom();

  void DrawSsaoPass();

  void DrawLightPass(CubemapRenderData cubemap, bool light_pass,
                     bool apply_shadows, bool render_light_sources,
                     bool render_bloom, bool ibl, int cubemap_type);

  void RenderToTheScreen();

  GLuint GetLightingFbo();

  void IblBakeRadiance(CubemapRenderData cubemap);
  void IblBakePrefiltered(CubemapRenderData cubemap);
  void IblBakeLut();

  void SaveZoneIbl(const std::string& filepath);

  void SaveIblLut(const std::string& filepath);

  void LoadIblZones(SceneZone::Properties cur, SceneZone::Properties next,
    float dist_to_next);

  void ReLoadIblZones();

  // returns appropriate ibl zone name or {} if it shouldn't be baked
  std::string GetIblBakeFilename(const std::string& zone_name);

  struct IblProperties {
    std::string_view path = {};
    IblZoneType type = {};

    // may be valid or may not - WorldManager and Renderer know what active
    GLuint ibl_irradiance_map = 0;
    GLuint ibl_prefilter_map = 0;
  };

  IblProperties ibl_zones_[2];
  float ibl_zone_blending_ = 0.0f; // 0 for cur; 1 for next

  void UpdateIblUbo(int apply_ibl);

  struct IblUbo {
    float uDoIbl = 1.0f; // int?
    float uIblBlendFactor = 0.0f;
    float cur_is_outside = 0.0f; // int?
    float cur_intensity_ambient = 1.0f;
    float cur_intensity_sun = 1.0f;
    float cur_exposure = 1.0f;
    float next_is_outside = 0.0f; // int?
    float next_intensity_ambient = 1.0f;
    float next_intensity_sun = 1.0f;
    float next_exposure = 1.0f;
  };
  IblUbo ibl_ubo_data_;
  GLuint ubo_ibl_ = 0;

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

  static const GLuint cBloomSize;
  static const GLuint cSsaoSize;

  static const int cShadowObjectLod;

 private:
  void Init();
  void DeInit();

  void DrawDirectionalLightShadowPass();
  void DrawPointLightShadowPass();

  void InitSsaoNoise();

  void InitSsaoFbo();

  Texture tex_ssao_noise_;
  GLuint fbo_ssao_ = 0;
  Texture tex_ssao_color_;
  Shader sh_ssao_;
  std::vector<glm::vec3> ssao_kernel_;

  void InitInstancedBuffer();
  GLuint ssbo_instanced_ = 0;

  const Scene* scene_;
  const Camera* camera_;  // to know is it 1st/3rd AND frustum
  const std::unique_ptr<PlayerController>* player_;

  const WorldManager::ZoneCulledData* culled_data_ = nullptr;

  std::vector<GLuint> shadow_maps_;      // point light
  std::vector<GLuint> shadow_cubemaps_;  // dir light

  void InitLights();

  Shader sh_shadow_dir_;
  Shader sh_shadow_dir5_;
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

  GLuint fbo_g_buffer_ = 0;
  GLuint tex_g_depth_ = 0;
  GLuint tex_g_normal_ = 0;
  GLuint tex_g_albedo_spec_ = 0;

  void InitBloom();

  /// bloom / post-light effects
  GLuint fbo_bloom_ = 0;
  GLuint tex_bloom_ = 0;
  Shader sh_bloom_;

  void InitScene();

  GLuint fbo_scene_ = 0;
  GLuint tex_scene_ = 0;

  void InitGauss();

  GLuint fbo_ping_pong_[2];
  GLuint tex_ping_pong_[2];
  Shader sh_gauss_;

  void InitCompositeFbo();

  Shader sh_composite_;
  Shader sh_pass_through_;
  GLuint fbo_composite_ = 0;
  Texture tex_composite_;

  void InitIbl();

  Shader sh_ibl_radiance_;
  GLuint fbo_capture_ = 0;
  GLuint rbo_capture_ = 0;

  Shader sh_ibl_prefiltered_;

  Shader sh_ibl_lut_;
  GLuint tex_ibl_lut_ = 0;

  void RenderTerrain(TerrainRenderData terrain);
  void RenderCubemap(CubemapRenderData cubemap);

  void InitializeProceduralWall();
  void RenderWalls();

  Shader sh_terrain_;
  Shader sh_cubemap_;

  Shader sh_geometry_dbg_;

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
