#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "IGraph.h"

/// similar to TerrainGrid, but for roads, fences, water, biomes ui-slots

// TerrainModeCallback + states -> ... -> ui_slots_.Press(id)
//
//
// ui_slots_ :: edit state (always) + create/select state (optional)
// ... something in between ... EITHER: UiMode | UiSlots | Graph | UiEditMode
// graph: Add/Edit/Remove <-> Vertex/Edge/Face

// undirected arbitrary graph

class ArbitraryGraph final : public IGraph {
 public:
  ArbitraryGraph(UiSharedResources& ui_shared_resources);

  ~ArbitraryGraph() {
    DeInit();
  }

  void AddVertex(GLuint vertex_id);

  void AddEdge(GLuint vertex_id);

  void AddFace(GLuint vertex_id);

  void SelectVertex(GLuint vertex_id);

  void SelectEdge(GLuint vertex_id);

  void SelectFace(GLuint vertex_id);

  void RemoveVertices();

  void RemoveEdge();

  void RemoveFaces();

  /// emplace back
  void CreateGraph() override;

  /// move instances_[slot_id] data to the end, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void SelectGraph(int slot_id) override;

  /// remove instances_[slot_id] from all buffers, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void RemoveGraph(int slot_id) override;

  void Render() override;

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept override {
    return selected_slot_id_;
  }

  [[nodiscard]] int GetSize() const noexcept override {
    return instances_.size();
  }

  const FixedSizeQueue<char, 64>* GetNamePtr(int instance_id) const override {
    return &instances_[instance_id].name;
  }

  FixedSizeQueue<char, 64>* GetNamePtr(int instance_id) override {
    return &instances_[instance_id].name;
  }

  BaseInstanceData* GetBaseInstanceData(int id) {
    return static_cast<BaseInstanceData*>(&instances_[id]);
  }

  static constexpr size_t gMaxPoints = 100;

 private:
  struct InstanceData : public IGraph::BaseInstanceData {
    int vertices_offset;
    int vertices_amount;
    int edges_offset;
    int edges_amount;
    /// i-th slot -> instances_[i], but
    /// graph data always bear selected forward in buffer,
    /// that means instances order != graph order in buffer
    int graph_id;
  };

  void Init();

  void DeInit();

  void UpdateBuffers();

  std::array<GLuint, ArbitraryGraph::gMaxPoints>::iterator
  FindVerticesOffsetById(GLuint id);
  std::array<glm::uvec2, ArbitraryGraph::gMaxPoints>::iterator
  FindEdgeBySelected(GLuint offset_1, GLuint offset_2);

  // wrt slots, so at creation we push_back, at remove we remove & decrement
  // instances_[slot_id] and NOT instances_[instance_id]
  // so the dependency is: graph_id == instances_[slot_id]
  std::vector<InstanceData> instances_;

  int selected_slot_id_{-1};

  // point as a single number - in shader decompose to x;y by mask
  std::array<GLuint, gMaxPoints>::size_type total_vertices_{0};
  std::array<glm::uvec2, gMaxPoints>::size_type total_edges_{0};
  std::array<GLuint, gMaxPoints> vertices_;
  std::array<glm::uvec2, gMaxPoints> edges_;

  GLuint points_vao_{0};
  GLuint points_vbo_{0};
  GLuint points_ebo_{0};

  std::vector<GLuint> selected_ids_;

  UiSharedResources& ui_shared_resources_;

  // ----
  // edge creation
/*  std::unordered_map<uint32_t, std::set<uint32_t>> adjacency_list_;
  std::vector<glm::uvec2> edge_vertices_; // flattened adjacency_list_ for vbo feed

  std::set<std::pair<uint32_t, uint32_t>> used_edges; // useful for face creation

  // face creation
  struct Face {
    // ccw convex
    // for concave poly we add another face with equal face_id
    int vertices_offset;
    int vertices_num;
    int face_id;
  };

  std::vector<Face> faces_;
  std::vector<uint_32t> face_vertices_; // flattened faces_ for vbo feed
  // the difference is we don't check was the vertex created before


  // -- faces updated dynamically after adges modification;*/
};


/*
class TerrainGraph {
 public:
  enum class Target {
    kVertex,
    kEdge,
    kFace
  };
  enum class Action {
    kSelect,
    kModify
  };

  void Render();

  /// we could avoid render ids to fbo, but
  /// this isn't a 3d-modeling program and we shouldn't have
  /// a lot of faces of edges;
  /// so here we choose fbo over math calculations
  void RenderPicking() {
    switch (target_) {
      case Target::kVertex:
        break;
      case Target::kEdge:
        RenderPickingEdge(); // GL_LINE? with picking_id (if works)
        break;
      case Target::kFace:
        RenderPickingFace(); // GL_TRIANGLE_FAN with picking_id
        break;
    }
  }
  void Press(glm::vec2 mouse_pos) {
    switch (target_) {
      case Target::kVertex:
        GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
        PressVertex(vertex_id);
        break;
      case Target::kEdge:
        GLuint edge_id = global_data->GetFboId(mouse_pos);
        PressEdge(edge_id);
        break;
      case Target::kFace:
        GLuint face_id = global_data->GetFboId(mouse_pos);
        PressFace(face_id);
        break;
    }
  }

  void Remove() = 0;

  void Drag() = 0;

 private:
  void PressVertex(int vertex_id) {
    if (!shift_pressed) {
      selected_vertices_.clear();
    }
    selected_vertices_.push_back(vertex_id);
    UpdateVertexBuffer();
  }

  */
/* --- BUT! ---
   * we can't add more vertices, BUT!
   * we can add more edges & faces
   * *//*


  void PressEdge(int edge_id) {
    if (!shift_pressed) {
      selected_edges_.clear();
    }
    selected_edges_.push_back(edge_id);
    UpdateEdgeBuffer();
  }

  void PressFace(int face_id) {
    if (!shift_pressed) {
      selected_faces_.clear();
    }
    selected_faces_.push_back(face_id);
    UpdateFaceBuffer();
  }

  /// --- SELECT SECTION ---

  // select type
  Target target_;

  // select for each type
  /// according to fbo picking id's (storing differs)
  /// (no copy, single instance -> std::set)
  std::set<GLuint> selected_vertices_;

  // we can modify only heights, that are related directly to vertex traits,
  // so "selecting edges / faces" select vertices as well AND we don't need:
  //  std::set<GLuint> selected_edges_;
  //  std::set<GLuint> selected_faces_;

  /// CREATE SECTION
  // no vertex creation - full terrain grid
  // no edge creation - no graphs, works on select only
  // no face creation - no graphs, works on select only

  /// RENDER SECTION
  // vertices only: draw selected in different color, GL_DRAW_POINTS
  GLuint vao_;
  GLuint vbo_;
};
*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
