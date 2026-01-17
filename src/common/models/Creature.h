#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_

#include <glm/glm.hpp>

#include "../../modes/UiSharedResources.h"
#include "RigidBody.h"

struct ModelData;

class Creature : public RigidBody {
 public:
  using RigidBody::RenderPicking;

  Creature();

  void Render(UiSharedResources& ui_shared_resources);

  void Update(UiSharedResources& ui_shared_resources);

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  [[nodiscard]] bool IsSelected() const noexcept { return selected_; }

 private:
  bool selected_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MODELS_CREATURE_H_
