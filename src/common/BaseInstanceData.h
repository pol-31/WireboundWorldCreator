#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_

#include <string>

#include <glm/glm.hpp>

struct BaseInstanceData {
  std::string name = {};
  glm::vec4 color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
  bool do_show = true;

  /// terrain: vertical layer
  /// fences: wooden, chain-link, picket
  /// roads: soil, asphalt, gravel
  /// water: river, waterfall, pond
  /// biomes: any possible created from params
  int type_id = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_
