#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_

#include <array>
#include <cinttypes>

#include "Details.h"

struct UiData {
  int32_t id;
  std::size_t vbo_offset;
  /// comparing to Wirebound, here we add description for each button,
  /// which showed in some specific area externally.
  std::size_t text_vbo_offset_;
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

  kUiSliderSizeMin,
  kUiSliderSizeMax,
  kUiSliderSizeTrack,
  kUiSliderSizeHandler,
  kUiSliderFalloffMin,
  kUiSliderFalloffMax,
  kUiSliderFalloffTrack,
  kUiSliderFalloffHandler,

  kUiSlotsPrev,
  kUiSlotsNext,
  kUiSlots1,
  kUiSlots2,
  kUiSlots3,
  kUiSlots4,
  kUiSlots5,

  kTerrainSmooth,
  kTerrainUpdate,

  kWaterLake,
  kWaterRiver,
  kWaterWaterfall,
  kRoadsAsphalt,
  kRoadsGravel,
  kRoadsSoil,
  kFencesPicket,
  kFencesChainLink,
  kFencesWooden,

  kPlacementSliderColorWhite,
  kPlacementSliderColorBlack,
  kPlacementSliderTrack,
  kPlacementSliderHandler,
  kPlacementTrees,
  kPlacementBushes,
  kPlacementTallGrass,
  kPlacementUndergrowth,

  kObjectPageUp,
  kObjectPageDown,
  kSlot, // only tex_coords

  kBiomesPageUp,
  kBiomesPageDown,
  kBiomesSlot, // only tex_coords

  kTilesPageUp,
  kTilesPageDown,
  kTilesPageLeft,
  kTilesPageRight,

  kTilesPreview,
  kFullScreen, // for dgb
  kTotal,
};

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
          GetVboTextOffset(description)};
}

inline UiData GetUiData(VboIdInstanced btn_type) {
  // TODO: description as 0?
  return {static_cast<int>(btn_type), GetVboInstancedOffset(btn_type), 0};
}

