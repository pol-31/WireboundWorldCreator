#ifndef WIREBOUNDWORLDCREATOR_AABB3D_H
#define WIREBOUNDWORLDCREATOR_AABB3D_H

#include <glm/glm.hpp>

// local space (rel to 0;0;0 centre)
struct Aabb3D {
  glm::vec3 min = glm::vec3(FLT_MAX);
  glm::vec3 max = glm::vec3(-FLT_MAX);

  static Aabb3D TransformAabb(const Aabb3D& aabb, const glm::mat4& m);
  static bool Intersect(const Aabb3D& a, const Aabb3D& b);
  static Aabb3D Combine(const Aabb3D& aabb1, const Aabb3D& aabb2);

  int LongestAxis();
};

struct BVHNode {
  Aabb3D bounds;
  int left = -1;
  int right = -1;
  int object = -1;  // leaf only
};

#endif  // WIREBOUNDWORLDCREATOR_AABB3D_H
