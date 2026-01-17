#ifndef WIREBOUNDWORLDCREATOR_ATTACKEVENT_H
#define WIREBOUNDWORLDCREATOR_ATTACKEVENT_H

#include <cinttypes>

#include "Aabb3D.h"

struct AttackEvent {
  Aabb3D hitbox;
  uint32_t attacker = 0;
  float damage = 0.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_ATTACKEVENT_H
