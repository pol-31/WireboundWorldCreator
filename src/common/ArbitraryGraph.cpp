#include "ArbitraryGraph.h"

#include "../core/TileRenderer.h"

#include <algorithm>

ArbitraryGraph::ArbitraryGraph(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {
  Init();
}

FixedSizeQueue<char, 64>* ArbitraryGraph::GetNamePtr(int id) {
  return &instances_[id].name;
}

const FixedSizeQueue<char, 64>* ArbitraryGraph::GetNamePtr(int id) const {
  return &instances_[id].name;
}

void ArbitraryGraph::CreateGraph() {
  if (total_vertices_ >= kMaxPoints || total_edges_ >= kMaxPoints) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_.size();
    selected_id_1_ = -1;
    selected_id_2_ = -1;
    instances_.emplace_back(
        FixedSizeQueue<char, 64>{}, total_vertices_, 0,
        total_edges_, 0, selected_slot_id_);
  }
}

void ArbitraryGraph::SelectGraph(int slot_id) {
  if (slot_id == selected_slot_id_) {
    return;
  }
  if (slot_id >= instances_.size()) {
    throw "select non-existent slot id";
  }
  auto graph_data = instances_[slot_id];

  /// can't be the end at first it (immediately returned);
  /// start from next
  GLuint add_vertices = 0;
  for (auto it = instances_.begin(); it != instances_.end(); ++it) {
    if (it->graph_id <= graph_data.graph_id) {
      continue;
    }
    --(it->graph_id);
    // decrement larger edge indices
    for (int i = it->edges_offset; i < it->edges_offset + it->edges_amount; ++i) {
      edges_[i] -= glm::uvec2{graph_data.vertices_amount,
                              graph_data.vertices_amount};
    }
    // move it offsets backward
    it->vertices_offset -= graph_data.vertices_amount;
    it->edges_offset -= graph_data.edges_amount;
    // move selected forward
    add_vertices += it->vertices_amount;
    instances_[slot_id].vertices_offset += it->vertices_amount;
    instances_[slot_id].edges_offset += it->edges_amount;
  }
  if (add_vertices != 0) {
    // increment current edge indices
    for (int i = graph_data.edges_offset;
         i < graph_data.edges_offset + graph_data.edges_amount; ++i) {
      edges_[i] += glm::uvec2{add_vertices};
    }
  }

  /// vertices
  std::rotate(
      vertices_.begin() + graph_data.vertices_offset,
      vertices_.begin() + graph_data.vertices_offset + graph_data.vertices_amount,
      vertices_.begin() + total_vertices_);

  /// edges
  std::rotate(
      edges_.begin() + graph_data.edges_offset,
      edges_.begin() + graph_data.edges_offset + graph_data.edges_amount,
      edges_.begin() + total_edges_);

  /// graphs (we don't rotate, .graph_id_ modifying only)
  instances_[slot_id].graph_id = instances_.size() - 1;
  selected_slot_id_ = slot_id;
  selected_id_1_ = -1;
  selected_id_2_ = -1;

  UpdateBuffers();
}

void ArbitraryGraph::RemoveGraph(int slot_id) {
  if (slot_id >= instances_.size()) {
    throw "remove non-existent graph id";
  }
  // deselect
  if (selected_slot_id_ > slot_id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == slot_id) {
    selected_slot_id_ = -1;
    // in the end (if selected_ids > removed ids
//    selected_id_1_ = -1;
//    selected_id_2_ = -1;
  }

  // last graph (not necessarily last slot!)
  if (instances_[slot_id].graph_id == instances_.size() - 1) {
    // remove from end
    total_vertices_ = instances_[slot_id].vertices_offset;
    total_edges_ = instances_[slot_id].edges_offset;
    instances_.erase(instances_.begin() + slot_id);
    return;
  }
  auto graph_data = instances_[slot_id];

  /// start from next
  for (auto it = instances_.begin(); it != instances_.end(); ++it) {
    if (it->graph_id <= graph_data.graph_id) {
      continue;
    }
    --(it->graph_id);
    // decrement larger edge indices
    for (int i = it->edges_offset; i < it->edges_offset + it->edges_amount; ++i) {
      edges_[i] -= glm::uvec2{graph_data.vertices_amount,
                              graph_data.vertices_amount};
    }
    // move offsets backward
    it->vertices_offset -= graph_data.vertices_amount;
    it->edges_offset -= graph_data.edges_amount;
  }

  std::move(
      vertices_.begin() + graph_data.vertices_offset + graph_data.vertices_amount,
      vertices_.begin() + total_vertices_,
      vertices_.begin() + graph_data.vertices_offset);
  std::move(
      edges_.begin() + graph_data.edges_offset + graph_data.edges_amount,
      edges_.begin() + total_edges_,
      edges_.begin() + graph_data.edges_offset);

  total_vertices_ -= graph_data.vertices_amount;
  total_edges_ -= graph_data.edges_amount;

  instances_.erase(instances_.begin() + slot_id);

  std::cout << total_vertices_ << ' ' << total_edges_ << std::endl;

  // anyway deselect because of vbo data changes (todo;)
  selected_id_1_ = -1;
  selected_id_2_ = -1;

  UpdateBuffers();
}

