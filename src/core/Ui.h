#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <vector>
#include <memory>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Shader.h"
#include "../common/Colors.h"
#include "../common/Vbos.h"
#include "../io/Window.h"

//TODO: we can accelerate our search by binary search, but
//  looks like the best way to do this is "in-place" binary search and
//  direct if-else branching

/** All components stores vbo offset and provides AABB-like functions:
 * GetLeftBorder(), GetRightBorder(), GetTopBorder(), GetBottomBorder()
 * for more effective collision detection (let's take Terrain edit mode;
 * then because we know that ui located on the left side, we can discard
 * everything on the right side before ui_component.GetRightBorder()
 * */

/// if function doesn't have shader in parameters,
/// you should bind it before the call

// details::kUiVboDataMain store data both for Dynamic and Static sprites
// in the same format, the only difference is how class initialize its data

// Let's draw the entire ui component with the same shader,
// so if there's dynamic component - we draw with dynamic shader only
// to reduce switching)

class UiDynamicSprite {
 public:
  explicit UiDynamicSprite(UiData ui_data)
      : ui_data_(ui_data) {}

  UiDynamicSprite(VboIdMain vbo_texture, VboIdText vbo_text)
      : UiDynamicSprite(GetUiData(vbo_texture, vbo_text)) {}

  void Render() const;

  [[nodiscard]] size_t Hover() const {
    return GetTextVboOffset();
  }

  void Rotate(float radians) {
    transform_ = glm::rotate(transform_, radians);
  }

  void Scale(glm::vec2 scale) {
    transform_ = glm::scale(transform_, scale);
  }

  void Translate(glm::vec2 position) {
    transform_ = glm::translate(transform_, position);
  }

  void SetTransform(glm::mat3 transform) {
    transform_ = transform;
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

  [[nodiscard]] std::uint32_t GetId() const {
    return ui_data_.id;
  }

  [[nodiscard]] std::size_t GetVboOffset() const {
    return ui_data_.vbo_offset;
  }

  [[nodiscard]] std::size_t GetTextVboOffset() const {
    return ui_data_.text_vbo_offset_;
  }

 protected:
  //TODO: we need only tex coords (?)
  UiData ui_data_;
  glm::mat3 transform_{1.0f};
};

class UiStaticSprite {
 public:
  explicit UiStaticSprite(UiData ui_data)
      : ui_data_(ui_data) {}

  UiStaticSprite(VboIdMain vbo_texture, VboIdText vbo_text)
      : UiStaticSprite(GetUiData(vbo_texture, vbo_text)) {}

  void Render() const;

  [[nodiscard]] size_t Hover() const {
    return GetTextVboOffset();
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

  [[nodiscard]] std::uint32_t GetId() const {
    return ui_data_.id;
  }

  [[nodiscard]] std::size_t GetVboOffset() const {
    return ui_data_.vbo_offset;
  }

  [[nodiscard]] std::size_t GetTextVboOffset() const {
    return ui_data_.text_vbo_offset_;
  }

 protected:
  UiData ui_data_;
};

class UiSliderBase {
 public:
  UiSliderBase(UiDynamicSprite&& handle, UiDynamicSprite&& track)
      : handle_(handle), track_(track) {}

  [[nodiscard]] float GetProgress() const {
    return progress_;
  }

  void RenderPicking() {
    track_.RenderPicking();
  }

  [[nodiscard]] size_t Hover(std::uint32_t id) const {
    if (id == track_.GetId()) {
      return track_.Hover();
    } else {
      return {};
    }
  }

  void Press() {
    pressed_ = true;
  }

  void Release() {
    pressed_ = false;
  }

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return track_.GetId();
  }

