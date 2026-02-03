#ifndef WIREBOUNDWORLDCREATOR_AABB3D_H
#define WIREBOUNDWORLDCREATOR_AABB3D_H

#include <glm/glm.hpp>

struct Obb3D {
  glm::vec3 center = glm::vec3(0.0f);
  glm::vec3 halfSize = glm::vec3(0.0f);
  glm::mat3 orientation = glm::mat3(1.0f);
};

struct Aabb2D {
  glm::vec2 min = glm::vec2(FLT_MAX);
  glm::vec2 max = glm::vec2(-FLT_MAX);
};

// local space (rel to 0;0;0 centre)
struct Aabb3D {
  glm::vec3 min = glm::vec3(FLT_MAX);
  glm::vec3 max = glm::vec3(-FLT_MAX);

  static Aabb3D TransformAabb(const Aabb3D& aabb, const glm::mat4& m);
  static bool Intersect(const Aabb3D& a, const Aabb3D& b);
  static Aabb3D Combine(const Aabb3D& aabb1, const Aabb3D& aabb2);

  int LongestAxis();
};

Obb3D AabbToObb(const Aabb3D& aabb, const glm::mat4& transform);

float GetObbBottomY(const Obb3D& obb);

struct BVHNode {
  Aabb3D bounds;
  int left = -1;
  int right = -1;
  int object = -1;  // leaf only
};

#endif  // WIREBOUNDWORLDCREATOR_AABB3D_H
