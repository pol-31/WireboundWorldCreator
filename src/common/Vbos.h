#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_

#include <array>
#include <cinttypes>

#include "Details.h"
#include "Text.h"

class UiTransformDbg;

namespace data {

struct UiData {
  int32_t id;
  std::size_t vbo_offset;
  data::TextId text_id;

  // parent id to take transform from it
  std::size_t parent_id_{0};

  // useful for e.g. slider to update centre/length of interactive area
  UiTransformDbg* ui{nullptr};
};

/// related to kUiVboDataMain
enum class VboIdMain {
  kMenuTerrain = details::kIdOffsetUi,
  kMenuWater,
  kMenuRoads,
  kMenuFences,
  kMenuPlacement,
  kMenuObjects,
  kMenuBiomes,
  kMenuTiles,
  kMenuCross,


  kMenuTerrainOff,
  kMenuTerrainOn1,
  kMenuTerrainOn2,
  kMenuTerrainOn3,

  kMenuWaterOff,
  kMenuWaterOn1,
  kMenuWaterOn2,
  kMenuWaterOn3,

  kMenuRoadsOff,
  kMenuRoadsOn1,
  kMenuRoadsOn2,
  kMenuRoadsOn3,

  kMenuFencesOff,
  kMenuFencesOn1,
  kMenuFencesOn2,
  kMenuFencesOn3,

  kMenuPlacementOff,
  kMenuPlacementOn1,
  kMenuPlacementOn2,
  kMenuPlacementOn3,

  kMenuObjectsOff,
  kMenuObjectsOn1,
  kMenuObjectsOn2,
  kMenuObjectsOn3,

  kMenuBiomesOff,
  kMenuBiomesOn1,
  kMenuBiomesOn2,
  kMenuBiomesOn3,

  kMenuTilesOff,
  kMenuTilesOn1,
  kMenuTilesOn2,
  kMenuTilesOn3,


  kMenuShaderWirebound,
  kMenuShadersOff,
  kMenuShadersOn1,
  kMenuShadersOn2,
  kMenuShadersOn3,

  kMenuSettings,


  kSettingsCross,
  kSettingsSettings,

  kSettingsResolutionFill,
  kSettingsResolutionBack,
  kSettingsResolutionIcon,

  kSettingsSoundFill,
  kSettingsSoundBack,
  kSettingsSoundIcon,

  kSettingsMusicFill,
  kSettingsMusicBack,
  kSettingsMusicIcon,

  kSettingsSensitivityFill,
  kSettingsSensitivityBack,
  kSettingsSensitivityIcon,

  kSettingsKeyboard,

  kSettingsSoundOff,
  kSettingsSoundOn1,
  kSettingsSoundOn2,
  kSettingsSoundOn3,

  kSettingsMusicOff,
  kSettingsMusicOn1,
  kSettingsMusicOn2,
  kSettingsMusicOn3,

  kCross,
  kLoading0,
  kLoading1,
  kLoading2,
  kLoading3,
  kLoading4,
  kLoading5,
  kLoading6,
  kLoading7,
  kLoading8,
  kLoading9,
  kLoading10,

  kTerrainFlatten,

  kTerrainFlattenOff,
  kTerrainFlattenOn1,
  kTerrainFlattenOn2,
  kTerrainFlattenOn3,

  kTerrainUiTerrainMode,
  kTerrainUpdate,
  kTerrainSettings,
  kTerrainSettingsDesk,
  kTerrainRegenerate,
  kTerrainSizeFill,
  kTerrainSizeBack,
  kTerrainSizeIcon,
  kTerrainFalloffFill,
  kTerrainFalloffBack,
  kTerrainFalloffIcon,

  kWaterUiWaterMode,
  kWaterAdd,
  kWaterRemove,
  kWaterUpdate,
  kWaterLake,
  kWaterRiver,
  kWaterWaterfall,

  kWaterSlotsSlot,
  kWaterSlotsRemove,
  kWaterSlotsSelected,
  kWaterSlotsHandler,
  kWaterSlotsSlider,
  kWaterSlotsBack,
  kWaterSlotsCreate,
  kWaterSlotsFlipPointEdge_Back,
  kWaterSlotsFlipSelectEdit_Back,
  kWaterSlotsFlipPointEdge,
  kWaterSlotsFlipSelectEdit,


  kWater1ScaleFill,
  kWater1ScaleBack,
  kWater1ScaleHandler,
  kWater1ScaleIcon,

  kWater1FetchFill,
  kWater1FetchBack,
  kWater1FetchHandler,
  kWater1FetchIcon,

  kWater1SpreadBlendFill,
  kWater1SpreadBlendBack,
  kWater1SpreadBlendHandler,
  kWater1SpreadBlendIcon,

  kWater1SwellFill,
  kWater1SwellBack,
  kWater1SwellHandler,
  kWater1SwellIcon,

  kWater1PeakEnhancementFill,
  kWater1PeakEnhancementBack,
  kWater1PeakEnhancementHandler,
  kWater1PeakEnhancementIcon,

  kWater1ShortWavesFadeFill,
  kWater1ShortWavesFadeBack,
  kWater1ShortWavesFadeHandler,
  kWater1ShortWavesFadeIcon,

  kWater1LambdaFill,
  kWater1LambdaBack,
  kWater1LambdaHandler,
  kWater1LambdaIcon,



  kWater2ScaleFill,
  kWater2ScaleBack,
  kWater2ScaleHandler,
  kWater2ScaleIcon,

  kWater2FetchFill,
  kWater2FetchBack,
  kWater2FetchHandler,
  kWater2FetchIcon,

  kWater2SpreadBlendFill,
  kWater2SpreadBlendBack,
  kWater2SpreadBlendHandler,
  kWater2SpreadBlendIcon,

