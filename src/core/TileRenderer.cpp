#include "TileRenderer.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <cmath>

#include "../common/Details.h"
#include "../common/OpenGlUtility.h"
#include "../modes/UiEditRiver.h"
#include "../modes/UiEditRoads.h"

TileRenderer::TileRenderer()
    : terrain(cur_tile_, mesh_),
      water(cur_tile_, mesh_),
      shader_apply_rivers_roads_("../shaders/Hmap2Deformation.comp", {}) {}

void TileRenderer::Render() {
  environment_.Update();
  // vegetation.Update(cur_tile_.map_scale); need to update mesh_
  if (show_terrain_) {
    terrain.Render();
  }
  if (show_water_) {
    water.Render();
  }
  if (show_placement_) {
    // vegetation.Render(cur_tile_.map_scale, cur_tile_.map_terrain_height);
  }
}

void TileRenderer::RenderInGame(const Camera* camera) {
  environment_.Update();
  mesh_.Update(camera, &cur_tile_);
  const auto& surface = mesh_.GetSurface();
  if (show_terrain_) {
    terrain.RenderInGame();
  }
  if (show_water_) {
    water.RenderInGame();
  }
  if (show_placement_) {
    vegetation.Update(cur_tile_.map_scale, &surface);
    vegetation.Render(cur_tile_.map_scale, cur_tile_.map_terrain_height);
  }
}

void TileRenderer::RenderUiTerrain(const Texture& tex_subtract) {
  environment_.Update();
  // vegetation.Update(cur_tile_.map_scale);
  if (show_terrain_) {
    terrain.RenderSubtract(tex_subtract);
  }
  if (show_water_) {
    water.Render();
  }
  if (show_placement_) {
    // vegetation.Render(cur_tile_.map_scale, cur_tile_.map_terrain_height);
  }
}

void TileRenderer::RenderPicking() {
  if (show_terrain_) {
    terrain.RenderPicking();
  }
}

void TileRenderer::BakeRivers(const std::vector<float>& heights_in) {
  int size = details::gTerrainSize;
  std::vector<uint8_t> distances(size * size, 0);  // now it's mask, not df
  std::vector<float> heights_out(size * size, 0.0f);
  for (const auto& r : cur_tile_.rivers_data) {
    BakeGraphs(r.GetGraphConfig(), heights_in, heights_out, distances,
               r.map_points, r.map_joints);
  }
  glTextureSubImage2D(cur_tile_.tex_rivers_mask_.GetId(), 0, 0, 0, size, size,
                      GL_RED, GL_UNSIGNED_BYTE, distances.data());
  const auto& tex_rivers_deform = cur_tile_.tex_rivers_deform_;
  glTextureSubImage2D(tex_rivers_deform.GetId(), 0, 0, 0, size, size, GL_RED,
                      GL_FLOAT, heights_out.data());
}

void TileRenderer::BakeRoads(const std::vector<float>& heights_in) {
  int size = details::gTerrainSize;
  std::vector<uint8_t> distances(size * size, 0);  // now it's mask, not df
  std::vector<float> heights_out(size * size, 0.0f);
  for (const auto& r : cur_tile_.roads_data) {
    BakeGraphs(r.GetGraphConfig(), heights_in, heights_out, distances,
               r.map_points, r.map_joints);
  }
  glTextureSubImage2D(cur_tile_.tex_roads_mask_.GetId(), 0, 0, 0, size, size,
                      GL_RED, GL_UNSIGNED_BYTE, distances.data());
  const auto& tex_roads_deform = cur_tile_.tex_roads_deform_;
  glTextureSubImage2D(tex_roads_deform.GetId(), 0, 0, 0, size, size, GL_RED,
                      GL_FLOAT, heights_out.data());
}

