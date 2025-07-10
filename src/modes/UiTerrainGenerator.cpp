#include "UiTerrainGenerator.h"

#include <stb_image_write.h>

const int UiTerrainGenerator::gTerrainWidth = 1024;
const int UiTerrainGenerator::gTerrainHeight = 1024;

UiTerrainGenerator::UiTerrainGenerator(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    Tile& cur_tile
    /*UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,

    UiStaticSprite&& btn_close,
    UiStaticSprite&& btn_accept,
    UiStaticSprite&& btn_decline,

    UiDynamicSprite&& cross*/)
    : Base(std::move(sprite), size_scale,
           std::move(pin), ui_shared_resources,
           window_queue),
      shader_gen_nmap_("../shaders/generate_shaders/NormalMap.comp"),
      shader_gen_slope_map_("../shaders/generate_shaders/SlopeMap.comp"),
      shader_gen_ao_map_("../shaders/generate_shaders/AoMap.comp"),
      shader_perturbate_("../shaders/generate_shaders/Perturbate.comp"),

      shader_terrain_cellular_("../shaders/noise_shaders/TerrainNoise.vert",
                               "../shaders/noise_shaders/Cellular.frag"),
      shader_terrain_fbmd_perlin_("../shaders/noise_shaders/TerrainNoise.vert",
                                  "../shaders/noise_shaders/FbmdPerlin.frag"),
      shader_terrain_fbm_grid_("../shaders/noise_shaders/TerrainNoise.vert",
                               "../shaders/noise_shaders/FbmGrid.frag"),
      shader_terrain_fbm_multi_("../shaders/noise_shaders/TerrainNoise.vert",
                                "../shaders/noise_shaders/FbmMulti.frag"),
      shader_terrain_fbm_perlin_warp_("../shaders/noise_shaders/TerrainNoise.vert",
                                      "../shaders/noise_shaders/FbmPerlinWarp.frag"),
      shader_terrain_fbm_warp_("../shaders/noise_shaders/TerrainNoise.vert",
                               "../shaders/noise_shaders/FbmWarp.frag"),
      shader_terrain_metaballs_("../shaders/noise_shaders/TerrainNoise.vert",
                                "../shaders/noise_shaders/Metaballs.frag"),
      shader_terrain_perlin_("../shaders/noise_shaders/TerrainNoise.vert",
                             "../shaders/noise_shaders/Perlin.frag"),

      tex_hmap_(cur_tile.map_terrain_height),
      tex_nmap_(cur_tile.map_terrain_normal),
      tex_slope_map_(cur_tile.map_erosion_deposition),
      tex_ao_map_(cur_tile.map_terrain_occlusion),
      tex_splat_map_(cur_tile.map_terrain_cavity),

      tex_erosion_thermal_map_(cur_tile.map_erosion_flow),
      tex_erosion_hydraulic_map_(cur_tile.map_erosion_wear),

      tex_water_accum_(cur_tile.map_terrain_wetness),
      tex_water_flow_(cur_tile.map_water_flow) {
  Init();
}

/*UiTerrainGenerator::UiTerrainGenerator(UiTerrainGenerator&& other) noexcept
    : Base(std::move(other)),
      btn_close_(std::move(other.btn_close_)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      cross_(std::move(other.cross_)),
      ui_event_handler_({&btn_close_, &btn_accept_, &btn_decline_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}*/

