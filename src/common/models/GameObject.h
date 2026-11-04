#ifndef WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
#define WIREBOUNDWORLDCREATOR_GAMEOBJECT_H

#include <string>

#include <Jolt/Jolt.h>

#include "AssetProperties.h"

class Character;
struct SceneNode;

class GameObject {
public:
  struct InteractPrompt {
    std::string prompt;
    JPH::Vec3 position;
  };

  virtual ~GameObject() = default;

  virtual void TakeDamage(JPH::Vec3 impulse, JPH::RVec3 hit_pos, float damage) {}

  virtual void Interact(Character* interactor) {}

  // if prompt is empty, we remove it from game::hovered_object and can drag it
  virtual InteractPrompt GetInteractPrompt(Character* interactor) { return {}; }

  virtual JPH::Vec3 GetPosition() const;

  SceneNode* GetSceneNode() noexcept {
    return scene_node_;
  }

  const SceneNode* GetSceneNode() const noexcept {
    return scene_node_;
  }

  const AssetProperties& GetAssetProperties() const;

  float lastCollisionSoundTime = 0.0f;

protected:
  SceneNode* scene_node_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
