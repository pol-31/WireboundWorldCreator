#include "UiTerrainBake.h"

#include <iostream>

#include "../core/TileRenderer.h"

UiTerrainBake::UiTerrainBake(
    Tile& cur_tile, UiSprite&& sprite, float size_scale, UiToggle2&& pin,
    UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
    TextRenderer& text_renderer, UiSprite&& accept,
    UiTextModeId&& erosion_label, UiTextInput&& erosion_input,
    UiTextModeId&& weathering_label, UiTextInput&& weathering_input)
    : Base(std::move(sprite), size_scale, std::move(pin), ui_shared_resources,
           window_queue),
      accept_(std::move(accept)),
      erosion_label_(std::move(erosion_label)),
      erosion_input_(std::move(erosion_input)),
      weathering_label_(std::move(weathering_label)),
      weathering_input_(std::move(weathering_input)),
      sprite_hmap_(data::VboIdMain::kTerrainBakeHmap),
      ui_event_handler_({&pin_, &accept_, &erosion_input_, &weathering_input_}),
      ui_shared_resources_(ui_shared_resources),

      shader_gen_nmap_("../shaders/generate_shaders/NormalMap.comp"),
      shader_gen_slope_map_("../shaders/generate_shaders/SlopeMap.comp"),
      shader_gen_splat_map_("../shaders/generate_shaders/SplatMap.comp"),
      shader_gen_ao_map_("../shaders/generate_shaders/AoMap.comp"),
      shader_perturbate_("../shaders/generate_shaders/Perturbate.comp"),

      hmap_heights_(cur_tile.terrain_heights_),
      tex_hmap_(cur_tile.map_terrain_height),

      tex_nmap_(cur_tile.map_terrain_normal),
      tex_slope_map_(cur_tile.map_terrain_slope),
      tex_ao_map_(cur_tile.map_terrain_ao),
      tex_splat_map_(cur_tile.map_terrain_splat),

      tex_erosion_thermal_map_(cur_tile.map_terrain_erosion_thermal),
      tex_erosion_hydraulic_map_(cur_tile.map_terrain_erosion_hydraulic),
      tex_water_accum_(cur_tile.map_water_accum),
      tex_water_flow_(cur_tile.map_water_flow),
      hierarchy_(&background_,
                 {&pin_, &accept_, &erosion_label_, &erosion_input_,
                  &weathering_label_, &weathering_input_, &sprite_hmap_}) {
  speed_ = 2.0f;
}

bool UiTerrainBake::Press(int id) { return ui_event_handler_.Press(id); }

void UiTerrainBake::Release() { ui_event_handler_.Release(); }

bool UiTerrainBake::Scroll(GLuint id, float yoffset) { return false; }

bool UiTerrainBake::Render() {
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }
  ui_shared_resources_.shader_sp_hmap_.Bind();

  ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_terrain_height
      .Bind();
  sprite_hmap_.Render();

  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();

  accept_.Render();
  erosion_input_.RenderBack();
  weathering_input_.RenderBack();

  // text section

  erosion_label_.Render();
  weathering_label_.Render();

  erosion_input_.RenderText();
  weathering_input_.RenderText();
  return false;
}

void UiTerrainBake::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.shader_sp_picking_.Bind();

  accept_.RenderPicking();

  sprite_hmap_.RenderPicking();

  erosion_input_.RenderPicking();
  weathering_input_.RenderPicking();

  ui_shared_resources_.shader_sp_picking_.Bind();
  erosion_label_.RenderPicking();
  ui_shared_resources_.shader_sp_picking_.Bind();
  weathering_label_.RenderPicking();
}

void UiTerrainBake::ErodeWithFlow(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum, int cycles) {
  int size = details::gTerrainSize;
  glm::vec2 dim = glm::vec2(size);
  float dt = 1.2f;
  float density = 1.0f;
  float evapRate = 0.001f;
  float depositionRate = 0.1f;
  float minVol = 0.01f;
  float friction = 0.05f;

  for (int i = 0; i < cycles; i++) {
    // Spawn droplet at a random position weighted by water accumulation to
    // spawn more where water is abundant
    glm::vec2 newpos;
    {
      // Example: choose position randomly, but biased by water_accum
      // For simplicity, just random now
      newpos = glm::vec2(rand() % (int)dim.x, rand() % (int)dim.y);
    }

    Particle drop(newpos);
    drop.volume =
        water_accum[(int)newpos.y]
                   [(int)newpos.x];  // Use water accumulation as initial volume

    while (drop.volume > minVol) {
      glm::ivec2 ipos{static_cast<int>(drop.pos.x),
                      static_cast<int>(drop.pos.y)};
      if (ipos.x < 0 || ipos.x >= (int)dim.x || ipos.y < 0 ||
          ipos.y >= (int)dim.y) {
        break;
      }

      // Instead of updating speed by surface normal, move drop along flow_dir
      // vector at current position
      glm::vec2 dir = flow_dir[ipos.y][ipos.x];  // assuming [row][col] access
      // Apply velocity along flow direction scaled by dt and drop volume and
      // maybe some factor
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

      float heightDiff =
          hmap_heights_[y1 * size + x1] - hmap_heights_[y2 * size + x2];
      float maxSediment =
          drop.volume * glm::length(drop.speed) * std::max(0.0f, heightDiff);

      float sdiff = maxSediment - drop.sediment;

      // Deposit or erode sediment
      drop.sediment += dt * depositionRate * sdiff;
      hmap_heights_[y1 * size + x1] -=
          dt * drop.volume * depositionRate * sdiff;
      hmap_heights_[y1 * size + x1] =
          std::max(0.0f, hmap_heights_[y1 * size + x1]);

      // Evaporate
      drop.volume *= (1.0f - dt * evapRate);
    }
  }
}

