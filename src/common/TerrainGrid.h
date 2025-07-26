#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_

#include <array>
#include <set>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "IGraph.h"

/// similar to ArbitraryGraph, but for terrain ui-slots

// no Target {vertex, edge, face} state
// no Press {edit, select} state
class TerrainGrid final : public IGraph {
 public:
  // select modes (Shift-Ctrl modifiers):
  // - by click - press : produces separate points
  // - circle area - press / hold : produces std::vector<GLuint> on each update
  // - square area - hold : produces std::vector<GLuint>
  enum class SelectMode {
    kCircle,
    kSingle,
    kSquare
  };

  TerrainGrid(UiSharedResources& ui_shared_resources);

  void CreateGraph() override;

  void SelectGraph(int slot_id) override;

  void RemoveGraph(int slot_id) override;

  void Render(glm::vec2 mouse_pos) override;

  void UpdateVertexBuffer();

  // vec2 position -> GLuint point
  std::set<GLuint> ProjectCursorOnGridRadius(glm::vec2 mouse_pos);
  std::set<GLuint> CursorOnGridRadius(GLuint point);

  GLuint ProjectCursorOnGrid(glm::vec2 mouse_pos);

  std::set<GLuint> FindSinglePath(glm::vec2 pos1, glm::vec2 pos2);

  void Press(glm::vec2 mouse_pos,
             bool shift_pressed, bool ctrl_pressed);

  void Release() override;

  void Select(glm::vec2 mouse_pos);

  void Select(GLuint id);

  SelectMode FlipSelectMode();

  [[nodiscard]] int GetSize() const noexcept override;

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept override {
    return selected_slot_id_;
  }

  BaseInstanceData* GetBaseInstanceData(int id) override {
    return &instances_[id];
  };

  const FixedSizeQueue<char, 64>* GetNamePtr(int instance_id) const override {
    return &instances_[instance_id].name;
  }

  FixedSizeQueue<char, 64>* GetNamePtr(int instance_id) override {
    return &instances_[instance_id].name;
  }

  static constexpr size_t gMaxLayers = 10;
  static constexpr int gRadius = 10;

 private:
  struct InstanceData : public IGraph::BaseInstanceData {
    glm::vec3 scale = glm::vec3{1.0f};
    glm::vec3 rotate = glm::vec3{0.0f};
    glm::vec3 translate = glm::vec3{0.0f};
    bool do_tiling = false;

    // original size 1024 by 1024, so we could edit after saving
    std::array<uint8_t, 1024 * 1024> heights;
    Texture hmap; // TODO: class_TextureF
  };

  void Init();

  void DeInit();

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

  //TODO: __fix code__

  // last mouse pos - dynamic, updated each render frame
  glm::vec2 last_mouse_pos_;
  // update only at the beginning of shape draw
  glm::vec2 square_last_mouse_pos_;

  SelectMode select_mode_;
  // no Target {vertex, edge, face} - we have grid of points only

  // -- ------ --- -- -- --- --- --- ---
  // -- ------ --- -- -- --- --- --- ---

  /*
   * here we need:
   * std::array data + int size
   * std::move views on removing
   * */
  //TODO: can't use due to stack limitations (1Mb)
//  std::array<InstanceData, gMaxLayers> instances_;
  std::vector<InstanceData> instances_;
  int instances_size_ = 0;


  int selected_slot_id_{-1};
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
