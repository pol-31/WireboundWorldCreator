#ifndef WIREBOUNDWORLDCREATOR_SRC_UICORE_H_
#define WIREBOUNDWORLDCREATOR_SRC_UICORE_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "UiComponents.h"

class ViewArea {
 public:
  void Press(GLuint id_from_picking_framebuffer);
  void Render();
};

enum class Mode {
  kWater,
  kTerrain,
  kRoads,
  kFences,
  kTrees
};

Mode gCurMode;
void ChangeMode(Mode new_mode) {
  switch(mode) {}
  gCurMode = new_mode;
}
void ChangeVisibility(unsigned int mask) { // mask1 | mask2 | mask3
  //...
}


class IGenMode {
 public:
  float panel_offset_x{0.0f};

  void Render() {
    RenderControlPanel();
    RenderViewPanel();
  }

  virtual void BindCallbacks() = 0; // for both View&Control area (including UiComponents) interaction

 protected:
  virtual void RenderControlPanel() = 0;
  virtual void RenderViewPanel() = 0;
};


bool terrain_visible_;
bool water_visible_;
bool roads_visible_;
// etc...

//TODO: global vars
//TerrainMode gTerrain;
//WaterMode gWater;
//RoadMode gRoads;
// etc (the rest)
IGenMode* cur_mode_;
int gSelectedPointId = -1; // selected related to the whole terrain, i.e. [0; 1024x1024]

class WaterMode final : public IGenMode {
 public:
  /// used in graph mode to modify underlying terrain
  enum class DrawType {
    kPosition,
    kSmooth,
    kSlopeBottom,
    kSlopeTop
  };

  void BindCallbacks() override {
    //TODO; separate on View/Control area wrt its ui components and call this->AddNew  or smt
    glm::vec2 converted_pos = ConvertCursorPos(cursor_pos);
    if (vertex_mode_btn_.Contains(converted_pos)) {
    } else if (graph_mode_btn_.Contains(converted_pos)) {
    } //TODO: more branches
  }

 private:
  void RenderControlPanel() override {
    //TODO: here bind shader, vbo, vao, but... its all default static_sprite and rect vbo, so once
    static_sprite_shader.Bind(); // TODO: or already bind? (don't think so)
    vertex_mode_btn_.Bind(static_sprite_shader);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    if (selected) {} // ... set appropriate color/brightness in shader
  }

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
  void BakeWaterfall();
  void BakeRiver();
  void BakeLake();


  /// MECHANIC which point to add or which to remove or how to render is here,
  /// while data stored in UiSlots. After both add and remove we leave the same order,
  /// so it's simply can be done in slots_. TODO: another question how to remove not from set
  /// of points but from graph with different in/out branches num ( do we need structure "graph"?
  /// - yes, obviously, so we have two UiSlots: points... idk
  void AddNew() {
    if (vectex_mode_) {
      AddNewVertex();
    } else {
      AddNewObject();
    }
  }
  void AddNewVertex() {
    if (gSelectedPointId == -1) {
      gSelectedPointId = cur_point_set_[cur_point_set_.size() - 1)];
    }
    //TODO: insert between this and its prev
    gSelectedPointId = currently_added_id;
  }


  // ui components
  UiSlots slots_; // all polygons/points data here
  UiSlider fuziness_; // TODO: unified interface
  UiSlider radius_;

  UiCollisionSprite vertex_mode_btn_;
  UiCollisionSprite graph_mode_btn_;
  UiCollisionSprite add_new_btn_;
  UiCollisionSprite remove_btn_;
  UiCollisionSprite bake_waterflow_btn_;
  UiCollisionSprite bake_river_btn_;
  UiCollisionSprite bake_lake_btn_; // lake/pond/sea/ocean
  UiCollisionSprite edit_position_btn_;
  UiCollisionSprite edit_smooth_btn_;
  UiCollisionSprite edit_slope_bottom_btn_;
  UiCollisionSprite edit_slope_top_btn_;

  bool vertex_mode_{false}; // otherwise "graph mode", used also for render outline
  DrawType draw_type_; // for graph_mode, used also for render outline
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UICORE_H_
