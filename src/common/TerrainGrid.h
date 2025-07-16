#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "IGraph.h"

/// similar to ArbitraryGraph, but for terrain ui-slots

class TerrainGrid final : public IGraph {
 public:
  TerrainGrid(UiSharedResources& ui_shared_resources);

  //TODO:
  void Press(GLuint id) {
    Select(id);
  }

  void Select(GLuint vertex_id);

  void SelectVertex(GLuint vertex_id);

  void SelectEdge(GLuint vertex_id);

  void SelectFace(GLuint vertex_id);

  /// emplace back
  void CreateGraph() override;

  /// move instances_[slot_id] data to the end, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void SelectGraph(int slot_id) override;

  /// remove instances_[slot_id] from all buffers, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void RemoveGraph(int slot_id) override;

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept override {
    return selected_slot_id_;
  }

  [[nodiscard]] int GetSize() const noexcept override {
    return instances_.size();
  }

  BaseInstanceData* GetBaseInstanceData(int id) override {
    return &instances_[id];
  };

  void FlipSelectMode();

  void Render() override;

  static constexpr size_t gMaxLayers = 100;

 private:
  enum class Target {
    kVertices,
    kEdges,
    kFaces
  };
  struct InstanceData : public IGraph::BaseInstanceData {
    std::array<float, 1024 * 1024> heights;
//    Texture tex_f_hmap_;
    //TODO: TextureF?
  };

  void EditPoint(GLuint vertex_id);
  void EditEdgePoint(GLuint vertex_id);

  // wrt slots, so at creation we push_back, at remove we remove & decrement
  // instances_[slot_id] and NOT instances_[instance_id]
  // so the dependency is: graph_id == instances_[slot_id]
  std::vector<InstanceData> instances_;

  int selected_slot_id_{-1};

  std::vector<GLuint> selected_ids_;

  Target target_;

  UiSharedResources& ui_shared_resources_;
};

class TerrainGrid {
 public:
  // no Target {vertex, edge, face} state
  // no Press {edit, select} state

  // select modes (Shift-Ctrl modifiers):
  // - by click - press : produces separate points
  // - circle area - press / hold : produces std::vector<GLuint> on each update
  // - square area - hold : produces std::vector<GLuint>
  enum class SelectMode {
    kSingle,
    kCircle,
    kSquare
  };

  void Render(glm::vec2 mouse_pos) {
    if (pressed_) {
      Select(mouse_pos, true)
    }
    //TODO: render
  }

  void Press(glm::vec2 mouse_pos) {
    pressed_ = true;
    if (shift_pressed) {
      if (select_mode == kCircle) {
        std::set<GLuint> new_vertices;
        // :=:=:=:=:=:
        new_vertices = FindCiclePath(last_mouse_pos_, mouse_pos);
      } else {
        // ._._._._._.
        new_vertices = FindSinglePath(last_mouse_pos_, mouse_pos);
      }
      selected_vertices_ = std::unite(selected_vertices_, new_vertices);
    } else if (!ctrl_pressed) {
      selected_vertices_.clear();
    }
    GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
    if (select_mode == kClick) {
      selected_vertices_.push_back(pressed_id);
    } else if (select_mode == kCircle) {
      auto new_vertices = ProjectCursorOnGridRadius(mouse_pos, radius);
      selected_vertices_ = std::unite(selected_vertices_, new_vertices);
    } else { // select_mode == kSquare
      last_mouse_pos_ = mouse_pos;
    }
    UpdateVertexBuffer();
  }

  void Release() {
    pressed_ = false;
    if (select_mode == kSquare) {
      auto new_vertices = picking_fbo.SelectSquare(last_mouse_pos_);
      selected_vertices_ = std::unite(selected_vertices_, new_vertices);
    }
  }

 private:
  /// according to fbo picking id's (storing differs)
  /// (no copy, single instance -> std::set)
  std::set<GLuint> selected_vertices_;

  // we can modify only heights, that are related directly to vertex traits,
  // so "selecting edges / faces" select vertices as well AND we don't need:
//  std::set<GLuint> selected_edges_;
//  std::set<GLuint> selected_faces_;

  /// CREATE SECTION -- no new geometry
  // no vertex creation - full terrain grid
  // no edge creation - no graphs, works on select only
  // no face creation - no graphs, works on select only

  /// RENDER SECTION
  // vertices only: draw selected in different color, GL_DRAW_POINTS
  GLuint vao_;
  GLuint vbo_;

  /// SELECT SECTION
  bool pressed_ = false;

  glm:vec2 last_mouse_pos_;

  SelectMode select_mode_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
