#ifndef WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_

#include <iostream>
#include <fstream>
#include <string_view>
#include <unordered_map>
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
  std::string shader_static_sprite_vert;
  std::string shader_static_sprite_frag;
  std::string shader_slider_handle_vert;
  std::string shader_slider_handle_frag;
  std::string world_map;

  std::string texture_skybox1_left;
  std::string texture_skybox1_right;
  std::string texture_skybox1_top;
  std::string texture_skybox1_bottom;
  std::string texture_skybox1_front;
  std::string texture_skybox1_back;

  Paths(std::string_view path) {
    std::ifstream file(path.data());
    if (!file.is_open()) {
      std::cerr << "Error opening file: " << path << std::endl;
      return;
    }

    std::unordered_map<std::string, std::string*> field_map = {
        {"shader_cubemap_frag", &shader_cubemap_frag},
        {"shader_cubemap_vert", &shader_cubemap_vert},
        {"shader_terrain_frag", &shader_terrain_frag},
        {"shader_terrain_picking_frag", &shader_terrain_picking_frag},
        {"shader_terrain_picking_vert", &shader_terrain_picking_vert},
        {"shader_terrain_vert", &shader_terrain_vert},
        {"shader_static_sprite_vert", &shader_static_sprite_vert},
        {"shader_static_sprite_frag", &shader_static_sprite_frag},
        {"shader_slider_handle_vert", &shader_slider_handle_vert},
        {"shader_slider_handle_frag", &shader_slider_handle_frag},
        {"texture_ui", &texture_ui},
        {"texture_text", &texture_text},
        {"world_map", &world_map},
        {"texture_skybox1_left", &texture_skybox1_left},
        {"texture_skybox1_right", &texture_skybox1_right},
        {"texture_skybox1_top", &texture_skybox1_top},
        {"texture_skybox1_bottom", &texture_skybox1_bottom},
        {"texture_skybox1_front", &texture_skybox1_front},
        {"texture_skybox1_back", &texture_skybox1_back}
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
