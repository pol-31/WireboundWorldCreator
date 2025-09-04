#include "ArbitraryGraph.h"

#include "../core/TileRenderer.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ArbitraryGraph::ArbitraryGraph(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {
  Init();
}

void ArbitraryGraph::CreateGraph() {
  if (total_vertices_ >= gMaxPoints || total_edges_ >= gMaxPoints) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_.size();
    selected_ids_.clear();
    InstanceData instance_data;
    instance_data.name = {};
    instance_data.color = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
    instance_data.do_show = true;
    instance_data.type_id = -1;
    instance_data.vertices_offset = static_cast<int>(total_vertices_);
    instance_data.vertices_amount = 0;
    instance_data.edges_offset = static_cast<int>(total_edges_);
    instance_data.edges_amount = 0;
    instance_data.graph_id = selected_slot_id_;
    instances_.push_back(instance_data);
  }
}

void ArbitraryGraph::SelectGraph(int slot_id) {
//  if (slot_id == selected_slot_id_) {
//    return;
//  }
//  if (slot_id >= instances_.size()) {
//    throw "select non-existent slot id";
//  }
//  auto graph_data = instances_[slot_id];
//
//  /// can't be the end at first it (immediately returned);
//  /// start from next
//  GLuint add_vertices = 0;
//  for (auto it = instances_.begin(); it != instances_.end(); ++it) {
//    if (it->graph_id <= graph_data.graph_id) {
//      continue;
//    }
//    --(it->graph_id);
//    // decrement larger edge indices
//    for (int i = it->edges_offset; i < it->edges_offset + it->edges_amount; ++i) {
//      edges_[i] -= glm::uvec2{graph_data.vertices_amount,
//                              graph_data.vertices_amount};
//    }
//    // move it offsets backward
//    it->vertices_offset -= graph_data.vertices_amount;
//    it->edges_offset -= graph_data.edges_amount;
//    // move selected forward
//    add_vertices += it->vertices_amount;
//    instances_[slot_id].vertices_offset += it->vertices_amount;
//    instances_[slot_id].edges_offset += it->edges_amount;
//  }
//  if (add_vertices != 0) {
//    // increment current edge indices
//    for (int i = graph_data.edges_offset;
//         i < graph_data.edges_offset + graph_data.edges_amount; ++i) {
//      edges_[i] += glm::uvec2{add_vertices};
//    }
//  }
//
//  /// vertices
//  std::rotate(
//      vertices_.begin() + graph_data.vertices_offset,
//      vertices_.begin() + graph_data.vertices_offset + graph_data.vertices_amount,
//      vertices_.begin() + total_vertices_);
//
//  /// edges
//  std::rotate(
//      edges_.begin() + graph_data.edges_offset,
//      edges_.begin() + graph_data.edges_offset + graph_data.edges_amount,
//      edges_.begin() + total_edges_);
//
//  /// graphs (we don't rotate, .graph_id_ modifying only)
//  instances_[slot_id].graph_id = instances_.size() - 1;
//  selected_slot_id_ = slot_id;
//  selected_ids_.clear();
//
//  UpdateBuffers();
}