glm::vec3 UiTerrainBake::SurfaceNormal(int i, int j) {
  //"Physical" Height scaling of the map
  int size = details::gTerrainSize;
  double scale = 16.0f * 16.0f;
  /*
    Note: Surface normal is computed in this way, because the square-grid
    surface is meshed using triangles. To avoid spatial artifacts,
    you need to weight properly with all neighbors.
  */
  i = std::clamp(i, 1, size - 2);
  j = std::clamp(j, 1, size - 2);
  glm::vec3 n =
      glm::normalize(glm::vec3(scale * (hmap_heights_[i * size + j] -
                                        hmap_heights_[(i + 1) * size + j]),
                               1.0, 0.0)) *
      glm::vec3(0.15);  // Positive X
  n += glm::normalize(glm::vec3(scale * (hmap_heights_[(i - 1) * size + j] -
                                         hmap_heights_[i * size + j]),
                                1.0, 0.0)) *
       glm::vec3(0.15);  // Negative X
  n += glm::normalize(glm::vec3(0.0, 1.0,
                                scale * (hmap_heights_[i * size + j] -
                                         hmap_heights_[i * size + j + 1]))) *
       glm::vec3(0.15);  // Positive Y
  n += glm::normalize(glm::vec3(0.0, 1.0,
                                scale * (hmap_heights_[i * size + j - 1] -
                                         hmap_heights_[i * size + j]))) *
       glm::vec3(0.15);  // Negative Y

  // Diagonals! (This removes the last spatial artifacts)
  n += glm::normalize(glm::vec3(scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i + 1) * size + j + 1]) /
                                    sqrt(2),
                                sqrt(2),
                                scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i + 1) * size + j + 1]) /
                                    sqrt(2))) *
       glm::vec3(0.1);  // Positive Y
  n += glm::normalize(glm::vec3(scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i + 1) * size + j - 1]) /
                                    sqrt(2),
                                sqrt(2),
                                scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i + 1) * size + j - 1]) /
                                    sqrt(2))) *
       glm::vec3(0.1);  // Positive Y
  n += glm::normalize(glm::vec3(scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i - 1) * size + j + 1]) /
                                    sqrt(2),
                                sqrt(2),
                                scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i - 1) * size + j + 1]) /
                                    sqrt(2))) *
       glm::vec3(0.1);  // Positive Y
  n += glm::normalize(glm::vec3(scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i - 1) * size + j - 1]) /
                                    sqrt(2),
                                sqrt(2),
                                scale *
                                    (hmap_heights_[i * size + j] -
                                     hmap_heights_[(i - 1) * size + j - 1]) /
                                    sqrt(2))) *
       glm::vec3(0.1);  // Positive Y

  return n;
}

