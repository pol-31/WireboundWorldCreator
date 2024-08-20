#include "Map.h"

#include <algorithm>
#include <fstream>

std::vector<TileInfo> LoadMap(std::string_view world_map) {
  std::ifstream file(world_map.data());
  if (!file.is_open()) {
    throw std::runtime_error("Error opening map file");
  }
  std::vector<TileInfo> map;
  TileInfo tile_info;
  auto tile_info_map = tile_info.CreateUnorderedMap();

  std::string line;
  while (std::getline(file, line)) {
    size_t delimiterPos = line.find('=');
    if (delimiterPos != std::string::npos) {
      // "=" means end of current tile
      if (delimiterPos == 0) {
        map.push_back(tile_info);
        tile_info = TileInfo{};
        tile_info_map = tile_info.CreateUnorderedMap();
        continue;
      }
      std::string key = line.substr(0, delimiterPos);
      std::string value = line.substr(delimiterPos + 1);

      auto it = tile_info_map.find(key);
      if (it != tile_info_map.end()) {
        *(it->second) = value;
      }
    }
  }
  for (auto& tile : map) {
    if (tile.pos_x_str.empty() || tile.pos_y_str.empty()) {
      throw std::runtime_error("wrong tile_pos data");
    }
    tile.pos_x = std::stoi(tile.pos_x_str);
    tile.pos_y = std::stoi(tile.pos_y_str);
  }
  // sort by pos starting from top-left corner
  std::sort(map.begin(), map.end(),
            [](const auto& tile_info_1, const auto& tile_info_2) {
              return tile_info_1.pos_x < tile_info_2.pos_x ||
                     (tile_info_1.pos_x == tile_info_2.pos_x &&
                      tile_info_1.pos_y < tile_info_2.pos_y);
            });
  if (map.empty()) { // TODO; generate on our own
    throw std::runtime_error("no height map");
  }
  return map;
}