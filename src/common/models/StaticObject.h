#ifndef WIREBOUNDWORLDCREATOR_STATICOBJECT_H
#define WIREBOUNDWORLDCREATOR_STATICOBJECT_H

// wall, floor, furniture

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "Scene.h"

struct StaticObject {
  const Scene::ModelNode* object_;

  StaticObject(const Scene::ModelNode* object) : object_(object) {}
};

#endif  // WIREBOUNDWORLDCREATOR_STATICOBJECT_H
