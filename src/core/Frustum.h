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

  Frustum(const Frustum& other) = default;
  Frustum(Frustum&& other) = default;

  Frustum& operator=(const Frustum& other) = default;
  Frustum& operator=(Frustum&& other) = default;

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

  inline bool Overlaps(const JPH::AABox &inBox, JPH::Vec3Arg inCenter, float inRadius) const {
    if (inBox.GetSqDistanceTo(inCenter) > inRadius * inRadius) {
      return false;
    }
    for (const JPH::Plane &p : mPlanes) {
      JPH::Vec3 support = inBox.GetSupport(p.GetNormal());
      if (p.SignedDistance(support) < 0.0f) {
        return false;
      }
    }

    return true;
  }

  // Checks if a specific point light cubemap face (0-5) overlaps this camera frustum
  inline bool OverlapsLightFace(JPH::Vec3Arg inLightPos, float inRadius, int inFaceIndex) const {
    JPH::Vec3 v[5];
    v[0] = inLightPos; // The light center is always the apex of the pyramid

    float r = inRadius;

    // Generate the 4 far corners of the 90-degree face pyramid
    switch (inFaceIndex) {
      case 0: // +X face
        v[1] = inLightPos + JPH::Vec3( r,  r,  r);
        v[2] = inLightPos + JPH::Vec3( r,  r, -r);
        v[3] = inLightPos + JPH::Vec3( r, -r,  r);
        v[4] = inLightPos + JPH::Vec3( r, -r, -r);
        break;
      case 1: // -X face
        v[1] = inLightPos + JPH::Vec3(-r,  r,  r);
        v[2] = inLightPos + JPH::Vec3(-r,  r, -r);
        v[3] = inLightPos + JPH::Vec3(-r, -r,  r);
        v[4] = inLightPos + JPH::Vec3(-r, -r, -r);
        break;
      case 2: // +Y face
        v[1] = inLightPos + JPH::Vec3( r,  r,  r);
        v[2] = inLightPos + JPH::Vec3(-r,  r,  r);
        v[3] = inLightPos + JPH::Vec3( r,  r, -r);
        v[4] = inLightPos + JPH::Vec3(-r,  r, -r);
        break;
      case 3: // -Y face
        v[1] = inLightPos + JPH::Vec3( r, -r,  r);
        v[2] = inLightPos + JPH::Vec3(-r, -r,  r);
        v[3] = inLightPos + JPH::Vec3( r, -r, -r);
        v[4] = inLightPos + JPH::Vec3(-r, -r, -r);
        break;
      case 4: // +Z face
        v[1] = inLightPos + JPH::Vec3( r,  r,  r);
        v[2] = inLightPos + JPH::Vec3(-r,  r,  r);
        v[3] = inLightPos + JPH::Vec3( r, -r,  r);
        v[4] = inLightPos + JPH::Vec3(-r, -r,  r);
        break;
      case 5: // -Z face
        v[1] = inLightPos + JPH::Vec3( r,  r, -r);
        v[2] = inLightPos + JPH::Vec3(-r,  r, -r);
        v[3] = inLightPos + JPH::Vec3( r, -r, -r);
        v[4] = inLightPos + JPH::Vec3(-r, -r, -r);
        break;
      default:
        return false;
    }

    // Test the 5 points against the 5 camera frustum planes
    for (const JPH::Plane &p : mPlanes) {
      bool all_points_behind_plane = true;

      for (int i = 0; i < 5; ++i) {
        // If even ONE point is in front of or inside the plane, it is NOT completely culled by this plane
        if (p.SignedDistance(v[i]) >= 0.0f) {
          all_points_behind_plane = false;
          break;
        }
      }

      // If all 5 points of the face are behind this camera plane, the face cannot be seen
      if (all_points_behind_plane) {
        return false;
      }
    }

    return true; // Face passed all planes, it is visible!
  }

 private:
  JPH::Plane mPlanes[5];  ///< Planes forming the frustum
};
