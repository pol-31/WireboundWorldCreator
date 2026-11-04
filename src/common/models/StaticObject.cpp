#include "StaticObject.h"

#include "Scene.h"

StaticObject::StaticObject(SceneNode* scene_node) {
  scene_node_ = scene_node;
}

StaticObject::InteractPrompt StaticObject::GetInteractPrompt(Character* interactor) {
  return {"static object", GetPosition()};
}
