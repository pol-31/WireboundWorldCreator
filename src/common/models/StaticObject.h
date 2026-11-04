#ifndef WIREBOUNDWORLDCREATOR_STATICOBJECT_H
#define WIREBOUNDWORLDCREATOR_STATICOBJECT_H

// wall, floor, furniture

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include "GameObject.h"

class StaticObject : public GameObject {
public:
  StaticObject(SceneNode* scene_node);
  InteractPrompt GetInteractPrompt(Character* interactor) override;
};

#endif  // WIREBOUNDWORLDCREATOR_STATICOBJECT_H
