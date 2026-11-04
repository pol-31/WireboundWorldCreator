#include "PointLight.h"

PointLight::PointLight(SceneNode* scene_node) {
  scene_node_ = scene_node;
}

PointLight::InteractPrompt PointLight::GetInteractPrompt(Character* interactor) {
  return {"point light object", GetPosition()};
}