void TileRenderer::UpdatePipeline() {
  // TODO: splatmask (?)

  int size = details::gTerrainSize;
  std::vector<float> heights_in(size * size, 0.0f);
  glGetTextureImage(cur_tile_.map_terrain_height_raw_.GetId(), 0, GL_RED,
                    GL_FLOAT, size * size * sizeof(float), heights_in.data());
  BakeRivers(heights_in);
  BakeRoads(heights_in);

  shader_apply_rivers_roads_.Bind();
  cur_tile_.map_terrain_height_raw_.BindImage(0, GL_READ_ONLY);
  cur_tile_.tex_rivers_deform_.BindImage(1, GL_READ_ONLY);
  cur_tile_.tex_rivers_mask_.BindImage(2, GL_READ_ONLY);
  cur_tile_.tex_roads_deform_.BindImage(3, GL_READ_ONLY);
  cur_tile_.tex_roads_mask_.BindImage(4, GL_READ_ONLY);
  cur_tile_.map_terrain_slope.BindImage(5, GL_READ_ONLY);
  cur_tile_.map_terrain_height.BindImage(6, GL_WRITE_ONLY);
  cur_tile_.tex_vegetation_mask.BindImage(7, GL_WRITE_ONLY);
  glDispatchCompute(size / 8, size / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glGetTextureImage(cur_tile_.map_terrain_height.GetId(), 0, GL_RED, GL_FLOAT,
                    size * size * sizeof(float),
                    cur_tile_.terrain_heights_.data());
  std::vector<uint8_t> vegetation_mask_data(size * size);

  glGetTextureImage(cur_tile_.tex_vegetation_mask.GetId(), 0, GL_RED,
                    GL_UNSIGNED_BYTE, size * size, vegetation_mask_data.data());

  // vegetation.UpdatePipeline() uses same pipeline, the last win
  cur_tile_.placement_trees_ = vegetation.UpdatePipeline(
      vegetation_mask_data, cur_tile_.tex_placement_trees_, 0.001f, 0);
  cur_tile_.placement_bushes_ = vegetation.UpdatePipeline(
      vegetation_mask_data, cur_tile_.tex_placement_bushes_, 0.01f, 1);
  cur_tile_.placement_tall_grass_ = vegetation.UpdatePipeline(
      vegetation_mask_data, cur_tile_.tex_placement_tall_grass_, 0.05f, 2);
  cur_tile_.placement_undergrowth_ = vegetation.UpdatePipeline(
      vegetation_mask_data, cur_tile_.tex_placement_undergrowth_, 0.1f, 3);

  // we can use vegetation_mask_data for further steps
  Build(cur_tile_.terrain_heights_);
}

void TileRenderer::Build(const std::vector<float>& originalHeightMap) {
  cur_tile_.m_maxPyramid.clear();
  cur_tile_.m_maxPyramid.push_back(originalHeightMap);

  // 10 levels, where 10th is 00 01 10 11

  int currentSize = details::gTerrainSize;
  int level = 0;

  while (currentSize > 1) {
    int nextSize = currentSize / 2;
    std::vector<float> nextLevel(nextSize * nextSize);
    const std::vector<float>& prev = cur_tile_.m_maxPyramid[level];
    for (int y = 0; y < nextSize; ++y) {
      for (int x = 0; x < nextSize; ++x) {
        float h1 = prev[(x * 2)     + (y * 2)     * currentSize];
        float h2 = prev[(x * 2 + 1) + (y * 2)     * currentSize];
        float h3 = prev[(x * 2)     + (y * 2 + 1) * currentSize];
        float h4 = prev[(x * 2 + 1) + (y * 2 + 1) * currentSize];
        nextLevel[x + y * nextSize] = std::max({ h1, h2, h3, h4 });
      }
    }
    cur_tile_.m_maxPyramid.push_back(nextLevel);
    currentSize = nextSize;
    level++;
  }
}

float TileRenderer::GetMaxHeight(int level, int x, int y) const {
  int size = details::gTerrainSize >> level;
  if (x < 0 || x >= size || y < 0 || y >= size) return -99999.0f;
  return cur_tile_.m_maxPyramid[level][x + y * size];
}

// Returns index (y * 1024 + x) OR -1000 TODO; if no collision
// start == drone pos, dir == normalized drone dir, maxDist to check
glm::vec3 TileRenderer::CastRay(glm::vec3 start, glm::vec3 dir, float maxDist) {
  float currentDist = 0.0f;
  int size = details::gTerrainSize;

  while (currentDist < maxDist) {
    glm::vec3 p = start + dir * currentDist;
    // -32-32 to 0-1024
    int x = static_cast<int>(p.x * 16.0f + 512.0f);
    int y = static_cast<int>(p.z * 16.0f + 512.0f);
    if (x < 0 || x >= size || y < 0 || y >= size) {
      return glm::vec3{-1000.0f};
    }
    bool potential_hit = true;
    for (int l = 5; l >= 0; --l) {
      // start at pow 5 (32x32 chunk)
      int lx = x >> l;
      int ly = y >> l;
      float chunkMaxH = GetMaxHeight(l, lx, ly);
      if (p.y > chunkMaxH) {
        potential_hit = false;
        int step = 1 << l;
        // if step 32 -> +-2
        // if step 16 -> +-1
        currentDist += static_cast<float>(step) / 16.0f;
      }
    }
    if (potential_hit) {
      float preciseHeight = cur_tile_.m_maxPyramid[0][x + y * size];
      if (p.y <= preciseHeight) {
        p.y = preciseHeight;
        return p;
      }
      currentDist += 1.0f; // we're close, check next tile cell
    }
  }
  return glm::vec3{-1000.0f};
}

void TileRenderer::BakeGraphs(const GraphBakeConfig& config,
                              const std::vector<float>& heights_in,
                              std::vector<float>& heights_out,
                              std::vector<uint8_t>& distances,
                              const std::vector<MapPoint>& points,
                              const std::vector<glm::uvec2>& joints) {
  for (int i = 0; i < joints.size(); ++i) {
    GLuint id1 = joints[i].x;
    glm::vec2 pos1 = points[id1].position;
    auto x1 = static_cast<GLuint>(pos1.x * 16.0f + 512.0f);
    auto y1 = static_cast<GLuint>(pos1.y * 16.0f + 512.0f);

    GLuint id2 = joints[i].y;
    glm::vec2 pos2 = points[id2].position;
    auto x2 = static_cast<GLuint>(pos2.x * 16.0f + 512.0f);
    auto y2 = static_cast<GLuint>(pos2.y * 16.0f + 512.0f);

    float height1 = cur_tile_.GetPositionY(x1, y1);
    float height2 = cur_tile_.GetPositionY(x2, y2);
    BakeGraph(config, heights_in, heights_out, distances, {x1, y1}, {x2, y2},
              height1, height2);
  }
}

/// MurMur3 0.0f-1.0f
float TileRenderer::Hash(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352d;
  x ^= x >> 15;
  x *= 0x846ca68b;
  x ^= x >> 16;
  return static_cast<float>(x) * (1.0f / 4294967296.0f);
}

void TileRenderer::BakeGraph(const GraphBakeConfig& config,
                             const std::vector<float>& heights_in,
                             std::vector<float>& heights_out,
                             std::vector<uint8_t>& distances, glm::uvec2 a,
                             glm::uvec2 b, float height_a, float height_b) {
  glm::uvec2 mid = (a + b) / 2u;
  if (mid == a || abs(height_a - height_b) < 0.001f) {
    return;
  }
  glm::vec2 dir = glm::normalize(glm::vec2(b) - glm::vec2(a));
  glm::vec2 normal = glm::vec2(-dir.y, dir.x);
  uint32_t seed = mid.x * 73856093u ^ mid.y * 19349663u;
  float noise = Hash(seed);  // 0..1
  float offset = (noise - 0.5f) * config.curve_amplitude;
  glm::vec2 xf = glm::vec2(mid) + normal * offset;
  glm::uvec2 x = glm::clamp(glm::uvec2(xf), 0u, 1023u);

  float height_x = (height_a + height_b) / 2.0f;
  float drift = (noise - 0.5f) * config.height_drift;
  float drifted_height_x = height_x + drift;
  for (int i = -config.radius; i < config.radius; ++i) {
    for (int j = -config.radius; j < config.radius; ++j) {
      int id = (x.y + i) * 1024 + x.x + j;
      float d = sqrt(float(i * i + j * j));
      distances[id] = std::min(distances[id], uint8_t(d));

      float w =
          1.0f - float(distances[id] - config.radius_flat) / config.radius;
      w = glm::clamp(w, 0.0f, 1.0f);
      w = w * w;  // smoothstep-like

      float sag = -config.side_sagging * w;
      heights_out[id] = glm::mix(heights_in[id], drifted_height_x, w) + sag +
                        config.height_raise - heights_in[id];

      // TODO: WARNING!!! we write mask, not distance field
      distances[id] = 255;
    }
  }
  BakeGraph(config, heights_in, heights_out, distances, x, a, height_x,
            height_a);
  BakeGraph(config, heights_in, heights_out, distances, x, b, height_x,
            height_b);
}