bool ArbitraryGraph::RemoveGraph(int slot_id) {
  return false;
//  if (slot_id >= instances_.size()) {
//    throw "remove non-existent graph id";
//  }
//  // deselect
//  if (selected_slot_id_ > slot_id) {
//    --selected_slot_id_;
//  } else if (selected_slot_id_ == slot_id) {
//    selected_slot_id_ = -1;
//  }
//
//  // last graph (not necessarily last slot!)
//  if (instances_[slot_id].graph_id == instances_.size() - 1) {
//    // remove from end
//    total_vertices_ = instances_[slot_id].vertices_offset;
//    total_edges_ = instances_[slot_id].edges_offset;
//    instances_.erase(instances_.begin() + slot_id);
//    return;
//  }
//  auto graph_data = instances_[slot_id];
//
//  /// start from next
//  for (auto it = instances_.begin(); it != instances_.end(); ++it) {
//    if (it->graph_id <= graph_data.graph_id) {
//      continue;
//    }
//    --(it->graph_id);
//    // decrement larger edge indices
//    for (int i = it->edges_offset; i < it->edges_offset + it->edges_amount; ++i) {
//      edges_[i] -= glm::uvec2{graph_data.vertices_amount,
//                              graph_data.vertices_amount};
//    }
//    // move offsets backward
//    it->vertices_offset -= graph_data.vertices_amount;
//    it->edges_offset -= graph_data.edges_amount;
//  }
//
//  std::move(
//      vertices_.begin() + graph_data.vertices_offset + graph_data.vertices_amount,
//      vertices_.begin() + total_vertices_,
//      vertices_.begin() + graph_data.vertices_offset);
//  std::move(
//      edges_.begin() + graph_data.edges_offset + graph_data.edges_amount,
//      edges_.begin() + total_edges_,
//      edges_.begin() + graph_data.edges_offset);
//
//  total_vertices_ -= graph_data.vertices_amount;
//  total_edges_ -= graph_data.edges_amount;
//
//  instances_.erase(instances_.begin() + slot_id);
//
//  std::cout << total_vertices_ << ' ' << total_edges_ << std::endl;
//
//  // anyway deselect because of vbo data changes (todo;)
//  selected_ids_.clear();
//
//  UpdateBuffers();
}

void ArbitraryGraph::Render(glm::vec2 mouse_pos) {
  return;
  //TODO:
  //1. render all vertices/edges
  //2. render all selected vertices/edges ON TOP OF ALL OTHERS

  ui_shared_resources_.arbitrary_graph_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_height.Bind();
  glm::mat4 transform = glm::scale(
      glm::mat4{1.0f}, glm::vec3{ui_shared_resources_.global_glfw_callback_data_
                                 .tile_renderer->cur_tile_.map_scale});
  glUniformMatrix4fv(7, 1, false, glm::value_ptr(transform));
  glBindVertexArray(points_vao_);
  glPointSize(5.0f);
  glDrawArrays(GL_POINTS, 0, total_vertices_);

//  glUniform1i(glGetUniformLocation(shader_id, "segments"), 16); // or dynamic
  /// no GL_LINE_STRIP - the graph is complex
  glDrawElements(GL_LINES, total_edges_ * 2, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  //TODO:
//  glUniform1ui(1, selected_id_1_);
//  glUniform1ui(2, selected_id_2_);
}

void ArbitraryGraph::Init() {
  glGenVertexArrays(1, &points_vao_);
  glGenBuffers(1, &points_vbo_);
  glGenBuffers(1, &points_ebo_);

  glBindVertexArray(points_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, points_vbo_);
  glBufferData(GL_ARRAY_BUFFER, gMaxPoints * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, gMaxPoints * sizeof(glm::uvec2), nullptr, GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

void ArbitraryGraph::DeInit() {
  glDeleteBuffers(1, &points_vbo_);
  glDeleteBuffers(1, &points_ebo_);
  glDeleteVertexArrays(1, &points_vao_);
}

void ArbitraryGraph::UpdateBuffers() {
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, total_vertices_ * sizeof(GLuint), vertices_.data());
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_ebo_);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, total_edges_ * sizeof(glm::uvec2), edges_.data());
}

std::array<glm::uvec2, ArbitraryGraph::gMaxPoints>::iterator
ArbitraryGraph::FindEdgeBySelected(GLuint offset_1, GLuint offset_2) {
  auto edges_offset = instances_[selected_slot_id_].edges_offset;
  auto edges_amount = instances_[selected_slot_id_].edges_amount;
  for (auto it = edges_.begin() + edges_offset;
       it != edges_.begin() + edges_offset + edges_amount; ++it) {
    if ((it->x == offset_1 && it->y == offset_2) ||
        (it->x == offset_2 && it->y == offset_1)) {
      return it;
    }
  }
  return edges_.end();
}

