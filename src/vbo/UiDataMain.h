#ifndef WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAMAIN_H_
#define WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAMAIN_H_

#include <array>

enum class UiVboDataMainId;

inline consteval std::size_t GetUiDataMainOffset(UiVboDataMainId id);

/// related to kUiVboDataMain
enum class UiVboDataMainId {
  kModeTerrain,
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

  kUiSliderRadiusMin,
  kUiSliderRadiusMax,
  kUiSliderRadiusArea,
  kUiSliderRadiusSlider,
  kUiSliderFalloffMin,
  kUiSliderFalloffMax,
  kUiSliderFalloffArea,
  kUiSliderFalloffSlider,

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

  kPlacementWhite,
  kPlacementLightGrey,
  kPlacementGrey,
  kPlacementDarkGrey,
  kPlacementBlack,
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
};

inline consteval std::size_t GetUiDataMainOffset(UiVboDataMainId id) {
  // for each Ui component 16 floats
  // (4 2d points with position and tex coords)
  return static_cast<std::size_t>(id) * 16;
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
    1.0f, -0.1f, 0.8f, 0.875f,
    1.0f, 0.0f, 0.8f, 1.0f,
    0.9f, -0.1f, 0.7f, 0.875f,
    0.9f, 0.0f, 0.7f, 1.0f,

    // UiSlider for "area radius" icon for max
    1.0f, -1.0f, 0.7f, 0.125f,
    1.0f, -0.9f, 0.7f, 0.25f,
    0.9f, -1.0f, 0.6f, 0.125f,
    0.9f, -0.9f, 0.6f, 0.25f,

    // UiSlider for "area radius" icon "slider area" (where to slide on)
    1.0f, -0.9f, 0.7f, 0.75f,
    1.0f, -0.1f, 0.7f, 1.0f,
    0.9f, -0.9f, 0.6f, 0.75f,
    0.9f, -0.1f, 0.6f, 1.0f,

    // UiSlider for "area radius" icon "slider" (starting position)
    1.0f, -0.2f, 0.4f, 0.125f,
    1.0f, -0.1f, 0.4f, 0.25f,
    0.9f, -0.2f, 0.3f, 0.125f,
    0.9f, -0.1f, 0.3f, 0.25f,

    // UiSlider for "area falloff" icon for min
    0.9f, -0.1, 0.7f, 0.125f,
    0.9f, 0.0f, 0.7f, 0.25f,
    0.8f, -0.1, 0.6f, 0.125f,
    0.8f, 0.0f, 0.6f, 0.25f,

    // UiSlider for "area falloff" icon for max
    0.9f, -1.0f, 0.7f, 0.0f,
    0.9f, -0.9f, 0.7f, 0.125f,
    0.8f, -1.0f, 0.6f, 0.0f,
    0.8f, -0.9f, 0.6f, 0.125f,

    // UiSlider for "area falloff" icon "slider area" (where to slide on)
    0.9f, -0.9f, 0.7f, 0.75f,
    0.9f, -0.1f, 0.7f, 1.0f,
    0.8f, -0.9f, 0.6f, 0.75f,
    0.8f, -0.1f, 0.6f, 1.0f,

    // UiSlider for "area falloff" icon "slider" (starting position)
    0.9f, -0.2f, 0.4f, 0.125f,
    0.9f, -0.1f, 0.4f, 0.25f,
    0.8f, -0.2f, 0.3f, 0.125f,
    0.8f, -0.1f, 0.3f, 0.25f,

    // UiSlots "previous slots"
    0.9f, 0.9f, 0.4f, 0.0f,
    0.9f, 1.0f, 0.4f, 0.125f,
    0.8f, 0.9f, 0.3f, 0.0f,
    0.8f, 1.0f, 0.3f, 0.125f,

    // UiSlots "next slots"
    0.9f, -1.0f, 0.5f, 0.0f,
    0.9f, -0.9f, 0.5f, 0.125f,
    0.8f, -1.0f, 0.4f, 0.0f,
    0.8f, -0.9f, 0.4f, 0.125f,

    // UiSlots "slot 1"
    0.9f, 0.55f, 0.3f, 0.125f,
    0.9f, 0.85f, 0.3f, 0.25f,
    0.8f, 0.55f, 0.2f, 0.125f,
    0.8f, 0.85f, 0.2f, 0.25f,

    // UiSlots "slot 2"
    0.9f, 0.2f, 0.3f, 0.125f,
    0.9f, 0.5f, 0.3f, 0.25f,
    0.8f, 0.2f, 0.2f, 0.125f,
    0.8f, 0.5f, 0.2f, 0.25f,

    // UiSlots "slot 3"
    0.9f, -0.15f, 0.3f, 0.125f,
    0.9f, 0.15f, 0.3f, 0.25f,
    0.8f, -0.15f, 0.2f, 0.125f,
    0.8f, 0.15f, 0.2f, 0.25f,

    // UiSlots "slot 4"
    0.9f, -0.5f, 0.3f, 0.125f,
    0.9f, -0.2f, 0.3f, 0.25f,
    0.8f, -0.5f, 0.2f, 0.125f,
    0.8f, -0.2f, 0.2f, 0.25f,

    // UiSlots "slot 5"
    0.9f, -0.85f, 0.3f, 0.125f,
    0.9f, -0.55f, 0.3f, 0.25f,
    0.8f, -0.85f, 0.2f, 0.125f,
    0.8f, -0.55f, 0.2f, 0.25f,

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

    // mode Placement btn "white color"
    1.0f, 0.9f, 0.7f, 0.625f,
    1.0f, 1.0f, 0.7f, 0.75f,
    0.9f, 0.9f, 0.6f, 0.625f,
    0.9f, 1.0f, 0.6f, 0.75f,

    // mode Placement btn "light grey color"
    1.0f, 0.8f, 0.7f, 0.5f,
    1.0f, 0.9f, 0.7f, 0.625f,
    0.9f, 0.8f, 0.6f, 0.5f,
    0.9f, 0.9f, 0.6f, 0.625f,

    // mode Placement btn "grey color"
    1.0f, 0.7f, 0.7f, 0.375f,
    1.0f, 0.8f, 0.7f, 0.5f,
    0.9f, 0.7f, 0.6f, 0.375f,
    0.9f, 0.8f, 0.6f, 0.5f,

    // mode Placement btn "dark grey color"
    1.0f, 0.6f, 0.7f, 0.25f,
    1.0f, 0.7f, 0.7f, 0.375f,
    0.9f, 0.6f, 0.6f, 0.25f,
    0.9f, 0.7f, 0.6f, 0.375f,

    // mode Placement btn "black color"
    1.0f, 0.5f, 0.7f, 0.125f,
    1.0f, 0.6f, 0.7f, 0.25f,
    0.9f, 0.5f, 0.6f, 0.125f,
    0.9f, 0.6f, 0.6f, 0.25f,

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
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAMAIN_H_
