#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TRAITS_TERRAINTRAITS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TRAITS_TERRAINTRAITS_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../common/BaseInstanceData.h"
#include "../TerrainNoiseData.h"

struct TerrainTraits : BaseInstanceData {
  NoiseTerrainData data = {};  // hmap inside
  Texture32F extra_heights{};  // painted (not generated)

  glm::vec3 scale = glm::vec3{1.0f};
  glm::quat rotate = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 translate = glm::vec3{0.0f};

  bool do_tiling = false;
  bool do_invert = false;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TRAITS_TERRAINTRAITS_H_
