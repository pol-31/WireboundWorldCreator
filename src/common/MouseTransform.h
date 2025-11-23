#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Shader.h"
#include "Texture.h"
#include "../modes/UiSharedResources.h"

class MouseTransform {
 public:
  MouseTransform(UiSharedResources& ui_shared_resources);

  void CancelTransform();
  void ApplyTransform();

  void InitTransform(
      glm::vec3* translate, glm::quat* rotate, glm::vec3* scale);

  glm::vec3 GetInstanceTransform();

  void TranslateSelected(
      double xpos, double ypos);
  void RotateSelected(double xpos, double ypos);
  void ScaleSelected(double xpos, double ypos);
 void TranslateSelectedVerticesUp(
      double xpos, double ypos,
      const Texture& selection_mask, float falloff);

 void Reset();

 void SetAxis(glm::vec3 axis) {
   axis_ = axis;
 }

 [[nodiscard]] glm::vec3 GetAxis() const noexcept {
   return axis_;
 }

 private:
  void UpdateStartAngle();
  float UpdateAngle(float xpos, float ypos);

  glm::vec3 axis_ = glm::vec3(1.0f);

  glm::vec3 temp_translate_{0.0f};
  glm::quat temp_rotate_{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 temp_scale_{0.0f};

  float last_angle_ = 0.0f;
  float zero_angle_ = 0.0f;
  float zero_scale_length_ = 0.0f;
  float prev_value_y_ = 0.0f;

  glm::vec3* translate_ = nullptr;
  glm::quat* rotate_ = nullptr;
  glm::vec3* scale_ = nullptr;

  UiSharedResources& ui_shared_resources_;

  Shader vertices_transform_shader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MOUSETRANSFORM_H_