bool ArbitraryGraph::FlipPointsMode() {
  target_points_ = !target_points_;
  selected_id_1_ = -1;
  selected_id_2_ = -1;
  std::cout << "targeting points: " << std::boolalpha
            << target_points_ << std::endl;
  return !target_points_;
}

bool ArbitraryGraph::FlipPressMode() {
  press_select_ = !press_select_;
  std::cout << "press select: " << std::boolalpha
            << press_select_ << std::endl;
  return !target_points_;
}

//TODO: maybe sort it everytime at modifying will improve the performance

std::array<glm::uvec2, ArbitraryGraph::kMaxPoints>::iterator
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

void ArbitraryGraph::AddEdge(GLuint vertex_id) {
  EditEdgePoint(vertex_id);
  if (selected_id_1_ == -1 || selected_id_2_ == -1) {
    return;
  }
  auto offset_it1 = FindVerticesOffsetById(selected_id_1_);
  auto offset_it2 = FindVerticesOffsetById(selected_id_2_);
  if (offset_it1 == vertices_.end() || offset_it2 == vertices_.end()) {
    std::cerr << "unable to find selected IDs entries" << std::endl;
    return;
  }
  /// check edge already exists
  auto offset_1 = std::distance(vertices_.begin(), offset_it1);
  auto offset_2 = std::distance(vertices_.begin(), offset_it2);
  auto existing_edge = FindEdgeBySelected(offset_1, offset_2);
  if (existing_edge != edges_.end()) {
    return;
  }
  edges_[total_edges_] = {offset_1, offset_2};
  ++instances_[selected_slot_id_].edges_amount;
  ++total_edges_;
  UpdateBuffers();
}

std::array<GLuint, ArbitraryGraph::kMaxPoints>::iterator
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

void ArbitraryGraph::Press(GLuint vertex_id) {
  if (instances_.empty()) {
    return;
  }
  if (press_select_) {
    if (target_points_) {
      EditPoint(vertex_id);
    } else {
      EditEdgePoint(vertex_id);
    }
  } else {
    if (target_points_) {
      AddPoint(vertex_id);
    } else {
      AddEdge(vertex_id);
    }
  }
}

void ArbitraryGraph::AddPoint(GLuint vertex_id) {
  if (total_vertices_ >= kMaxPoints) {
    std::cerr << "Unable to add more vertices (data overflow)" << std::endl;
    return;
  }
  auto vertices_offset = instances_[selected_slot_id_].vertices_offset;
  for (int i = 0; i < instances_[selected_slot_id_].vertices_amount; ++i) {
    if (vertex_id == vertices_[vertices_offset + i]) {
      std::cerr << "point already exists in the graph, skip" << std::endl;
      return;
    }
  }

  vertices_[total_vertices_] = vertex_id;
  ++(instances_[selected_slot_id_].vertices_amount);
  ++total_vertices_;

  selected_id_1_ = vertex_id;
  std::cout << selected_id_1_ << ' ' << total_vertices_ << std::endl;

  if (instances_[selected_slot_id_].vertices_amount > 1) {
    auto offset = static_cast<GLuint>(
        instances_[selected_slot_id_].vertices_offset
        + instances_[selected_slot_id_].vertices_amount);
    edges_[total_edges_] = {offset - 2, offset - 1};
    ++instances_[selected_slot_id_].edges_amount;
    ++total_edges_;
  }
  UpdateBuffers();
}

void ArbitraryGraph::Remove() {
  if (instances_.empty()) {
    return;
  }
  if (target_points_) {
    RemovePoint();
  } else {
    RemoveEdge();
  }
}

