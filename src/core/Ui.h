#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <vector>
#include <memory>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Shader.h"
#include "../common/Colors.h"
#include "../common/Vbos.h"

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

class UiDynamicSprite {
 public:
  explicit UiDynamicSprite(UiData ui_data)
      : ui_data_(ui_data),
        scale_(1.0f, 1.0f),
        centre_pos_(
            (details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8],
             + details::kUiVboDataMain[ui_data_.vbo_offset * 4]) / 2,
            (details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
             + details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1]) / 2) {}

  UiDynamicSprite(VboIdMain vbo_texture, VboIdText vbo_text)
      : UiDynamicSprite(GetUiData(vbo_texture, vbo_text)) {}

  void Render() const {
    glUniform2fv(0, 1, glm::value_ptr(centre_pos_));
    glUniform2fv(1, 1, glm::value_ptr(scale_));
    glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
  }

  int Hover() const {
    return GetTextVboOffset();
  }

  void SetScale(glm::vec2 scale) {
    scale_ = scale;
  }

  void SetPosition(glm::vec2 position) {
    centre_pos_ = position;
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking(const Shader& picking_shader) const {
    glUniform1ui(shader::kSpritePickingId, static_cast<uint32_t>(ui_data_.id));
    glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
  }

  //TODO: looks like we don't need it
  /// we still need position for UiSlider (set progress) or composite
  /// ui components to check id and only then Press();
  /// position is needed for ui components like slider
  /// void Press(glm::vec2 position, std::uint32_t id) = 0;
  [[nodiscard]] float GetLeftBorder() const {
    float width = details::kUiVboDataMain[ui_data_.vbo_offset * 4]
                  - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
    return centre_pos_.x + width / 2;
  }

  [[nodiscard]] float GetRightBorder() const {
    float width = details::kUiVboDataMain[ui_data_.vbo_offset * 4]
                  - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
    return centre_pos_.x - width / 2;
  }

  [[nodiscard]] float GetTopBorder() const {
    float height = details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                  - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
    return centre_pos_.y + height / 2;
  }

  [[nodiscard]] float GetBottomBorder() const {
    float height = details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                   - details::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
    return centre_pos_.y - height / 2;
  }

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
  //TODO: we need only tex coords
  UiData ui_data_;
  glm::vec2 centre_pos_;
  glm::vec2 scale_;
};

class UiStaticSprite {
 public:
  explicit UiStaticSprite(UiData ui_data)
      : ui_data_(ui_data) {}

  UiStaticSprite(VboIdMain vbo_texture, VboIdText vbo_text)
      : UiStaticSprite(GetUiData(vbo_texture, vbo_text)) {}

  void Render() const;

  int Hover() const {
    return GetTextVboOffset();
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking(const Shader& picking_shader) const;

  //TODO: looks like we don't need it
  /// we still need position for UiSlider (set progress) or composite
  /// ui components to check id and only then Press();
  /// position is needed for ui components like slider
  /// void Press(glm::vec2 position, std::uint32_t id) = 0;

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








class UiSlider {
 public:
  //TODO: interpolation type enum
  UiSlider(UiDynamicSprite&& handler, UiDynamicSprite&& track,
           glm::vec2 pos_min, glm::vec2 pos_max)
      : handler_(handler),
        track_(track),
        pos_min_(pos_min),
        size_(pos_max - pos_min),
        length_(glm::length(size_)) {}

  void Set(glm::vec2 mouse_pos) {
    glm::vec2 offset = glm::clamp(mouse_pos - pos_min_, {}, size_);
    progress_ = glm::length(offset) / length_;
  }
  [[nodiscard]] float GetProgress() const {
    return progress_;
  }

  // shader has already been bind (as well as vao and texture...
  // Let's draw the entire ui component with the same shader,
  // so if there's dynamic component - we draw with dynamic shader only
  // to reduce switching)
  void Render() {
    track_.Render();
    handler_.SetPosition(pos_min_ + progress_ * size_);
    handler_.Render();
  }

  void RenderPicking() {
    track_.RenderPicking();
  }

 private:
  UiDynamicSprite handler_;
  UiDynamicSprite track_;
  const glm::vec2 pos_min_;
  const glm::vec2 size_; // width & height of the path
  const float length_;
  float progress_{0.0f};
};






class UiSliderHandle final : public UiStaticSprite {
 public:
  using UiStaticSprite::UiStaticSprite;

  void Render(const Shader& slider_shader) const;

  void SetPositionOffset(float pos_offset) {
    translate_.y = pos_offset;
  }

 private:
  glm::vec2 translate_{0.0f};
};

/// to check was it pressed see UiSlots::Press(id)
class UiSlots {
 public:
  UiSlots(const std::size_t& total_size, UiStaticSprite&& btn_next,
          UiStaticSprite&& btn_prev, UiStaticSprite&& slot1,
          UiStaticSprite&& slot2, UiStaticSprite&& slot3,
          UiStaticSprite&& slot4, UiStaticSprite&& slot5,
          int& edit_mode_selected_sample_id);

  void Render(const Shader& shader) const;
  void RenderPicking(const Shader& shader) const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] int Hover(std::uint32_t id) const;

 private:
  void InitColors();

  void RenderSlot(const Shader& shader, const UiStaticSprite& slot, int i) const;

  std::array<glm::vec4, 8> colors_{};

  //TODO: array of pointers IUi* + sort by Y and then binary search
  UiStaticSprite btn_next_;
  UiStaticSprite btn_prev_;

  UiStaticSprite slot1_;
  UiStaticSprite slot2_;
  UiStaticSprite slot3_;
  UiStaticSprite slot4_;
  UiStaticSprite slot5_;

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

/*/// used to set amount of points captured by
/// single vertex transform (falloff, radius).
/// To deduce was it pressed you should use GetTrackId() and compare
class UiSlider {
 public:
  UiSlider(UiStaticSprite&& min_handle, UiStaticSprite&& max_handle,
           UiStaticSprite&& track, UiSliderHandle&& handle);

  void Render(const Shader& sprite_shader, const Shader& slider_shader) const;
  void RenderPicking(const Shader& picking_shader) const;

  [[nodiscard]] int Hover(std::uint32_t id) const;

  void UpdateSliderPos(glm::vec2 position);

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const;

  [[nodiscard]] float GetProgress() const {
    return progress_;
  }

 private:
  UiStaticSprite min_handle_;
  UiStaticSprite max_handle_;
  UiStaticSprite track_;
  UiSliderHandle handle_;

  /// to not to calculate it each time at Press()
  float height_{0.0f};
  float progress_{0.0f}; // [0;1]
};*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
