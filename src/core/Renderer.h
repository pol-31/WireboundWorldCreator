#pragma once

#include <map>

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Math/Float2.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

#include <glm/glm.hpp>

#include "../common/models/DirLight.h"
#include "../common/models/PointLight.h"
#include "../render/Shader.h"
#include "../render/Texture.h"
#include "Frustum.h"
#include "vec3.hpp"

class Scene;
struct Character;
class StaticObject;
struct DirLight;
struct PointLight;

/// Implementation of DebugRenderer
class Renderer {
 public:
  struct InstanceInfo {
    JPH::RMat44 matrix;
    JPH::Color color;
    int mesh_id;
    JPH::AABox bounds;
  };
  struct InstanceInfoRigged {
    JPH::RMat44 matrix;
    JPH::Color color;
    int mesh_id;
    JPH::AABox bounds;
    int bones_offset;
  };

  Renderer();

  ~Renderer() { DeInit(); }

  // ModelLoader loads 3 files: collision shapes, characters (same rigging),
  // and actual scene. Only one scene can be set, it contains vao, vbo, ebo and
  // all primitives data (instances as well, since the whole scene
  // decorated and set up in Blender)
  void SetScene(const Scene* scene) {
    scene_ = scene;
  }

  void AddInstance(DirLight* dir_light, InstanceInfo info);

  void AddInstance(const PointLight* point_light, InstanceInfo info);

  void AddCharacter(InstanceInfoRigged info);

  void AddInstance(InstanceInfo info);

  /// order: updateVboBuffer -> shadow -> geometry -> light (separated for ui?)

  void UpdateVboBuffer(Frustum frustum_camera);

  void DrawShadowPass(Frustum frustum_camera);

  void DrawGeometryPass();

  void DrawLightPass();

  /// Clear all primitives (to be called after drawing)
  void Clear();

  /// Unused func for composite draw (picked apart by lines/triangles)
  // void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor);
  // void DrawTriangle(
  // RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor);

  static const GLuint cShadowMapSize;
  static const int cMaxInstances;
  static const int cMaxInstancesRigged;
  static const int cMaxLines;

 private:
  void Init();
  void DeInit();

  void DrawDirectionalLightShadowPass();
  void DrawPointLightShadowPass();

  const Scene* scene_ = nullptr; // all models, meshes, vao data

  struct InstanceGpu {
    JPH::Mat44 model;
    JPH::Vec4 color;
  };
  GLuint ssbo_instanced_ = 0;

  struct InstanceGpuRigged {
    JPH::Mat44 model;
    JPH::Vec4 color;
    uint32_t  boneOffset;    // Index where this instance's skeleton begins in the SSBO
    uint32_t  padding1;    // std430 layout (16-byte alignment)
    uint32_t  padding2;    // std430 layout (16-byte alignment)
    uint32_t  padding3;    // std430 layout (16-byte alignment)
  };
  GLuint ssbo_instanced_rigged_ = 0;

  /// all needed for glDrawElementsInstancedBaseVertexBaseInstance
  struct SsboOffset {
    int instance_offset = 0;
    int instances_num = 0;
    int rename__id = 0;
  };

  //TODO: separate render (+weights, +joints)
  // struct CharacterData {
  //   const Character* object;
  //   InstanceInfo info;
  // };
  std::vector<InstanceInfoRigged> characters_;
  SsboOffset character_offset;

  struct SsboOffsetData {
    std::vector<InstanceGpu> instances; // auto num; no offset before linearizt
  };

  // key is mesh_id; need for index_count, index_type, base_vertex offset
  using SsboMeshMap = std::map<int, SsboOffset>;
  using SsboDataMeshMap = std::map<int, SsboOffsetData>;

  // struct StaticObjectData {
  //   const StaticObject* object;
  //   InstanceInfo info;
  //   // SsboMeshMap objects; // TODO: duplicated mesh_id in ::info and ::instances
  // };
  // collected every frame, based on primitive id (before frustum culling)
  std::vector<InstanceInfo> objects_;

  /// std::vector<SsboOffset>, not map, because we need just render call,
  /// don't care what order, just separated and grouped by primitives
  struct CameraData {
    Frustum frustum;
    std::vector<SsboOffset> objects;
  };
  CameraData camera_data_;

  struct DirLightData {
    DirLight* source;
    std::vector<SsboOffset> objects;
  };
  static const int cMaxDirLights = 2;
  std::array<GLuint, cMaxDirLights> depth_maps_;  // opengl ids
  Shader sh_shadow_dir_;
  Shader sh_shadow_dir_apply_;
  GLuint fbo_depth_map_ = 0;
  std::vector<DirLightData> dir_lights_;

  struct PointLightData {
    const PointLight* source;
    InstanceInfo info;
    std::vector<SsboOffset> objects;
  };
  static const int cMaxPointLights = 32;
  std::array<GLuint, cMaxPointLights> depth_cubemaps_;  // opengl ids
  Shader sh_shadow_point_;
  Shader sh_shadow_point_apply_;
  GLuint fbo_depth_cubemap_ = 0;
  std::vector<PointLightData> point_lights_;

  /// deferred lighting
  Shader sh_geometry_;
  Shader sh_geometry5_;
  Shader sh_deferred_shading_;
  Shader sh_light_emitter_;

  GLuint g_buffer_ = 0;
  GLuint g_position_ = 0;
  GLuint g_normal_ = 0;
  GLuint g_albedo_spec_ = 0;

  /// bloom / post-light effects
  GLuint fbo_bloom_ = 0;
  GLuint bloom_tex_ = 0;
  Shader sh_bloom_;

  GLuint fbo_hdr_scene_ = 0;
  GLuint tex_hdr_scene_ = 0;

  GLuint fbo_ping_pong_[2];
  GLuint buffer_ping_pong_[2];
  Shader sh_gauss_;

  Shader sh_composite_;


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
};
