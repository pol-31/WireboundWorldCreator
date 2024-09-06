#include "PlacementRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

PlacementRenderer::PlacementRenderer(
    Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(paths.shader_placement_vert, paths.shader_terrain_tesc,
              paths.shader_terrain_tese, paths.shader_placement_frag),
      grass_(paths),
      poisson_shader_(paths.shader_poisson_points),
      density_low_(paths.placement_density_low, GL_RGBA8),
      density_medium_low_(paths.placement_density_medium_low, GL_RGBA8),
      density_medium_(paths.placement_density_medium, GL_RGBA8),
      density_medium_high_(paths.placement_density_medium_high, GL_RGBA8),
      density_high_(paths.placement_density_high, GL_RGBA8),
      density_very_high_(paths.placement_density_very_high, GL_RGBA8),
      density_ultra_high_(paths.placement_density_ultra_high, GL_RGBA8),
      density_extreme_(paths.placement_density_extreme, GL_RGBA8) {
  Init();
}

void PlacementRenderer::Init() {
  //    return; //TODO: we use not "empty" but fill with tex_coords and pos !!!
  // Define the vertices (not actually used, but required for the draw call)
  GLfloat vertices[] = {
      -0.5f, 0.0f, -0.5f, 1.0f,
      0.5f, 0.0f, -0.5f, 1.0f,
      -0.5f, 0.0f,  0.5f, 1.0f,
      0.5f, 0.0f,  0.5f, 1.0f
  }; // TODO: its useless

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Enable the vertex attribute array
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
                        reinterpret_cast<void*>(0));

  shader_.Bind();
  glUniform1i(shader::kPlacementHeightMap, 0);
  glUniform1i(shader::kPlacementTexture, 1);

  InitPlacementPipeline();
  UpdatePipeline();
}

void PlacementRenderer::InitPlacementPipeline() {
  Shader poisson_shader("../shaders/PoissonPoints.comp");
  poisson_shader.Bind();
  glUniform1i(shader::kPoissonAreaSize, 2);
  placement_temp_ = Texture(1024, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
}

void PlacementRenderer::Render() {
  grass_.UpdateAnimation(gDeltaTime);
  if(glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  grass_.Render();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void PlacementRenderer::RenderDraw() const {
  if(glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  //TODO; we still need tessellation - we want to see how it should be in game
  //TODO: render terrain here

  shader_.Bind();
  glBindVertexArray(vao_);

  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();

  glActiveTexture(GL_TEXTURE1);
  tile_.cur_placement_mode_tex_->Bind();

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PlacementRenderer::UpdatePipeline() {
  /*TODO:
   * here we have placement heightmaps (e.g. trees, bushes, tall grass,
   * undergrowth); we need to generate __placement_points__
   * according to priority: trees->bushes->...->grass, so we can generate
   * them in the same way (using different Poisson points textures) and
   * after collecting them to std::vector<id> we discard overlapping points
   * at lower priorities.
   * Then we should call UpdatePlacement() for each affected placement type
   * */
  poisson_shader_.Bind();
  glBindImageTexture(0, density_extreme_.GetId(),
                     0, GL_FALSE, 0, GL_READ_ONLY, density_extreme_.GetFormat());
  glBindImageTexture(1, tile_.map_placement_undergrowth.GetId(),
                     0, GL_FALSE, 0, GL_READ_ONLY, tile_.map_placement_undergrowth.GetFormat());
  glBindImageTexture(2, placement_temp_.GetId(),
                     0, GL_FALSE, 0, GL_WRITE_ONLY, placement_temp_.GetFormat());
  glDispatchCompute(1024 / 16, 1024 / 16, 1); // TODO:
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  std::vector<unsigned char> data(1024 * 1024);
  placement_temp_.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());

  tile_.undergrowth_.clear();
  for (int i = 0; i < 1024 * 1024; ++i) {
    if (data[i] > 100) {
      std::cout << static_cast<int>(data[i]) << '\n';
      tile_.undergrowth_.push_back(i);
    }
  }
  std::cout << "grass blades to draw: "
            << tile_.undergrowth_.size() << std::endl;


  // need map_terrain_height to init blades heights
  grass_.UpdatePlacement(tile_.undergrowth_, tile_.terrain_heights_);
}
