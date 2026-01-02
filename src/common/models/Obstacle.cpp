#include "Obstacle.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "ModelLoader.h"

void Obstacle::Render(UiSharedResources& ui_shared_resources) {
  if (instances_num_ == 0) {
    return;
  }
  ui_shared_resources.shader_mdl_instanced_.Bind();
  auto map_scale =
      ui_shared_resources.gltf_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(map_model));
  model_data_->BindTextures();
  glBindVertexArray(model_data_->vao);
  model_data_->RenderModelNodesInstanced(instances_num_);
  glBindVertexArray(0);
}

void Obstacle::SetPlacement(UiSharedResources& ui_shared_resources,
                            const std::vector<GLuint>& positions) {
  std::vector<glm::mat4> model_matrices;
  model_matrices.reserve(positions.size());
  for (const auto& pos_id : positions) {
    glm::vec3 position = GenPosition(ui_shared_resources, pos_id);
    glm::quat rotation = GenRotation(pos_id);
    glm::vec3 scale = GenScale(pos_id);
    glm::mat4 object_model = glm::mat4{1.0f};
    object_model = glm::translate(object_model, position);
    object_model *= glm::mat4_cast(rotation);
    object_model = glm::scale(object_model, scale);
    model_matrices.push_back(object_model);
  }
  GenerateVbo(model_matrices);
}

void Obstacle::ClearPlacement() { instances_num_ = 0; }

void Obstacle::GenerateVbo(const std::vector<glm::mat4>& model_matrices) {
  instances_num_ = model_matrices.size();
  GLuint prev_vbo = placement_vbo_;
  glBindVertexArray(model_data_->vao);
  glGenBuffers(1, &placement_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, placement_vbo_);
  glBufferData(GL_ARRAY_BUFFER, instances_num_ * sizeof(glm::mat4),
               model_matrices.data(), GL_STATIC_DRAW);

  std::size_t vec4Size = sizeof(glm::vec4);
  for (int i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void*)(i * vec4Size));
    glVertexAttribDivisor(3 + i, 1);
  }
  DeleteVbo(prev_vbo);
}

void Obstacle::DeleteVbo(GLuint vbo) { glDeleteBuffers(1, &vbo); }

glm::vec3 Obstacle::GenPosition(UiSharedResources& ui_shared_resources,
                                GLuint pos_id) {
  glm::vec3 position(0.0f);
  glm::uvec2 pos(pos_id >> 10, pos_id & 1023);
  position.x = pos.x / 16.0f - 32.0f;
  position.z = pos.y / 16.0f - 32.0f;

  int x = std::clamp(static_cast<int>(floorf(pos.x)), 1, 1022);
  int y = std::clamp(static_cast<int>(floorf(pos.y)), 1, 1022);

  float tx = pos.x - x;  // 0..1
  float ty = pos.y - y;  // 0..1

  const auto& h = ui_shared_resources.gltf_context_.tile_renderer->cur_tile_
                      .terrain_heights_;
  int stride = 1024;

  float h00 = h[y * stride + x];
  float h10 = h[y * stride + x + 1];
  float h01 = h[(y + 1) * stride + x];
  float h11 = h[(y + 1) * stride + x + 1];

  // bilinear interpolation
  float hx0 = h00 + (h10 - h00) * tx;
  float hx1 = h01 + (h11 - h01) * tx;
  position.y = hx0 + (hx1 - hx0) * ty;
  return position;
}

glm::quat Obstacle::GenRotation(GLuint pos_id) {
  glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
  return rotation;
}

glm::vec3 Obstacle::GenScale(GLuint pos_id) {
  glm::vec3 scale(0.3f);
  return scale;
}
