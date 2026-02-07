#ifndef WIREBOUNDWORLDCREATOR_FPV_H
#define WIREBOUNDWORLDCREATOR_FPV_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../ui/UiRenderData.h"
#include "Animator.h"
#include "RigidBody.h"

class Fpv : public RigidBody {
 public:
  Fpv() = default;

  void Render(float map_scale);

  void Update(UiRenderData& render_data);

  void Select() { selected_ = true; }

  void DeSelect() { selected_ = false; }

  [[nodiscard]] bool IsSelected() const noexcept { return selected_; }

 protected:
  bool selected_ = false;
};

#endif  // WIREBOUNDWORLDCREATOR_FPV_H