std::array<GLuint, ArbitraryGraph::gMaxPoints>::iterator
ArbitraryGraph::FindVerticesOffsetById(GLuint id) {
  auto vertices_offset = instances_[selected_slot_id_].vertices_offset;
  auto vertices_amount = instances_[selected_slot_id_].vertices_amount;
  for (auto it = vertices_.begin() + vertices_offset;
       it != vertices_.begin() + vertices_offset + vertices_amount; ++it) {
    if (*it == id) {
      return it;
    }
  }
  return vertices_.end();
}

//void foo() {
//  faces_.clear();
//  face_vertices_vbo_.clear();
//  int face_id = 1;
//
//  int new_offset = 0;
//  int new_amount = 0;
//
//  for (const auto& [u, neighbors] : adjacency_list_) {
//    for (uint32_t v : neighbors) {
//      std::pair<uint32_t, uint32_t> half_edge = {u, v};
//      if (used_edges.count(half_edge)) continue;
//
//      // Start walking the face from u→v
//      std::vector<uint32_t> face_indices;
//
//      face_indices.push_back(u);
//      new_offset  = face_vertices_.size();
//      new_amount += 1;
//      uint32_t curr = u;
//      uint32_t next = v;
//
//      while (true) {
//        face_indices.push_back(next);
//        used_edges.insert({curr, next});
//
//        const auto& next_neighbors = adjacency_list_[next];
//
//        // Find neighbor after curr in CCW order around 'next'
//        float best_angle = std::numeric_limits<float>::max();
//        uint32_t best_candidate = -1;
//
//        glm::vec2 dir_prev = glm::normalize(vertices_[curr] - vertices_[next]);
//
//        for (uint32_t candidate : next_neighbors) {
//          if (candidate == curr) continue;
//          glm::vec2 dir_candidate = glm::normalize(vertices_[candidate] - vertices_[next]);
//          float angle = atan2(dir_candidate.y, dir_candidate.x) - atan2(dir_prev.y, dir_prev.x);
//          if (angle <= 0) angle += 2.0f * glm::pi<float>();
//          if (angle < best_angle) {
//            best_angle = angle;
//            best_candidate = candidate;
//          }
//        }
//
//        if (best_candidate == -1 || best_candidate == face_indices[0]) {
//          break; // loop closed or dead-end
//        }
//
//        curr = next;
//        next = best_candidate;
//
//        // loop closed
//        if (next == face_indices[0]) {
//          used_edges.insert({curr, next});
//          break;
//        }
//      }
//      if (dead-end) {
//        RemoveLastAdded();
//      }
//
//      // Validate face
//      if (face_indices.size() < 4) continue; // triangle at least (first + last repeated)
//
//      // Remove the repeated closing vertex for convexity check
//      face_indices.pop_back();
//
//      // Store vertex data
//      int offset = face_vertices_vbo_.size();
//      for (uint32_t idx : face_indices) {
//        face_vertices_vbo_.push_back(vertices_[idx]);
//      }
//
//      Face f;
//      f.offset = offset;
//      f.num = face_indices.size();
//      f.face_id = face_id++;
//      faces_.push_back(f);
//    }
//  }
//}

