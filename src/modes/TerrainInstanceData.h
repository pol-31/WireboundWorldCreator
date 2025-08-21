#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_

#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../common/Texture.h"
#include "../common/IGraph.h"
#include "TerrainNoiseData.h"

struct TerrainInstanceData : public IGraph::BaseInstanceData {
  NoiseTerrainData data;

  glm::vec3 scale = glm::vec3{1.0f};
  glm::vec3 rotate = glm::vec3{1.0f, 0.0f, 0.0f}; // quats are less convenient, cast later
  glm::vec3 translate = glm::vec3{0.0f};

  bool do_tiling = false;
  bool do_invert = false;

  // including transformation & modifiers - final result
  Texture32F hmap;
  std::array<uint8_t, 1024 * 1024> heights;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAININSTANCEDATA_H_
