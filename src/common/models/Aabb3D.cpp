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

Obb3D AabbToObb(const Aabb3D& aabb, const glm::mat4& transform) {
  Obb3D obb;

  // center in local space
  glm::vec3 localCenter = (aabb.min + aabb.max) * 0.5f;
  glm::vec3 half = (aabb.max - aabb.min) * 0.5f;

  // transform center
  obb.center = glm::vec3(transform * glm::vec4(localCenter, 1.0f));

  // orientation = rotation part of matrix
  obb.orientation = glm::mat3(transform);

  // scale the halfSize by matrix scale
  obb.halfSize = glm::vec3(glm::length(obb.orientation[0]) * half.x,
                           glm::length(obb.orientation[1]) * half.y,
                           glm::length(obb.orientation[2]) * half.z);

  // normalize orientation axes
  obb.orientation[0] = glm::normalize(obb.orientation[0]);
  obb.orientation[1] = glm::normalize(obb.orientation[1]);
  obb.orientation[2] = glm::normalize(obb.orientation[2]);

  return obb;
}

float GetObbBottomY(const Obb3D& obb) {
  glm::vec3 up = obb.orientation[1];  // local up axis
  return obb.center.y - obb.halfSize.y * fabs(up.y) -
         obb.halfSize.x * fabs(obb.orientation[0].y) -
         obb.halfSize.z * fabs(obb.orientation[2].y);
}
