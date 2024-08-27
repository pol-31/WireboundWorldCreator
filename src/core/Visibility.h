#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_VISIBILITY_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_VISIBILITY_H_

#include <cinttypes>

struct Visibility {
  std::uint8_t visibility{0b1111'1111};

  inline void SetMode(int idx) {
    visibility ^= 1 << idx;
  }

  inline bool IsTerrainVisible() const {
    return visibility & 0b0000'0001;
  }
  inline bool IsWaterVisible() const {
    return visibility & 0b0000'0010;
  }
  inline bool IsRoadsVisible() const {
    return visibility & 0b0000'0100;
  }
  inline bool IsFencesVisible() const {
    return visibility & 0b0000'1000;
  }
  inline bool IsPlacementVisible() const {
    return visibility & 0b0001'0000;
  }
  inline bool IsObjectsVisible() const {
    return visibility & 0b0010'0000;
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_VISIBILITY_H_
