#ifndef WIREBOUNDWORLDCREATOR_CHARACTER_H
#define WIREBOUNDWORLDCREATOR_CHARACTER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "Scene.h"

struct Character {
  const Scene::Model& model;
  JPH::Ref<JPH::CharacterVirtual> jph_character_;
  int animation_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_CHARACTER_H
