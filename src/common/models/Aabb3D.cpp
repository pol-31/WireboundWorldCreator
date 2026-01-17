#include "Aabb3D.h"

Aabb3D Aabb3D::TransformAabb(const Aabb3D& aabb, const glm::mat4& m) {
  glm::vec3 corners[8] = {{aabb.min.x, aabb.min.y, aabb.min.z},
                          {aabb.max.x, aabb.min.y, aabb.min.z},
                          {aabb.min.x, aabb.max.y, aabb.min.z},
                          {aabb.max.x, aabb.max.y, aabb.min.z},
                          {aabb.min.x, aabb.min.y, aabb.max.z},
                          {aabb.max.x, aabb.min.y, aabb.max.z},
                          {aabb.min.x, aabb.max.y, aabb.max.z},
                          {aabb.max.x, aabb.max.y, aabb.max.z}};

  Aabb3D out;

  for (int i = 0; i < 8; ++i) {
    glm::vec3 p = glm::vec3(m * glm::vec4(corners[i], 1.0f));
    out.min = glm::min(out.min, p);
    out.max = glm::max(out.max, p);
  }
  return out;
}

bool Aabb3D::Intersect(const Aabb3D& a, const Aabb3D& b) {
  return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
         (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
         (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

Aabb3D Aabb3D::Combine(const Aabb3D& aabb1, const Aabb3D& aabb2) {
  return {glm::min(aabb1.min, aabb2.min), glm::max(aabb1.max, aabb2.max)};
}

int Aabb3D::LongestAxis() {
  float x = (max.x - min.x) / 2.0f;
  float y = (max.y - min.y) / 2.0f;
  float z = (max.z - min.z) / 2.0f;
  if (x > y && x > z) {
    return 0;
  } else if (y > z) {
    return 1;
  } else {
    return 2;
  }
}
