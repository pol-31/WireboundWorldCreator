#ifndef WIREBOUNDWORLDCREATOR_ENTITYIDMANAGER_H
#define WIREBOUNDWORLDCREATOR_ENTITYIDMANAGER_H

#include <cinttypes>

struct EntityIdManager {
  static constexpr uint32_t InvalidEntityId = 0;
  static constexpr uint32_t PlayerHumanId = 1;
  static constexpr uint32_t PlayerFpvId = 2;
  static constexpr uint32_t ResetId = 3;

  uint32_t next = ResetId;

  void Reset() { next = ResetId; }
  uint32_t Allocate() { return next++; }
};

extern EntityIdManager gEntityIdManager;

#endif  // WIREBOUNDWORLDCREATOR_ENTITYIDMANAGER_H
