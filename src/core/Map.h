#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_MAP_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_MAP_H_

#include <string_view>
#include <vector>

#include "Tile.h"

using Map = std::vector<TileInfo>;

std::vector<TileInfo> LoadMap(std::string_view world_map);

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_MAP_H_
