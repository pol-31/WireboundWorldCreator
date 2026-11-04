#include "GameObject.h"

#include "Scene.h"
#include "AssetProperties.h"

JPH::Vec3 GameObject::GetPosition() const {
  return scene_node_->global_bounds.GetCenter();
}

const AssetProperties& GameObject::GetAssetProperties() const {
  return GetAssetDefaultProperties(scene_node_->mesh->name);
}
