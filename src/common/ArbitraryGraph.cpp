#include "ArbitraryGraph.h"

#include <algorithm>

ArbitraryGraph::ArbitraryGraph(
    SharedResources& shared_resources)
    : shared_resources_(shared_resources) {
  Init();
}

void ArbitraryGraph::CreateGraph() {
  if (total_vertices_ >= kMaxPoints || total_edges_ >= kMaxPoints) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    instances_.emplace_back(total_vertices_, 0, total_edges_, 0);
  }
}

void ArbitraryGraph::SelectGraph() {
  // bring back (so we could call instances_.back())
}

void ArbitraryGraph::RemoveGraph() {
  // remove & shift & deselect/select?
  // Anyway we need cur_graph, etc...
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

GLuint ArbitraryGraph::FindEdgeBySelected(GLuint offset_1, GLuint offset_2) {
  auto edges_offset = instances_.back().edges_offset_;
  auto edges_amount = instances_.back().edges_amount_;
  for (int i = edges_offset; i < edges_offset + edges_amount; ++i) {
    if ((edges_[i].x == offset_1 && edges_[i].y == offset_2) ||
        (edges_[i].x == offset_1 && edges_[i].y == offset_2)) {
      return i;
    }
  }
  return -1;
}

void ArbitraryGraph::AddEdge(GLuint vertex_id) {
  EditEdgePoint(vertex_id);
  if (selected_id_1_ == -1 || selected_id_2_ == -1) {
    return;
  }
  auto offset_1 = FindVerticesOffsetById(selected_id_1_);
  auto offset_2 = FindVerticesOffsetById(selected_id_2_);
  if (offset_1 == -1 || offset_2 == -1) {
    std::cerr << "unable to find selected IDs entries" << std::endl;
    return;
  }
  /// check edge already exists
  auto existing_edge = FindEdgeBySelected(offset_1, offset_2);
  if (existing_edge != -1) {
    return;
  }
  edges_[total_edges_] = {offset_1, offset_2};
  ++instances_.back().edges_amount_;
  ++total_edges_;
  UpdateBuffers();
}

GLuint ArbitraryGraph::FindVerticesOffsetById(GLuint id) {
  auto vertices_offset = instances_.back().vertices_offset_;
  auto vertices_amount = instances_.back().vertices_amount_;
  for (int i = vertices_offset; i < vertices_offset + vertices_amount; ++i) {
    if (vertices_[i] == id) {
      return i;
    }
  }
  return -1;
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
  auto vertices_offset = instances_.back().vertices_offset_;
  for (int i = 0; i < instances_.back().vertices_amount_; ++i) {
    if (vertex_id == vertices_[vertices_offset + i]) {
      std::cerr << "point already exists in the graph, skip" << std::endl;
      return;
    }
  }

  vertices_[total_vertices_] = vertex_id;
  ++(instances_.back().vertices_amount_);
  ++total_vertices_;

  selected_id_1_ = vertex_id;
  std::cout << selected_id_1_ << ' ' << total_vertices_ << std::endl;

  if (instances_.back().vertices_amount_ > 1) {
    auto offset = static_cast<GLuint>(
        instances_.back().vertices_offset_
        + instances_.back().vertices_amount_);
    edges_[total_edges_] = {offset - 2, offset - 1};
    ++instances_.back().edges_amount_;
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
  int vertices_offset = instances_.back().vertices_offset_;
  int vertices_amount = instances_.back().vertices_amount_;
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
  --instances_.back().vertices_amount_;

  /// --- remove connected edges
  auto edges_amount = instances_.back().edges_amount_;
  auto edges_offset = instances_.back().edges_offset_;
  for (int i = edges_offset; i < edges_amount; ++i) {
    if (edges_[i].x == erased_local_offset ||
        edges_[i].y == erased_local_offset) {
      std::cout << "removed" << std::endl;
      auto edge_offset =
          std::distance(edges_.begin(), edges_.begin() + i);
      std::copy(edges_.begin() + edge_offset + 1,
                edges_.begin() + edges_offset + edges_amount,
                edges_.begin() + edge_offset);
      edges_amount = --instances_.back().edges_amount_;
      --total_edges_;
      --i;
    }
  }
  //decrement larger indices
  for (int i = instances_.back().edges_offset_;
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
  if (offset_1 == -1 || offset_2 == -1) {
    std::cerr << "unable to find selected IDs entries" << std::endl;
    return;
  }
  auto existing_edge = FindEdgeBySelected(offset_1, offset_2);
  if (existing_edge != -1) {
    std::cerr << "the edge doesn't exist" << std::endl;
    return;
  }
  auto edges_offset = instances_.back().edges_offset_;
  auto edges_amount = instances_.back().edges_amount_;
  std::copy(edges_.begin() + existing_edge + 1,
            edges_.begin() + edges_offset + edges_amount,
            edges_.begin() + existing_edge);
  --total_edges_; // just render 1 point less
  --instances_.back().edges_amount_;
}

void ArbitraryGraph::EditEdgePoint(GLuint vertex_id) {
  auto found_id = FindVerticesOffsetById(vertex_id);
  if (found_id != -1) {
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
  if (selected_id_1_ == -1) {
    selected_id_1_ = vertex_id;
  } else if (selected_id_2_ == -1) {
    selected_id_2_ = vertex_id;
  } else {
    /// else rewrite first (so something always changes)
    selected_id_1_ = vertex_id;
  }
}

void ArbitraryGraph::EditPoint(GLuint vertex_id) {
  auto found_id = FindVerticesOffsetById(vertex_id);
  if (found_id == -1) {
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
  shared_resources_.arbitrary_grapn_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tile_.map_terrain_height.Bind();
  glUniform1ui(1, selected_id_1_);
  glUniform1ui(2, selected_id_2_);
  glBindVertexArray(points_vao_);
  glPointSize(5.0f);
  glDrawArrays(GL_POINTS, 0, total_vertices_);
  /// we could make it just GL_LINE_STRIP, but the graph is complex
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