namespace details {

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
inline constexpr std::array<float, 65 * 16> kUiVboDataMain = {
    // --- --- Menu --- ---

    // menu btn "Terrain Mode"
    -0.3f, 0.0f, 1.0f, 0.875f,
    -0.3f, 0.1f, 1.0f, 1.0f,
    -0.4f, 0.0f, 0.9f, 0.875f,
    -0.4f, 0.1f, 0.9f, 1.0f,

    // menu btn "Water Mode"
    -0.2f, 0.0f, 1.0f, 0.75f,
    -0.2f, 0.1f, 1.0f, 0.875,
    -0.3f, 0.0f, 0.9f, 0.75f,
    -0.3f, 0.1f, 0.9f, 0.875,

    // menu btn "Roads Mode"
    -0.1f, 0.0f, 1.0f, 0.625f,
    -0.1f, 0.1f, 1.0f, 0.75f,
    -0.2f, 0.0f, 0.9f, 0.625f,
    -0.2f, 0.1f, 0.9f, 0.75f,

    // menu btn "Fences Mode"
    0.0f, 0.0f, 1.0f, 0.5f,
    0.0f, 0.1f, 1.0f, 0.625f,
    -0.1f, 0.0f, 0.9f, 0.5f,
    -0.1f, 0.1f, 0.9f, 0.625f,

    // menu btn "Placement Mode"
    0.1f, 0.0f, 1.0f, 0.375f,
    0.1f, 0.1f, 1.0f, 0.5f,
    0.0f, 0.0f, 0.9f, 0.375f,
    0.0f, 0.1f, 0.9f, 0.5f,

    // menu btn "Objects Mode"
    0.2f, 0.0f, 1.0f, 0.25f,
    0.2f, 0.1f, 1.0f, 0.375f,
    0.1f, 0.0f, 0.9f, 0.25f,
    0.1f, 0.1f, 0.9f, 0.375f,

    // menu btn "Biomes Mode"
    0.3f, 0.0f, 1.0f, 0.125f,
    0.3f, 0.1f, 1.0f, 0.25f,
    0.2f, 0.0f, 0.9f, 0.125f,
    0.2f, 0.1f, 0.9f, 0.25f,

    // menu btn "Tiles Mode"
    0.4f, 0.0f, 1.0f, 0.0f,
    0.4f, 0.1f, 1.0f, 0.125f,
    0.3f, 0.0f, 0.9f, 0.0f,
    0.3f, 0.1f, 0.9f, 0.125f,

    // menu btn "Terrain Vision"
    -0.3f, -0.1f, 1.0f, 0.875f,
    -0.3f, 0.0f, 1.0f, 1.0f,
    -0.4f, -0.1f, 0.9f, 0.875f,
    -0.4f, 0.0f, 0.9f, 1.0f,

    // menu btn "Water Vision"
    -0.2f, -0.1f, 1.0f, 0.75f,
    -0.2f, 0.0f, 1.0f, 0.875,
    -0.3f, -0.1f, 0.9f, 0.75f,
    -0.3f, 0.0f, 0.9f, 0.875,

    // menu btn "Roads Vision"
    -0.1f, -0.1f, 1.0f, 0.625f,
    -0.1f, 0.0f, 1.0f, 0.75f,
    -0.2f, -0.1f, 0.9f, 0.625f,
    -0.2f, 0.0f, 0.9f, 0.75f,

    // menu btn "Fences Vision"
    0.0f, -0.1f, 1.0f, 0.5f,
    0.0f, 0.0f, 1.0f, 0.625f,
    -0.1f, -0.1f, 0.9f, 0.5f,
    -0.1f, 0.0f, 0.9f, 0.625f,

    // menu btn "Placement Vision"
    0.1f, -0.1f, 1.0f, 0.375f,
    0.1f, 0.0f, 1.0f, 0.5f,
    0.0f, -0.1f, 0.9f, 0.375f,
    0.0f, 0.0f, 0.9f, 0.5f,

    // menu btn "Objects Vision"
    0.2f, -0.1f, 1.0f, 0.25f,
    0.2f, 0.0f, 1.0f, 0.375f,
    0.1f, -0.1f, 0.9f, 0.25f,
    0.1f, 0.0f, 0.9f, 0.375f,

    // menu btn "Biomes Vision"
    0.3f, -0.1f, 1.0f, 0.125f,
    0.3f, 0.0f, 1.0f, 0.25f,
    0.2f, -0.1f, 0.9f, 0.125f,
    0.2f, 0.0f, 0.9f, 0.25f,

    // menu btn "Tiles Vision"
    0.4f, -0.1f, 1.0f, 0.0f,
    0.4f, 0.0f, 1.0f, 0.125f,
    0.3f, -0.1f, 0.9f, 0.0f,
    0.3f, 0.0f, 0.9f, 0.125f,

    // menu btn "render with Wirebound shaders" with Wirebound logo sprite
    -0.3f, -0.2f, 0.9f, 0.0f,
    -0.3f, -0.1f, 0.9f, 0.125f,
    -0.4f, -0.2f, 0.8f, 0.0f,
    -0.4f, -0.1f, 0.8f, 0.125f,

    // Some components are shared across few Modes, but
    // we keep the same screen position for them. Here they are:

    // --- --- Ui components (shared) --- ---

    // btn "Add New"
    1.0f, 0.9f, 0.5f, 0.125f,
    1.0f, 1.0f, 0.5f, 0.25f,
    0.9f, 0.9f, 0.4f, 0.125f,
    0.9f, 1.0f, 0.4f, 0.25f,

    // btn "Remove"
    1.0f, 0.8f, 0.6f, 0.125f,
    1.0f, 0.9f, 0.6f, 0.25f,
    0.9f, 0.8f, 0.5f, 0.125f,
    0.9f, 0.9f, 0.5f, 0.25f,

    // UiSlider for "area radius" icon for min
    1.0f, -0.2f, 0.7f, 0.125f,
    1.0f, -0.1f, 0.7f, 0.25f,
    0.9f, -0.2f, 0.6f, 0.125f,
    0.9f, -0.1f, 0.6f, 0.25f,

    // UiSlider for "area radius" icon for max
    1.0f, -1.0f, 0.8f, 0.875f,
    1.0f, -0.9f, 0.8f, 1.0f,
    0.9f, -1.0f, 0.7f, 0.875f,
    0.9f, -0.9f, 0.7f, 1.0f,

    // UiSlider for "area radius" icon "slider area" (where to slide on)
    1.0f, -0.85f, 0.7f, 0.85f,
    1.0f, -0.25f, 0.7f, 0.9f,
    0.9f, -0.85f, 0.6f, 0.85f,
    0.9f, -0.25f, 0.6f, 0.9f,

    // UiSlider for "area radius" icon "slider" (starting position)
    1.0f, -0.8f, 0.4f, 0.125f,
    1.0f, -0.9f, 0.4f, 0.25f,
    0.9f, -0.8f, 0.3f, 0.125f,
    0.9f, -0.9f, 0.3f, 0.25f,

    // UiSlider for "area falloff" icon for min
    0.9f, -0.2f, 0.7f, 0.125f,
    0.9f, -0.1f, 0.7f, 0.25f,
    0.8f, -0.2f, 0.6f, 0.125f,
    0.8f, -0.1f, 0.6f, 0.25f,

    // UiSlider for "area falloff" icon for max
    0.9f, -1.0f, 0.7f, 0.0f,
    0.9f, -0.9f, 0.7f, 0.125f,
    0.8f, -1.0f, 0.6f, 0.0f,
    0.8f, -0.9f, 0.6f, 0.125f,

    // UiSlider for "area falloff" icon "slider area" (where to slide on)
    0.9f, -0.85f, 0.7f, 0.85f,
    0.9f, -0.25f, 0.7f, 0.9f,
    0.8f, -0.85f, 0.6f, 0.85f,
    0.8f, -0.25f, 0.6f, 0.9f,

    // UiSlider for "area falloff" icon "slider" (starting position)
    0.9f, -0.8f, 0.4f, 0.125f,
    0.9f, -0.9f, 0.4f, 0.25f,
    0.8f, -0.8f, 0.3f, 0.125f,
    0.8f, -0.9f, 0.3f, 0.25f,

    // UiSlots "previous slots"
    0.9f, 0.4f, 0.4f, 0.0f,
    0.9f, 0.5f, 0.4f, 0.125f,
    0.8f, 0.4f, 0.3f, 0.0f,
    0.8f, 0.5f, 0.3f, 0.125f,

    // UiSlots "next slots"
    0.9f, -0.5f, 0.5f, 0.0f,
    0.9f, -0.4f, 0.5f, 0.125f,
    0.8f, -0.5f, 0.4f, 0.0f,
    0.8f, -0.4f, 0.4f, 0.125f,

    // UiSlots "slot 1"
    0.9f, 0.25f, 0.3f, 0.125f,
    0.9f, 0.35f, 0.3f, 0.25f,
    0.8f, 0.25f, 0.2f, 0.125f,
    0.8f, 0.35f, 0.2f, 0.25f,

    // UiSlots "slot 2"
    0.9f, 0.1f, 0.3f, 0.125f,
    0.9f, 0.2f, 0.3f, 0.25f,
    0.8f, 0.1f, 0.2f, 0.125f,
    0.8f, 0.2f, 0.2f, 0.25f,

    // UiSlots "slot 3"
    0.9f, -0.05f, 0.3f, 0.125f,
    0.9f, 0.05f, 0.3f, 0.25f,
    0.8f, -0.05f, 0.2f, 0.125f,
    0.8f, 0.05f, 0.2f, 0.25f,

    // UiSlots "slot 4"
    0.9f, -0.2f, 0.3f, 0.125f,
    0.9f, -0.1f, 0.3f, 0.25f,
    0.8f, -0.2f, 0.2f, 0.125f,
    0.8f, -0.1f, 0.2f, 0.25f,

    // UiSlots "slot 5"
    0.9f, -0.35f, 0.3f, 0.125f,
    0.9f, -0.25f, 0.3f, 0.25f,
    0.8f, -0.35f, 0.2f, 0.125f,
    0.8f, -0.25f, 0.2f, 0.25f,

    // --- --- Mode Terrain -specific --- ---

    // mode Terrain btn "Enable smooth mode"
    1.0f, 0.9f, 0.6f, 0.0f,
    1.0f, 1.0f, 0.6f, 0.125f,
    0.9f, 0.9f, 0.5f, 0.0f,
    0.9f, 1.0f, 0.5f, 0.125f,

    // mode Terrain btn "Update/Bake"
    1.0f, 0.8f, 0.1f, 0.0f,
    1.0f, 0.9f, 0.1f, 0.125f,
    0.9f, 0.8f, 0.0f, 0.0f,
    0.9f, 0.9f, 0.0f, 0.125f,

    // --- --- Mode Water -specific --- ---

    // mode Water btn "bake lake"
    1.0f, 0.7f, 0.9f, 0.375f,
    1.0f, 0.8f, 0.9f, 0.5f,
    0.9f, 0.7f, 0.8f, 0.375f,
    0.9f, 0.8f, 0.8f, 0.5f,

    // mode Water btn "bake river"
    1.0f, 0.6f, 0.9f, 0.25f,
    1.0f, 0.7f, 0.9f, 0.375f,
    0.9f, 0.6f, 0.8f, 0.25f,
    0.9f, 0.7f, 0.8f, 0.375f,

    // mode Water btn "bake waterfall"
    1.0f, 0.5f, 0.9f, 0.125f,
    1.0f, 0.6f, 0.9f, 0.25f,
    0.9f, 0.5f, 0.8f, 0.125f,
    0.9f, 0.6f, 0.8f, 0.25f,

    // --- --- Mode Roads -specific --- ---

    // mode Roads btn "bake asphalt road"
    1.0f, 0.7f, 0.8f, 0.625f,
    1.0f, 0.8f, 0.8f, 0.75f,
    0.9f, 0.7f, 0.7f, 0.625f,
    0.9f, 0.8f, 0.7f, 0.75f,

    // mode Roads btn "bake gravel road"
    1.0f, 0.6f, 0.8f, 0.5f,
    1.0f, 0.7f, 0.8f, 0.625f,
    0.9f, 0.6f, 0.7f, 0.5f,
    0.9f, 0.7f, 0.7f, 0.625f,

    // mode Roads btn "bake soil road"
    1.0f, 0.5f, 0.8f, 0.375f,
    1.0f, 0.6f, 0.8f, 0.5f,
    0.9f, 0.5f, 0.7f, 0.375f,
    0.9f, 0.6f, 0.7f, 0.5f,

    // --- --- Mode Fences -specific --- ---

    // mode Fences btn "bake picket fence"
    1.0f, 0.7f, 0.8f, 0.25f,
    1.0f, 0.8f, 0.8f, 0.375f,
    0.9f, 0.7f, 0.7f, 0.25f,
    0.9f, 0.8f, 0.7f, 0.375f,

    // mode Fences btn "bake chain link"
    1.0f, 0.6f, 0.8f, 0.125f,
    1.0f, 0.7f, 0.8f, 0.25f,
    0.9f, 0.6f, 0.7f, 0.125f,
    0.9f, 0.7f, 0.7f, 0.25f,

    // mode Fences btn "bake wooden fence"
    1.0f, 0.5f, 0.8f, 0.0f,
    1.0f, 0.6f, 0.8f, 0.125f,
    0.9f, 0.5f, 0.7f, 0.0f,
    0.9f, 0.6f, 0.7f, 0.125f,

    // --- --- Mode Placement -specific --- ---

    // mode Placement slider color white
    1.0f, 0.9f, 0.7f, 0.625f,
    1.0f, 1.0f, 0.7f, 0.75f,
    0.9f, 0.9f, 0.6f, 0.625f,
    0.9f, 1.0f, 0.6f, 0.75f,

    // mode Placement slider color black
    1.0f, 0.1f, 0.7f, 0.125f,
    1.0f, 0.2f, 0.7f, 0.25f,
    0.9f, 0.1f, 0.6f, 0.125f,
    0.9f, 0.2f, 0.6f, 0.25f,

    // mode Placement slider track
    1.0f, 0.25f, 0.7f, 0.85f,
    1.0f, 0.85f, 0.7f, 0.9f,
    0.9f, 0.25f, 0.6f, 0.85f,
    0.9f, 0.85f, 0.6f, 0.9f,

    // mode Placement slider handler
    1.0f, 0.2f, 0.4f, 0.125f,
    1.0f, 0.3f, 0.4f, 0.25f,
    0.9f, 0.2f, 0.3f, 0.125f,
    0.9f, 0.3f, 0.3f, 0.25f,

    // mode Placement btn "draw trees"
    0.9f, 0.9f, 0.9f, 0.5f,
    0.9f, 1.0f, 0.9f, 0.625f,
    0.8f, 0.9f, 0.8f, 0.5f,
    0.8f, 1.0f, 0.8f, 0.625f,

    // mode Placement btn "draw bushes"
    0.9f, 0.8f, 0.9f, 0.625f,
    0.9f, 0.9f, 0.9f, 0.75f,
    0.8f, 0.8f, 0.8f, 0.625f,
    0.8f, 0.9f, 0.8f, 0.75f,

    // mode Placement btn "draw tall grass"
    0.9f, 0.7f, 0.9f, 0.75f,
    0.9f, 0.8f, 0.9f, 0.875f,
    0.8f, 0.7f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.8f, 0.875f,

    // mode Placement btn "draw undergrowth"
    0.9f, 0.6f, 0.9f, 0.875f,
    0.9f, 0.7f, 0.9f, 1.0f,
    0.8f, 0.6f, 0.8f, 0.875f,
    0.8f, 0.7f, 0.8f, 1.0f,

    // --- --- Mode Objects -specific --- ---

    // mode Objects btn "page up"
    0.9f, 0.9f, 0.4f, 0.0f,
    0.9f, 1.0f, 0.4f, 0.125f,
    0.8f, 0.9f, 0.3f, 0.0f,
    0.8f, 1.0f, 0.3f, 0.125f,

    // mode Objects btn "page down"
    0.9f, -1.0f, 0.5f, 0.0f,
    0.9f, -0.9f, 0.5f, 0.125f,
    0.8f, -1.0f, 0.4f, 0.0f,
    0.8f, -0.9f, 0.4f, 0.125f,

    // mode Objects btn "slot"
    // (only tex_coords; positions in Data2 for instanced draw)
    0.0f, 0.0f, 0.3f, 0.125f,
    0.0f, 0.0f, 0.3f, 0.25f,
    0.0f, 0.0f, 0.2f, 0.125f,
    0.0f, 0.0f, 0.2f, 0.25f,

    // --- --- Mode Biomes -specific --- ---

    // mode Biomes btn "page up"
    0.9f, 0.9f, 0.4f, 0.0f,
    0.9f, 1.0f, 0.4f, 0.125f,
    0.8f, 0.9f, 0.3f, 0.0f,
    0.8f, 1.0f, 0.3f, 0.125f,

    // mode Biomes btn "page down"
    0.9f, -1.0f, 0.5f, 0.0f,
    0.9f, -0.9f, 0.5f, 0.125f,
    0.8f, -1.0f, 0.4f, 0.0f,
    0.8f, -0.9f, 0.4f, 0.125f,

    // mode Biomes btn "page down"
    // (only tex_coords; positions in Data2 for instanced draw)
    0.0f, 0.0f, 0.2f, 0.25f,
    0.0f, 0.0f, 0.2f, 0.125f,
    0.0f, 0.0f, 0.0f, 0.25f,
    0.0f, 0.0f, 0.0f, 0.125f,

    // --- --- Mode Tiles -specific --- ---

    // mode Tiles btn "page up"
    -0.6f, 0.9f, 0.4f, 0.0f,
    -0.6f, 1.0f, 0.4f, 0.125f,
    -0.7f, 0.9f, 0.3f, 0.0f,
    -0.7f, 1.0f, 0.3f, 0.125f,

    // mode Tiles btn "page down"
    -0.6f, -1.0f, 0.5f, 0.0f,
    -0.6f, -0.9f, 0.5f, 0.125f,
    -0.7f, -1.0f, 0.4f, 0.0f,
    -0.7f, -0.9f, 0.4f, 0.125f,

    // mode Tiles btn "page left"
    -0.9f, -0.05f, 0.2f, 0.0f,
    -0.9f, 0.05f, 0.2f, 0.125f,
    -1.0f, -0.05f, 0.1f, 0.0f,
    -1.0f, 0.05f, 0.1f, 0.125f,

    // mode Tiles btn "page right"
    -0.2f, -0.05f, 0.3f, 0.0f,
    -0.2f, 0.05f, 0.3f, 0.125f,
    -0.3f, -0.05f, 0.2f, 0.0f,
    -0.3f, 0.05f, 0.2f, 0.125f,

    // mode Tiles btn "tile preview"
    0.9f, -0.4f, 0.6f, 0.25f,
    0.9f, 0.4f, 0.6f, 1.0f,
    0.1f, -0.4f, 0.0f, 0.25f,
    0.1f, 0.4f, 0.0f, 1.0f,

    // for debug (render texture to full screen)
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
};

// tex coords were generated with https://github.com/pol-31/WireboundTextBaker;
inline constexpr std::array<float, 402> kUiVboDataText = {
    0.3456f, 0.1264f, 0.3456f, 0.1888f, 0.2688f, 0.1264f, 0.2688f, 0.1888f,
    0.8768f, 0.1888f, 0.8768f, 0.2512f, 0.7744f, 0.1888f, 0.7744f, 0.2512f,
    0.4288f, 0.1888f, 0.4288f, 0.2512f, 0.3008f, 0.1888f, 0.3008f, 0.2512f,
    0.9728f, 0.1888f, 0.9728f, 0.2512f, 0.8768f, 0.1888f, 0.8768f, 0.2512f,
    0.0960f, 0.1264f, 0.0960f, 0.1888f, 0.0f,    0.1264f, 0.0f,    0.1888f,
    0.5440f, 0.1888f, 0.5440f, 0.2512f, 0.4288f, 0.1888f, 0.4288f, 0.2512f,
    0.9984f, 0.6256f, 0.9984f, 0.6880f, 0.8320f, 0.6256f, 0.8320f, 0.6880f,
    0.6592f, 0.1888f, 0.6592f, 0.2512f, 0.5440f, 0.1888f, 0.5440f, 0.2512f,
    0.1856f, 0.1264f, 0.1856f, 0.1888f, 0.0960f, 0.1264f, 0.0960f, 0.1888f,
    0.2688f, 0.1264f, 0.2688f, 0.1888f, 0.1856f, 0.1264f, 0.1856f, 0.1888f,
    0.9952f, 0.3136f, 0.9952f, 0.3760f, 0.8608f, 0.3136f, 0.8608f, 0.3760f,
    0.5824f, 0.2512f, 0.5824f, 0.3136f, 0.4032f, 0.2512f, 0.4032f, 0.3136f,
    0.7744f, 0.1888f, 0.7744f, 0.2512f, 0.6592f, 0.1888f, 0.6592f, 0.2512f,
    0.6752f, 0.5008f, 0.6752f, 0.5632f, 0.3520f, 0.5008f, 0.3520f, 0.5632f,
    0.4032f, 0.2512f, 0.4032f, 0.3136f, 0.2016f, 0.2512f, 0.2016f, 0.3136f,
    0.5152f, 0.3760f, 0.5152f, 0.4384f, 0.2624f, 0.3760f, 0.2624f, 0.4384f,
    0.3008f, 0.1888f, 0.3008f, 0.2512f, 0.1664f, 0.1888f, 0.1664f, 0.2512f,
    0.9952f, 0.5632f, 0.9952f, 0.6256f, 0.7136f, 0.5632f, 0.7136f, 0.6256f,
    0.2624f, 0.3760f, 0.2624f, 0.4384f, 0.0f,    0.3760f, 0.0f,    0.4384f,
    0.5696f, 0.4384f, 0.5696f, 0.5008f, 0.2880f, 0.4384f, 0.2880f, 0.5008f,
    0.7136f, 0.5632f, 0.7136f, 0.6256f, 0.3616f, 0.5632f, 0.3616f, 0.6256f,
    0.9536f, 0.8752f, 0.9536f, 0.9376f, 0.5184f, 0.8752f, 0.5184f, 0.9376f,
    0.9856f, 0.9376f, 0.9856f, 1.0f,    0.5888f, 0.9376f, 0.5888f, 1.0f,
    0.3520f, 0.5008f, 0.3520f, 0.5632f, 0.0f,    0.5008f, 0.0f,    0.5632f,
    0.8320f, 0.6256f, 0.8320f, 0.6880f, 0.4160f, 0.6256f, 0.4160f, 0.6880f,
    0.5184f, 0.8128f, 0.5184f, 0.8752f, 0.0f,    0.8128f, 0.0f,    0.8752f,
    0.4224f, 0.6880f, 0.4224f, 0.7504f, 0.0f,    0.6880f, 0.0f,    0.7504f,
    0.9888f, 0.3760f, 0.9888f, 0.4384f, 0.7552f, 0.3760f, 0.7552f, 0.4384f,
    0.1664f, 0.1888f, 0.1664f, 0.2512f, 0.0f,    0.1888f, 0.0f,    0.2512f,
    0.8608f, 0.3136f, 0.8608f, 0.3760f, 0.6560f, 0.3136f, 0.6560f, 0.3760f,
    0.2208f, 0.3136f, 0.2208f, 0.3760f, 0.0f,    0.3136f, 0.0f,    0.3760f,
    0.2880f, 0.4384f, 0.2880f, 0.5008f, 0.0f,    0.4384f, 0.0f,    0.5008f,
    0.9920f, 0.5008f, 0.9920f, 0.5632f, 0.6752f, 0.5008f, 0.6752f, 0.5632f,
    0.9472f, 0.8128f, 0.9472f, 0.8752f, 0.5184f, 0.8128f, 0.5184f, 0.8752f,
    0.5184f, 0.8752f, 0.5184f, 0.9376f, 0.0f,    0.8752f, 0.0f,    0.9376f,
    0.4160f, 0.6256f, 0.4160f, 0.6880f, 0.0f,    0.6256f, 0.0f,    0.6880f,
    0.5120f, 0.7504f, 0.5120f, 0.8128f, 0.0f,    0.7504f, 0.0f,    0.8128f,
    0.9408f, 0.7504f, 0.9408f, 0.8128f, 0.5120f, 0.7504f, 0.5120f, 0.8128f,
    0.7552f, 0.3760f, 0.7552f, 0.4384f, 0.5152f, 0.3760f, 0.5152f, 0.4384f,
    0.7552f, 0.2512f, 0.7552f, 0.3136f, 0.5824f, 0.2512f, 0.5824f, 0.3136f,
    0.4416f, 0.3136f, 0.4416f, 0.3760f, 0.2208f, 0.3136f, 0.2208f, 0.3760f,
    0.9920f, 0.4384f, 0.9920f, 0.5008f, 0.8384f, 0.4384f, 0.8384f, 0.5008f,
    0.8384f, 0.4384f, 0.8384f, 0.5008f, 0.5696f, 0.4384f, 0.5696f, 0.5008f,
    0.2016f, 0.2512f, 0.2016f, 0.3136f, 0.0f,    0.2512f, 0.0f,    0.3136f,
    0.6560f, 0.3136f, 0.6560f, 0.3760f, 0.4416f, 0.3136f, 0.4416f, 0.3760f,
    0.3616f, 0.5632f, 0.3616f, 0.6256f, 0.0f,    0.5632f, 0.0f,    0.6256f,
    0.5888f, 0.9376f, 0.5888f, 1.0f,    0.0f,    0.9376f, 0.0f,    1.0f,
    0.9984f, 0.6880f, 0.9984f, 0.7504f, 0.8416f, 0.6880f, 0.8416f, 0.7504f,
    0.9248f, 0.2512f, 0.9248f, 0.3136f, 0.7552f, 0.2512f, 0.7552f, 0.3136f,
    0.8416f, 0.6880f, 0.8416f, 0.7504f, 0.4224f, 0.6880f, 0.4224f, 0.7504f,
};

/// here position instanced data (only for Modes "Objects", "Biomes", "Tiles")
inline constexpr std::array<float, (8 * 4 + 8 + 6 * 6) * 8>
    kUiVboDataInstanced = {
        // Objects slots (8 x 4)
        0.4f, 0.6f, 0.4f, 0.8f, 0.2f, 0.6f, 0.2f, 0.8f,
        0.6f, 0.6f, 0.6f, 0.8f, 0.4f, 0.6f, 0.4f, 0.8f,
        0.8f, 0.6f, 0.8f, 0.8f, 0.6f, 0.6f, 0.6f, 0.8f,
        1.0f, 0.6f, 1.0f, 0.8f, 0.8f, 0.6f, 0.8f, 0.8f,

        0.4f, 0.4f, 0.4f, 0.6f, 0.2f, 0.4f, 0.2f, 0.6f,
        0.6f, 0.4f, 0.6f, 0.6f, 0.4f, 0.4f, 0.4f, 0.6f,
        0.8f, 0.4f, 0.8f, 0.6f, 0.6f, 0.4f, 0.6f, 0.6f,
        1.0f, 0.4f, 1.0f, 0.6f, 0.8f, 0.4f, 0.8f, 0.6f,

        0.4f, 0.2, 0.4f, 0.4f, 0.2f, 0.2f, 0.2f, 0.4f,
        0.6f, 0.2, 0.6f, 0.4f, 0.4f, 0.2f, 0.4f, 0.4f,
        0.8f, 0.2, 0.8f, 0.4f, 0.6f, 0.2f, 0.6f, 0.4f,
        1.0f, 0.2, 1.0f, 0.4f, 0.8f, 0.2f, 0.8f, 0.4f,

        0.4f, 0.0f, 0.4f, 0.2f, 0.2f, 0.0f, 0.2f, 0.2f,
        0.6f, 0.0f, 0.6f, 0.2f, 0.4f, 0.0f, 0.4f, 0.2f,
        0.8f, 0.0f, 0.8f, 0.2f, 0.6f, 0.0f, 0.6f, 0.2f,
        1.0f, 0.0f, 1.0f, 0.2f, 0.8f, 0.0f, 0.8f, 0.2f,

        0.4f, -0.2f, 0.4f, 0.0f, 0.2f, -0.2f, 0.2f, 0.0f,
        0.6f, -0.2f, 0.6f, 0.0f, 0.4f, -0.2f, 0.4f, 0.0f,
        0.8f, -0.2f, 0.8f, 0.0f, 0.6f, -0.2f, 0.6f, 0.0f,
        1.0f, -0.2f, 1.0f, 0.0f, 0.8f, -0.2f, 0.8f, 0.0f,

        0.4f, -0.4f, 0.4f, -0.2f, 0.2f, -0.4f, 0.2f, -0.2f,
        0.6f, -0.4f, 0.6f, -0.2f, 0.4f, -0.4f, 0.4f, -0.2f,
        0.8f, -0.4f, 0.8f, -0.2f, 0.6f, -0.4f, 0.6f, -0.2f,
        1.0f, -0.4f, 1.0f, -0.2f, 0.8f, -0.4f, 0.8f, -0.2f,

        0.4f, -0.6f, 0.4f, -0.4f, 0.2f, -0.6f, 0.2f, -0.4f,
        0.6f, -0.6f, 0.6f, -0.4f, 0.4f, -0.6f, 0.4f, -0.4f,
        0.8f, -0.6f, 0.8f, -0.4f, 0.6f, -0.6f, 0.6f, -0.4f,
        1.0f, -0.6f, 1.0f, -0.4f, 0.8f, -0.6f, 0.8f, -0.4f,

        0.4f, -0.8f, 0.4f, -0.6f, 0.2f, -0.8f, 0.2f, -0.6f,
        0.6f, -0.8f, 0.6f, -0.6f, 0.4f, -0.8f, 0.4f, -0.6f,
        0.8f, -0.8f, 0.8f, -0.6f, 0.6f, -0.8f, 0.6f, -0.6f,
        1.0f, -0.8f, 1.0f, -0.6f, 0.8f, -0.8f, 0.8f, -0.6f,

        // Biomes slots (8)
        1.0f, 0.6f, 1.0f, 0.8f, 0.2f, 0.6f, 0.2f, 0.8f,
        1.0f, 0.4f, 1.0f, 0.6f, 0.2f, 0.4f, 0.2f, 0.6f,
        1.0f, 0.2, 1.0f, 0.4f, 0.2f, 0.2f, 0.2f, 0.4f,
        1.0f, 0.0f, 1.0f, 0.2f, 0.2f, 0.0f, 0.2f, 0.2f,

        1.0f, -0.2f, 1.0f, 0.0f, 0.2f, -0.2f, 0.2f, 0.0f,
        1.0f, -0.4f, 1.0f, -0.2f, 0.2f, -0.4f, 0.2f, -0.2f,
        1.0f, -0.6f, 1.0f, -0.4f, 0.2f, -0.6f, 0.2f, -0.4f,
        1.0f, -0.8f, 1.0f, -0.6f, 0.2f, -0.8f, 0.2f, -0.6f,

        // Tiles slots (6 x 6)
        -0.8f, 0.2f, -0.8f, 0.3f, -0.9f, 0.2f, -0.9f, 0.3f,
        -0.7f, 0.2f, -0.7f, 0.3f, -0.8f, 0.2f, -0.8f, 0.3f,
        -0.6f, 0.2f, -0.6f, 0.3f, -0.7f, 0.2f, -0.7f, 0.3f,
        -0.5f, 0.2f, -0.5f, 0.3f, -0.6f, 0.2f, -0.6f, 0.3f,
        -0.4f, 0.2f, -0.4f, 0.3f, -0.5f, 0.2f, -0.5f, 0.3f,
        -0.3f, 0.2f, -0.3f, 0.3f, -0.4f, 0.2f, -0.4f, 0.3f,

        -0.8f, 0.1f, -0.8f, 0.2f, -0.9f, 0.1f, -0.9f, 0.2f,
        -0.7f, 0.1f, -0.7f, 0.2f, -0.8f, 0.1f, -0.8f, 0.2f,
        -0.6f, 0.1f, -0.6f, 0.2f, -0.7f, 0.1f, -0.7f, 0.2f,
        -0.5f, 0.1f, -0.5f, 0.2f, -0.6f, 0.1f, -0.6f, 0.2f,
        -0.4f, 0.1f, -0.4f, 0.2f, -0.5f, 0.1f, -0.5f, 0.2f,
        -0.3f, 0.1f, -0.3f, 0.2f, -0.4f, 0.1f, -0.4f, 0.2f,

        -0.8f, 0.0f, -0.8f, 0.1f, -0.9f, 0.0f, -0.9f, 0.1f,
        -0.7f, 0.0f, -0.7f, 0.1f, -0.8f, 0.0f, -0.8f, 0.1f,
        -0.6f, 0.0f, -0.6f, 0.1f, -0.7f, 0.0f, -0.7f, 0.1f,
        -0.5f, 0.0f, -0.5f, 0.1f, -0.6f, 0.0f, -0.6f, 0.1f,
        -0.4f, 0.0f, -0.4f, 0.1f, -0.5f, 0.0f, -0.5f, 0.1f,
        -0.3f, 0.0f, -0.3f, 0.1f, -0.4f, 0.0f, -0.4f, 0.1f,

        -0.8f, -0.1f, -0.8f, 0.0f, -0.9f, -0.1f, -0.9f, 0.0f,
        -0.7f, -0.1f, -0.7f, 0.0f, -0.8f, -0.1f, -0.8f, 0.0f,
        -0.6f, -0.1f, -0.6f, 0.0f, -0.7f, -0.1f, -0.7f, 0.0f,
        -0.5f, -0.1f, -0.5f, 0.0f, -0.6f, -0.1f, -0.6f, 0.0f,
        -0.4f, -0.1f, -0.4f, 0.0f, -0.5f, -0.1f, -0.5f, 0.0f,
        -0.3f, -0.1f, -0.3f, 0.0f, -0.4f, -0.1f, -0.4f, 0.0f,

        -0.8f, -0.2f, -0.8f, -0.1f, -0.9f, -0.2f, -0.9f, -0.1f,
        -0.7f, -0.2f, -0.7f, -0.1f, -0.8f, -0.2f, -0.8f, -0.1f,
        -0.6f, -0.2f, -0.6f, -0.1f, -0.7f, -0.2f, -0.7f, -0.1f,
        -0.5f, -0.2f, -0.5f, -0.1f, -0.6f, -0.2f, -0.6f, -0.1f,
        -0.4f, -0.2f, -0.4f, -0.1f, -0.5f, -0.2f, -0.5f, -0.1f,
        -0.3f, -0.2f, -0.3f, -0.1f, -0.4f, -0.2f, -0.4f, -0.1f,

        -0.8f, -0.3f, -0.8f, -0.2f, -0.9f, -0.3f, -0.9f, -0.2f,
        -0.7f, -0.3f, -0.7f, -0.2f, -0.8f, -0.3f, -0.8f, -0.2f,
        -0.6f, -0.3f, -0.6f, -0.2f, -0.7f, -0.3f, -0.7f, -0.2f,
        -0.5f, -0.3f, -0.5f, -0.2f, -0.6f, -0.3f, -0.6f, -0.2f,
        -0.4f, -0.3f, -0.4f, -0.2f, -0.5f, -0.3f, -0.5f, -0.2f,
        -0.3f, -0.3f, -0.3f, -0.2f, -0.4f, -0.3f, -0.4f, -0.2f,
};

} // namespace details

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
