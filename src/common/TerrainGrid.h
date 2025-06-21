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


#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TERRAINGRID_H_