void UiTerrainGenerator::ErodeWithFlow(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum,
    int cycles) {
  glm::vec2 dim = glm::vec2(height_map_data_.size(), height_map_data_[0].size());
  float dt = 1.2f;
  float density = 1.0f;
  float evapRate = 0.001f;
  float depositionRate = 0.1f;
  float minVol = 0.01f;
  float friction = 0.05f;

  for(int i = 0; i < cycles; i++) {
    // Spawn droplet at a random position weighted by water accumulation to spawn more where water is abundant
    glm::vec2 newpos;
    {
      // Example: choose position randomly, but biased by water_accum
      // For simplicity, just random now
      newpos = glm::vec2(rand() % (int)dim.x, rand() % (int)dim.y);
    }

    Particle drop(newpos);
    drop.volume = water_accum[(int)newpos.y][(int)newpos.x];  // Use water accumulation as initial volume

    while(drop.volume > minVol) {
      glm::ivec2 ipos{static_cast<int>(drop.pos.x), static_cast<int>(drop.pos.y)};
      if (ipos.x < 0 || ipos.x >= (int)dim.x || ipos.y < 0 || ipos.y >= (int)dim.y) {
        break;
      }

      // Instead of updating speed by surface normal, move drop along flow_dir vector at current position
      glm::vec2 dir = flow_dir[ipos.y][ipos.x];  // assuming [row][col] access
      // Apply velocity along flow direction scaled by dt and drop volume and maybe some factor
      drop.speed.x = dir.x * dt * drop.volume;
      drop.speed.y = dir.y * dt * drop.volume;

      drop.pos.x += drop.speed.x;
      drop.pos.y += drop.speed.y;

      // Compute sediment capacity difference as before
      int x1 = ipos.x;
      int y1 = ipos.y;
      int x2 = (int)drop.pos.x;
      int y2 = (int)drop.pos.y;

      if (x2 < 0 || x2 >= (int)dim.x || y2 < 0 || y2 >= (int)dim.y) {
        break;
      }

      float heightDiff = height_map_data_[y1][x1] - height_map_data_[y2][x2];
      float maxSediment = drop.volume * glm::length(drop.speed) * std::max(0.0f, heightDiff);

      float sdiff = maxSediment - drop.sediment;

      // Deposit or erode sediment
      drop.sediment += dt * depositionRate * sdiff;
      height_map_data_[y1][x1] -= dt * drop.volume * depositionRate * sdiff;
      height_map_data_[y1][x1] = std::max(0.0f, height_map_data_[y1][x1]);

      // Evaporate
      drop.volume *= (1.0f - dt * evapRate);
    }
  }
}


