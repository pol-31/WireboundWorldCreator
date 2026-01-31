#include "GeoClipmaps.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../io/Camera.h"
#include "../core/Tile.h"

const int GeoClipmaps::gPatchNum = 40;
const int GeoClipmaps::gPatchNumLowPoly = 64 * 64;

GeoClipmaps::GeoClipmaps() {
  Init();
}

void GeoClipmaps::Init() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  GLuint vbos[2];
  glGenBuffers(std::size(vbos), vbos);
  vbo_quad_ = vbos[0];
  ssbo_mesh_ = vbos[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_);

  const float quad[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glVertexAttribDivisor(0, 0);
  glBindVertexArray(0);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_mesh_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, gPatchNum * sizeof(Patch), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo_mesh_);
}

void GeoClipmaps::DeInit() {
  GLuint vbos[] = {vbo_quad_, ssbo_mesh_};
  glDeleteBuffers(std::size(vbos), vbos);
  glDeleteVertexArrays(1, &vao_);
}

void GeoClipmaps::BindVao() const noexcept {
  glBindVertexArray(vao_);
}

void GeoClipmaps::Render() {
  BindVao();
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, gPatchNum);
}

void GeoClipmaps::RenderLowPoly() const noexcept {
  BindVao();
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArraysInstanced(GL_PATCHES, 0, 4, gPatchNumLowPoly);
}

void GeoClipmaps::Update(const Camera* camera, Tile* tile) {
  // map_scale = tile_.target_map_scale;
  GroundView view = RaysToGround(camera, tile);
  /// lod0 lod1 lod2 lod3
  Band bands[] = {{0.00f, 0.15f, 8, 0.0f},
                  {0.15f, 0.35f, 6, 0.0f},
                  {0.35f, 0.65f, 4, 0.0f},
                  {0.65f, 1.00f, 2, 0.0f}};
  std::vector<Patch> patches;
  GeoMesh surface[4];
  for (int i = 0; i < std::size(bands); ++i) {
    Band& b = bands[i];
    float t0 = b.z0;
    float t1 = b.z1;

    glm::vec2 left0 = glm::mix(view.nearL, view.farL, t0);
    glm::vec2 right0 = glm::mix(view.nearR, view.farR, t0);
    glm::vec2 left1 = glm::mix(view.nearL, view.farL, t1);
    glm::vec2 right1 = glm::mix(view.nearR, view.farR, t1);

    for (int x = 0; x < b.columns; ++x) {
      float u0 = static_cast<float>(x) / b.columns;
      float u1 = static_cast<float>(x + 1) / b.columns;

      glm::vec2 p00 = glm::mix(left0, right0, u0);
      glm::vec2 p10 = glm::mix(left0, right0, u1);
      glm::vec2 p01 = glm::mix(left1, right1, u0);
      glm::vec2 p11 = glm::mix(left1, right1, u1);

      Patch p{.p0 = glm::vec4{p00.x, p00.y, p01.x, p01.y},
              .p1 = glm::vec4{p10.x, p10.y, p11.x, p11.y},
              .heightLod = b.heightLod};
      patches.push_back(p);
    }
    surface[i].centre = (left0 + right0 + left1 + right1) / 4.0f;
    surface[i].width = glm::length((left0 + left1) / 2.0f - surface[i].centre);
    surface[i].density = 128.0f / (surface[i].width * 2.0f);
  }
  surface_.lod0 = surface[0];
  surface_.lod1 = surface[1];
  surface_.lod2 = surface[2];
  surface_.lod3 = surface[3];
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_mesh_);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, patches.size() * sizeof(Patch),
                  patches.data());
}

GeoClipmaps::GroundView GeoClipmaps::RaysToGround(
    const Camera* camera, Tile* tile) {
  glm::vec4 ndcCorners[4] = {
      {-1.2, -1.2, 1, 1},  // near-left
      {1.2, -1.2, 1, 1},   // near-right
      {-1.2, 1.2, 1, 1},   // far-left
      {1.2, 1.2, 1, 1}     // far-right
  };
  auto map_scale = tile->map_scale;
  float height = camera->GetOrigin().y / map_scale;
  glm::mat4 model_obj = glm::translate(glm::mat4(1.0f), glm::vec3(height));
  glm::mat4 model_map = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  glm::mat4 invVP =
      glm::inverse(camera->GetProjMatrix() * camera->GetViewMatrix(1.0f) *
                   model_map /* * model_obj*/);
  glm::vec3 groundPts[4];
  for (int i = 0; i < 4; ++i) {
    glm::vec4 p = invVP * ndcCorners[i];
    p /= p.w;
    auto camera_pos = camera->GetPosition() / map_scale;
    glm::vec3 dir = glm::normalize(glm::vec3(p) - camera_pos);

    // intersect ray with y=0
    float t = (height - camera_pos.y) / dir.y;
    groundPts[i] = camera_pos + dir * t;
  }
  GroundView view{
      .nearL = {groundPts[0].x, groundPts[0].z},
      .nearR = {groundPts[1].x, groundPts[1].z},
      .farL = {groundPts[2].x, groundPts[2].z},
      .farR = {groundPts[3].x, groundPts[3].z},
  };
  return view;
}
