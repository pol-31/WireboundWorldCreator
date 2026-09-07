#ifndef WIREBOUNDWORLDCREATOR_STATICOBJECT_H
#define WIREBOUNDWORLDCREATOR_STATICOBJECT_H

// wall, floor, furniture

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

class SceneNode;

struct StaticObject {
  const SceneNode* object_;

  StaticObject(const SceneNode* object) : object_(object) {}
};

#endif  // WIREBOUNDWORLDCREATOR_STATICOBJECT_H
