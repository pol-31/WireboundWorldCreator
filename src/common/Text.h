#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_

#include <array>
#include <string>

namespace data {

enum class TextId {
  // terrain noise traits
  kScale,
  kSeed,
  kJitter,
  kPhase,
  kPhases,
  kOctaves,
  kShift,
  kTimeShift,
  kGain,
  kLacunarity,
  kTranslate,
  kWarpStrength,
  kOctaveFactor,
  kTransform,
  kSlopeness,
  kNegative,
  kFactors,
  kQ,
  kR,

  kErosion,
  kWeathering,


  kMenuTerrain,
  kMenuWater,
  kMenuRoads,
  kMenuFences,
  kMenuPlacement,
  kMenuObjects,
  kMenuBiomes,
  kMenuTiles,
  kMenu,

  kResolution,
  kShowTips,

  Newgame,
  Loadgame,
  Continue,
  Archive,
  Options,
  Credits,
  Quit,
  Loading,
  Copyright,
  https,
  github,
  com,
  Superman,
  LambofGod,
  Pantera,
  Hatebreed,

  kTotal,
  kNotYet, // not yet done
  kNone, // shouldn't be used (always check != kNone)
};

inline constexpr int gTotalText = static_cast<int>(TextId::kTotal)
                                  - static_cast<int>(TextId::kScale);

extern const std::array<std::string_view, gTotalText> gText;

} // namespace text

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