glm::vec3 UiTerrainGenerator::SurfaceNormal(int i, int j) {
  //"Physical" Height scaling of the map
  double scale = 16.0f * 16.0f;
  /*
    Note: Surface normal is computed in this way, because the square-grid
    surface is meshed using triangles. To avoid spatial artifacts,
    you need to weight properly with all neighbors.
  */
  i = std::clamp(i, 1, static_cast<int>(height_map_data_[0].size()) - 2);
  j = std::clamp(j, 1, static_cast<int>(height_map_data_.size()) - 2);
  glm::vec3 n = glm::normalize(glm::vec3(scale*(height_map_data_[i][j]-height_map_data_[i+1][j]), 1.0, 0.0)) * glm::vec3(0.15);  //Positive X
  n += glm::normalize(glm::vec3(scale*(height_map_data_[i-1][j]-height_map_data_[i][j]), 1.0, 0.0)) * glm::vec3(0.15);  //Negative X
  n += glm::normalize(glm::vec3(0.0, 1.0, scale*(height_map_data_[i][j]-height_map_data_[i][j+1]))) * glm::vec3(0.15);    //Positive Y
  n += glm::normalize(glm::vec3(0.0, 1.0, scale*(height_map_data_[i][j-1]-height_map_data_[i][j]))) * glm::vec3(0.15);  //Negative Y

  //Diagonals! (This removes the last spatial artifacts)
  n += glm::normalize(
           glm::vec3(scale*(height_map_data_[i][j]-height_map_data_[i+1][j+1])/sqrt(2), sqrt(2),scale*(height_map_data_[i][j]-height_map_data_[i+1][j+1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(height_map_data_[i][j]-height_map_data_[i+1][j-1])/sqrt(2), sqrt(2), scale*(height_map_data_[i][j]-height_map_data_[i+1][j-1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(height_map_data_[i][j]-height_map_data_[i-1][j+1])/sqrt(2), sqrt(2), scale*(height_map_data_[i][j]-height_map_data_[i-1][j+1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(height_map_data_[i][j]-height_map_data_[i-1][j-1])/sqrt(2), sqrt(2), scale*(height_map_data_[i][j]-height_map_data_[i-1][j-1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y

  return n;
}

void UiTerrainGenerator::ErodeWeathering(int cycles) {
  //Size of the heightmap array
  glm::vec2 dim = glm::vec2(gTerrainWidth, gTerrainHeight);
  //Erosion Steps
  //  int remaining = 200000;
  //  int erosionstep = 1000;

  //Particle Properties
  float dt = 1.2;
  float density = 1.0;  //This gives varying amounts of inertia and stuff...
  float evapRate = 0.001;
  //  float depositionRate = 1.0f;
  float depositionRate = 0.1;

  float minVol = 0.01;
  float friction = 0.05;

  for(int i = 0; i < cycles; i++){

    //Spawn New Particle
    glm::vec2 newpos = glm::vec2(rand()%(int)dim.x, rand()%(int)dim.y);
    Particle drop(newpos);
    std::cout << "cycle " << i << std::endl;

    //As long as the droplet exists...
    while(drop.volume > minVol){
      glm::ivec2 ipos{static_cast<int>(drop.pos.x),
                      static_cast<int>(drop.pos.y)};
      //Floored Droplet Initial Position
      //Surface Normal at Position
      glm::vec3 n = SurfaceNormal(ipos.x, ipos.y);

      //Accelerate particle using newtonian mechanics using the surface normal.
      //F = ma, so a = F/m
      drop.speed.x += n.x * dt/(drop.volume*density);
      drop.speed.y += n.z * dt/(drop.volume*density);
      drop.pos.x   += drop.speed.x;
      drop.pos.y   += drop.speed.y;
      drop.speed.x *= (1.0-dt*friction);       //Friction Factor
      drop.speed.y *= (1.0-dt*friction);       //Friction Factor

      if (!(drop.pos.x >= 0.0f && drop.pos.y >= 0.0f &&
            drop.pos.x < dim.x && drop.pos.y < dim.y))
        break;

      //Compute sediment capacity difference
      float maxsediment =
          drop.volume * glm::length(drop.speed)
          * (height_map_data_[ipos.x][ipos.y]
             - height_map_data_[(int)drop.pos.x][(int)drop.pos.y]);
      if(maxsediment < 0.0) maxsediment = 0.0;
      float sdiff = maxsediment - drop.sediment;

      //Act on the Heightmap and Droplet!
      drop.sediment += dt*depositionRate*sdiff;
      height_map_data_[ipos.x][ipos.y] -= dt*drop.volume*depositionRate*sdiff;
      height_map_data_[ipos.x][ipos.y] =
          std::max(0.0f, height_map_data_[ipos.x][ipos.y]);

      //Evaporate the Droplet (Note: Proportional to Volume!
      // Better: Use shape factor to make proportional to the area instead.)
      drop.volume *= (1.0-dt*evapRate);
    }
  }
}


void UiTerrainGenerator::ProcessErosion(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum,
    int iterations) {
  int height = height_map_data_.size();
  int width = height_map_data_[0].size();

  auto start_height_map_data = height_map_data_;

  ErodeWithFlow(flow_dir, water_accum, iterations);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> height_map_data_updated(width * height);
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = -height_map_data_[i][j] + start_height_map_data[i][j];
      height_map_data_updated[i * width + j] =
          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_hydraulic_map_ = Texture(gTerrainWidth, gTerrainHeight,
                                       GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_erosion_hydraulic_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, height_map_data_updated.data());
  tex_erosion_hydraulic_map_.Store("erosion_wear.png", 1, GL_RED,
                                   GL_UNSIGNED_BYTE);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data_[i][j];
      height_map_data_updated[i * width + j] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8, GL_LINEAR,
                      GL_CLAMP_TO_EDGE);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, height_map_data_updated.data());
  tex_hmap_.Store("height_map_erosed.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::ProcessThermalWeathering(
    int iterations, float talus) {
  int height = height_map_data_.size();
  int width = height_map_data_[0].size();

  auto start_height_map_data = height_map_data_;

  ErodeWeathering(iterations);
  //  RunThermalWeathering(height_map_data, 50, 0.2f);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> height_map_data_updated(width * height);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data_[i][j] - start_height_map_data[i][j];
      height_map_data_updated[i * width + j] =
          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_thermal_map_ = Texture(gTerrainWidth, gTerrainHeight,
                                     GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_erosion_thermal_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0,
               GL_RED, GL_UNSIGNED_BYTE, height_map_data_updated.data());
  tex_erosion_thermal_map_.Store("weathering_wear.png", 1,
                                 GL_RED, GL_UNSIGNED_BYTE);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = height_map_data_[i][j];
      height_map_data_updated[i * width + j] =
          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8, GL_LINEAR,
                      GL_CLAMP_TO_EDGE);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, height_map_data_updated.data());
  tex_hmap_.Store("height_map_weathered.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::GenerateSplatmap() {}

void UiTerrainGenerator::GenerateNmap() {
  shader_gen_nmap_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_nmap_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_nmap_.GetFormat());
  glDispatchCompute(gTerrainWidth / 8, gTerrainHeight / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_nmap_.Store("normal_map.png", 2, GL_RG, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::GenerateSlope() {
  shader_gen_slope_map_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_slope_map_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_slope_map_.GetFormat());
  glDispatchCompute(gTerrainWidth / 8, gTerrainHeight / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_slope_map_.Store("slope_map.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::GenerateAo() {
  shader_gen_ao_map_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_ao_map_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_ao_map_.GetFormat());
  glDispatchCompute(gTerrainWidth / 8, gTerrainHeight / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_ao_map_.Store("ao_map.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::Perturbate() {
  Texture tex_hmap_new(gTerrainWidth, gTerrainHeight, GL_R8);
  shader_perturbate_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_hmap_new.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_hmap_new.GetFormat());
  glDispatchCompute(gTerrainWidth / 8, gTerrainHeight / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_hmap_ = std::move(tex_hmap_new);
  tex_hmap_.Store("tex_hmap_perturbated.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::ComputeFlowMaps(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int height = height_map_data_.size();
  int width = height_map_data_[0].size();

  flow_dir.assign(height, std::vector<glm::vec2>(width, {0.0f, 0.0f}));
  // Start with 1 unit of water
  flow_accum.assign(height, std::vector<float>(width, 1.0f));

  // Step 1: Flow direction
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      float h0 = height_map_data_[y][x];
      float maxSlope = 0.0f;
      glm::vec2 best = {0.0f, 0.0f};

      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dx == 0 && dy == 0) continue;

          float h1 = height_map_data_[y + dy][x + dx];
          float drop = h0 - h1;
          float dist = std::sqrt(dx * dx + dy * dy);
          float slope = drop / dist;

          if (slope > maxSlope) {
            maxSlope = slope;
            best = { (float)dx, (float)dy };
          }
        }
      }

      float len = std::sqrt(best.x * best.x + best.y * best.y);
      if (len > 0.0f)
        flow_dir[y][x] = { best.x / len, best.y / len };
    }
  }

  // Step 2: Flow accumulation (simple topological sort by descending height)
  struct Cell {
    int x, y;
    float height;
  };
  std::vector<Cell> cells;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      cells.push_back({x, y, height_map_data_[y][x]});

  std::sort(cells.begin(), cells.end(), [](const Cell& a, const Cell& b) {
    return a.height > b.height;
  });

  for (auto& cell : cells) {
    int x = cell.x;
    int y = cell.y;
    int nx = x + std::round(flow_dir[y][x].x);
    int ny = y + std::round(flow_dir[y][x].y);
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      flow_accum[ny][nx] += 0.1f * flow_accum[y][x];
    }
  }
}

void UiTerrainGenerator::GenerateFlowMap(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int height = height_map_data_.size();
  int width = height_map_data_[0].size();
  ComputeFlowMaps(flow_dir, flow_accum);

  //  int width = flow_accum[0].size();
  //  int height = flow_accum.size();

  // flow_dir: RGB -> (dx, dy, 0)
  std::vector<float> flow_dir_data(width * height * 3);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      int idx = (y * width + x) * 3;
      flow_dir_data[idx + 0] = flow_dir[y][x].x * 0.5f + 0.5f; // map [-1,1] to [0,1]
      flow_dir_data[idx + 1] = flow_dir[y][x].y * 0.5f + 0.5f;
      flow_dir_data[idx + 2] = 0.0f; // unused
    }

  // flow_accum: single channel
  std::vector<float> flow_accum_data(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // tone-mapped

      flow_accum_data[y * width + x] = std::log(1.0f + flow_accum[y][x]);
    }
  }

  GLuint flowDirTex, flowAccumTex;

  // Upload flowDirTex (RGB)
  glGenTextures(1, &flowDirTex);
  glBindTexture(GL_TEXTURE_2D, flowDirTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
               GL_FLOAT, flow_dir_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Upload flowAccumTex (R)
  glGenTextures(1, &flowAccumTex);
  glBindTexture(GL_TEXTURE_2D, flowAccumTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
               GL_FLOAT, flow_accum_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Convert float -> uint8 for saving
  std::vector<uint8_t> flowDirData_u8(width * height * 3);
  std::vector<uint8_t> flowAccumData_u8(width * height);

  for (int i = 0; i < width * height; ++i) {
    // Clamp and convert each channel
    for (int c = 0; c < 3; ++c) {
      float v = flow_dir_data[i * 3 + c];
      flowDirData_u8[i * 3 + c] = static_cast<uint8_t>(std::clamp(v, 0.0f, 1.0f) * 255.0f);
    }

    float acc = flow_accum_data[i];
    flowAccumData_u8[i] = static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
  }


  tex_water_flow_ = Texture(gTerrainWidth, gTerrainHeight, GL_RGB8,
                            GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_water_flow_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, flowDirData_u8.data());
  tex_water_flow_.Store("water_flow.png", 3, GL_RGB, GL_UNSIGNED_BYTE);

  tex_water_accum_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8,
                             GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_water_accum_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0,
               GL_RED, GL_UNSIGNED_BYTE, flowAccumData_u8.data());
  tex_water_accum_.Store("water_accum.png", 1, GL_RED, GL_UNSIGNED_BYTE);
}

void UiTerrainGenerator::GenerateHmap() {
  int width = tex_hmap_.GetWidth();
  int height = tex_hmap_.GetHeight();

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
  glViewport(0, 0, width, height);
  glBindVertexArray(vao_id_);

  std::vector<unsigned char> buffer(width * height);
  std::cout << "CH0 (start)" << std::endl;
  Texture tex_cellular = HmapFromNoise(shader_terrain_cellular_,
                                       buffer, "hmap_cellular.png");
//  tex_hmap_ = HmapFromNoise(
//      shader_terrain_cellular_, buffer, "hmap_cellular.png");
  Texture tex_fbmd_perlin = HmapFromNoise(
      shader_terrain_fbmd_perlin_, buffer, "hmap_fbmd_perlin.png");
  Texture tex_fbm_grid = HmapFromNoise(
      shader_terrain_fbm_grid_, buffer, "hmap_fbm_grid.png");
  Texture tex_fbm_multi = HmapFromNoise(
      shader_terrain_fbm_multi_, buffer, "hmap_fbm_multi.png");
  Texture tex_fbm_perlin_warp = HmapFromNoise(
      shader_terrain_fbm_perlin_warp_, buffer, "hmap_fbm_perlin_warp.png");
  tex_hmap_ = HmapFromNoise(
      shader_terrain_fbm_perlin_warp_, buffer, "hmap_fbm_perlin_warp.png");
  Texture tex_fbm_warp = HmapFromNoise(
      shader_terrain_fbm_warp_, buffer, "hmap_fbm_warp.png");
  Texture tex_metaballs = HmapFromNoise(
      shader_terrain_metaballs_, buffer, "hmap_metaballs.png");
  Texture tex_perlin = HmapFromNoise(
      shader_terrain_perlin_, buffer, "hmap_perlin.png");
  tex_hmap_.Store("hmap_start.png", 1, GL_RED, GL_UNSIGNED_BYTE);
  std::cout << "CH1" << std::endl;
}

Texture UiTerrainGenerator::HmapFromNoise(
    const Shader& shader,
    std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader.Bind();
  glUniform2f(1, static_cast<float>(gTerrainWidth),
              static_cast<float>(gTerrainHeight));
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glReadPixels(0, 0, gTerrainWidth, gTerrainHeight, GL_RED, GL_UNSIGNED_BYTE,
               buffer.data());

  Texture height_map(gTerrainWidth, gTerrainHeight, GL_R8, GL_LINEAR,
                     GL_CLAMP_TO_EDGE);
  height_map.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, gTerrainWidth, gTerrainHeight,
               0, GL_RED, GL_UNSIGNED_BYTE, buffer.data());
  height_map.Store(tex_name, 1, GL_RED, GL_UNSIGNED_BYTE);
  return std::move(height_map);
}

void UiTerrainGenerator::Init() {
  height_map_data_.resize(gTerrainHeight);
  for (auto& row : height_map_data_) {
    row.resize(gTerrainWidth);
  }

  tex_hmap_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);
  tex_nmap_ = Texture(gTerrainWidth, gTerrainHeight, GL_RG8);
  tex_slope_map_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);
  tex_ao_map_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);
  tex_splat_map_ = Texture(gTerrainWidth, gTerrainHeight, GL_RGBA);

  tex_erosion_thermal_map_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);
  tex_erosion_hydraulic_map_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);

  tex_water_accum_ = Texture(gTerrainWidth, gTerrainHeight, GL_R8);
  tex_water_flow_ = Texture(gTerrainWidth, gTerrainHeight, GL_RG8);

  float vertices[] = {
      1.0f,  1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
  };
  unsigned int indices[] = {
      0, 1, 3,
      1, 2, 3
  };
  glGenVertexArrays(1, &vao_id_);
  glGenBuffers(1, &vbo_id_);
  glGenBuffers(1, &ebo_id_);

  glBindVertexArray(vao_id_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenTextures(1, &fbo_tex_id_);
  glBindTexture(GL_TEXTURE_2D, fbo_tex_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, gTerrainWidth, gTerrainHeight,
               0, GL_RED, GL_FLOAT, nullptr);

  glGenFramebuffers(1, &fbo_id_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, fbo_tex_id_, 0);
}

void UiTerrainGenerator::DeInit() {
  glDeleteBuffers(1, &vbo_id_);
  glDeleteBuffers(1, &ebo_id_);
  glDeleteTextures(1, &fbo_tex_id_);
  glDeleteFramebuffers(1, &fbo_id_);
  glDeleteVertexArrays(1, &vao_id_);
}

void UiTerrainGenerator::UpdateCpuData() {
  tex_hmap_.Bind();
  std::vector<unsigned char> height_map_data_ui(gTerrainWidth * gTerrainHeight);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                height_map_data_ui.data());
  for (int y = 0; y < gTerrainHeight; ++y) {
    for (int x = 0; x < gTerrainWidth; ++x) {
      height_map_data_[y][x] =
          height_map_data_ui[y * gTerrainWidth + x] / 255.0f;
    }
  }
}

void UiTerrainGenerator::Bake() {
  GenerateHmap();
  std::cout << "CH2" << std::endl;
  UpdateCpuData();

  Perturbate();
  std::cout << "CH3" << std::endl;
  UpdateCpuData();

  std::vector<std::vector<glm::vec2>> flow_dir;
  std::vector<std::vector<float>> flow_accum;
  GenerateFlowMap(flow_dir, flow_accum);
  std::cout << "CH4" << std::endl;

  /// erosion & thermal weathering
  ProcessErosion(flow_dir, flow_accum, 10000);
  std::cout << "CH5" << std::endl;
  UpdateCpuData();

  ProcessThermalWeathering(10000, 0.1f);
  std::cout << "CH6" << std::endl;
  UpdateCpuData();

  GenerateNmap();
  std::cout << "CH7" << std::endl;
  GenerateSlope();
  std::cout << "CH8" << std::endl;
  GenerateAo();
  std::cout << "CH9 (final)" << std::endl;

  tex_hmap_.Store("hmap_end.png", 1, GL_RED, GL_UNSIGNED_BYTE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glBindVertexArray(0);
}
