#ifndef WIREBOUNDWORLDCREATOR_STATICOBJECT_H
#define WIREBOUNDWORLDCREATOR_STATICOBJECT_H

// wall, floor, furniture

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "Scene.h"

struct StaticObject {
  const Scene::Model& model;
  JPH::BodyID body_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_STATICOBJECT_H
