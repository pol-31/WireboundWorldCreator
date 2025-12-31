#include "TerrainRenderer.h"

#include <stb_image.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

#include "../io/Window.h"
#include "../common/ShadersBinding.h"

#include "../modes/TerrainInstanceData.h"

TerrainRenderer::TerrainRenderer(Tile& tile, const Paths& paths)
    : tile_(tile),
      shader_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          paths.shader_terrain_tese, paths.shader_terrain_frag),
      shader_subtract_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          "../shaders/TerrainSubtract.tese", paths.shader_terrain_frag),
      shader_picking_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          paths.shader_terrain_tese, paths.shader_terrain_picking_frag),
      nmap_("../../ProvingGround\\cmake-build-debug\\normal_map.png", GL_RG8),
      shader_selection_(
          paths.shader_terrain_vert, paths.shader_terrain_tesc,
          "../shaders/TerrainSelection.tese",
          "../shaders/TerrainSelection.frag"),
      shader_wireframe_(
          paths.shader_terrain_vert, "../shaders/TerrainWireframe.tesc",
          paths.shader_terrain_tese, "../shaders/TerrainWireframe.frag"),
      border_shader_(
          "../shaders/TerrainBorder.vert", "../shaders/TerrainBorder.frag"),
      border_tex_("../assets/border_tex1.png", GL_RGBA) {
  Init();
}

void TerrainRenderer::DeInit() {
  GLuint vbos[] = {vbo_quad_, vbo_ids_, border_vbo_, border_ebo_};
  glDeleteBuffers(4, vbos);
  GLuint vaos[] = {vao_, border_vao_};
  glDeleteVertexArrays(2, vaos);
}

void TerrainRenderer::Render() {
#ifndef NDEBUG
  if (shader_.Update()) {
    shader_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
    glUniform1i(1, 1); // material (temp)
    glUniform1i(2, 2); // normal
    glUniform1i(3, 3); // ao
    glUniform1i(8, 8);
    glUniform1i(9, 9);
    glUniform1i(12, 12);
    glUniform1i(13, 13); // splat
    shader_picking_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
  }
#endif
  if(glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_.Bind();
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

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderSubtract(const Texture& tex_subtract) {
#ifndef NDEBUG
  if (shader_subtract_.Update()) {
    shader_subtract_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
    glUniform1i(1, 1); // material (temp)
    glUniform1i(2, 2); // normal
    glUniform1i(3, 3); // ao
    glUniform1i(8, 8);
    glUniform1i(6, 6);
    glUniform1i(9, 9);
    glUniform1i(12, 12);
    glUniform1i(13, 13); // splat
    shader_picking_.Bind();
    glUniform1i(shader::kTerrainHeightMap, 0);
  }
#endif
  if(glfwGetKey(gWindow, GLFW_KEY_1)) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  shader_subtract_.Bind();
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

  glActiveTexture(GL_TEXTURE6);
  tex_subtract.Bind();

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.albedo);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.normal);
  glActiveTexture(GL_TEXTURE12);
  glBindTexture(GL_TEXTURE_2D_ARRAY, material_.occlusion);
  glActiveTexture(GL_TEXTURE13);
  tile_.map_terrain_splat.Bind();

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: remove?
void TerrainRenderer::Render(TerrainInstanceData* terrain) {
  std::cerr << "TerrainRenderer::Render(terrain) unimplemented" << std::endl;
}

void TerrainRenderer::RenderWireframe(
    TerrainInstanceData* terrain, BaseInstanceData* data) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  shader_wireframe_.Bind();
  glActiveTexture(GL_TEXTURE0);
//  terrain->hmap.Bind();
  terrain->data.hmap.Bind();

  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, terrain->translate);
  object_model *= glm::mat4_cast(terrain->rotate);
  object_model = glm::scale(object_model, terrain->scale);
  glm::mat4 map_model = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale));
  glm::mat4 model = map_model * object_model;
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));

  glUniform3fv(1, 1, glm::value_ptr(data->color));

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  //  glDrawArrays(GL_PATCHES, 0, patch_vertices.size());
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::RenderSelection(
    const Texture32F* surface,
    const Texture& selection_mask, glm::vec3 color) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); /// wireframe
  shader_selection_.Bind();
  glActiveTexture(GL_TEXTURE0);
  surface->Bind();
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale));
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  // NOT terrain->hmap.Bind();
  glActiveTexture(GL_TEXTURE1);
  selection_mask.Bind();
  glUniform3fv(2, 1, glm::value_ptr(color));

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

  //  glDrawArrays(GL_PATCHES, 0, patch_vertices.size());
  glBindVertexArray(0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//TODO: fbo shoudl be bind at Interface::Draw() or somewhere else
void TerrainRenderer::RenderPicking() const {
  shader_picking_.Bind();
  glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  glActiveTexture(GL_TEXTURE0);
  tile_.map_terrain_height.Bind();
  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
  glBindVertexArray(0);
}

void TerrainRenderer::RenderPicking(TerrainInstanceData* terrain) const {
  return;
  shader_picking_.Bind();
  glActiveTexture(GL_TEXTURE0);
  terrain->data.hmap.Bind();

  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(object_model, terrain->translate);
  object_model *= glm::mat4_cast(terrain->rotate);
  object_model = glm::scale(object_model, terrain->scale);
  glm::mat4 map_model = glm::scale(glm::mat4(1.0f), glm::vec3(tile_.map_scale));
  glm::mat4 model = map_model * object_model;
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));

  glBindVertexArray(vao_);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
  glBindVertexArray(0);
}

