#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "IGraph.h"

/// similar to TerrainGrid, but for roads, fences, water, biomes ui-slots

class ArbitraryGraph final : public IGraph {
 public:
  ArbitraryGraph(UiSharedResources& ui_shared_resources);

  ~ArbitraryGraph() {
    DeInit();
  }

  void Press(GLuint vertex_id) override;

  /// emplace back
  void CreateGraph() override;

  /// move instances_[slot_id] data to the end, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void SelectGraph(int slot_id) override;

  /// remove instances_[slot_id] from all buffers, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void RemoveGraph(int slot_id) override;

  void Remove();

  // btn flip points/edges, btn flip select/create
//  bool FlipPointsMode(); // points, edges
//  bool FlipPressMode(); // select, modify

  void Render() override;

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

  void EditPoint(GLuint vertex_id);
  void EditEdgePoint(GLuint vertex_id);

  void AddPoint(GLuint vertex_id);
  void AddEdge(GLuint vertex_id);

  void RemovePoint();
  void RemoveEdge();

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

  bool target_points_{true}; // opposite target - edges
  bool press_select_{true}; // opposite mode - edit (modify)

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
