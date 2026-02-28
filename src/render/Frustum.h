#ifndef WIREBOUNDWORLDCREATOR_RENDER_FRUSTRUM_H
#define WIREBOUNDWORLDCREATOR_RENDER_FRUSTRUM_H

#include <Jolt/Geometry/AABox.h>
#include <Jolt/Geometry/Plane.h>
#include <Jolt/Jolt.h>

#include <glm/glm.hpp>

/// A camera frustum containing of 6 planes (left, right, top, bottom, near,
/// far) pointing inwards
class Frustum {
 public:
  /// Empty constructor
  Frustum() = default;

  /// Construct frustum from position, forward, up, field of view x and y and
  /// near plane. Note that inUp does not need to be perpendicular to inForward
  /// but cannot be collinear.
  Frustum(glm::vec3 inPosition, glm::vec3 inForward, glm::vec3 inUp,
          float inFOVX, float inFOVY, float inNear) {
    // TODO:
    auto jph_position = JPH::Vec3(inPosition.x, inPosition.y, inPosition.z);
    auto jph_forward = JPH::Vec3(inForward.x, inForward.y, inForward.z);
    auto jph_up = JPH::Vec3(inUp.x, inUp.y, inUp.z);

    JPH::Vec3 right = jph_forward.Cross(jph_up).Normalized();
    JPH::Vec3 up = right.Cross(jph_forward).Normalized();

    // Near and far planes
    mPlanes[0] = JPH::Plane::sFromPointAndNormal(
        jph_position + inNear * jph_forward, jph_forward);
    mPlanes[1] = JPH::Plane::sFromPointAndNormal(
        jph_position + 100 * inNear * jph_forward, -jph_forward);

    // Top and bottom planes
    mPlanes[2] = JPH::Plane::sFromPointAndNormal(
        jph_position, JPH::Mat44::sRotation(right, 0.5f * inFOVY) * -up);
    mPlanes[3] = JPH::Plane::sFromPointAndNormal(
        jph_position, JPH::Mat44::sRotation(right, -0.5f * inFOVY) * up);

    // Left and right planes
    mPlanes[4] = JPH::Plane::sFromPointAndNormal(
        jph_position, JPH::Mat44::sRotation(up, 0.5f * inFOVX) * right);
    mPlanes[5] = JPH::Plane::sFromPointAndNormal(
        jph_position, JPH::Mat44::sRotation(up, -0.5f * inFOVX) * -right);
  }

  bool Overlaps(const JPH::AABox &inBox) const {
    for (const JPH::Plane &p : mPlanes) {
      // Get support point (the maximum extent) in the direction of our normal
      JPH::Vec3 support = inBox.GetSupport(p.GetNormal());
      // If this is behind our plane, the box is not inside the frustum
      if (p.SignedDistance(support) < 0.0f) return false;
    }
    return true;
  }

 private:
  JPH::Plane mPlanes[6];
};

#endif  // WIREBOUNDWORLDCREATOR_RENDER_FRUSTRUM_H
