#ifndef WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
#define WIREBOUNDWORLDCREATOR_GAMEOBJECT_H

#include <string_view>

#include <Jolt/Jolt.h>

class Character;

class GameObject {
public:
  virtual ~GameObject() = default;

  virtual void TakeDamage(JPH::Vec3 impulse, JPH::RVec3 hit_pos, float damage) {}

  virtual void Interact(Character* interactor) {}

  virtual std::string GetInteractPrompt() { return ""; }
};

#endif  // WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
