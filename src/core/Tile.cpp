#include "Tile.h"

#include <algorithm>  // clamp
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../common/Details.h"

Tile::Tile() : map_scale(0.1f) {
  map_terrain_height = Texture32F(Texture::Type::TerrainR32F);

  map_terrain_normal =
      Texture(Texture::Type::TerrainRG8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_slope = Texture(Texture::Type::TerrainR8);
  map_terrain_ao = Texture(Texture::Type::TerrainR8);
  map_terrain_splat = Texture(Texture::Type::TerrainRGBA8);

  map_terrain_erosion_thermal = Texture32F(Texture::Type::TerrainR32F);
  map_terrain_erosion_hydraulic = Texture32F(Texture::Type::TerrainR32F);
  map_water_accum = Texture32F(Texture::Type::TerrainR32F);
  map_water_flow = Texture(Texture::Type::TerrainRG8);

  map_water_height = Texture32F(Texture::Type::TerrainR32F);

  map_ocean_surface_ = Texture32F(Texture::Type::TerrainR32F);

  tex_roads_deform_ = Texture32F(Texture::Type::TerrainR32F);
  tex_roads_df_ = Texture32F(Texture::Type::TerrainR32F);
  tex_roads_mask_ = Texture(Texture::Type::TerrainR8);

  tex_rivers_deform_ = Texture32F(Texture::Type::TerrainR32F);
  tex_rivers_df_ = Texture32F(Texture::Type::TerrainR32F);
  tex_rivers_mask_ = Texture(Texture::Type::TerrainR8);

  tex_placement_trees_ = Texture(Texture::Type::TerrainR8);
  tex_placement_bushes_ = Texture(Texture::Type::TerrainR8);
  tex_placement_tall_grass_ = Texture(Texture::Type::TerrainR8);
  tex_placement_undergrowth_ = Texture(Texture::Type::TerrainR8);
  tex_vegetation_mask = Texture(Texture::Type::TerrainR8);

  map_terrain_height_raw_ = Texture32F(Texture::Type::TerrainR32F);
  map_water_height = Texture32F(Texture::Type::TerrainR32F);

  GLsizei size = details::gTerrainSize;
  terrain_heights_ = std::vector<float>(size * size, 0.0f);
  water_heights_ = std::vector<float>(size * size, 0.0f);
}

void Tile::OnScroll(float yoffset) {
  if (yoffset > 0.0f)
    target_map_scale *= details::kMapScaleFactor;
  else
    target_map_scale /= details::kMapScaleFactor;

  target_map_scale = std::clamp(target_map_scale, 0.05f, 100.0f);
}

void Tile::UpdateMapScale(float delta_time) {
  float response =
      glm::mix(6.0f, 14.0f,
               glm::clamp(glm::log(map_scale) / glm::log(100.0f), 0.0f, 1.0f));
  map_scale += (target_map_scale - map_scale) *
               (1.0f - std::exp(-response * delta_time));
}

void Tile::ResetTerrain() {
  map_terrain_slope.Clear();
  map_terrain_ao.Clear();
  map_terrain_splat.Clear();
  map_terrain_normal.Clear();
  map_water_flow.Clear();
  map_water_height.Clear();

  map_terrain_height.Clear();
  map_terrain_erosion_thermal.Clear();
  map_terrain_erosion_hydraulic.Clear();
  map_water_accum.Clear();

  std::fill(terrain_heights_.begin(), terrain_heights_.end(), 0.0f);
  std::fill(water_heights_.begin(), water_heights_.end(), 0.0f);
}

float Tile::GetPositionY(float fx, float fy) {
  int x = std::min(1022, static_cast<int>(floorf(fx)));
  int y = std::min(1022, static_cast<int>(floorf(fy)));

  float tx = fx - x;  // 0..1
  float ty = fy - y;  // 0..1

  const auto& h = terrain_heights_;
  int stride = 1024;

  // sample 4 neighbors
  float h00 = h[y * stride + x];
  float h10 = h[y * stride + x + 1];
  float h01 = h[(y + 1) * stride + x];
  float h11 = h[(y + 1) * stride + x + 1];

  // bilinear interpolation
  float hx0 = h00 + (h10 - h00) * tx;
  float hx1 = h01 + (h11 - h01) * tx;
  return hx0 + (hx1 - hx0) * ty;
}

float Tile::GetPositionY32(float fx, float fy) {
  return GetPositionY(fx * 16.0f + 512.0f, fy * 16.0f + 512.0f);
}

void WriteTerrainExtraHeights(const std::vector<TerrainTraits>& terrain) {
  for (int i = 0; i < terrain.size(); ++i) {
    std::string name = "terrain_" + std::to_string(i) + ".r32";
    terrain[i].extra_heights.StoreData(name, 1);
  }
}

void ReadTerrainExtraHeights(std::vector<TerrainTraits>& terrain) {
  for (int i = 0; i < terrain.size(); ++i) {
    std::string name = "terrain_" + std::to_string(i) + ".r32";
    if (std::filesystem::exists(name)) {
      terrain[i].extra_heights = Texture32F(name, Texture::Type::TerrainR32F);
    } else {
      // GPU BUG #3
      // need it two times, otherwise UB data
      terrain[i].extra_heights = Texture32F(Texture::Type::TerrainR32F);
      terrain[i].extra_heights = Texture32F(Texture::Type::TerrainR32F);
    }
  }
}

void LoadPlacement(std::string_view path, Texture& texture) {
  namespace fs = std::filesystem;
  if (fs::exists(path)) {
    texture = Texture(path, Texture::Type::TerrainR8);
  }
}

void Tile::Parse(std::string_view path) {
  namespace fs = std::filesystem;
  LoadPlacement("tex_placement_trees_.png", tex_placement_trees_);
  LoadPlacement("tex_placement_bushes_.png", tex_placement_bushes_);
  LoadPlacement("tex_placement_tall_grass_.png", tex_placement_tall_grass_);
  LoadPlacement("tex_placement_undergrowth_.png", tex_placement_undergrowth_);
  if (std::filesystem::exists("OceanHmap.r32")) {
    map_ocean_surface_ =
        Texture32F("OceanHmap.r32", Texture::Type::TerrainR32F);
  } else {
    map_ocean_surface_ = Texture32F(Texture::Type::TerrainR32F);
  }
  std::ifstream in("Serialize.txt");
  std::string tag;
  in >> tag;  // WORLD_BEGIN
  objects_data = ReadObjects(in);
  biomes_data = ReadBiomes(in);
  roads_data = ReadRoads(in);
  rivers_data = ReadRivers(in);
  ocean_data = ReadOceans(in);
  terrain_data = ReadTerrain(in);
  in >> tag;  // WORLD_END
  std::cout << "parse" << std::endl;
  ReadTerrainExtraHeights(terrain_data);
}

void Tile::Serialize(std::string_view path) {
  tex_placement_trees_.StoreData("tex_placement_trees_.png", 1);
  tex_placement_bushes_.StoreData("tex_placement_bushes_.png", 1);
  tex_placement_tall_grass_.StoreData("tex_placement_tall_grass_.png", 1);
  tex_placement_undergrowth_.StoreData("tex_placement_undergrowth_.png", 1);
  map_ocean_surface_.StoreData("OceanHmap.r32", 1);
  std::ofstream out("Serialize.txt");
  out << "WORLD_BEGIN\n";
  WriteObjects(out, objects_data);
  WriteBiomes(out, biomes_data);
  WriteRoads(out, roads_data);
  WriteRivers(out, rivers_data);
  WriteOceans(out, ocean_data);
  WriteTerrain(out, terrain_data);
  out << "WORLD_END\n";
  std::cout << "serialize" << std::endl;
  WriteTerrainExtraHeights(terrain_data);
}

void ReadBaseData(std::istream& in, BaseInstanceData& data) {
  in >> data.name;
  in >> data.color.r >> data.color.g >> data.color.b >> data.color.a;
  int s;
  in >> s;
  data.do_show = (s != 0);
}

void ReadNoiseBaseData(std::istream& in, NoiseDataBase& data) {
  in >> data.strength;
  int s;
  in >> s;
  data.do_invert = (s != 0);
  in >> s;
  data.do_tiling = (s != 0);
}

void ReadGraph(std::istream& in, GraphBakeConfig& graph) {
  in >> graph.curve_amplitude;
  in >> graph.height_drift;
  in >> graph.height_raise;
  in >> graph.side_sagging;
  in >> graph.radius;
  in >> graph.radius_flat;
}

void ReadOceanLayer(std::istream& in, OceanLayerTraits& layer) {
  in >> layer.scale;
  in >> layer.fetch;
  in >> layer.wind;
  in >> layer.spread_blend;
  in >> layer.swell;
  in >> layer.peak_enhancement;
  in >> layer.short_waves_fade;
  in >> layer.lambda;
  int s;
  in >> s;
  layer.visible = (s != 0);
}

void ReadMapPoints(std::istream& in, std::string& tag,
                   std::vector<MapPoint>& map_points) {
  size_t count;
  in >> count;
  map_points.resize(count);
  for (size_t k = 0; k < count; ++k) {
    in >> map_points[k].position.x >> map_points[k].position.y;
    int sel;
    in >> sel;
    map_points[k].selected = sel != 0;
  }
}

void ReadMapJoints(std::istream& in, std::string& tag,
                   std::vector<glm::uvec2>& map_joints) {
  size_t count;
  in >> count;
  map_joints.resize(count);
  for (size_t k = 0; k < count; ++k) {
    in >> map_joints[k].x >> map_joints[k].y;
  }
}

std::vector<ObjectTraits> ReadObjects(std::istream& in) {
  std::vector<ObjectTraits> objects;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    ObjectTraits o;
    in >> tag;
    if (tag != "OBJECT_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    in >> o.hp;
    in >> o.speed;
    in >> o.attack;
    in >> o.attack_speed;
    ReadMapPoints(in, tag, o.map_points);
    ReadMapJoints(in, tag, o.map_joints);
    size_t count;
    in >> count;
    o.rotations.resize(count);
    for (size_t k = 0; k < count; ++k) {
      in >> o.rotations[k].x >> o.rotations[k].y >> o.rotations[k].z >>
          o.rotations[k].w;
    }
    in >> count;
    o.scales.resize(count);
    for (size_t k = 0; k < count; ++k) {
      in >> o.scales[k].x >> o.scales[k].y >> o.scales[k].z;
    }
    objects.push_back(std::move(o));
  }
  return objects;
}

std::vector<BiomeTraits> ReadBiomes(std::istream& in) {
  std::vector<BiomeTraits> biomes;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    BiomeTraits o;
    in >> tag;
    if (tag != "BIOME_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    in >> o.wind_angle;
    in >> o.wind_speed;
    in >> o.sun_x;
    in >> o.sun_y;
    in >> o.sun_z;
    in >> o.sun_r;
    in >> o.sun_g;
    in >> o.sun_b;
    ReadMapPoints(in, tag, o.map_points);
    biomes.push_back(std::move(o));
  }
  return biomes;
}

std::vector<RoadTraits> ReadRoads(std::istream& in) {
  std::vector<RoadTraits> roads;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    RoadTraits o;
    in >> tag;
    if (tag != "ROAD_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    ReadGraph(in, o.config);
    ReadMapPoints(in, tag, o.map_points);
    ReadMapJoints(in, tag, o.map_joints);
    roads.push_back(std::move(o));
  }
  return roads;
}

std::vector<RiverTraits> ReadRivers(std::istream& in) {
  std::vector<RiverTraits> rivers;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    RiverTraits o;
    in >> tag;
    if (tag != "RIVER_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    ReadGraph(in, o.config);
    in >> o.transparency;
    in >> o.viscosity;
    ReadMapPoints(in, tag, o.map_points);
    ReadMapJoints(in, tag, o.map_joints);
    rivers.push_back(std::move(o));
  }
  return rivers;
}

std::vector<OceanTraits> ReadOceans(std::istream& in) {
  std::vector<OceanTraits> oceans;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    OceanTraits o;
    in >> tag;
    if (tag != "OCEAN_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    ReadOceanLayer(in, o.near);
    ReadOceanLayer(in, o.mid);
    ReadOceanLayer(in, o.far);
    ReadMapPoints(in, tag, o.map_points);
    oceans.push_back(std::move(o));
  }
  return oceans;
}

std::vector<TerrainTraits> ReadTerrain(std::istream& in) {
  std::vector<TerrainTraits> terrain;
  std::string tag;
  size_t total = 0;
  in >> tag >> total;
  for (size_t i = 0; i < total; ++i) {
    TerrainTraits o;
    in >> tag;
    if (tag != "TERRAIN_BEGIN") {
      continue;
    }
    ReadBaseData(in, o);
    in >> o.scale.x >> o.scale.y >> o.scale.z;
    in >> o.rotate.x >> o.rotate.y >> o.rotate.z >> o.rotate.w;
    in >> o.translate.x >> o.translate.y >> o.translate.z;
    int s;
    in >> s;
    o.do_invert = (s != 0);
    in >> s;
    o.do_tiling = (s != 0);
    ReadNoiseBaseData(in, o.data.perlin);
    in >> o.data.perlin.scale_x >> o.data.perlin.scale_y >> o.data.perlin.seed;
    ReadNoiseBaseData(in, o.data.cellular);
    in >> o.data.cellular.scale_x >> o.data.cellular.scale_y >>
        o.data.cellular.jitter >> o.data.cellular.seed;
    ReadNoiseBaseData(in, o.data.metaballs);
    in >> o.data.metaballs.scale_x >> o.data.metaballs.scale_y >>
        o.data.metaballs.jitter >> o.data.metaballs.seed;
    ReadNoiseBaseData(in, o.data.fbm_grid);
    in >> o.data.fbm_grid.scale_x >> o.data.fbm_grid.scale_y >>
        o.data.fbm_grid.octaves >> o.data.fbm_grid.shift >>
        o.data.fbm_grid.gain >> o.data.fbm_grid.lacunarity >>
        o.data.fbm_grid.warp_strength >> o.data.fbm_grid.octave_factor >>
        o.data.fbm_grid.seed;
    ReadNoiseBaseData(in, o.data.fbm_multi);
    in >> o.data.fbm_multi.scale_x >> o.data.fbm_multi.scale_y >>
        o.data.fbm_multi.lacunarity >> o.data.fbm_multi.octaves >>
        o.data.fbm_multi.seed;
    ReadNoiseBaseData(in, o.data.fbmd_perlin);
    in >> o.data.fbmd_perlin.scale_x >> o.data.fbmd_perlin.scale_y >>
        o.data.fbmd_perlin.octaves >> o.data.fbmd_perlin.gain >>
        o.data.fbmd_perlin.lacunarity >> o.data.fbmd_perlin.slopeness >>
        o.data.fbmd_perlin.octave_factor >> o.data.fbmd_perlin.seed;
    ReadNoiseBaseData(in, o.data.fbm_warp);
    in >> o.data.fbm_warp.scale_x >> o.data.fbm_warp.scale_y >>
        o.data.fbm_warp.octaves >> o.data.fbm_warp.gain >>
        o.data.fbm_warp.lacunarity >> o.data.fbm_warp.slopeness >>
        o.data.fbm_warp.octave_factor >> o.data.fbm_warp.seed >>
        o.data.fbm_warp.q >> o.data.fbm_warp.r;
    ReadNoiseBaseData(in, o.data.fbm_perlin_warp);
    in >> o.data.fbm_perlin_warp.scale_x >> o.data.fbm_perlin_warp.scale_y >>
        o.data.fbm_perlin_warp.octaves >> o.data.fbm_perlin_warp.gain >>
        o.data.fbm_perlin_warp.lacunarity >> o.data.fbm_perlin_warp.slopeness >>
        o.data.fbm_perlin_warp.octave_factor >> o.data.fbm_perlin_warp.seed >>
        o.data.fbm_perlin_warp.q >> o.data.fbm_perlin_warp.r;
    terrain.push_back(std::move(o));
  }
  return terrain;
}

void WriteBaseData(std::ostream& out, const BaseInstanceData& data) {
  if (data.name.empty()) {
    out << "nameless" << '\n';
  } else {
    out << data.name << '\n';
  }
  out << data.color.r << ' ' << data.color.g << ' ' << data.color.b << ' '
      << data.color.a << '\n';
  out << (data.do_show ? 1 : 0) << "\n";
}

void WriteNoiseBaseData(std::ostream& out, const NoiseDataBase& data) {
  out << data.strength << ' ' << (data.do_invert ? 1 : 0) << ' '
      << (data.do_tiling ? 1 : 0) << '\n';
}

void WriteGraph(std::ostream& out, const GraphBakeConfig& graph) {
  out << graph.curve_amplitude << ' ' << graph.height_drift << ' '
      << graph.height_raise << ' ' << graph.side_sagging << ' ' << graph.radius
      << ' ' << graph.radius_flat << '\n';
}

void WriteOceanLayer(std::ostream& out, const OceanLayerTraits& layer) {
  out << layer.scale << ' ' << layer.fetch << ' ' << layer.wind << ' '
      << layer.spread_blend << ' ' << layer.swell << ' '
      << layer.peak_enhancement << ' ' << layer.short_waves_fade << ' '
      << layer.lambda << ' ' << (layer.visible ? 1 : 0) << '\n';
}

void WriteMapPoints(std::ostream& out,
                    const std::vector<MapPoint>& map_points) {
  out << map_points.size() << '\n';
  for (auto& p : map_points)
    out << p.position.x << ' ' << p.position.y << ' ' << (p.selected ? 1 : 0)
        << '\n';
}

void WriteMapJoints(std::ostream& out,
                    const std::vector<glm::uvec2>& map_joints) {
  out << map_joints.size() << '\n';
  for (auto& j : map_joints) out << j.x << ' ' << j.y << '\n';
}

void WriteObjects(std::ostream& out, const std::vector<ObjectTraits>& objects) {
  out << "OBJECTS " << objects.size() << "\n";
  for (const auto& o : objects) {
    out << "OBJECT_BEGIN\n";
    WriteBaseData(out, o);
    out << o.hp << '\n'
        << o.speed << '\n'
        << o.attack << '\n'
        << o.attack_speed << "\n";
    WriteMapPoints(out, o.map_points);
    WriteMapJoints(out, o.map_joints);
    out << o.rotations.size() << '\n';
    for (auto& r : o.rotations)
      out << r.x << ' ' << r.y << ' ' << r.z << ' ' << r.w << '\n';
    out << o.scales.size() << '\n';
    for (auto& s : o.scales) out << s.x << ' ' << s.y << ' ' << s.z << '\n';
  }
}

void WriteBiomes(std::ostream& out, const std::vector<BiomeTraits>& biomes) {
  out << "BIOMES " << biomes.size() << "\n";
  for (const auto& o : biomes) {
    out << "BIOME_BEGIN\n";
    WriteBaseData(out, o);
    out << o.wind_angle << '\n'
        << o.wind_speed << '\n'
        << o.sun_x << ' ' << o.sun_y << ' ' << o.sun_z << '\n'
        << o.sun_r << ' ' << o.sun_g << ' ' << o.sun_b << '\n';
    WriteMapPoints(out, o.map_points);
  }
}

void WriteRoads(std::ostream& out, const std::vector<RoadTraits>& roads) {
  out << "ROADS " << roads.size() << "\n";
  for (const auto& o : roads) {
    out << "ROAD_BEGIN\n";
    WriteBaseData(out, o);
    WriteGraph(out, o.config);
    WriteMapPoints(out, o.map_points);
    WriteMapJoints(out, o.map_joints);
  }
}

void WriteRivers(std::ostream& out, const std::vector<RiverTraits>& rivers) {
  out << "RIVERS " << rivers.size() << "\n";
  for (const auto& o : rivers) {
    out << "RIVER_BEGIN\n";
    WriteBaseData(out, o);
    WriteGraph(out, o.config);
    out << o.transparency << '\n' << o.viscosity << '\n';
    WriteMapPoints(out, o.map_points);
    WriteMapJoints(out, o.map_joints);
  }
}

void WriteOceans(std::ostream& out, const std::vector<OceanTraits>& oceans) {
  out << "OCEANS " << oceans.size() << "\n";
  for (const auto& o : oceans) {
    out << "OCEAN_BEGIN\n";
    WriteBaseData(out, o);
    WriteOceanLayer(out, o.near);
    WriteOceanLayer(out, o.mid);
    WriteOceanLayer(out, o.far);
    WriteMapPoints(out, o.map_points);
  }
}

void WriteTerrain(std::ostream& out,
                  const std::vector<TerrainTraits>& terrain) {
  out << "TERRAIN " << terrain.size() << "\n";
  for (const auto& o : terrain) {
    out << "TERRAIN_BEGIN\n";
    WriteBaseData(out, o);
    out << o.scale.x << ' ' << o.scale.y << ' ' << o.scale.z << '\n';
    out << o.rotate.x << ' ' << o.rotate.y << ' ' << o.rotate.z << ' '
        << o.rotate.w << '\n';
    out << o.translate.x << ' ' << o.translate.y << ' ' << o.translate.z
        << '\n';
    out << (o.do_tiling ? 1 : 0) << '\n';
    out << (o.do_invert ? 1 : 0) << '\n';

    WriteNoiseBaseData(out, o.data.perlin);
    out << o.data.perlin.scale_x << ' ' << o.data.perlin.scale_y << ' '
        << o.data.perlin.seed << '\n';

    WriteNoiseBaseData(out, o.data.cellular);
    out << o.data.cellular.scale_x << ' ' << o.data.cellular.scale_y << ' '
        << o.data.cellular.jitter << ' ' << o.data.cellular.seed << '\n';

    WriteNoiseBaseData(out, o.data.metaballs);
    out << o.data.metaballs.scale_x << ' ' << o.data.metaballs.scale_y << ' '
        << o.data.metaballs.jitter << ' ' << o.data.metaballs.seed << '\n';

    WriteNoiseBaseData(out, o.data.fbm_grid);
    out << o.data.fbm_grid.scale_x << ' ' << o.data.fbm_grid.scale_y << ' '
        << o.data.fbm_grid.octaves << ' ' << o.data.fbm_grid.shift << ' '
        << o.data.fbm_grid.gain << ' ' << o.data.fbm_grid.lacunarity << ' '
        << o.data.fbm_grid.warp_strength << ' ' << o.data.fbm_grid.octave_factor
        << ' ' << o.data.fbm_grid.seed << '\n';

    WriteNoiseBaseData(out, o.data.fbm_multi);
    out << o.data.fbm_multi.scale_x << ' ' << o.data.fbm_multi.scale_y << ' '
        << o.data.fbm_multi.lacunarity << ' ' << o.data.fbm_multi.octaves << ' '
        << o.data.fbm_multi.seed << '\n';

    WriteNoiseBaseData(out, o.data.fbmd_perlin);
    out << o.data.fbmd_perlin.scale_x << ' ' << o.data.fbmd_perlin.scale_y
        << ' ' << o.data.fbmd_perlin.octaves << ' ' << o.data.fbmd_perlin.gain
        << ' ' << o.data.fbmd_perlin.lacunarity << ' '
        << o.data.fbmd_perlin.slopeness << ' '
        << o.data.fbmd_perlin.octave_factor << ' ' << o.data.fbmd_perlin.seed
        << '\n';

    WriteNoiseBaseData(out, o.data.fbm_warp);
    out << o.data.fbm_warp.scale_x << ' ' << o.data.fbm_warp.scale_y << ' '
        << o.data.fbm_warp.octaves << ' ' << o.data.fbm_warp.gain << ' '
        << o.data.fbm_warp.lacunarity << ' ' << o.data.fbm_warp.slopeness << ' '
        << o.data.fbm_warp.octave_factor << ' ' << o.data.fbm_warp.seed << ' '
        << o.data.fbm_warp.q << ' ' << o.data.fbm_warp.r << '\n';

    WriteNoiseBaseData(out, o.data.fbm_perlin_warp);
    out << o.data.fbm_perlin_warp.scale_x << ' '
        << o.data.fbm_perlin_warp.scale_y << ' '
        << o.data.fbm_perlin_warp.octaves << ' ' << o.data.fbm_perlin_warp.gain
        << ' ' << o.data.fbm_perlin_warp.lacunarity << ' '
        << o.data.fbm_perlin_warp.slopeness << ' '
        << o.data.fbm_perlin_warp.octave_factor << ' '
        << o.data.fbm_perlin_warp.seed << ' ' << o.data.fbm_perlin_warp.q << ' '
        << o.data.fbm_perlin_warp.r << '\n';
  }
}
