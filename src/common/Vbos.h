#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_

#include <array>
#include <cinttypes>

#include "Details.h"

class UiTransformDbg;

namespace vbos {

struct UiData {
  int32_t id;
  std::size_t vbo_offset;
  /// comparing to Wirebound, here we add description for each button,
  /// which showed in some specific area externally.
  std::size_t text_vbo_offset_;

  // more than 0 for complex objects
  // e.g. for Wheel-slider fill has 3 children and comes first
  int children_num{0};

  // useful for e.g. slider to update centre/length of interactive area
  UiTransformDbg* ui{nullptr};
};

enum class VboIdMain;
enum class VboIdText;
enum class VboIdInstanced;

inline constexpr std::size_t GetVboMainOffset(VboIdMain id);
inline constexpr std::size_t GetVboTextOffset(VboIdText id);
inline constexpr std::size_t GetVboInstancedOffset(VboIdInstanced id);

/// related to kUiVboDataMain
enum class VboIdMain {
  kModeTerrain = details::kIdOffsetUi,
  kModeWater,
  kModeRoads,
  kModeFences,
  kModePlacement,
  kModeObjects,
  kModeBiome,
  kModeTiles,

  kVisionTerrain,
  kVisionWater,
  kVisionRoads,
  kVisionFences,
  kVisionPlacement,
  kVisionObjects,
  kVisionBiome,
  kVisionTiles,

  kWireboundLogo,

  kAddNew,
  kRemove,

  kTerrainSmooth,
  kTerrainUpdate,

  kTerrainSliderSizeFill,
  kTerrainSliderSizeSlow,
  kTerrainSliderSizeModerate,
  kTerrainSliderSizeFast,

  kTerrainSliderFalloffFill,
  kTerrainSliderFalloffSlow,
  kTerrainSliderFalloffModerate,
  kTerrainSliderFalloffFast,

  kWaterLake,
  kWaterRiver,
  kWaterWaterfall,

  // 3 cascades (near, mid, far : 1, 2, 3)

  kWater1SliderScaleFill,
  kWater1SliderScaleWheel,
  kWater1SliderFetchFill,
  kWater1SliderFetchWheel,
  kWater1SliderSpreadBlendFill,
  kWater1SliderSpreadBlendWheel,
  kWater1SliderSwellFill,
  kWater1SliderSwellWheel,
  kWater1SliderPeakEnhancementFill,
  kWater1SliderPeakEnhancementWheel,
  kWater1SliderShortWavesFadeFill,
  kWater1SliderShortWavesFadeWheel,
  kWater1SliderLambdaFill,
  kWater1SliderLambdaWheel,

  kWater2SliderScaleFill,
  kWater2SliderScaleWheel,
  kWater2SliderFetchFill,
  kWater2SliderFetchWheel,
  kWater2SliderSpreadBlendFill,
  kWater2SliderSpreadBlendWheel,
  kWater2SliderSwellFill,
  kWater2SliderSwellWheel,
  kWater2SliderPeakEnhancementFill,
  kWater2SliderPeakEnhancementWheel,
  kWater2SliderShortWavesFadeFill,
  kWater2SliderShortWavesFadeWheel,
  kWater2SliderLambdaFill,
  kWater2SliderLambdaWheel,

  kWater3SliderScaleFill,
  kWater3SliderScaleWheel,
  kWater3SliderFetchFill,
  kWater3SliderFetchWheel,
  kWater3SliderSpreadBlendFill,
  kWater3SliderSpreadBlendWheel,
  kWater3SliderSwellFill,
  kWater3SliderSwellWheel,
  kWater3SliderPeakEnhancementFill,
  kWater3SliderPeakEnhancementWheel,
  kWater3SliderShortWavesFadeFill,
  kWater3SliderShortWavesFadeWheel,
  kWater3SliderLambdaFill,
  kWater3SliderLambdaWheel,

  kWaterUpdate,

  kRoadsAsphalt,
  kRoadsGravel,
  kRoadsSoil,

  kFencesPicket,
  kFencesChainLink,
  kFencesWooden,

  kPlacementSliderColorFill,
  kPlacementSliderColorSlow,
  kPlacementSliderColorModerate,
  kPlacementSliderColorFast,

  kPlacementSliderSizeFill,
  kPlacementSliderSizeSlow,
  kPlacementSliderSizeModerate,
  kPlacementSliderSizeFast,

  kPlacementSliderFalloffFill,
  kPlacementSliderFalloffSlow,
  kPlacementSliderFalloffModerate,
  kPlacementSliderFalloffFast,

  kPlacementTrees,
  kPlacementBushes,
  kPlacementTallGrass,
  kPlacementUndergrowth,

  kSlot, // only tex_coords

  kTilesPageUp,
  kTilesPageDown,
  kTilesPageLeft,
  kTilesPageRight,

