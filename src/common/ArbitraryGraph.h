#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_

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


#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_ARBITRARYGRAPH_H_
