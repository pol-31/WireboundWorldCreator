#include "TerrainRenderer.h"

#include <stb_image.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/quaternion.hpp>

#include "../io/Window.h"
#include "../modes/traits/TerrainTraits.h"

TerrainRenderer::TerrainRenderer(Tile& tile, GeoClipmaps& mesh)
    : tile_(tile),
      shader_("../shaders/Terrain.vert", "../shaders/Terrain.tesc",
              "../shaders/Terrain.tese", "../shaders/Terrain.frag"),
      shader_game_(
        "../shaders/game/Terrain.vert", "../shaders/game/Terrain.tesc",
        "../shaders/game/Terrain.tese", "../shaders/game/Terrain.frag"),
      shader_subtract_(
        "../shaders/Terrain.vert", "../shaders/Terrain.tesc",
        "../shaders/TerrainSubtract.tese", "../shaders/Terrain.frag"),
      shader_picking_(
        "../shaders/Terrain.vert", "../shaders/Terrain.tesc",
        "../shaders/Terrain.tese", "../shaders/TerrainPicking.frag"),
      nmap_("../../ProvingGround\\cmake-build-debug\\normal_map.png", GL_RG8),
      shader_selection_(
        "../shaders/Terrain.vert", "../shaders/Terrain.tesc",
        "../shaders/Terrain.tese", "../shaders/TerrainSelection.frag"),
      shader_wireframe_(
          "../shaders/Terrain.vert", "../shaders/Terrain.tesc",
          "../shaders/Terrain.tese", "../shaders/TerrainWireframe.frag"),
      mesh_(mesh) {
  UpdateShaders();
  InitMaterial();
}

void TerrainRenderer::Render() {
  if (shader_.Update()) {
    UpdateShaders();
  }
  if (glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glEnable(GL_CULL_FACE);
  shader_.Bind();
  BindUniforms();
  mesh_.RenderLowPoly();
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderInGame() {
  if (shader_game_.Update()) {
    UpdateShaders();
  }
  if (glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glEnable(GL_CULL_FACE);
  shader_game_.Bind();
  BindUniforms();
  mesh_.Render();
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::BindUniforms() {
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));

  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();
  glActiveTexture(GL_TEXTURE1);
  tile_.map_terrain_ao.Bind();

  glActiveTexture(GL_TEXTURE2);
  tile_.map_terrain_normal.Bind();
  glActiveTexture(GL_TEXTURE3);
  tile_.map_terrain_erosion_thermal.Bind();

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.albedo);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.normal);
  glActiveTexture(GL_TEXTURE12);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.occlusion);
  glActiveTexture(GL_TEXTURE13);
  tile_.map_terrain_splat.Bind();
}

void TerrainRenderer::RenderSubtract(const Texture& tex_subtract) {
  glActiveTexture(GL_TEXTURE6);
  tex_subtract.Bind();
  Render();
}

void TerrainRenderer::RenderWireframe(TerrainTraits* terrain) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_wireframe_.Bind();
  glActiveTexture(GL_TEXTURE0);
  terrain->extra_heights.Bind();

  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, terrain->translate);
  object_model *= glm::mat4_cast(terrain->rotate);
  object_model = glm::scale(object_model, terrain->scale);
  glm::mat4 map_model = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale));
  glm::mat4 model = map_model * object_model;
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  glUniform3fv(1, 1, glm::value_ptr(terrain->color));
  mesh_.RenderLowPoly();
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderSelection(const Texture32F* surface,
                                      const Texture& selection_mask,
                                      glm::vec3 color) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  /// wireframe
  shader_selection_.Bind();
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale));
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  glActiveTexture(GL_TEXTURE0);
  surface->Bind();
  glActiveTexture(GL_TEXTURE1);
  selection_mask.Bind();
  glUniform3fv(2, 1, glm::value_ptr(color));
  mesh_.RenderLowPoly();
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderPicking() const {
  shader_picking_.Bind();
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();
  mesh_.RenderLowPoly();
  glBindVertexArray(0);
}

void TerrainRenderer::InitMaterial() {
  InitAlbedo();
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.albedo);
  LoadMaterialTexture(0, "../assets/materials/grass/grass_albedo.png");
  LoadMaterialTexture(1, "../assets/materials/mud/mud_albedo.png");
  LoadMaterialTexture(2, "../assets/materials/rock/rock_albedo.png");
  LoadMaterialTexture(3, "../assets/materials/sand/sand_albedo.png");
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  InitNormal();
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.normal);
  LoadMaterialTexture(0, "../assets/materials/grass/grass_normal.png");
  LoadMaterialTexture(1, "../assets/materials/mud/mud_normal.png");
  LoadMaterialTexture(2, "../assets/materials/rock/rock_normal.png");
  LoadMaterialTexture(3, "../assets/materials/sand/sand_normal.png");
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  InitAo();
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.occlusion);
  LoadMaterialTexture(0, "../assets/materials/grass/grass_ao.png");
  LoadMaterialTexture(1, "../assets/materials/mud/mud_ao.png");
  LoadMaterialTexture(2, "../assets/materials/rock/rock_ao.png");
  LoadMaterialTexture(3, "../assets/materials/sand/sand_ao.png");
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void TerrainRenderer::InitAlbedo() {
  glGenTextures(1, &material_.albedo);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.albedo);
  int width = 2048;
  int height = 2048;
  int layers = 4;
  int mipLevels = 1 + floor(log2(std::max(width, height)));
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_SRGB8_ALPHA8, width, height,
                 layers);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void TerrainRenderer::InitNormal() {
  glGenTextures(1, &material_.normal);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.normal);
  int width = 2048;
  int height = 2048;
  int layers = 4;
  int mipLevels = 1 + floor(log2(std::max(width, height)));
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_SRGB8_ALPHA8, width, height,
                 layers);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void TerrainRenderer::InitAo() {
  glGenTextures(1, &material_.occlusion);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.occlusion);
  int width = 2048;
  int height = 2048;
  int layers = 4;
  int mipLevels = 1 + floor(log2(std::max(width, height)));
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_SRGB8_ALPHA8, width, height,
                 layers);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void TerrainRenderer::LoadMaterialTexture(int layer, std::string_view path) {
  GLint channels;
  int width, height;
  unsigned char* pixels = stbi_load(path.data(), &width, &height, &channels, 4);
  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1,
                  GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  stbi_image_free(pixels);
}

void TerrainRenderer::UpdateShaders() {
  shader_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);  // material (temp)
  glUniform1i(2, 2);  // normal
  glUniform1i(3, 3);  // ao
  glUniform1i(4, 4);  // slope (seems useless because of normal and splat)
  glUniform1i(8, 8);
  glUniform1i(9, 9);
  glUniform1i(12, 12);
  glUniform1i(13, 13);  // splat
  shader_game_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);  // material (temp)
  glUniform1i(2, 2);  // normal
  glUniform1i(3, 3);  // ao
  glUniform1i(4, 4);  // slope (seems useless because of normal and splat)
  glUniform1i(8, 8);
  glUniform1i(9, 9);
  glUniform1i(12, 12);
  glUniform1i(13, 13);  // splat
  shader_subtract_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  glUniform1i(2, 2);
  glUniform1i(3, 3);
  glUniform1i(6, 6);
  glUniform1i(8, 8);
  glUniform1i(9, 9);
  glUniform1i(12, 12);
  glUniform1i(13, 13);
  shader_selection_.Bind();
  glUniform1i(0, 0);
  glUniform1i(1, 1);
  shader_wireframe_.Bind();
  glUniform1i(0, 0);
  shader_picking_.Bind();
  glUniform1i(0, 0);
}
