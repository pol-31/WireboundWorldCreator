#ifndef WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_

#include <iostream>
#include <string_view>
#include <map>
#include <vector>

struct Paths {
  std::string texture_ui;
  std::string texture_text;
  std::string texture_cursor;
  std::string shader_cubemap_frag;
  std::string shader_cubemap_vert;
  std::string shader_terrain_frag;
  std::string shader_terrain_vert;
  std::string shader_terrain_picking_frag;
  std::string shader_terrain_picking_vert;
  std::string shader_ui_frag;
  std::string shader_ui_vert;
  std::string world_map;

  Paths(std::string_view path) {
    std::ifstream file(path.data());
    if (!file.is_open()) {
      std::cerr << "Error opening file: " << path << std::endl;
      return;
    }

    std::map<std::string, std::string*> field_map = {
        {"shader_cubemap_frag", &shader_cubemap_frag},
        {"shader_cubemap_vert", &shader_cubemap_vert},
        {"shader_terrain_frag", &shader_terrain_frag},
        {"shader_terrain_picking_frag", &shader_terrain_picking_frag},
        {"shader_terrain_picking_vert", &shader_terrain_picking_vert},
        {"shader_terrain_vert", &shader_terrain_vert},
        {"shader_ui_frag", &shader_ui_frag},
        {"shader_ui_vert", &shader_ui_vert},
        {"texture_ui", &texture_ui},
        {"texture_text", &texture_text},
        {"texture_cursor", &texture_cursor},
        {"world_map", &world_map}
    };

    std::string line;
    while (std::getline(file, line)) {
      size_t delimiterPos = line.find('=');
      if (delimiterPos != std::string::npos) {
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        auto it = field_map.find(key);
        if (it != field_map.end()) {
          *(it->second) = value;
        }
      }
    }
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_
