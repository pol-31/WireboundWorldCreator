#include "PlacementRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../io/Window.h"

PlacementRenderer::PlacementRenderer(
    Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(paths.shader_placement_vert,
              paths.shader_terrain_tesc, paths.shader_terrain_tese,
              paths.shader_placement_frag),
      grass_(paths, tile_.map_terrain_height) {
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
  shader_.SetUniform("tex_displacement", 0);
  //    shader_.SetUniform("tex_color", 1);
  shader_.SetUniform("tex_occlusion", 2);
  shader_.SetUniform("tex_draw", 3);
}

void PlacementRenderer::Render() {
//  TODO: grass_.Render(); ADD COMPUTE SHADER (we call it not directly to allow
//   for GPU to decide on its own when to run it, so we don't _block_ at point of
//   Update() but just at Render()
  grass_.Render(gDeltaTime);
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

  glActiveTexture(GL_TEXTURE2);
  tile_.map_terrain_occlusion.Bind();

  glActiveTexture(GL_TEXTURE3);
  tile_.cur_placement_mode_tex_->Bind();

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*
void RunLoop(GLFWwindow* window) {
  Texture dither_texture("../assets/map_test_placement_tree_rgba.png");
  Shader dither_shader("../shaders/Dithering.comp");
  dither_shader.Bind();

  GLuint dithered_texture_id;
  glGenTextures(1, &dithered_texture_id);
  glBindTexture(GL_TEXTURE_2D, dithered_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 1024, 1024, 0, GL_RED_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  Texture dithered_texture(dithered_texture_id, 1024, 1024, GL_RED_INTEGER);

  glBindImageTexture(0, dither_texture.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
  glBindImageTexture(1, dithered_texture.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
  glDispatchCompute((1024 + 15) / 16, (1024 + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


  Shader poisson_shader("../shaders/PoissonPoints.comp");

  GLuint poissonSSBO;
  glGenBuffers(1, &poissonSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, poissonSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(details::PoissonPoints),
               details::PoissonPoints, GL_DYNAMIC_COPY);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, poissonSSBO);

  poisson_shader.Bind();
  poisson_shader.SetUniform("threshold", 20.0f);
  poisson_shader.SetUniform("textureSize", 1024);

  GLuint placement_tex_id;
  glGenTextures(1, &placement_tex_id);
  glBindTexture(GL_TEXTURE_2D, placement_tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 1024, 1024, 0, GL_RED_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  Texture placement_texture(placement_tex_id, 1024, 1024, GL_RED_INTEGER);

  glBindImageTexture(2, placement_texture.GetId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
  glBindImageTexture(1, dithered_texture.GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8UI);
  glDispatchCompute(1018, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


  std::vector<unsigned char> data(1024 * 1024, 255);

  glBindTexture(GL_TEXTURE_2D, placement_tex_id);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  int passed_threshold{0};

  std::vector<glm::vec3> positions;
  for (int i = 0; i < 1024 * 1024; ++i) {
    if (data[i] != 0) {
      ++passed_threshold;
      auto position = terrain.GetPosition(0, static_cast<int>(i & 1023), i >> 10);
      positions.push_back(position);
    }
  }

  std::cout << "total passed: " << passed_threshold << std::endl;

}*/
