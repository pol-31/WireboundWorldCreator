#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_

#include <array>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../modes/SharedResources.h"

//template <size_t kMaxPoints>
class ArbitraryGraph {
 public:
  ArbitraryGraph(SharedResources& shared_resources);

  void Press(GLuint vertex_id);

  /// emplace back
  void CreateGraph();

  /// move instances_[slot_id] data to the end, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void SelectGraph(int slot_id);

  /// remove instances_[slot_id] from all buffers, as well as
  /// modify all offsets and edge vertices id (ebo buffer data)
  void RemoveGraph(int slot_id);

  [[nodiscard]] int GetGraphNum() const noexcept {
    return instances_.size();
  }

  /// -1 in case of non-selected
  [[nodiscard]] int GetSlotId() const noexcept {
    return selected_slot_id_;
  }

  void Remove();

  // btn flip points/edges, btn flip select/create
  bool FlipPointsMode(); // points, edges
  bool FlipPressMode(); // select, modify

  void Render();

  static constexpr size_t kMaxPoints = 100;

 private:
  struct InstanceData {
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

  void AddEdge(GLuint vertex_id);
  void AddPoint(GLuint vertex_id);

  void RemovePoint();
  void RemoveEdge();

  std::array<GLuint, ArbitraryGraph::kMaxPoints>::iterator
  FindVerticesOffsetById(GLuint id);
  std::array<glm::uvec2, ArbitraryGraph::kMaxPoints>::iterator
  FindEdgeBySelected(GLuint offset_1, GLuint offset_2);

  // wrt slots, so at creation we push_back, at remove we remove & decrement
  // instances_[slot_id] and NOT instances_[instance_id]
  // so the dependency is: graph_id == instances_[slot_id]
  std::vector<InstanceData> instances_;

  int selected_slot_id_{-1};

  // point as a single number - in shader decompose to x;y by mask
  std::array<GLuint, kMaxPoints>::size_type total_vertices_{0};
  std::array<glm::uvec2, kMaxPoints>::size_type total_edges_{0};
  std::array<GLuint, kMaxPoints> vertices_;
  std::array<glm::uvec2, kMaxPoints> edges_;

  GLuint points_vao_{0};
  GLuint points_vbo_{0};
  GLuint points_ebo_{0};

  // point OR point & point
  GLuint selected_id_1_{static_cast<GLuint>(-1)};
  GLuint selected_id_2_{static_cast<GLuint>(-1)};

  bool target_points_{true}; // opposite target - edges
  bool press_select_{true}; // opposite mode - edit (modify)

  SharedResources& shared_resources_;
};






using Point = glm::ivec2;

// used for roads & fences representation, that
// can be placed solely on terrain (not objects)
struct ArbitraryGraph_ {
  std::vector<Point> vertices; // x, y
  // vertex1 id, vertex2 id from vertices
  std::vector<std::pair<int, int>> edges;
  /** Serialization example:
 * 0,3,3 # uint32 type_id, uint32 vertices_num, uint32 edges_num;
 * 0,0 # vertices_num=3, so we have three points (x,y); this is vertex1
 * 10,20 # point 2
 * 10,30 # point 3
 * 0,1 # edges_num=3, so we have three edges(vertex1, vertex2); this is edge1
 * 0,2
 * 1,2
 * 0,7,12 # because vertices_num and edges_num of previous section is over, we start next graph
 * */
};
//TODO: we use the same struct for water, so edges_num is always == 0,
//  while other data (graph type id and position for each point) are the same

//TODO: replace with glm (currently I'm not sure about its internal order)
int CrossProduct(const Point& A, const Point& B, const Point& C);

bool IsCcw(const std::vector<Point>& points);

bool DoIntersect(const Point& p1, const Point& q1,
                 const Point& p2, const Point& q2);

bool IsConvexPolygon(const std::vector<Point>& points);

// Function to check if the point p lies on the left side of the line segment from p1 to p2
bool isLeft(Point p1, Point p2, Point p);

// Function to check if a point lies inside a convex polygon
bool isInsideConvexPolygon(const std::vector<Point>& polygon, Point p);

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