void ArbitraryGraph::RemovePoint() {
  if (selected_id_1_ == static_cast<GLuint>(-1)) {
    std::cerr << "select point to remove" << std::endl;
    return;
  }
  /// --- remove point
  int vertices_offset = instances_[selected_slot_id_].vertices_offset;
  int vertices_amount = instances_[selected_slot_id_].vertices_amount;
  auto found = std::find(
      vertices_.begin() + vertices_offset,
      vertices_.end() + vertices_offset + vertices_amount,
                         selected_id_1_);
  /// if selected, it present (no std::end() check)
  auto offset = std::distance(vertices_.begin(), found);
  auto erased_local_offset = std::distance(vertices_.begin() + vertices_offset, found);

  std::copy(vertices_.begin() + offset + 1,
            vertices_.begin() + vertices_offset + vertices_amount,
            vertices_.begin() + offset);
  --total_vertices_; // just render 1 point less
  --instances_[selected_slot_id_].vertices_amount;

  /// --- remove connected edges
  auto edges_amount = instances_[selected_slot_id_].edges_amount;
  auto edges_offset = instances_[selected_slot_id_].edges_offset;
  for (int i = edges_offset; i < edges_amount; ++i) {
    if (edges_[i].x == erased_local_offset ||
        edges_[i].y == erased_local_offset) {
      std::cout << "removed" << std::endl;
      auto edge_offset =
          std::distance(edges_.begin(), edges_.begin() + i);
      std::copy(edges_.begin() + edge_offset + 1,
                edges_.begin() + edges_offset + edges_amount,
                edges_.begin() + edge_offset);
      edges_amount = --instances_[selected_slot_id_].edges_amount;
      --total_edges_;
      --i;
    }
  }
  //decrement larger indices
  for (int i = instances_[selected_slot_id_].edges_offset;
       i < edges_amount; ++i) {
    if (edges_[i].x > erased_local_offset) {
      --edges_[i].x;
    }
    if (edges_[i].y > erased_local_offset) {
      --edges_[i].y;
    }
  }
  selected_id_1_ = static_cast<GLuint>(-1);
  UpdateBuffers();
}

void ArbitraryGraph::RemoveEdge() {
  if (selected_id_1_ == -1 || selected_id_2_ == -1) {
    std::cerr << "unable to remove; select second point" << std::endl;
    return;
  }
  auto offset_1 = FindVerticesOffsetById(selected_id_1_);
  auto offset_2 = FindVerticesOffsetById(selected_id_2_);
  if (offset_1 == vertices_.end() || offset_2 == vertices_.end()) {
    std::cerr << "unable to find selected IDs entries" << std::endl;
    return;
  }
  auto existing_edge = FindEdgeBySelected(*offset_1, *offset_2);
  if (existing_edge != edges_.end()) {
    std::cerr << "the edge doesn't exist" << std::endl;
    return;
  }
  auto existing_edge_offset = std::distance(edges_.begin(), existing_edge);
  auto edges_offset = instances_[selected_slot_id_].edges_offset;
  auto edges_amount = instances_[selected_slot_id_].edges_amount;
  std::copy(edges_.begin() + existing_edge_offset + 1,
            edges_.begin() + edges_offset + edges_amount,
            edges_.begin() + existing_edge_offset);
  --total_edges_; // just render 1 point less
  --instances_[selected_slot_id_].edges_amount;
}

void ArbitraryGraph::EditEdgePoint(GLuint vertex_id) {
  auto found_id = FindVerticesOffsetById(vertex_id);
  if (found_id != vertices_.end()) {
    if (selected_id_1_ == vertex_id) {
      std::cout << "deselect 1" << std::endl;
      selected_id_1_ = -1;
      return;
    } else if (selected_id_2_ == vertex_id) {
      std::cout << "deselect 2" << std::endl;
      selected_id_2_ = -1;
      return;
    }
  } else {
    return;
  }
  /// rewrite first (so something always changes)
  if (selected_id_1_ == -1 || selected_id_2_ != -1) {
    selected_id_1_ = vertex_id;
  } else {
    selected_id_2_ = vertex_id;
  }
}

void ArbitraryGraph::EditPoint(GLuint vertex_id) {
  auto found_id = FindVerticesOffsetById(vertex_id);
  if (found_id == vertices_.end()) {
    return;
  }
  if (selected_id_1_ == vertex_id) {
    std::cout << "deselect" << std::endl;
    selected_id_1_ = static_cast<GLuint>(-1);
  } else {
    selected_id_1_ = vertex_id;
  }
}

