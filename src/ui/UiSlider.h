#ifndef WIREBOUNDWORLDCREATOR_UISLIDER_H
#define WIREBOUNDWORLDCREATOR_UISLIDER_H

#include "UiElement.h"
#include "UiSprite.h"
#include "UiText.h"

/// implemented as a UiSprite for track with child sprite handle and value text
class UiSliderH2 final : public UiElement {
 public:
  using BaseType = UiElement;

  UiSliderH2(data::UiId id_track, data::UiId id_handle);

  void Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
              float rotate, glm::vec2 scale) override;

  void Press(unsigned int pressed_id) override;

  void Release() override;

  void MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) override;

  void Scroll(unsigned int id, float yoffset) override;

  [[nodiscard]] float GetValue() const noexcept { return value_; }

  void Set(float value);

  static const glm::vec4 gColorDefault;
  static const glm::vec4 gColorHovered;
  static const float gTrackLengthFactor;

 private:
  // to handle dynamic position by parent transforms for sliders handle
  void CachePosition(const glm::mat3& transform, glm::vec2 scale);

  void SetValueByMousePos(glm::vec2 mouse_pos);

  void SetHandlePositionByValue();

  glm::vec4 color_ = gColorDefault;

  UiText txt_text_;
  UiSprite sp_handle_;

  // TODO: min, max, step values

  // TODO: quad add with Update() manual scissors test

  float min_ = 0.0f;
  float max_ = 1.0f;

  float value_ = 0.0f;
  bool pressed_ = false;

  float cached_length_ = 0.0f;
  glm::vec2 cached_center_ = glm::vec2(0.0f);
};
//
// class UiSliderV3 final : public UiElement {
//   UiSprite sp_fill_;
//   UiSprite sp_track_;
//   UiSprite sp_handle_;
//
//   float progress_{0.0f};
//   bool pressed_ = false;
//   float centre_;
//   float length_;
//   float scale_{1.0f};
//   static const float kTrackLengthFactor;
// };
//
// class UiSliderH3 final : public UiElement {
//   UiSprite sp_fill_;
//   UiSprite sp_track_;
//   UiSprite sp_handle_;
//
//   float progress_{0.0f};
//   bool pressed_{false};
//   float centre_;
//   float length_;
//   float scale_{1.0f};
//   static const float kTrackLengthFactor;
// };
//
// /// used for ui palette, so X-axis is Hue, Y-axis is Saturation
// class UiSlider2D final : public UiElement {
//   UiSprite sp_track_;
//   UiSprite sp_handle_;
//
//   glm::vec2 progress_ = glm::vec2{0.0f};
//   bool pressed_ = false;
//   glm::vec2 centre_;
//   glm::vec2 length_;
//   glm::vec2 scale_;
//
//   static const float kTrackWidthFactor;
//   static const float kTrackHeightFactor;
// };

#endif  // WIREBOUNDWORLDCREATOR_UISLIDER_H
