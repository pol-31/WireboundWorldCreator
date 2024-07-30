#ifndef WIREBOUNDWORLDCREATOR_SRC_UICORE_H_
#define WIREBOUNDWORLDCREATOR_SRC_UICORE_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "UiComponents.h"

//TODO:
// - scree_coord to pos on window [0;details::kWindowWidth] -> [-1;1]
// inside the callback we need (cur_pos / (kWindowWidth / 2.0f) - 1.0f)

//TODO: we also need "contrast" for placement

struct Visibility {
  bool terrain{true};
  bool water{true};
  bool roads{true};
  bool fences{true};
  bool biome{true};
  bool objects{true};
};

class IGenMode;
class TerrainMode;
class WaterMode;
class RoadsMode;
class FencesMode;
class BiomesMode;
class ObjectsMode;
class HeightMapDrawMode;

//TODO: it's not necessary to make them global; we can avoid it by now

extern TerrainMode gTerrainMode;
extern WaterMode gWaterMode;
extern RoadsMode gRoadsMode;
extern FencesMode gFencesMode;
extern BiomesMode gBiomesMode;
extern ObjectsMode gObjectsMode;
extern HeightMapDrawMode gHeightMapDrawMode;

extern IGenMode* gCurMode;
extern Visibility gVisibility;

class IGenMode {
 public:
  virtual void Render() = 0;
  virtual void BindCallbacks() = 0;
};

class TerrainMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

class WaterMode final : public IGenMode {
 public:
  /// used in graph mode to modify underlying terrain
  enum class DrawType {
    kPosition,
    kSmooth,
    kSlopeBottom,
    kSlopeTop
  };

  //TODO: all components by onw with setup highlightnig (easy)
  void Render() override {
    //TODO: here bind shader, vbo, vao, but... its all default static_sprite and rect vbo, so once
    static_sprite_shader.Bind(); // TODO: or already bind? (don't think so)
    vertex_mode_btn_.Bind(static_sprite_shader);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    if (selected) {} // ... set appropriate color/brightness in shader
  }

  void BindCallbacks() override {
    //TODO; separate on View/Control area wrt its ui components and call this->AddNew  or smt
    glm::vec2 converted_pos = ConvertCursorPos(cursor_pos);
    if (vertex_mode_btn_.Contains(converted_pos)) {
    } else if (graph_mode_btn_.Contains(converted_pos)) {
    } //TODO: more branches
  }

 private:
  //  - we render water separately from terrain (we have different shaders)
  //  - we have few FloodFill algorithms: Pond/River/Waterfall
  //  - we have std::vector<std::vector<GLuint>> water_headers_; to draw
  //    multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)
  //  - we can remove/add new header, but after each such move we recalculate all water
  /**
   generate 3 types: pond (take the highest point and fill everything below),
   river(inside the polygon generate pond, but the take bottom
   height of pond and for outside point if height less than
   pond bottom - reduce water height on difference), waterfall(fill only below)
   */
  // TODO: check: point idx in ccw order, convex polygons only
  void BakeWaterfall();
  void BakeRiver();
  void BakeLake();

  bool IsConvex();


  /// MECHANIC which point to add or which to remove or how to render is here,
  /// while data stored in UiSlots. After both add and remove we leave the same order,
  /// so it's simply can be done in slots_. TODO: another question how to remove not from set
  /// of points but from graph with different in/out branches num ( do we need structure "graph"?
  /// - yes, obviously, so we have two UiSlots: points... idk

  // it's called only from vertex_mode_ and only from GLFW_MOUSE_LEFT + GLFW_PRESS
  void AddNewVertex(GLuint new_id) {
    if (cur_point_set_.empty()) {
      cur_point_set_.push_back({nullptr, {}, new_id});
      cur_selected_point_ = 0;
      return;
    }
    if (cur_selected_point_ == -1) {
      cur_selected_point_ = cur_point_set_[cur_point_set_.size() - 1].id;
    }
    if (cur_point_set_[cur_selected_point_].children.size() == 0) {
      cur_point_set_[cur_selected_point_].children.push_back({nullptr, {}, new_id});
    } else {
      auto next_child = cur_point_set_[cur_selected_point_].children[0];
      cur_point_set_[cur_selected_point_].children[0] = {nullptr, {}, new_id};
      cur_point_set_[cur_selected_point_].children[0].children.push_back(next_child);
    }
  }

  void AddNewObject() {
    if (vertex_mode_) {
      return; // we already here
    }
  }

  UiSlots::PointSetType cur_point_set_;

  /// -1 as "not selected"
  GLuint cur_selected_point_{static_cast<GLuint>(-1)};

  /// --- --- ui components --- ---
  UiSlots slots_; // all graph data
  UiSlider falloff_;
  UiSlider radius_;

  UiCollisionSprite add_new_btn_;
  UiCollisionSprite remove_btn_;
  UiCollisionSprite bake_waterflow_btn_;
  UiCollisionSprite bake_river_btn_;
  UiCollisionSprite bake_lake_btn_; // lake/pond/sea/ocean
  UiCollisionSprite edit_position_btn_;
  UiCollisionSprite edit_smooth_btn_;
  UiCollisionSprite edit_slope_bottom_btn_;
  UiCollisionSprite edit_slope_top_btn_;

  bool vertex_mode_{false}; // vertex (polygon creation) or graph mode
  DrawType draw_type_{DrawType::kPosition};
};

class RoadsMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

class FencesMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

class BiomesMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

class ObjectsMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

class HeightMapDrawMode final : public IGenMode {
 public:
  void Render() override {}
  void BindCallbacks() override {}
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UICORE_H_