void ArbitraryGraph::AddVertex(GLuint vertex_id) {
  std::cerr << "AddVertex is not implemented" << std::endl;
  return;
  // 1
  //    if (total_vertices_ >= gMaxPoints) {
  //      std::cerr << "Unable to add more vertices (data overflow)" << std::endl;
  //      return;
  //    }
  //    auto vertices_offset = instances_[selected_slot_id_].vertices_offset;
  //    for (int i = 0; i < instances_[selected_slot_id_].vertices_amount; ++i) {
  //      if (vertex_id == vertices_[vertices_offset + i]) {
  //        std::cerr << "point already exists in the graph, skip" << std::endl;
  //        return;
  //      }
  //    }
  //
  //    vertices_[total_vertices_] = vertex_id;
  //    ++(instances_[selected_slot_id_].vertices_amount);
  //    ++total_vertices_;
  //
  //    selected_ids_.clear();
  //    selected_ids_.push_back(vertex_id);
  //
  //    //TODO: remove
  //    if (instances_[selected_slot_id_].vertices_amount > 1) {
  //      auto offset = static_cast<GLuint>(
  //          instances_[selected_slot_id_].vertices_offset
  //          + instances_[selected_slot_id_].vertices_amount);
  //      edges_[total_edges_] = {offset - 2, offset - 1};
  //      ++instances_[selected_slot_id_].edges_amount;
  //      ++total_edges_;
  //    }
  //    UpdateBuffers();

  // 2
  //    if (instances_.empty()) {
  //      return;
  //    }
  //    auto found_id = FindVerticesOffsetById(vertex_id);
  //    if (found_id == vertices_.end()) {
  //      return;
  //    }
  //    vertices_.push_back(vertex_id);
}

void ArbitraryGraph::AddEdge(GLuint vertex_id) {
  std::cerr << "AddEdge is not implemented" << std::endl;
  return;

  // 1
  //    EditEdgePoint(vertex_id);
  //    if (selected_ids_.size() < 2) {
  //      return;
  //    }
  //    // connect first and last (fan)
  //    auto offset_it1 = FindVerticesOffsetById(selected_ids_[0]);
  //    auto offset_it2 = FindVerticesOffsetById(selected_ids_.back());
  //    if (offset_it1 == vertices_.end() || offset_it2 == vertices_.end()) {
  //      std::cerr << "unable to find selected IDs entries" << std::endl;
  //      return;
  //    }
  //    /// check edge already exists
  //    auto offset_1 = std::distance(vertices_.begin(), offset_it1);
  //    auto offset_2 = std::distance(vertices_.begin(), offset_it2);
  //    auto existing_edge = FindEdgeBySelected(offset_1, offset_2);
  //    if (existing_edge != edges_.end()) {
  //      return;
  //    }
  //    edges_[total_edges_] = {offset_1, offset_2};
  //    ++instances_[selected_slot_id_].edges_amount;
  //    ++total_edges_;
  //    UpdateBuffers();

  // 2
  //    if (instances_.empty()) {
  //      return;
  //    }
  //    for (auto vertex_id_2 : selected_vertices_) {
  //      auto found_id = FindVerticesOffsetById(vertex_id);
  //      if (found_id == vertices_.end()) {
  //        return;
  //      }
  //      edges_.push_back({vertex_id, vertex_id_2});
  //      adjacency_list_[vertex_id].insert(vertex_id_2);
  //      adjacency_list_[vertex_id_2].insert(vertex_id); // for undirected graph
  //    }
  //
  //    UpdateFaces(); // in case one separated by other
}

void ArbitraryGraph::AddFace(GLuint vertex_id) {
  std::cerr << "AddFace is not implemented" << std::endl;
}

void ArbitraryGraph::SelectVertex(GLuint vertex_id) {
  std::cerr << "SelectVertex is not implemented" << std::endl;
}

void ArbitraryGraph::SelectEdge(GLuint vertex_id) {
  std::cerr << "SelectEdge is not implemented" << std::endl;
}

void ArbitraryGraph::SelectFace(GLuint vertex_id) {
  std::cerr << "SelectFace is not implemented" << std::endl;
}