void UiTerrainBake::ErodeWeathering(int cycles) {
  // Size of the heightmap array
  int size = details::gTerrainSize;
  glm::vec2 dim = glm::vec2(size);
  // Erosion Steps
  //   int remaining = 200000;
  //   int erosionstep = 1000;

  // Particle Properties
  float dt = 1.2;
  float density = 1.0;  // This gives varying amounts of inertia and stuff...
  float evapRate = 0.001;
  //  float depositionRate = 1.0f;
  float depositionRate = 0.1;

  float minVol = 0.01;
  float friction = 0.05;

  for (int i = 0; i < cycles; i++) {
    // Spawn New Particle
    glm::vec2 newpos = glm::vec2(rand() % (int)dim.x, rand() % (int)dim.y);
    Particle drop(newpos);
    std::cout << "cycle " << i << std::endl;

    // As long as the droplet exists...
    while (drop.volume > minVol) {
      glm::ivec2 ipos{static_cast<int>(drop.pos.x),
                      static_cast<int>(drop.pos.y)};
      // Floored Droplet Initial Position
      // Surface Normal at Position
      glm::vec3 n = SurfaceNormal(ipos.x, ipos.y);

      // Accelerate particle using newtonian mechanics using the surface normal.
      // F = ma, so a = F/m
      drop.speed.x += n.x * dt / (drop.volume * density);
      drop.speed.y += n.z * dt / (drop.volume * density);
      drop.pos.x += drop.speed.x;
      drop.pos.y += drop.speed.y;
      drop.speed.x *= (1.0 - dt * friction);  // Friction Factor
      drop.speed.y *= (1.0 - dt * friction);  // Friction Factor

      if (!(drop.pos.x >= 0.0f && drop.pos.y >= 0.0f && drop.pos.x < dim.x &&
            drop.pos.y < dim.y))
        break;

      // Compute sediment capacity difference
      float maxsediment =
          drop.volume * glm::length(drop.speed) *
          (hmap_heights_[(ipos.x) * size + ipos.y] -
           hmap_heights_[((int)drop.pos.x) * size + (int)drop.pos.y]);
      if (maxsediment < 0.0) maxsediment = 0.0;
      float sdiff = maxsediment - drop.sediment;

      // Act on the Heightmap and Droplet!
      drop.sediment += dt * depositionRate * sdiff;
      hmap_heights_[(ipos.x) * size + ipos.y] -=
          dt * drop.volume * depositionRate * sdiff;
      hmap_heights_[(ipos.x) * size + ipos.y] =
          std::max(0.0f, hmap_heights_[(ipos.x) * size + ipos.y]);

      // Evaporate the Droplet (Note: Proportional to Volume!
      //  Better: Use shape factor to make proportional to the area instead.)
      drop.volume *= (1.0 - dt * evapRate);
    }
  }
}

void UiTerrainBake::ProcessErosion(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum, int iterations) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  auto start_height_map_data = hmap_heights_;

  ErodeWithFlow(flow_dir, water_accum, iterations);

  // Convert float -> uint8 for saving
  std::vector<float> hmap_heights_updated(width * height);
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc =
          -hmap_heights_[i * size + j] + start_height_map_data[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
      //          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_hydraulic_map_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_erosion_hydraulic_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               hmap_heights_updated.data());
  // tex_erosion_hydraulic_map_.Store("erosion_wear.png", 1, GL_RED);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = hmap_heights_[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
      //          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               hmap_heights_updated.data());
  // tex_hmap_.Store("height_map_erosed.png", 1, GL_RED);
}

void UiTerrainBake::ProcessThermalWeathering(int iterations, float talus) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  auto start_height_map_data = hmap_heights_;

  ErodeWeathering(iterations);
  //  RunThermalWeathering(height_map_data, 50, 0.2f);

  // Convert float -> uint8 for saving
  std::vector<float> hmap_heights_updated(width * height);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc =
          hmap_heights_[i * size + j] - start_height_map_data[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
      //          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_thermal_map_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_erosion_thermal_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               hmap_heights_updated.data());
  // tex_erosion_thermal_map_.Store("weathering_wear.png", 1, GL_RED);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = hmap_heights_[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
      //          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               hmap_heights_updated.data());
  // tex_hmap_.Store("height_map_weathered.png", 1, GL_RED);
}

void UiTerrainBake::GenerateSplatmap() {
  shader_gen_splat_map_.Bind();
  glBindImageTexture(0, tex_splat_map_.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     tex_splat_map_.GetFormat());
  glBindImageTexture(1, tex_slope_map_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     tex_slope_map_.GetFormat());
  glBindImageTexture(2, tex_hmap_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     tex_hmap_.GetFormat());
  // glBindImageTexture(
  // 1, tex_water_hmap_.GetId(), 0,
  // GL_FALSE, 0, GL_WRITE_ONLY, tex_slope_map_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  // tex_splat_map_.Store("splat_map.png", 1, GL_RED);
}

