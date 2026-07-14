#ifndef WIREBOUNDWORLDCREATOR_TERRAINRENDERER_H
#define WIREBOUNDWORLDCREATOR_TERRAINRENDERER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "../render/Shader.h"
#include "../render/Texture.h"
#include "GeoClipmaps.h"

struct TerrainRenderData {
  GLuint vao;
  int patch_num = 0;
  GLuint hmap_id;
  int material_id;
  //bool enable_wireframe;
};

namespace JPH {
class BodyInterface;
} // namespace JPH

class MaterialArray;

class TerrainRenderer {
public:
  TerrainRenderer(const MaterialArray* material);

  void InitializeBody(JPH::BodyInterface* body_interface);

  void Update(const Camera* camera) {
    mesh_.Update(camera);
  }

  TerrainRenderData GetRenderData() const noexcept;

private:
  JPH::BodyID body_id_;
  GeoClipmaps mesh_;
  Texture32F hmap_;
  const MaterialArray* material_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_TERRAINRENDERER_H