 protected:
  UiDynamicSprite handle_;
  UiDynamicSprite track_;
  float progress_{0.0f};
  bool pressed_{false};
};

/// Horizontal slider
/// 1. we don't use scale - its uniform through the app (same as artist draw)
/// 2. we always rotate on 90 degrees, because in our sprites it's the same
///   as UiHSlider, but rotated horizontally
class UiHSlider final : public UiSliderBase {
 public:
  UiHSlider(UiDynamicSprite&& handle, UiDynamicSprite&& track,
            glm::vec2 track_position, glm::vec2 handle_position,
            float track_width);

  void Render(glm::vec2 mouse_pos);

  void Set(glm::vec2 mouse_pos);

 private:
  glm::vec2 handle_position_;
  glm::vec2 track_position_;
  float width_;
};

/// Vertical slider
/// 1. we don't use scale - its uniform through the app (same as artist draw)
class UiVSlider final : public UiSliderBase {
 public:
  UiVSlider(UiDynamicSprite&& handle, UiDynamicSprite&& track,
            glm::vec2 track_position, glm::vec2 handle_position,
            float track_height);

  void Render(glm::vec2 mouse_pos);

  void Set(glm::vec2 mouse_pos);

 private:
  glm::vec2 handle_position_;
  glm::vec2 track_position_;
  float height_;
};

/// Circle slider
/// 1. we don't use scale - its uniform through the app (same as artist draw)
class UiCircleSlider final : public UiSliderBase {
 public:
  UiCircleSlider(UiDynamicSprite&& handle, UiDynamicSprite&& track,
                 glm::vec2 track_position, glm::vec2 handle_offset)
      : UiSliderBase(std::move(handle), std::move(track)),
        handle_offset_(handle_offset),
        track_position_(track_position) {
    track_.SetTransform(glm::translate(glm::mat3{1.0f}, track_position));
    Set(glm::vec2(0.0f, -2.0f));
  }

  void Render(glm::vec2 mouse_pos) {
    if (pressed_) {
      Set(mouse_pos);
    }
    track_.Render();
    handle_.Render();
  }

  void Set(glm::vec2 mouse_pos) {
    float angle = glm::atan(mouse_pos.y - track_position_.y,
                            mouse_pos.x - track_position_.x);
    auto transform = glm::translate({1.0f}, handle_offset_);
    transform = glm::rotate(transform, angle);
    handle_.SetTransform(transform);
    progress_ = (angle + std::numbers::pi_v<float>)
                / (2.0f * std::numbers::pi_v<float>);
  }

 private:
  glm::vec2 handle_offset_;
  glm::vec2 track_position_;
};

/// Circle slider
/// 1. we don't use scale - its uniform through the app (same as artist draw)
/// 2. technically the same as UiVSlider
/// 3. on each iteration if we hover, we change sprite of a hand, but
///    then at Render() we "unhover it" after drawing a hover version
class UiWheelSlider {
 public:
  UiWheelSlider(UiStaticSprite&& wheel, UiStaticSprite&& rope,
                UiStaticSprite&& hand, glm::vec2 track_position,
                float track_height)
      : wheel_(wheel), rope_(rope), hand_(hand),
        track_position_(track_position),
        height_(track_height) {
    Set(glm::vec2(0.0f, -2.0f));
  }

  [[nodiscard]] float GetProgress() const {
    return progress_;
  }

  void RenderPicking() {
    rope_.RenderPicking();
  }

  void Render(glm::vec2 mouse_pos) {
    if (pressed_) {
      float prev_progress = progress_;
      Set(mouse_pos);
      float diff = glm::abs(prev_progress - progress_);
      if (diff == 0.0f) {
        wheel_.SetVboOffset(details::kWheelCalm);
      } else if (diff < 0.3f) {
        wheel_.SetVboOffset(details::kWheelSlow);
      } else {
        wheel_.SetVboOffset(details::kWheelFast);
      }
    }
    wheel_.Render();
    rope_.Render();
    hand_.Render();
    UnHover();
  }

  [[nodiscard]] size_t Hover(std::uint32_t id) const {
    hand_.SetVboOffset(details::kHandRelease);
    return rope_.Hover();
  }

