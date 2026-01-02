#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_

#include <glm/glm.hpp>
#include <vector>

#include "../../modes/UiSharedResources.h"

struct ModelData;

class Obstacle {
 public:
  Obstacle() = default;

  ~Obstacle() { DeleteVbo(placement_vbo_); }

  void Render(UiSharedResources& ui_shared_resources);

  void SetPlacement(UiSharedResources& ui_shared_resources,
                    const std::vector<GLuint>& positions);

  void ClearPlacement();

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(const ModelData* model) { model_data_ = model; }

 private:
  void GenerateVbo(const std::vector<glm::mat4>& model_matrices);

  static void DeleteVbo(GLuint vbo);

  glm::vec3 GenPosition(UiSharedResources& ui_shared_resources, GLuint pos_id);

  glm::quat GenRotation(GLuint pos_id);

  glm::vec3 GenScale(GLuint pos_id);

  GLuint placement_vbo_ = 0;

  const ModelData* model_data_ = nullptr;
  int instances_num_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_
