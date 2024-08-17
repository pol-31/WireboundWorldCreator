#include "Tile.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, std::string*> TileInfo::CreateUnorderedMap() {
  return {
      {"pos_x", &pos_x_str},
      {"pos_y", &pos_y_str},
      {"map_terrain_height", &map_terrain_height},
      {"map_erosion_wear", &map_erosion_wear},
      {"map_erosion_flow", &map_erosion_flow},
      {"map_erosion_deposition", &map_erosion_deposition},
      {"map_terrain_cavity", &map_terrain_cavity},
      {"map_terrain_occlusion", &map_terrain_occlusion},
      {"map_terrain_normal", &map_terrain_normal},
      {"map_terrain_wetness", &map_terrain_wetness},
      {"map_water_height", &map_water_height},
      {"map_water_flow", &map_water_flow},
      {"graph_water", &graph_water},
      {"graph_roads", &graph_roads},
      {"graph_fences", &graph_fences},
      {"map_placement_trees", &map_placement_trees},
      {"map_placement_bushes", &map_placement_bushes},
      {"map_placement_tall_grass", &map_placement_tall_grass},
      {"map_placement_undergrowth", &map_placement_undergrowth},
      {"points_objects", &points_objects},
      {"tile_info", &points_biomes}
  };
}

std::vector<GraphTraits> GraphTraits::Parse(std::string_view path) {
  std::vector<GraphTraits> graphs{};
  if (path.empty()) {
    return graphs;
  }
  std::ifstream file(path.data());
  if (!file.is_open()) {
    std::cerr << "cannot open the file " << path << std::endl;
    return graphs;
  }
  std::string line;
  std::istringstream iss_line;
  while (std::getline(file, line)) {
    iss_line.clear();
    iss_line.str(line);
    // new graph instance
    GraphTraits graph;
    std::getline(iss_line, line, ';');
    graph.type_id = std::stoi(line);

    std::getline(iss_line, line, ';');
    int vertices_num = std::stoi(line);
    std::getline(iss_line, line, ';');
    int edges_num = std::stoi(line);

    for (int i = 0; i < vertices_num; ++i) {
      std::getline(file, line); // example: 10;20;
      size_t delimiterPos = line.find(';');
      graph.graph.vertices.push_back({
          std::stoi(line.substr(0, delimiterPos)),
          std::stoi(line.substr(delimiterPos + 1))
      });
    }
    for (int i = 0; i < edges_num; ++i) {
      std::getline(file, line); // example: 0;1;
      size_t delimiterPos = line.find(';');
      graph.graph.edges.push_back({
          std::stoi(line.substr(0, delimiterPos)),
          std::stoi(line.substr(delimiterPos + 1))
      });
    }
    graphs.push_back(graph);
  }
  return graphs;
}

void GraphTraits::Serialize(std::string_view path,
                            std::vector<GraphTraits> graphs) {
  std::ofstream file(path.data());
  if (!file.is_open()) {
    std::cerr << "cannot open the file " << path << std::endl;
    return;
  }
  std::ostringstream all_data;
  for (const auto& graph : graphs) {
    all_data << std::to_string(graph.type_id) << ';'
             << std::to_string(graph.graph.vertices.size()) << ';'
             << std::to_string(graph.graph.vertices.size()) << ";\n";
    for (const auto& vertex : graph.graph.vertices) {
      all_data << std::to_string(vertex.x) << ';'
               << std::to_string(vertex.y) << ";\n";
    }
    for (const auto& edge : graph.graph.edges) {
      all_data << std::to_string(edge.first) << ';'
               << std::to_string(edge.second) << ";\n";
    }
  }
}

// uint32 pos_x, uint32 pos_y, uint32 id
std::vector<ObjectTraits> ObjectTraits::Parse(std::string_view path) {
  //TODO:
}
void ObjectTraits::Serialize(std::string_view path,
                             std::vector<ObjectTraits> objects) {}

// uint32 pos_x, uint32 pos_y, uint32 id
std::vector<BiomeTraits> BiomeTraits::Parse(std::string_view path) {
  //TODO:
}
void BiomeTraits::Serialize(std::string_view path,
                            std::vector<BiomeTraits> bimoes) {}


Tile::Tile(const TileInfo& tile_info) {
  // TODO; use placeholders (full black / full white texture)

  // pos data already valid
  // (we've thrown at TileRenderer::Init()) in case of missing
  pos_x = tile_info.pos_x;
  pos_y = tile_info.pos_y;
  map_terrain_height = Texture(tile_info.map_terrain_height);
  map_erosion_wear = Texture(tile_info.map_erosion_wear);
  map_erosion_flow = Texture(tile_info.map_erosion_flow);
  map_erosion_deposition = Texture(tile_info.map_erosion_deposition);
  map_terrain_cavity = Texture(tile_info.map_terrain_cavity);
  map_terrain_occlusion = Texture(tile_info.map_terrain_occlusion);
  map_terrain_normal = Texture(tile_info.map_terrain_normal);
  map_terrain_wetness = Texture(tile_info.map_terrain_wetness);
  InitHeightMap(tile_info.map_water_height, map_water_height);
  map_water_flow = Texture(tile_info.map_water_flow);
  graph_water = GraphTraits::Parse(tile_info.graph_water);
  graph_roads = GraphTraits::Parse(tile_info.graph_roads);
  graph_fences = GraphTraits::Parse(tile_info.graph_fences);


  //TODO: shrink to fit?
  std::vector<std::uint8_t> white_height_map(1024 * 1024, 255);
  InitHeightMap(tile_info.map_placement_trees, map_placement_trees,
                white_height_map);
  InitHeightMap(tile_info.map_placement_bushes, map_placement_bushes,
                white_height_map);
  InitHeightMap(tile_info.map_placement_tall_grass, map_placement_tall_grass,
                white_height_map);
  InitHeightMap(tile_info.map_placement_undergrowth,
                map_placement_undergrowth, white_height_map);

  points_objects = ObjectTraits::Parse(tile_info.points_objects);
  points_biomes = BiomeTraits::Parse(tile_info.points_biomes);

  // duplicating data both on CPU & GPU (see Tile.h for more info)
  terrain_heights_.resize(1024 * 1024);
  map_terrain_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                terrain_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  water_heights_init_.resize(1024 * 1024);
  map_water_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                water_heights_init_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  /// see explanation at header file (Tile.h)
  water_heights_ = water_heights_init_;
}

void Tile::InitHeightMap(std::string_view path, Texture& texture) {
  if (!path.empty()) {
    texture = Texture(path);
    return;
  }
  // we use only 1024x1024
  GLuint tex_id;
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  // default-init with 0 height
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  texture = Texture(tex_id, 1024, 1024, GL_RED);
}

void Tile::InitHeightMap(std::string_view path, Texture& texture,
                         const std::vector<std::uint8_t>& data) {
  if (!path.empty()) {
    texture = Texture(path);
    return;
  }
  // we use only 1024x1024
  GLuint tex_id;
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 1024, 1024, 0, GL_RED,
               GL_UNSIGNED_BYTE, data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  texture = Texture(tex_id, 1024, 1024, GL_R8);
}
