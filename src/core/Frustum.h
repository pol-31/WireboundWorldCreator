// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Geometry/Plane.h>

/// A camera frustum containing of 6 planes (left, right, top, bottom, near,
/// far) pointing inwards
class Frustum {
 public:
  /// Empty constructor
  Frustum() = default;

  /// Construct frustum from position, forward, up, field of view x and y and
  /// near plane. Note that inUp does not need to be perpendicular to inForward
  /// but cannot be collinear.
  Frustum(JPH::Vec3Arg inPosition, JPH::Vec3Arg inForward, JPH::Vec3Arg inUp,
          float inFOVX, float inFOVY, float inNear) {
    JPH::Vec3 right = inForward.Cross(inUp).Normalized();
    JPH::Vec3 up =
        right.Cross(inForward)
            .Normalized();  // Calculate the real up vector (inUp does not
                            // need to be perpendicular to inForward)

    // Near plane
    mPlanes[0] = JPH::Plane::sFromPointAndNormal(
        inPosition + inNear * inForward, inForward);

    // Top and bottom planes
    mPlanes[1] = JPH::Plane::sFromPointAndNormal(
        inPosition, JPH::Mat44::sRotation(right, 0.5f * inFOVY) * -up);
    mPlanes[2] = JPH::Plane::sFromPointAndNormal(
        inPosition, JPH::Mat44::sRotation(right, -0.5f * inFOVY) * up);

    // Left and right planes
    mPlanes[3] = JPH::Plane::sFromPointAndNormal(
        inPosition, JPH::Mat44::sRotation(up, 0.5f * inFOVX) * right);
    mPlanes[4] = JPH::Plane::sFromPointAndNormal(
        inPosition, JPH::Mat44::sRotation(up, -0.5f * inFOVX) * -right);
  }

  /// Test if frustum overlaps with axis aligned box. Note that this is a
  /// conservative estimate and can return true if the frustum doesn't actually
  /// overlap with the box. This is because we only test the plane axis as
  /// separating axis and skip checking the cross products of the edges of the
  /// frustum
  inline bool Overlaps(const JPH::AABox &inBox) const {
    // Loop over all frustum planes
    for (const JPH::Plane &p : mPlanes) {
      // Get support point (the maximum extent) in the direction of our normal
      JPH::Vec3 support = inBox.GetSupport(p.GetNormal());

      // If this is behind our plane, the box is not inside the frustum
      if (p.SignedDistance(support) < 0.0f) return false;
    }

    return true;
  }

 private:
  JPH::Plane mPlanes[5];  ///< Planes forming the frustum
};
