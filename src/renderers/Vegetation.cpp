#include "Vegetation.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Details.h"
#include "../common/OpenGLUtility.h"
#include "../io/Window.h"
#include "GeoClipmaps.h"

Vegetation::Vegetation()
    : grass_shader_("../shaders/Grass.vert", "../shaders/Grass.frag", {0, 2}),
      grass_compute_shader_("../shaders/Grass.comp", {}),
      tex_grass_("../assets/grass2_64.png", Texture::Type::TerrainRGBA8),
      blades_num_(16384) {
  Init();
}

void Vegetation::Init() {
  glCreateBuffers(1, &ssbo_);
  std::vector<glm::vec4> blades_data(blades_num_, glm::vec4(1.0f));
  glNamedBufferStorage(ssbo_, blades_data.size() * sizeof(glm::vec4),
                       blades_data.data(),
                       GL_DYNAMIC_STORAGE_BIT  // This is now valid!
  );
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, ssbo_);
  glGenVertexArrays(1, &vao_);
}

void Vegetation::DeInit() { glDeleteBuffers(1, &ssbo_); }

void Vegetation::Render(float map_scale, const Texture& hmap) {
  if (glfwGetKey(gWindow, GLFW_KEY_7)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // grass_shader_.DebugUpdate();
  glBindVertexArray(vao_);
  grass_shader_.Bind();
  glUniform1f(3, sin(glfwGetTime()));
  hmap.BindSampler(0);
  tex_grass_.BindSampler(2);
  glm::mat4 mat_world = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  glUniformMatrix4fv(1, 1, false, glm::value_ptr(mat_world));
  glDisable(GL_BLEND);
  glDrawArrays(GL_TRIANGLES, 0, blades_num_ * 3);
  glEnable(GL_BLEND);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderGeoMesh(const GeoMesh& mesh, int size, int lod_id) {
  glUniform1i(0, size * lod_id);
  glUniform2fv(2, 1, glm::value_ptr(mesh.centre));
  glUniform1f(3, mesh.width);
  glUniform1f(4, mesh.density);
  glDispatchCompute((size + 255) / 256, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Vegetation::Update(float map_scale, const GeoSurface* surface) {
  int blades_num_quater = blades_num_ / 4;
  // grass_compute_shader_.DebugUpdate();
  grass_compute_shader_.Bind();
  glUniform1f(1, map_scale);
  glUniform1ui(5, static_cast<GLuint>(blades_num_quater));
  RenderGeoMesh(surface->lod0, blades_num_quater, 0);
  RenderGeoMesh(surface->lod1, blades_num_quater, 1);
  RenderGeoMesh(surface->lod2, blades_num_quater, 2);
  RenderGeoMesh(surface->lod3, blades_num_quater, 3);
}

float Vegetation::PlacementHash(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352d;
  x ^= x >> 15;
  x *= 0x846ca68b;
  x ^= x >> 16;
  return float(x) * (1.0f / 4294967296.0f);
}

std::vector<GLuint> Vegetation::UpdatePipeline(std::vector<uint8_t>& mask,
                                               Texture& placement,
                                               float density, uint32_t seed) {
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