  kTilesPreview,
  kFullScreen, // for dgb
  kTotal,
};

inline constexpr int gVboIdSize =
    static_cast<int>(VboIdMain::kTotal)
    - static_cast<int>(VboIdMain::kModeTerrain);

#ifndef NDEBUG

// before RELEASE all transform data should be converted to exact
// coord values, so it would be the same as for all we set kNoTransform

// for each coord we have "in vec3 transform;"
// and we sipmly move it like:
// pos * transform[2] + vec2(transform[0], transform[1])

// for complex ui You should set the same transform,
// so each components has local transform in VboMain.
// This way we can check positions by module without
// adding excessive transform uniform or dependencies

//TODO: depends on DEBUG or NDEBUG use different vao/vbo
//  and accordingly std::arrays

// TODO: but it's seems to be not cache-friendly...

inline constexpr int gUiVboTransformSize = gVboIdSize * 4 * 3;

extern const std::array<float, gUiVboTransformSize> kUiVboDataTransform;

#endif // NDEBUG


enum class VboIdText {
  kMode = static_cast<int>(VboIdMain::kTotal) + 1,
  kVision,
  kTerrain,
  kWater,
  kRoads,
  kFences,
  kPlacement,
  kObject,
  kBiome,
  kTiles,
  kShaders,
  kWirebound,
  kSmooth,
  kUpdateModesMaps,
  kAdjustSize,
  kAdjustFalloff,
  kAddNew,
  kRemoveSelected,
  kBakeAsALake,
  kBakeAsARiver,
  kBakeAsAWaterfall,
  kBakeAsAnAsphaltRoad,
  kBakeAsAGravelRoad,
  kBakeAsASoilRoad,
  kBakeAsAPicketFence,
  kBakeAsAChainLinkedFence,
  kBakeAsAWoodenFence,
  kPreviousSlot,
  kNextSlot,
  kPlaceTrees,
  kPlaceBushes,
  kPlaceTallGrass,
  kPlaceUndergrowth,
  kSetDrawColorToWhite,
  kSetDrawColorToLightGrey,
  kSetDrawColorToGrey,
  kSetDrawColorToDarkGrey,
  kSetDrawColorToBlack,
  kPreviousPage,
  kNextPage,
  kPreviousRow,
  kNextRow,
  kPreviousColumn,
  kNextColumn,
  kCurrentTile,
  kBakingInProcess,
  kObjectWasNotBakedSoRemoved,
  kSaving,
  kLoading,
  kUnableToOpenTheFile,
  kUnableToSaveTheFile,
  kTotal,
  kNone, // doesn't describe any data, but used to set -1 at ctors
};

enum class VboIdInstanced {
  kObjects = static_cast<int>(VboIdText::kTotal) + 1,
  kBiomes,
  kTiles,
  kTotal,
};

inline constexpr std::size_t GetVboMainOffset(VboIdMain id) {
  // for each Ui component 16 floats
  // (4 2d points with position and tex coords)
  return (static_cast<std::size_t>(id) - (details::kIdOffsetUi)) * 4; // TODO: 4?
}

inline constexpr std::size_t GetVboTextOffset(VboIdText id) {
  if (id == VboIdText::kNone) {
    return -1;
  } else {
    return (static_cast<std::size_t>(id) -
        static_cast<int>(VboIdText::kMode)) * 4; // TODO: or 8?
  }
}

inline constexpr std::size_t GetVboInstancedOffset(VboIdInstanced id) {
  switch(id) {
    case VboIdInstanced::kObjects:
      return 0;
    case VboIdInstanced::kBiomes:
      return 8 * 4 * 8;
    case VboIdInstanced::kTiles:
      return (8 * 4 + 8) * 8;
  }
}

inline UiData GetUiData(VboIdMain btn_type, VboIdText description) {
  return {static_cast<int>(btn_type), GetVboMainOffset(btn_type),
          GetVboTextOffset(description), 0};
}

inline UiData GetUiData(VboIdInstanced btn_type) {
  // TODO: description as 0?
  return {static_cast<int>(btn_type), GetVboInstancedOffset(btn_type), 0, 0};
}

// We put all button data to the same vbo buffer;
// here all data for ui inteface. We need only 4
// veftices for GL_DRAW TRIANGLE in order:
// right bottom - right top - left bottom - left top.
// Here for each button we need position and tex coord.
// Because we need it only in 2d, x;y is enought. (u,v fo tex coords)
// Here data represented in such order:
// - btn1 vertex1: x, y, u, v
// - btn1 vertex2: x, y, u, v
// - btn1 vertex3: x, y, u, v
// - btn1 vertex4: x, y, u, v
// - btn2 buttons data...
// This way kUiData can use the same vbo and shader, but
// in case of dynamic position component (like slider) we
// need to use another shader (vbo with starting position is the same)
extern const std::array<float, gVboIdSize * 16> kUiVboDataMain;

// tex coords were generated with https://github.com/pol-31/WireboundTextBaker;
extern const std::array<float, 402> kUiVboDataText;

/// here position instanced data (only for Modes "Objects", "Biomes", "Tiles")
extern const std::array<float, (8 * 4 + 8 + 6 * 6) * 8> kUiVboDataInstanced;

inline int GetTextVbo(std::string_view str) {
  //TODO: lookup by perf hash table
  throw;
}

inline int GetTextVbo(char c) {
  //TODO: lookup by perf hash table
  throw;
}

inline int GetTextWidth(int vbo_offset) {
  return kUiVboDataText[vbo_offset * 2]
         - kUiVboDataText[vbo_offset * 2 + 4];
}

inline int GetTextHeight(int vbo_offset) {
  return kUiVboDataText[vbo_offset * 2 + 3]
         - kUiVboDataText[vbo_offset * 2 + 1];
}

} // namespace vbos

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
