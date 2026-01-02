#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

struct ModelData;

class Creature : public RigidBody {
 public:
  Creature() = default;

  void Render(UiSharedResources& ui_shared_resources, GLuint position,
              glm::vec3 color);

  void Render(UiSharedResources& ui_shared_resources);

  void RenderPicking(UiSharedResources& ui_shared_resources);

  void RenderPicking(UiSharedResources& ui_shared_resources, GLuint position,
                     int id);

  void Update(UiSharedResources& ui_shared_resources);

  [[nodiscard]] const ModelData* GetModelData() const noexcept {
    return model_data_;
  }

  void SetModelData(const ModelData* model) { model_data_ = model; }

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  [[nodiscard]] bool IsSelected() const noexcept { return selected_; }

 private:
  const ModelData* model_data_ = nullptr;
  bool selected_ = false;
  GLuint instance_id_ = 0;  // screen "pixel"? anyway both can't fit one "pixel"
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_
