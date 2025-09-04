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
  bool RemoveGraph(int slot_id) override;

  void Render(glm::vec2 mouse_pos) override;

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept override {
    return selected_slot_id_;
  }

  [[nodiscard]] int GetSize() const noexcept override {
    return instances_.size();
  }

  const std::string* GetNameRef(int instance_id) const override {
    return &instances_[instance_id].name;
  }

  std::string* GetNameRef(int instance_id) override {
    return &instances_[instance_id].name;
  }

  BaseInstanceData* GetBaseInstanceData(int id) override {
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

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
