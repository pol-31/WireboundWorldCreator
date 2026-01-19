#include "Vegetation.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Details.h"
#include "../common/OpenGLUtility.h"
#include "../io/Window.h"

Vegetation::Vegetation()
    : grass_shader_("../shaders/Grass.vert", "../shaders/Grass.frag"),
      grass_compute_shader_("../shaders/Grass.comp"),
      tex_grass_("../assets/grass_triangle.png", GL_RGBA),
      blades_num_(512) {
  Init();
}

void Vegetation::Init() {
  glCreateBuffers(1, &ssbo_);
  std::vector<glm::vec4> blades_data(blades_num_, glm::vec4(1.0f));
  glNamedBufferStorage(
    ssbo_,
    blades_data.size() * sizeof(glm::vec4),
    blades_data.data(),
    GL_DYNAMIC_STORAGE_BIT // This is now valid!
  );
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, ssbo_);
  glGenVertexArrays(1, &vao_);
}

void Vegetation::DeInit() {
  glDeleteBuffers(1, &ssbo_);
}

void Vegetation::Render(float map_scale, const Texture& hmap) {
  if (glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  grass_shader_.Update();
  glBindVertexArray(vao_);
  grass_shader_.Bind();
  glUniform1i(0, 0);
  glActiveTexture(GL_TEXTURE0);
  hmap.Bind();
  glUniform1i(2, 2);
  glActiveTexture(GL_TEXTURE2);
  tex_grass_.Bind();
  glm::mat4 mat_world = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  glUniformMatrix4fv(1, 1, false, glm::value_ptr(mat_world));
  glDisable(GL_BLEND);
  glDrawArrays(GL_TRIANGLES, 0, blades_num_ * 3);
  glEnable(GL_BLEND);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Vegetation::Update(float map_scale) {
  grass_compute_shader_.Update();
  grass_compute_shader_.Bind();
  glUniform1f(0, sin(glfwGetTime()));
  glUniform1f(1, map_scale);
  glDispatchCompute((blades_num_ + 255) / 256, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

float Vegetation::PlacementHash(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352d;
  x ^= x >> 15;
  x *= 0x846ca68b;
  x ^= x >> 16;
  return float(x) * (1.0f / 4294967296.0f);
}

std::vector<GLuint> Vegetation::UpdatePipeline(
    std::vector<uint8_t>& mask, Texture& placement, float density,
    uint32_t seed) {
  density = std::clamp(density, 0.0f, 1.0f);
  std::vector<GLuint> result;
  const int size = details::gTerrainSize * details::gTerrainSize;
  std::vector<uint8_t> data(size);
  glGetTextureImage(placement.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, size,
                    data.data());
  for (uint32_t i = 0; i < size; ++i) {
    if (!data[i]) {
      continue;
    }
    seed = i ^ (seed * 0x9E3779B9u);
    if (PlacementHash(seed) < density && mask[i] == 0) {
      mask[i] = 255;
      result.push_back(i);
    }
  }
  return result;
}
