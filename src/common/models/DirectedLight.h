#ifndef WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H
#define WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H

#include <Jolt/Jolt.h>

#include "../../core/Frustum.h"

  struct DirectedLight {
  JPH::Mat44 lightSpaceMatrix = JPH::Mat44::sIdentity();
  JPH::Vec3 dir = JPH::Vec3(1.0f, 0.0f, 0.0f);
  JPH::Vec3 color = JPH::Vec3::sOne();
  Frustum frustum = {};
};

#endif  //  WIREBOUNDWORLDCREATOR_DIRECTEDLIGHT_H
