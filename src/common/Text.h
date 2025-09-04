#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_

#include <array>
#include <string>

namespace data {

enum class TextId {
  /// --- menu prerender (common for each mode) ---
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

  kLoading,
  kAccept,
  kDecline,
  kConfirmationExit,
  kConfirmationFileOverwrite,
  kConfirmationFileLoad,
  kFileOpen,

  /// --- kTerrainRender prerender ---
  kScaleTerrain,
  kScaleX,
  kScaleY,
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
  kPerlin,
  kCellular,
  kMetaballs,
  kFbmGrid,
  kFbmMulti,
  kFbmdPerlin,
  kFbmWarp,
  kFbmPerlinWarp,

  kErosion,
  kWeathering,

  kTiling,
  kInvert,
  kStrength,

  /// --- kWaterMode prerender ---
  kScaleWater, // scale for each mode to prerender section together
  kFetch,
  kSpreadBlend,
  kSwell,
  kPeakEnhancement,
  kShortWavesFade,
  kLambda,
  kLayer1,
  kLayer2,
  kLayer3,

  /// ---

  kTotal,
  kNotYet, // not yet done
  kNone, // shouldn't be used (always check != kNone)
};

inline constexpr int gTotalText = static_cast<int>(TextId::kTotal)
                                  - static_cast<int>(TextId::kMenuTerrain);

extern const std::array<std::string_view, gTotalText> gText;

} // namespace text

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXT_H_