  void Set(glm::vec2 mouse_pos) {
    float half_heigth_ = height_ / 2.0f;
    float offset = glm::clamp(mouse_pos.y - track_position_.y,
                              -half_heigth_, +half_heigth_);
    progress_ = (offset + half_heigth_) / height_;
  }

  void Press() {
    pressed_ = true;
  }

  void Release() {
    pressed_ = false;
  }

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return rope_.GetId();
  }

 private:
  void UnHover() {
    hand_.SetVboOffset(details::kHandHold);
  }

  UiStaticSprite wheel_;
  UiStaticSprite rope_;
  UiStaticSprite hand_;

  glm::vec2 track_position_;
  float height_;

  float progress_{0.0f};
  bool pressed_{false};
};

/// position lerp based on time
class UiTabAnimation {
 public:
  UiTabAnimation(UiDynamicSprite&& sprite, glm::vec2 start_pos,
                 glm::vec2 end_pos, float speed = 1.0f)
      : sprite_(sprite),
        start_pos_(start_pos),
        end_pos_(end_pos),
        speed_(speed) {
    End();
  }

  void Render() {
    if (progress_ < 1.0f) {
      /// if active
      progress_ += gDeltaTime * speed_;
      glm::vec2 position = glm::mix(start_pos_, end_pos_, progress_);
      sprite_.SetTransform(glm::translate(glm::mat3{}, position));
      if (progress_ > 1.0f) {
        progress_ = 1.0f;
      }
    }
    sprite_.Render();
  }

  void RenderPicking() {
    sprite_.RenderPicking();
  }

  [[nodiscard]] size_t Hover() {
    return sprite_.Hover();
  }

  [[nodiscard]] std::uint32_t GetId() const {
    return sprite_.GetId();
  }

  void Start() {
    progress_ = 0.0f;
  }

  void End() {
    sprite_.SetTransform(glm::translate(glm::mat3{}, start_pos_));
    progress_ = 1.0f;
  }

 private:
  UiDynamicSprite sprite_;
  glm::vec2 start_pos_;
  glm::vec2 end_pos_;

  // 0-1 for progress; 1 means either "not active" or end_pos_
  float progress_{1.0f};
  float speed_;
};





/// to check was it pressed see UiSlots::Press(id)
class UiSlots {
 public:
  UiSlots(const std::size_t& total_size, UiStaticSprite&& btn_next,
          UiStaticSprite&& btn_prev, UiStaticSprite&& slot1,
          UiStaticSprite&& slot2, UiStaticSprite&& slot3,
          UiStaticSprite&& slot4, UiStaticSprite&& slot5,
          int& edit_mode_selected_sample_id);

  void Render() const;
  void RenderPicking() const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] int Hover(std::uint32_t id) const;

 private:
  void InitColors();

  void RenderSlot(int i) const;

  std::array<glm::vec4, 8> colors_{};

  // 5 x slots, btn_next, btn_prev
  std::array<UiStaticSprite, 7> slots_;

  // we don't own point/graph data and all what we need is size (dynamically)
  const std::size_t& total_size_;
  int start_idx_{0}; // from where in view to start
  int& edit_mode_selected_sample_id_;
};

/// UiBiomesList, UiObjectTable, UiTilesMap all use instanced vbo
/// from stc/vbos/UiDataInstanced.h

/// to check was it pressed see UiSlots::Press(id)
class UiBiomesList {
 public:
  // vbo_offset for instanced
  UiBiomesList(int vbo_offset, int instances_num, UiStaticSprite&& btn_next,
               UiStaticSprite&& btn_prev);

  void Render() const;
  void RenderPicking() const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  int Hover(std::uint32_t id) const;

 private:
  int vbo_offset_;
  int instances_num_;

  UiStaticSprite btn_next_;
  UiStaticSprite btn_prev_;

  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8 (8 biomes at once)
};

class UiObjectTable {
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8x4 (if we have n=8x4 buttons)
};
class UiTilesMap {
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 6x6 (if we have n=6x6 tiles at once)
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
