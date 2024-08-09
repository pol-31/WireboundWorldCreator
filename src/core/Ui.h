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
#include "../vbos/UiDataMain.h"

/** All components stores vbo offset and provides AABB-like functions:
 * GetLeftBorder(), GetRightBorder(), GetTopBorder(), GetBottomBorder()
 * for more effective collision detection (let's take Terrain edit mode;
 * then because we know that ui located on the left side, we can discard
 * everything on the right side before ui_component.GetRightBorder()
 * */

struct Aabb {
  float left{std::numeric_limits<float>::max()};
  float right{std::numeric_limits<float>::min()};
  float top{std::numeric_limits<float>::min()};
  float bottom{std::numeric_limits<float>::max()};
};

// TODO: at program init we create button,
//  put it to std::vector<IUiBase all_buttons_ and then pass it as a pointer
//  to composite ui components or bvh (SIMPLE POINTER).
//  BUT need guarantee destruction safety
//TODO: NO, we store it locally for each edit mode
//   - there's no need to make it global

class IUi {
 public:
  IUi(const Shader& shader, std::string_view description)
      : shader_(shader),
        description_(description) {}
  virtual ~IUi() = default;

  [[nodiscard]] virtual bool AabbContains(glm::vec2 position) const = 0;

  virtual void Render() const = 0;

  /// we still need position for UiSlider (set progress) or composite
  /// ui components to check AabbContains(pos) and only then Press()
  virtual void Press(glm::vec2 position) = 0;

  /// returns description & draw outlining / draw with higher brightness, etc
  [[nodiscard]] virtual std::string_view Hover(glm::vec2 position) const {
    return description_;
  }

 protected:
  const Shader& shader_;

  /// comparing to Wirebound, here we add description for each button,
  /// which showed in some specific area externally
  std::string_view description_;
};

class UiButton : public IUi {
 public:
  UiButton(const Shader& shader, std::string_view description,
           int vbo_offset)
      : IUi(shader, description),
        vbo_offset_(vbo_offset) {}

  [[nodiscard]] bool AabbContains(glm::vec2 position) const override;

  void Render() const override;

  void Press(glm::vec2 position) override = 0;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

 protected:
  int vbo_offset_{0};
};

class TerrainMode;

class UiButtonTerrainModeBake final : public UiButton {
 public:
  UiButtonTerrainModeBake(
      const Shader& shader, std::string_view description,
      int vbo_offset, TerrainMode& terrain_mode)
      : UiButton(shader, description, vbo_offset),
        terrain_mode_(terrain_mode) {}

  void Press(glm::vec2 position) override;

 private:
  TerrainMode& terrain_mode_;
};

class UiButtonTerrainModeSmooth final : public UiButton {
 public:
  UiButtonTerrainModeSmooth(
      const Shader& shader, std::string_view description,
      int vbo_offset, TerrainMode& terrain_mode)
      : UiButton(shader, description, vbo_offset),
        terrain_mode_(terrain_mode) {}

  void Press(glm::vec2 position [[maybe_unused]]) override;

 private:
  TerrainMode& terrain_mode_;
};

class UiStaticSprite final : public UiButton {
 public:
  using UiButton::UiButton;
  void Press(glm::vec2 position) override {}
};

class UiSliderHandle final : public UiButton {
 public:
  UiSliderHandle(const Shader& shader, std::string_view description,
                 int vbo_offset, float& edit_mode_progres_ref)
      : UiButton(shader, description, vbo_offset),
        edit_mode_progres_ref_(edit_mode_progres_ref) {}

  void Press(glm::vec2 position) override {}
  void Render() const override;

  void SetPositionOffset(float progress, float pos_offset) {
    edit_mode_progres_ref_ = progress;
    pos_y_down_offset_ = pos_offset;
  }

 private:
  float pos_y_down_offset_{0.0f};
  float& edit_mode_progres_ref_;
};

class UiSlots final : public IUi {
 public:
  using PointDataType = const std::vector<ArbitraryGraph::Point>&;
  UiSlots(const Shader& shader, std::string_view description,
          PointDataType points_data, UiStaticSprite& btn_next,
          UiStaticSprite& btn_prev, UiStaticSprite& slot1,
          UiStaticSprite& slot2, UiStaticSprite& slot3,
          UiStaticSprite& slot4, UiStaticSprite& slot5,
          int& edit_mode_selected_sample_id);

  [[nodiscard]] bool AabbContains(glm::vec2 position) const override;

  void Render() const override;

  void Press(glm::vec2 position) override;

  [[nodiscard]] std::string_view Hover(glm::vec2 position) const override;

 private:
  void InitAabb();

  void InitColors();

  void RenderSlot(UiStaticSprite& slot, int i) const;

  std::array<glm::vec4, 8> colors_{};

  //TODO: array of pointers IUi* + sort by Y and then binary search
  UiStaticSprite& btn_next_;
  UiStaticSprite& btn_prev_;

  UiStaticSprite& slot1_;
  UiStaticSprite& slot2_;
  UiStaticSprite& slot3_;
  UiStaticSprite& slot4_;
  UiStaticSprite& slot5_;

  Aabb aabb_;

  // we don't own it and all what we need it its size (dynamically)
  PointDataType points_data_;
  int start_idx_{0}; // from where in view to start
  int& edit_mode_selected_sample_id_;
};

// UiBiomesList, UiObjectTable, UiTilesMap all use instanced vbo
// from stc/vbos/UiDataInstanced.h
class UiBiomesList final : public IUi {
 public:
  UiBiomesList(const Shader& shader, std::string_view description,
               int vbo_offset_tex_cell, int vbo_offset_tex_all,
               int total_num, int vbo_pos_offset, int instances_num);

  void Press(glm::vec2 position) override;

  void Render() const override;

  std::string_view Hover(glm::vec2 position) const override;

 private:
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8x4 (if we have n=8x4 buttons)
};

class UiObjectTable {};
class UiTilesMap {};

/// used to set amount of points captured by
/// single vertex transform (falloff, radius)
//TODO: looks like we don't need inheritance here
class UiSlider final : public IUi {
 public:
  UiSlider(const Shader& shader, std::string_view description,
           UiStaticSprite&& min_handle, UiStaticSprite&& max_handle,
           UiStaticSprite&& track, UiSliderHandle&& handle);

  [[nodiscard]] bool AabbContains(glm::vec2 position) const override;

  void Render() const override;

  [[nodiscard]] std::string_view Hover(glm::vec2 position) const override;

  void Press(glm::vec2 position) override;

  void UpdateSliderPos(glm::vec2 position);

  bool TrackAreaAabbContains(glm::vec2 position);

 private:
  void InitAabb();

  const UiStaticSprite& min_handle_;
  const UiStaticSprite& max_handle_;
  const UiStaticSprite& track_;
  UiSliderHandle& handle_;

  Aabb aabb_;

  /// to not to calculate it each time at Press()
  float height_{0.0f};
  float progress_{0.0f}; // [0;1]
};

class BvhNode {
 public:
  explicit BvhNode(IUi* left_child, IUi* right_child)
      : left_child_(left_child), right_child_(right_child) {}

  void Render() const;

  void Press(glm::vec2 position);

  std::string_view Hover(glm::vec2 position) const;

 private:
  IUi* left_child_;
  IUi* right_child_;
};

//TODO: each Edit Mode stores this rather that std::vector
//  as an acceleration structure for Press() and Hover()
BvhNode* GenBvhHierarchy(std::vector<IUi*> components);

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
