#include "MapMarker.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../Details.h"
#include "ModelLoader.h"

void MapMarker::Render(UiRenderData& render_data, glm::vec3 color,
                       glm::vec2 position, glm::quat rotation,
                       glm::vec3 scale) {
  glUniform3fv(1, 1, glm::value_ptr(color));
  model_data_->BindTextures();
  auto model = GenModelMat(render_data, position, rotation, scale);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
}

glm::mat4 MapMarker::GenModelMat(UiRenderData& render_data, glm::vec2 position,
                                 glm::quat rotation, glm::vec3 scale) {
  auto fx = static_cast<int>(position.x * 16.0f + 512.0f);
  auto fz = static_cast<int>(position.y * 16.0f + 512.0f);
  float ground_height =
      render_data.glfw_context_.tile_renderer->cur_tile_.GetPositionY(fx, fz);
  auto map_scale = render_data.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glm::mat4 object_model = glm::mat4{1.0f};
  object_model = glm::translate(
      object_model, glm::vec3(position.x, ground_height, position.y));
  object_model *= glm::mat4_cast(rotation);
  object_model = glm::scale(object_model, scale);
  glm::mat4 map_model =
      glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));  // upscaled
  return map_model * object_model;
}