  kWater2SwellFill,
  kWater2SwellBack,
  kWater2SwellHandler,
  kWater2SwellIcon,

  kWater2PeakEnhancementFill,
  kWater2PeakEnhancementBack,
  kWater2PeakEnhancementHandler,
  kWater2PeakEnhancementIcon,

  kWater2ShortWavesFadeFill,
  kWater2ShortWavesFadeBack,
  kWater2ShortWavesFadeHandler,
  kWater2ShortWavesFadeIcon,

  kWater2LambdaFill,
  kWater2LambdaBack,
  kWater2LambdaHandler,
  kWater2LambdaIcon,



  kWater3ScaleFill,
  kWater3ScaleBack,
  kWater3ScaleHandler,
  kWater3ScaleIcon,

  kWater3FetchFill,
  kWater3FetchBack,
  kWater3FetchHandler,
  kWater3FetchIcon,

  kWater3SpreadBlendFill,
  kWater3SpreadBlendBack,
  kWater3SpreadBlendHandler,
  kWater3SpreadBlendIcon,

  kWater3SwellFill,
  kWater3SwellBack,
  kWater3SwellHandler,
  kWater3SwellIcon,

  kWater3PeakEnhancementFill,
  kWater3PeakEnhancementBack,
  kWater3PeakEnhancementHandler,
  kWater3PeakEnhancementIcon,

  kWater3ShortWavesFadeFill,
  kWater3ShortWavesFadeBack,
  kWater3ShortWavesFadeHandler,
  kWater3ShortWavesFadeIcon,

  kWater3LambdaFill,
  kWater3LambdaBack,
  kWater3LambdaHandler,
  kWater3LambdaIcon,


  kWaterSettings,
  kWaterSettingsDesk,
  kWaterLayer1,
  kWaterLayer1Window,
  kWaterLayer2,
  kWaterLayer2Window,
  kWaterLayer3,
  kWaterLayer3Window,


  kRoadsUiRoadsMode,
  kRoadsAsphalt,
  kRoadsGravel,
  kRoadsSoil,
  kRoadsAdd,
  kRoadsRemove,
  kRoadsUpdate,

  kRoadsSlotsSlot,
  kRoadsSlotsRemove,
  kRoadsSlotsSelected,
  kRoadsSlotsHandler,
  kRoadsSlotsSlider,
  kRoadsSlotsBack,
  kRoadsSlotsCreate,
  kRoadsSlotsFlipPointEdge_Back,
  kRoadsSlotsFlipSelectEdit_Back,
  kRoadsSlotsFlipPointEdge,
  kRoadsSlotsFlipSelectEdit,


  kFencesUiFencesMode,
  kFencesPicket,
  kFencesChainLink,
  kFencesWooden,
  kFencesAdd,
  kFencesRemove,
  kFencesUpdate,

  kFencesSlotsSlot,
  kFencesSlotsRemove,
  kFencesSlotsSelected,
  kFencesSlotsHandler,
  kFencesSlotsSlider,
  kFencesSlotsBack,
  kFencesSlotsCreate,
  kFencesSlotsFlipPointEdge_Back,
  kFencesSlotsFlipSelectEdit_Back,
  kFencesSlotsFlipPointEdge,
  kFencesSlotsFlipSelectEdit,


  kPlacementUiPlacementMode,
  kPlacementColorFill,
  kPlacementColorBack,
  kPlacementColorIcon,

  kPlacementSizeFill,
  kPlacementSizeBack,
  kPlacementSizeIcon,

  kPlacementFalloffFill,
  kPlacementFalloffBack,
  kPlacementFalloffIcon,

  kPlacementTrees,
  kPlacementBushes,
  kPlacementTallGrass,
  kPlacementUndergrowth,

  kPlacementChangeMode,

  kPlacementDrawOff,
  kPlacementDrawOn1,
  kPlacementDrawOn2,
  kPlacementDrawOn3,


  kObjectsUiObjectsMode,
  kObjectsMavka,
  kObjectsVodyaniy,
  kObjectsChugaister,
  kObjectsPedestal,
  kObjectsCampfire,
  kObjectsRoadSign,
  kObjectsHuman,

  kBiomesUiBiomesMode,
  kBiomesWind,
  kBiomesSun,
  kBiomesTime,
  kBiomesPrecipitations,
  kBiomesTemperature,
  kBiomesClouds,

  kTilesUiTilesMode,
  kTilesLeft,
  kTilesRight,
  kTilesUp,
  kTilesDown,

  kMapArrow,
  kMapDot,
  kMapLine,
  kMapTarget,
  kMapX,

  kConfirmationClose,
  kConfirmationAccept,
  kConfirmationDecline,
  kConfirmationCross,

  kAcceptDeclineDesk,
  kConfigDesk,
  kTabDesk,

  kFullScreen,
  kTotal,
};

inline constexpr int gVboIdSize =
    static_cast<int>(VboIdMain::kTotal)
    - static_cast<int>(VboIdMain::kMenuTerrain);

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

inline constexpr int gUiVboTransformSize = gVboIdSize * 4 * 3;

extern const std::array<float, gUiVboTransformSize> kUiVboDataTransform;

#endif // NDEBUG

inline constexpr std::size_t GetVboMainOffset(VboIdMain id) {
  // for each Ui component 16 floats
  // (4 2d points with position and tex coords)
  return (static_cast<std::size_t>(id) - (details::kIdOffsetUi)) * 4; // TODO: 4?
}

inline UiData GetUiData(VboIdMain btn_type, data::TextId text_id) {
  return {static_cast<int>(btn_type), GetVboMainOffset(btn_type), text_id, 0};
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

} // namespace vbos

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