void ArbitraryGraph::RemoveVertices() {
  std::cerr << "RemoveVertices is not implemented" << std::endl;
  // impl
  //    if (selected_ids_.empty()) {
  //      std::cerr << "select point to remove" << std::endl;
  //      return;
  //    }
  //    /// --- remove point
  //    int vertices_offset = instances_[selected_slot_id_].vertices_offset;
  //    int vertices_amount = instances_[selected_slot_id_].vertices_amount;
  //    for (auto selected_id : selected_ids_) {
  //      auto found = std::find(
  //          vertices_.begin() + vertices_offset,
  //          vertices_.end() + vertices_offset + vertices_amount,
  //          selected_id);
  //      /// if selected, it present (no std::end() check)
  //      auto offset = std::distance(vertices_.begin(), found);
  //      auto erased_local_offset = std::distance(vertices_.begin() + vertices_offset, found);
  //
  //      std::copy(vertices_.begin() + offset + 1,
  //                vertices_.begin() + vertices_offset + vertices_amount,
  //                vertices_.begin() + offset);
  //      --total_vertices_; // just render 1 point less
  //      --instances_[selected_slot_id_].vertices_amount;
  //
  //      /// --- remove connected edges
  //      auto edges_amount = instances_[selected_slot_id_].edges_amount;
  //      auto edges_offset = instances_[selected_slot_id_].edges_offset;
  //      for (int i = edges_offset; i < edges_amount; ++i) {
  //        if (edges_[i].x == erased_local_offset ||
  //            edges_[i].y == erased_local_offset) {
  //          std::cout << "removed" << std::endl;
  //          auto edge_offset =
  //              std::distance(edges_.begin(), edges_.begin() + i);
  //          std::copy(edges_.begin() + edge_offset + 1,
  //                    edges_.begin() + edges_offset + edges_amount,
  //                    edges_.begin() + edge_offset);
  //          edges_amount = --instances_[selected_slot_id_].edges_amount;
  //          --total_edges_;
  //          --i;
  //        }
  //      }
  //      //decrement larger indices
  //      for (int i = instances_[selected_slot_id_].edges_offset;
  //           i < edges_amount; ++i) {
  //        if (edges_[i].x > erased_local_offset) {
  //          --edges_[i].x;
  //        }
  //        if (edges_[i].y > erased_local_offset) {
  //          --edges_[i].y;
  //        }
  //      }
  //    }
  //    selected_ids_.clear();
  //    UpdateBuffers();
}

void ArbitraryGraph::RemoveEdge() {
  std::cerr << "RemoveEdge is not implemented" << std::endl;
  // impl
  //    if (selected_ids_.size() < 2) {
  //      std::cerr << "unable to remove; select second point" << std::endl;
  //      return;
  //    }
  //    for (auto selected_id_1 : selected_ids_) {
  //      for (auto selected_id_2 : selected_ids_) {
  //        if (selected_id_1 == selected_id_2) {
  //          continue;
  //        }
  //        auto offset_1 = FindVerticesOffsetById(selected_id_1);
  //        auto offset_2 = FindVerticesOffsetById(selected_id_2);
  //        if (offset_1 == vertices_.end() || offset_2 == vertices_.end()) {
  //          std::cerr << "unable to find selected IDs entries" << std::endl;
  //          return;
  //        }
  //        auto existing_edge = FindEdgeBySelected(*offset_1, *offset_2);
  //        if (existing_edge != edges_.end()) {
  //          std::cerr << "the edge doesn't exist" << std::endl;
  //          return;
  //        }
  //        auto existing_edge_offset = std::distance(edges_.begin(), existing_edge);
  //        auto edges_offset = instances_[selected_slot_id_].edges_offset;
  //        auto edges_amount = instances_[selected_slot_id_].edges_amount;
  //        std::copy(edges_.begin() + existing_edge_offset + 1,
  //                  edges_.begin() + edges_offset + edges_amount,
  //                  edges_.begin() + existing_edge_offset);
  //        --total_edges_; // just render 1 point less
  //        --instances_[selected_slot_id_].edges_amount;
  //      }
  //    }
}

void ArbitraryGraph::RemoveFaces() {
  std::cerr << "RemoveFaces is not implemented" << std::endl;
}
