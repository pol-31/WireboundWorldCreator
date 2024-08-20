#ifndef WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PATHSPARSER_H_

#include <iostream>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <vector>

//TODO: order
struct Paths {
  std::string texture_ui;
  std::string texture_text;
  std::string texture_cursor;
  std::string shader_cubemap_frag;
  std::string shader_cubemap_vert;
  std::string shader_terrain_vert;
  std::string shader_terrain_tesc;
  std::string shader_terrain_tese;
  std::string shader_terrain_frag;
  std::string shader_placement_vert;
  std::string shader_placement_frag;
  std::string shader_water_vert;
  std::string shader_water_tesc;
  std::string shader_water_tese;
  std::string shader_water_frag;
  std::string shader_grass_vert;
  std::string shader_grass_tesc;
  std::string shader_grass_tese;
  std::string shader_grass_frag;
  std::string shader_grass_comp;
  std::string shader_height_map_picking_frag;
  std::string shader_height_map_picking_vert;
  std::string shader_static_sprite_vert;
  std::string shader_static_sprite_frag;
  std::string shader_static_sprite_pick_vert;
  std::string shader_static_sprite_pick_frag;
  std::string shader_slider_handle_vert;
  std::string shader_slider_handle_frag;
  std::string shader_points_polygon_vert;
  std::string shader_points_polygon_frag;
  std::string world_map;
  std::string shader_placement_draw_comp;
  std::string shader_dithering;
  std::string shader_poisson_points;

  std::string placement_density_low;
  std::string placement_density_medium_low;
  std::string placement_density_medium;
  std::string placement_density_medium_high;
  std::string placement_density_high;
  std::string placement_density_very_high;
  std::string placement_density_ultra_high;
  std::string placement_density_extreme;

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
        {"shader_terrain_vert", &shader_terrain_vert},
        {"shader_terrain_tesc", &shader_terrain_tesc},
        {"shader_terrain_tese", &shader_terrain_tese},
        {"shader_terrain_frag", &shader_terrain_frag},
        {"shader_placement_vert", &shader_placement_vert},
        {"shader_placement_frag", &shader_placement_frag},
        {"shader_water_vert", &shader_water_vert},
        {"shader_water_tesc", &shader_water_tesc},
        {"shader_water_tese", &shader_water_tese},
        {"shader_water_frag", &shader_water_frag},
        {"shader_grass_vert", &shader_grass_vert},
        {"shader_grass_tesc", &shader_grass_tesc},
        {"shader_grass_tese", &shader_grass_tese},
        {"shader_grass_frag", &shader_grass_frag},
        {"shader_grass_comp", &shader_grass_comp},
        {"shader_height_map_picking_frag", &shader_height_map_picking_frag},
        {"shader_height_map_picking_vert", &shader_height_map_picking_vert},
        {"shader_static_sprite_vert", &shader_static_sprite_vert},
        {"shader_static_sprite_frag", &shader_static_sprite_frag},
        {"shader_static_sprite_pick_vert", &shader_static_sprite_pick_vert},
        {"shader_static_sprite_pick_frag", &shader_static_sprite_pick_frag},
        {"shader_slider_handle_vert", &shader_slider_handle_vert},
        {"shader_slider_handle_frag", &shader_slider_handle_frag},
        {"shader_dithering", &shader_dithering},
        {"shader_poisson_points", &shader_poisson_points},
        {"texture_ui", &texture_ui},
        {"texture_text", &texture_text},
        {"shader_placement_draw_comp", &shader_placement_draw_comp},
        {"world_map", &world_map},
        {"placement_density_low", &placement_density_low},
        {"placement_density_medium_low", &placement_density_medium_low},
        {"placement_density_medium", &placement_density_medium},
        {"placement_density_medium_high", &placement_density_medium_high},
        {"placement_density_high", &placement_density_high},
        {"placement_density_very_high", &placement_density_very_high},
        {"placement_density_ultra_high", &placement_density_ultra_high},
        {"placement_density_extreme", &placement_density_extreme},
        {"texture_skybox1_left", &texture_skybox1_left},
        {"texture_skybox1_right", &texture_skybox1_right},
        {"texture_skybox1_top", &texture_skybox1_top},
        {"texture_skybox1_bottom", &texture_skybox1_bottom},
        {"texture_skybox1_front", &texture_skybox1_front},
        {"texture_skybox1_back", &texture_skybox1_back},
        {"shader_points_polygon_vert", &shader_points_polygon_vert},
        {"shader_points_polygon_frag", &shader_points_polygon_frag}
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
