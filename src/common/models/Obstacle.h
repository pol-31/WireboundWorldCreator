#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_

#include <vector>

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

struct ModelData;

class Obstacle : public RigidBody {
 public:
  Obstacle() = default;

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(const ModelData* model) {
    model_data_ = model;
  }

 private:
  const ModelData* model_data_ = nullptr;
  std::vector<glm::vec3> seeds_; // for pseudo-rand transform matrices
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_OBSTACLE_H_
