#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_

#include <vector>

// used for roads & fences representation, that
// can be placed solely on terrain (not objects)
struct ArbitraryGraph {
  struct Point {
    int x;
    int y;
  };
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

struct Point {
  int x;
  int y;
};
//TODO:
//typedef glm::vec2 Point;

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
