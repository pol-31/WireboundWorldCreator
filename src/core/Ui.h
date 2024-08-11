#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <vector>
#include <memory>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/ArbitraryGraph.h"
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
  UiButton(std::string_view description,
           UiData ui_data)
      : description_(description),
        ui_data_(ui_data) {}

  void Render() const;

  /// returns description & draw outlining / draw with higher brightness, etc
  [[nodiscard]] virtual std::string_view Hover() const {
    return description_;
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

 protected:
  /// comparing to Wirebound, here we add description for each button,
  /// which showed in some specific area externally
  std::string_view description_;
  UiData ui_data_;
};

class UiSliderHandle final : public UiButton {
 public:
  UiSliderHandle(std::string_view description,
                 UiData ui_data, float& edit_mode_progres_ref)
      : UiButton(description, ui_data),
        edit_mode_progres_ref_(edit_mode_progres_ref) {}

  void Render(const Shader& slider_shader) const;

  void SetPositionOffset(float progress, float pos_offset) {
    edit_mode_progres_ref_ = progress;
    pos_y_down_offset_ = pos_offset;
  }

 private:
  float pos_y_down_offset_{0.0f};
  float& edit_mode_progres_ref_;
};

/// to check was it pressed see UiSlots::Press(id)
class UiSlots {
 public:
  using PointDataType = const std::vector<ArbitraryGraph::Point>&;
  UiSlots(PointDataType points_data, UiButton&& btn_next,
          UiButton&& btn_prev, UiButton&& slot1,
          UiButton&& slot2, UiButton&& slot3,
          UiButton&& slot4, UiButton&& slot5,
          int& edit_mode_selected_sample_id);

  void Render(const Shader& shader) const;
  void RenderPicking(const Shader& shader) const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] std::string_view Hover(std::uint32_t id) const;

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

  // we don't own it and all what we need it its size (dynamically)
  PointDataType points_data_;
  int start_idx_{0}; // from where in view to start
  int& edit_mode_selected_sample_id_;
};

// UiBiomesList, UiObjectTable, UiTilesMap all use instanced vbo
// from stc/vbos/UiDataInstanced.h
class UiBiomesList {
 public:
  UiBiomesList(int vbo_offset_tex_cell, int vbo_offset_tex_all,
               int total_num, int vbo_pos_offset, int instances_num);

  void Press(glm::vec2 position, std::uint32_t id);

  void Render() const;

  std::string_view Hover(std::uint32_t id) const;

 private:
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8x4 (if we have n=8x4 buttons)
};

class UiObjectTable {};
class UiTilesMap {};

/// used to set amount of points captured by
/// single vertex transform (falloff, radius).
/// To deduce was it pressed you should use GetTrackId() and compare
class UiSlider {
 public:
  UiSlider(UiButton&& min_handle, UiButton&& max_handle,
           UiButton&& track, UiSliderHandle&& handle);

  void Render(const Shader& sprite_shader, const Shader& slider_shader) const;
  void RenderPicking(const Shader& picking_shader) const;

  [[nodiscard]] std::string_view Hover(std::uint32_t id) const;

  void UpdateSliderPos(glm::vec2 position);

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const;

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
