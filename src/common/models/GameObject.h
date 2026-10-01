#ifndef WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
#define WIREBOUNDWORLDCREATOR_GAMEOBJECT_H

#include <string_view>

#include <Jolt/Jolt.h>

class Character;

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

  virtual JPH::Vec3 GetPosition() const {
    return JPH::Vec3::sZero();
  }
};

#endif  // WIREBOUNDWORLDCREATOR_GAMEOBJECT_H
