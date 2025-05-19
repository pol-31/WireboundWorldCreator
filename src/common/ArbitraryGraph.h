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

  // btn create button
  void CreateGraph();

  //TODO: SelectGraph && RemoveGraph we can't test (don't have UiSlots sprites)
  // UiSlots (2 btns for each)
  void SelectGraph();
  void RemoveGraph();

  void Remove(); // btn remove

  // btn flip points/edges, btn flip select/create
  // such a weird functions
  bool FlipPointsMode(); // points, edges
  bool FlipPressMode(); // select, modify

  void Render();

 private:
  void Init();

  void DeInit();

  void UpdateBuffers();

  void EditPoint(GLuint vertex_id);
  void EditEdgePoint(GLuint vertex_id);

  void AddEdge(GLuint vertex_id);
  void AddPoint(GLuint vertex_id);

  void RemovePoint();
  void RemoveEdge();

  //TODO: use iterators
  GLuint FindVerticesOffsetById(GLuint id);
  GLuint FindEdgeBySelected(GLuint offset_1, GLuint offset_2);

  struct InstanceData {
    int vertices_offset_;
    int vertices_amount_;
    int edges_offset_;
    int edges_amount_;
  };
  std::vector<InstanceData> instances_;

  static constexpr size_t kMaxPoints = 100;

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

  bool target_points_{true}; // opposite target_edges_
  bool press_select_{true}; // opposite modify

  SharedResources& shared_resources_;
};

//TODO:
// huge std::array<int, 1000 vertices> graph_vertex_data_;
// huge std::array<int, 500 indices * 2> graph_index_data_;
// int total_vertices, total_edges;
// create new graph: {
//   offset_vertex(total_vertices), amount_vertex,
//   offset_index(total_edges * 2), offset_index
//   }
// edit created graph: move it back, modify all offsets, start editing












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