void UiTerrainBake::GenerateNmap() {
  shader_gen_nmap_.Bind();
  glBindImageTexture(0, tex_hmap_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     GL_R32F);
  glBindImageTexture(1, tex_nmap_.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     tex_nmap_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_nmap_.Store("normal_map.png", 2, GL_RG, 1);
}

void UiTerrainBake::GenerateSlope() {
  shader_gen_slope_map_.Bind();
  glBindImageTexture(0, tex_hmap_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     tex_hmap_.GetFormat());
  glBindImageTexture(1, tex_slope_map_.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     tex_slope_map_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_slope_map_.Store("slope_map.png", 1, GL_RED);
}

void UiTerrainBake::GenerateAo() {
  shader_gen_ao_map_.Bind();
  glBindImageTexture(0, tex_hmap_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     tex_hmap_.GetFormat());
  glBindImageTexture(1, tex_ao_map_.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     tex_ao_map_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_ao_map_.Store("ao_map.png", 1, GL_RED);
}

void UiTerrainBake::Perturbate() {
  Texture32F tex_hmap_new(details::gTerrainSize, GL_R32F);
  shader_perturbate_.Bind();
  glBindImageTexture(0, tex_hmap_.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY,
                     tex_hmap_.GetFormat());
  glBindImageTexture(1, tex_hmap_new.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     tex_hmap_new.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_hmap_ = std::move(tex_hmap_new);
  // tex_hmap_.Store("tex_hmap_perturbated.png", 1, GL_RED);
}

void UiTerrainBake::ComputeFlowMaps(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  flow_dir.assign(height, std::vector<glm::vec2>(width, {0.0f, 0.0f}));
  // Start with 1 unit of water
  flow_accum.assign(height, std::vector<float>(width, 1.0f));

  // Step 1: Flow direction
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      float h0 = hmap_heights_[y * size + x];
      float maxSlope = 0.0f;
      glm::vec2 best = {0.0f, 0.0f};

      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dx == 0 && dy == 0) continue;

          float h1 = hmap_heights_[(y + dy) * size + x + dx];
          float drop = h0 - h1;
          float dist = std::sqrt(dx * dx + dy * dy);
          float slope = drop / dist;

          if (slope > maxSlope) {
            maxSlope = slope;
            best = {(float)dx, (float)dy};
          }
        }
      }

      float len = std::sqrt(best.x * best.x + best.y * best.y);
      if (len > 0.0f) flow_dir[y][x] = {best.x / len, best.y / len};
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
      cells.push_back({x, y, hmap_heights_[y * size + x]});

  std::sort(cells.begin(), cells.end(),
            [](const Cell& a, const Cell& b) { return a.height > b.height; });

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

void UiTerrainBake::GenerateFlowMap(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;
  ComputeFlowMaps(flow_dir, flow_accum);

  //  int width = flow_accum[0].size();
  //  int height = flow_accum.size();

  // flow_dir: RGB -> (dx, dy, 0)
  std::vector<GLuint> flow_dir_data(width * height * 2);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      int idx = (y * width + x) * 2;
      flow_dir_data[idx + 0] = static_cast<GLuint>(
          (flow_dir[y][x].x * 0.5f + 0.5f) * 255);  // map [-1,1] to [0,1]
      flow_dir_data[idx + 1] =
          static_cast<GLuint>((flow_dir[y][x].y * 0.5f + 0.5f) * 255);
    }

  // flow_accum: single channel
  std::vector<float> flow_accum_data(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      flow_accum_data[y * width + x] = std::log(1.0f + flow_accum[y][x]);
    }
  }

  tex_water_flow_ = Texture(details::gTerrainSize, details::gTerrainSize,
                            GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_water_flow_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG,
               GL_UNSIGNED_BYTE, flow_dir_data.data());
  tex_water_flow_.Store("water_flow.png", 3, GL_RGB);

  tex_water_accum_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_water_accum_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               flow_accum_data.data());
  tex_water_accum_.Store("water_accum.png", 1, GL_RED);
}

void UiTerrainBake::UpdateCpuData() {
  tex_hmap_.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, hmap_heights_.data());
}

void UiTerrainBake::Bake(int steps_thermal, int steps_weathering, float talus) {
  //  Perturbate();
  std::cout << "CH1" << std::endl;
  UpdateCpuData();

  std::vector<std::vector<glm::vec2>> flow_dir;
  std::vector<std::vector<float>> flow_accum;
  // //////////GenerateFlowMap(flow_dir, flow_accum);
  std::cout << "CH4" << std::endl;

  /// erosion & thermal weathering
  // ProcessErosion(flow_dir, flow_accum, steps_thermal);
  std::cout << "CH5" << std::endl;
  UpdateCpuData();

  //    ProcessThermalWeathering(steps_weathering, talus);
  std::cout << "CH6" << std::endl;
  UpdateCpuData();

  GenerateNmap();
  std::cout << "CH7" << std::endl;
  GenerateSlope();
  std::cout << "CH8" << std::endl;
  GenerateSplatmap();
  std::cout << "CH9" << std::endl;
  GenerateAo();
  std::cout << "CH10 (final)" << std::endl;

  // tex_hmap_.Store("hmap_end.png", 1, GL_RED);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glBindVertexArray(0);
  std::cout << "Erosion :: DONE" << std::endl;
}