glm::vec3 TerrainRenderer::GetYPosition(int vertex_id) const {
  int coord_x = vertex_id & 1023;
  int coord_z = vertex_id >> 10;
  /*
   auto where =
       glm::vec3(static_cast<float>(coord_x), 0, coord_z) / 16.0f - 32.0f;
   where.y = static_cast<float>(terrain_heights_[coord_x + coord_z * 1024]);
   where.y /= 64.0f; // TODO: idk why 64.0f
   */
  return {};
}

void TerrainRenderer::UpdateTransformUniform(glm::mat4 model) {
  shader_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  shader_subtract_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  shader_picking_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  shader_selection_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
  shader_wireframe_.Bind();
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(model));
}

void TerrainRenderer::Init() {
  GLuint vaos[2];
  glGenVertexArrays(2, vaos);
  vao_ = vaos[0];
  border_vao_ = vaos[1];
  glBindVertexArray(vao_);

  GLuint vbos[4];
  glGenBuffers(4, vbos);
  vbo_quad_ = vbos[0];
  vbo_ids_ = vbos[1];
  border_vbo_ = vbos[2];
  border_ebo_ = vbos[3];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);

  const float quad[] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f
  };

  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glVertexAttribDivisor(0, 0);

  for (int i = 0; i < gGridSize * gGridSize; ++i) {
    patch_grid_[i] = i;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * patch_grid_.size(), patch_grid_.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);
  glVertexAttribDivisor(1, 1);

  glBindVertexArray(0);

  float cubeVertices[] = {
      // --- front face (z = +0.5)
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

      // --- back face (z = -0.5)
      -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      // --- left face (x = -0.5)
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

      // --- right face (x = +0.5)
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      // --- bottom face (y = -0.5)
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      // --- top face (y = +0.5)
      -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
  };

  unsigned int cubeIndices[] = {
      0, 2, 1,   2, 0, 3,       // front
      4, 6, 5,   6, 4, 7,       // back
      8, 10, 9,  10,8, 11,       // left
      12,14,13,  14,12,15,       // right
      16,18,17,  18,16,19,       // bottom
      20,22,21,  22,20,23        // top
  };

  glBindVertexArray(border_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, border_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, border_ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  shader_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // material (temp)
  glUniform1i(2, 2); // normal
  glUniform1i(3, 3); // ao
  glUniform1i(8, 8);
  glUniform1i(9, 9);
  glUniform1i(12, 12);
  glUniform1i(13, 13); // splat
  shader_subtract_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // material (temp)
  glUniform1i(2, 2); // normal
  glUniform1i(3, 3); // ao
    glUniform1i(6, 6);
  glUniform1i(8, 8);
  glUniform1i(9, 9);
  glUniform1i(12, 12);
  glUniform1i(13, 13); // splat
  shader_selection_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  glUniform1i(1, 1); // selection mask
  shader_wireframe_.Bind();
  glUniform1i(shader::kTerrainHeightMap, 0);
  shader_picking_.Bind();
  glUniform1i(shader::kHeightMapPickingHeightMap, 0);

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
  glTexStorage3D(
      GL_TEXTURE_2D_ARRAY,
      mipLevels,
      GL_SRGB8_ALPHA8,
      width,
      height,
      layers
  );
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
  glTexStorage3D(
      GL_TEXTURE_2D_ARRAY,
      mipLevels,
      GL_SRGB8_ALPHA8,
      width,
      height,
      layers
  );
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
  glTexStorage3D(
      GL_TEXTURE_2D_ARRAY,
      mipLevels,
      GL_SRGB8_ALPHA8,
      width,
      height,
      layers
  );
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void TerrainRenderer::LoadMaterialTexture(int layer, std::string_view path) {
  GLint channels;
  int width, height;
  unsigned char* pixels = stbi_load(path.data(), &width, &height, &channels, 4);
  glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        0, 0, layer,
        width, height, 1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels);
  stbi_image_free(pixels);
}
