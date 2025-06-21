#include "TerrainGrid.h"

#include "../core/TileRenderer.h"


TerrainGrid::TerrainGrid(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {}

void TerrainGrid::Select(GLuint vertex_id) {
  if (instances_.empty()) {
    return;
  }
  switch (target_) {
    case Target::kVertices:
      SelectVertex(vertex_id);
      break;
    case Target::kEdges:
      SelectEdge(vertex_id);
      break;
    case Target::kFaces:
      SelectFace(vertex_id);
      break;
  }
}

void TerrainGrid::SelectVertex(GLuint vertex_id) {
  selected_ids_.push_back(vertex_id);
}

void TerrainGrid::SelectEdge(GLuint vertex_id) {
  selected_ids_.push_back(vertex_id);
  //TODO
}

void TerrainGrid::SelectFace(GLuint vertex_id) {
  selected_ids_.push_back(vertex_id);
  //TODO
}

void TerrainGrid::CreateGraph() {
  if (instances_.size() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_.size();
    selected_ids_.clear();
    InstanceData instance_data;
    instance_data.name = FixedSizeQueue<char, 64>{};
    instance_data.color = glm::vec3{1.0f};
    instance_data.do_show = true;
    instance_data.heights = {};
    instances_.push_back(instance_data);
  }
}

void TerrainGrid::SelectGraph(int slot_id) {
  if (slot_id == selected_slot_id_) {
    return;
  }
  if (slot_id >= instances_.size()) {
    throw "select non-existent slot id";
  }
  selected_slot_id_ = slot_id;
  selected_ids_.clear();
}

void TerrainGrid::RemoveGraph(int slot_id) {
  if (slot_id >= instances_.size()) {
    throw "remove non-existent graph id";
  }
  // deselect
  if (selected_slot_id_ > slot_id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == slot_id) {
    selected_slot_id_ = -1;
    selected_ids_.clear();
  }
  instances_.erase(instances_.begin() + slot_id);
}

void TerrainGrid::FlipSelectMode() {
  switch (target_) {
    case Target::kVertices:
      target_ = Target::kEdges;
      break;
    case Target::kEdges:
      target_ = Target::kFaces;
      break;
    case Target::kFaces:
      target_ = Target::kVertices;
      break;
  }
  selected_ids_.clear();
}

void TerrainGrid::Render() {
  //TODO:
  //1. render all vertices/edges
  //2. render all selected vertices/edges ON TOP OF ALL OTHERS

  //TODO:
  //  glUniform1ui(1, selected_id_1_);
  //  glUniform1ui(2, selected_id_2_);
}

void TerrainGrid::EditPoint(GLuint vertex_id) {
  selected_ids_.push_back(vertex_id);
}

void TerrainGrid::EditEdgePoint(GLuint vertex_id) {
  selected_ids_.push_back(vertex_id);
}