void ArbitraryGraph::Render() {
  ui_shared_resources_.arbitrary_graph_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_height.Bind();
  glUniform1ui(1, selected_id_1_);
  glUniform1ui(2, selected_id_2_);
  glBindVertexArray(points_vao_);
  glPointSize(5.0f);
//  glDrawArrays(GL_POINTS, 0, total_vertices_);

//  glUniform1i(glGetUniformLocation(shader_id, "segments"), 16); // or dynamic
  /// no GL_LINE_STRIP - the graph is complex
  glDrawElements(GL_LINES, total_edges_ * 2, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void ArbitraryGraph::Init() {
  glGenVertexArrays(1, &points_vao_);
  glGenBuffers(1, &points_vbo_);
  glGenBuffers(1, &points_ebo_);

  glBindVertexArray(points_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, points_vbo_);
  glBufferData(GL_ARRAY_BUFFER, kMaxPoints * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, kMaxPoints * sizeof(glm::uvec2), nullptr, GL_DYNAMIC_DRAW);

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

// --- --- --- --- --- --- --- ---
// --- --- --- --- --- --- --- ---
// --- --- --- --- --- --- --- ---
// --- --- --- --- --- --- --- ---
// --- --- --- --- --- --- --- ---
// --- --- --- --- --- --- --- ---


//TODO: replace with glm (currently I'm not sure about its internal order)
int CrossProduct(const Point& A, const Point& B, const Point& C) {
  int ABx = B.x - A.x;
  int ABy = B.y - A.y;
  int ACx = C.x - A.x;
  int ACy = C.y - A.y;
  return ABx * ACy - ABy * ACx;
}

bool IsCcw(const std::vector<Point>& points) {
  int n = points.size();
  if (n < 3) {
    // A polygon must have at least 3 points
    return false;
  }

  for (int i = 0; i < n; ++i) {
    int j = (i + 1) % n;
    int k = (i + 2) % n;
    if (CrossProduct(points[i], points[j], points[k]) <= 0) {
      return false;
    }
  }
  return true;
}

bool DoIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2) {
  // Helper function to find the orientation of the ordered triplet (p, q, r)
  // 0 -> p, q and r are collinear
  // 1 -> Clockwise
  // 2 -> Counterclockwise
  auto orientation = [](const Point& p, const Point& q, const Point& r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
  };

  // Check if point q lies on segment pr
  auto onSegment = [](const Point& p, const Point& q, const Point& r) {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) {
      return true;
    }
    return false;
  };

  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  // General case
  if (o1 != o2 && o3 != o4)
    return true;

  // Special cases
  // p1, q1 and p2 are collinear and p2 lies on segment p1q1
  if (o1 == 0 && onSegment(p1, p2, q1)) return true;

  // p1, q1 and q2 are collinear and q2 lies on segment p1q1
  if (o2 == 0 && onSegment(p1, q2, q1)) return true;

  // p2, q2 and p1 are collinear and p1 lies on segment p2q2
  if (o3 == 0 && onSegment(p2, p1, q2)) return true;

  // p2, q2 and q1 are collinear and q1 lies on segment p2q2
  if (o4 == 0 && onSegment(p2, q1, q2)) return true;

  return false; // Doesn't fall in any of the above cases
}

bool IsConvexPolygon(const std::vector<Point>& points) {
  int n = points.size();
  if (n < 3) {
    return false; // A polygon must have at least 3 points
  }
  if (!IsCcw(points)) {
    return false;
  }
  for (int i = 0; i < n; ++i) {
    for (int j = i + 2; j < n; ++j) {
      // Ignore adjacent edges and the first and last edge in a closed polygon
      if (i == 0 && j == n - 1) continue;

      if (DoIntersect(points[i], points[(i + 1) % n], points[j], points[(j + 1) % n])) {
        return false;
      }
    }
  }

  return true;
}

bool isLeft(Point p1, Point p2, Point p) {
  return (p2.x - p1.x) * (p.y - p1.y) - (p.x - p1.x) * (p2.y - p1.y) > 0;
}

bool isInsideConvexPolygon(const std::vector<Point>& polygon, Point p) {
  int n = polygon.size();
  if (n < 3) return false;
  for (int i = 0; i < n; i++) {
    Point p1 = polygon[i];
    Point p2 = polygon[(i + 1) % n];
    if (!isLeft(p1, p2, p)) {
      return false;
    }
  }
  return true;
}
