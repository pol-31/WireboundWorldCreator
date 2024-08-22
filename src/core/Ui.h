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

class UiButton {
 public:
  explicit UiButton(UiData ui_data)
      : ui_data_(ui_data) {}

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

class UiSliderHandle final : public UiButton {
 public:
  explicit UiSliderHandle(UiData ui_data)
      : UiButton(ui_data) {}

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
  UiSlots(const std::size_t& total_size, UiButton&& btn_next,
          UiButton&& btn_prev, UiButton&& slot1,
          UiButton&& slot2, UiButton&& slot3,
          UiButton&& slot4, UiButton&& slot5,
          int& edit_mode_selected_sample_id);

  void Render(const Shader& shader) const;
  void RenderPicking(const Shader& shader) const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] int Hover(std::uint32_t id) const;

 private:
  void InitColors();

  void RenderSlot(const Shader& shader, const UiButton& slot, int i) const;

  std::array<glm::vec4, 8> colors_{};

  //TODO: array of pointers IUi* + sort by Y and then binary search
  UiButton btn_next_;
  UiButton btn_prev_;

  UiButton slot1_;
  UiButton slot2_;
  UiButton slot3_;
  UiButton slot4_;
  UiButton slot5_;

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
  UiBiomesList(int vbo_offset, int instances_num,
               UiButton&& btn_next, UiButton&& btn_prev);

  void Render() const;
  void RenderPicking() const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  int Hover(std::uint32_t id) const;

 private:
  int vbo_offset_;
  int instances_num_;

  UiButton btn_next_;
  UiButton btn_prev_;

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

/// used to set amount of points captured by
/// single vertex transform (falloff, radius).
/// To deduce was it pressed you should use GetTrackId() and compare
class UiSlider {
 public:
  UiSlider(UiButton&& min_handle, UiButton&& max_handle,
           UiButton&& track, UiSliderHandle&& handle);

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
  UiButton min_handle_;
  UiButton max_handle_;
  UiButton track_;
  UiSliderHandle handle_;

  /// to not to calculate it each time at Press()
  float height_{0.0f};
  float progress_{0.0f}; // [0;1]
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
