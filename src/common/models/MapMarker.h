#ifndef WIREBOUNDWORLDCREATOR_MAPMARKER_H
#define WIREBOUNDWORLDCREATOR_MAPMARKER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../ui/UiRenderData.h"

class ModelData;

/// only color, no animations
class MapMarker {
 public:
  void Render(UiRenderData& render_data, glm::vec3 color,
              glm::vec2 position = glm::vec2(0.0f),
              glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
              glm::vec3 scale = glm::vec3(1.0f));

  void SetModel(ModelData* model_data) { model_data_ = model_data; }

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  glm::mat4 GenModelMat(UiRenderData& render_data,
                        glm::vec2 position, glm::quat rotation,
                        glm::vec3 scale);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

 private:
  bool selected_ = false;
  ModelData* model_data_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_MAPMARKER_H
