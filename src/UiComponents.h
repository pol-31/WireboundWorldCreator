#ifndef WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_

#include <array>
#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Sprite.h"
#include "Aabb.h"

/// the rest of the "potential" ui components has single usage across
/// all modes, so they were directly embedded to modes

//TODO: this like sprite... and... with Contains() method... idk how to name
/// class used for all "buttons", so here we render and say, was cursor_pos click on it
class UiCollisionSprite {
 public:
  UiCollisionSprite(const Texture& tex, const TexCoords& tex_coords)
      : sprite_(tex, tex_coords), pos_() {}

  UiCollisionSprite(const Texture& tex, const TexCoords& tex_coords,
                    glm::mat2 pos)
      : sprite_(tex, tex_coords), pos_(pos) {
    ComputeAabb();
  }

  void SetPosition(glm::mat2 pos) {
    pos_ = pos;
    ComputeAabb();
  }

  [[nodiscard]] bool Contains(glm::vec2 pos) const {
    return aabb_.DoContain(pos);
  }

  void ComputeAabb();

  void Render(const Shader& already_bind_shader);

  void Bind(const Shader& already_bind_shader);

 private:
  glm::mat2 pos_;
  TexCoordsAabb aabb_;
  Sprite sprite_;
};


// total num: 5 is enough;
// don't store buttons, only idx, which MUST be invalidated with Add() or Remove()
class UiSlots {
 public:
  struct GraphNode {
    /// let's keep it as much simpler as possible;
    /// for water always children.size() == 1 (I could bear it to another
    /// class, but I'm too lazy for it now)
    GraphNode* parent{nullptr};
    std::vector<GraphNode> children;
    GLuint id;
  };

  struct Slot {
    UiCollisionSprite sprite;
    glm::vec4 color;
  };

  /// not std::map, because we don't need to add/remove single points,
  /// only modify their positions
  using PointSetType = std::vector<GraphNode>;

  UiSlots(const TexPosition& position, const Texture& tex)
      : btn_next_(tex, details::kBtnSlotsNext),
        btn_prev_(tex, details::kBtnSlotsPrev),
        slots_({{UiCollisionSprite(tex, details::kBtnSlot), {}},
                {UiCollisionSprite(tex, details::kBtnSlot), {}},
                {UiCollisionSprite(tex, details::kBtnSlot), {}},
                {UiCollisionSprite(tex, details::kBtnSlot), {}},
                {UiCollisionSprite(tex, details::kBtnSlot)}}) {
    InitPositions(position);
    InitColors();
  }

  // vao, vbo of single rectangular texture as well as shader
  void Render(const Shader& shader);

  // this function called from GlfwCallback on GLFW_MOUSE_LEFT && GLFW_PRESS
  /// returns idx of point_set
  int Press(glm::vec2 pos);

  /// we can't open "raw mode" in any other way except AddNew;
  /// we can't modify existing set of point, but only its graph representation;

  /// how graph is crated:
  /// water (assume it's circle-like) is one 1:1 graph (may be looped)


  /// we it only for adding a new point set, not a point to already existent,
  /// because if we baked, we already have all needed points (for each
  /// terrain grid), while unbaked just can't be saved - consequently modified
  void Add(PointSetType new_point_set);

  void Remove(int idx);

  [[nodiscard]] const std::vector<PointSetType>& GetPointSets() const {
    return point_sets_;
  }

  [[nodiscard]] int GetSelectedIdx() const {
    return selected_;
  }

 private:
  void InitPositions(const TexPosition& position);

  void InitColors();

  UiCollisionSprite btn_next_;
  UiCollisionSprite btn_prev_;
  std::array<Slot, 5> slots_;

  int selected_{-1};
  int start_idx_{0}; // from where in view to start

  std::vector<PointSetType> point_sets_;
};

/// used to set amount of points captured by
/// single vertex transform (falloff, radius)
class UiSlider {
 public:
  UiSlider(const TexPosition& position, const Texture& tex)
      : sprite_max_threshold_(tex, details::kBtnSliderMax),
        sprite_min_threshold_(tex, details::kBtnSliderMin),
        sprite_slider_obj_(tex, details::kBtnSliderObjLow),
        slider_area_(tex, details::kBtnSliderArea) {
    InitPositions(position);
    SetProgress(0.0f);
  }

  void Render(const Shader& shader);

  [[nodiscard]] float GetProgress() const {
    return progress_;
  }

  void SetProgress(float progress);

 private:
  void InitPositions(const TexPosition& position);

  //TODO: we don't use collision here, can be optimized
  UiCollisionSprite sprite_max_threshold_;
  UiCollisionSprite sprite_min_threshold_;
  UiCollisionSprite sprite_slider_obj_;

  UiCollisionSprite slider_area_;

  float progress_{0.0f}; // [0;1]
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_
