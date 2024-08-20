#include "Grass.h"

#include <random>
#include <vector>

#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

/*TODO:
 * subdivide all tile into sub-tiles, where withing single tile, all grass
 * have the same properties (density, render approach (full/half), shading, etc);
 * - each tile - separate draw call.
 * - because we already have points from dithering, we draw bunches of blades to
 *   to the same point.
 * - each tile has it __centre_point__, so we're bind to closest tile and
 *   trying to update grass-LOD at each iteration with condition:
 *     if(cur_tile_is_the_same)
 * - for each tile: different tessellation (two or one blade), shading, density,
 *   height, etc...
 * - we call GenerateBlades every time we step into next tile.
 * - because we use the same std::vector<...>positions, we can also keep it sorted
 *   like this:
 *   1 2 3 4 5
 *   6 7 8 9 0,
 *   so then we can skip simply as i*=2 or i*=4 or i*=8 and get:
 *   1, 3, 5, 7, 9 OR 1, 5, 9, etc...
 * */


std::vector<Grass::Blade> Grass::GenerateBlades(
    const std::vector<int>& points,
    const std::vector<uint8_t>& heights) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> orientation_dis(0, glm::pi<float>());
  std::uniform_real_distribution<float> height_dis(0.6f, 1.2f);
  std::uniform_real_distribution<float> dis(-1, 1);

  //TODO: we should've discard
  //  most of points at PlacementRenderer::UpdatePipeline()
  std::vector<Blade> blades;
  for (auto id : points) {
    for (int i = 0; i < 10; ++i) {
      const auto x = static_cast<float>(id & 1023) / 16.0f - 32.0f - 1 + dis(gen) * 0.5f;
      const auto y = static_cast<float>(id >> 10) / 16.0f - 32.0f - 1 + dis(gen) * 0.5f;
      const auto blade_height = height_dis(gen);
      auto z = static_cast<float>(heights[id]) / 16.0f;

      //TODO: we can put such data into 1 blade info and rng them in shader
      blades.push_back({
          glm::vec4(x, z, y, 1.0f/*orientation_dis(gen)*/),
          glm::vec4(x, z + blade_height, y, blade_height),
          glm::vec4(x, z + blade_height, y, 0.1f),
          glm::vec4(0, z, 0, dis(gen) / 2 + 0.5f)});
//          glm::vec4(0, z, 0, /*0.8f*/0.7f + dis(gen) * 0.3f)});
    }
  }
  return blades;
}


Grass::Grass(const Paths& paths)
    : grass_shader_(paths.shader_grass_vert, paths.shader_grass_tesc,
                    paths.shader_grass_tese, paths.shader_grass_frag),
      grass_compute_shader_(paths.shader_grass_comp) {
  Init();
}

void Grass::Init() {
  glGenVertexArrays(1, &grass_vao_);
  glBindVertexArray(grass_vao_);
}

void Grass::UpdatePlacement(const std::vector<int>& points,
                            const std::vector<uint8_t>& heights) {
  DeleteBuffers();

  const std::vector<Blade> blades = GenerateBlades(points, heights);
  blades_num_ = static_cast<GLsizei>(blades.size());

  glGenBuffers(1, &grass_input_buffer_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_input_buffer_);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizei>(blades.size() * sizeof(Blade)),
               blades.data(), GL_DYNAMIC_COPY);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grass_input_buffer_);

  glGenBuffers(1, &grass_output_buffer_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_output_buffer_);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizei>(blades.size() * sizeof(Blade)), nullptr,
               GL_STREAM_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grass_output_buffer_);

  NumBlades numBlades;
  glGenBuffers(1, &grass_indirect_buffer_);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, grass_indirect_buffer_);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(NumBlades), &numBlades,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grass_indirect_buffer_);

  glBindVertexArray(grass_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, grass_output_buffer_);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
                        reinterpret_cast<void*>(offsetof(Blade, v0)));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
                        reinterpret_cast<void*>(offsetof(Blade, v1)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
                        reinterpret_cast<void*>(offsetof(Blade, v2)));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
                        reinterpret_cast<void*>(offsetof(Blade, up)));
  glEnableVertexAttribArray(3);
}

void Grass::DeleteBuffers() const {
  GLuint buffers[3];
  buffers[0] = grass_input_buffer_;
  buffers[1] = grass_output_buffer_;
  buffers[2] = grass_indirect_buffer_;
  glDeleteBuffers(3, buffers);
}

void Grass::UpdateAnimation(float delta_time) {
  grass_compute_shader_.Bind();
  grass_compute_shader_.SetUniform(
      "current_time", static_cast<float>(glfwGetTime()));
  grass_compute_shader_.SetUniform("delta_time", static_cast<float>(delta_time / 1e3f)); // TODO: what's this
  grass_compute_shader_.SetUniform("wind_magnitude", 1000.0f/*wind_magnitude_*/);
  grass_compute_shader_.SetUniform("wind_wave_length", wind_wave_length_);
  grass_compute_shader_.SetUniform("wind_wave_period", wind_wave_period_);

  /*
   * tile 1024x1024 subdivided on n tiles, where n depends on distance to camera;
   * if we in the centre of the tile, we draw 9 tiles with max res, so
   * each subtile == 1 subtile,
   * while for furthest 1 subtile == 4 subtiles or 9 or 16 or 25
   * (looks like 1:4:9 is enough). Let's call them tile_1, tile_2, tile_3
   * each subtile draw 1000 blades, so:
   * tile_1 draw 1000 blades on tile, while tile_3 draw 1000 blades on 9 tiles
   * each subtile has 1 single draw call, where:
   * - in args: wind, placement_map, texture for randomizing (to randomize
   * all params).
   * we can pre-generate all blades data into textures TODO: id or full data?
   * - out args: ssbo / buffer for indirect draw call;
   *   we don't have it's data on CPU
   * */
  glDispatchCompute(static_cast<GLuint>(std::clamp(blades_num_, 1, 1000)), 1, 1);
}

void Grass::Render() {
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glBindVertexArray(grass_vao_);
  grass_shader_.Bind();
  glPatchParameteri(GL_PATCH_VERTICES, 1);
  glDrawArraysIndirect(GL_PATCHES, reinterpret_cast<void*>(0));
}

void Grass::SetWindMagnitude(float wind_magnitude) {
  wind_magnitude_ += wind_magnitude;
}
void Grass::SetWindWaveLength(float wind_wave_length) {
  wind_wave_length_ += wind_wave_length;
}
void Grass::SetWindWavePeriod(float wind_wave_period) {
  wind_wave_period_ += wind_wave_period;
}
