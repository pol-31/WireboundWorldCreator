#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_VBOS_H_

#include <array>
#include <cinttypes>

#include "Details.h"
#include "Text.h"

class UiBase;

namespace data {

struct UiData {
  int32_t id;
  std::size_t vbo_offset;

  // parent id to take transform from it
  std::size_t parent_id_{0};

  // useful for e.g. slider to update centre/length of interactive area
  UiBase* ui = nullptr;
};

/// related to kUiVboDataMain
enum class VboIdMain {
  kMenuTerrain = details::kIdOffsetUi,
  kMenuTerrainOff,
  kMenuTerrainOn1,
  kMenuTerrainOn2,
  kMenuTerrainOn3,

  kMenuWater,
  kMenuWaterOff,
  kMenuWaterOn1,
  kMenuWaterOn2,
  kMenuWaterOn3,

  kMenuRoads,
  kMenuRoadsOff,
  kMenuRoadsOn1,
  kMenuRoadsOn2,
  kMenuRoadsOn3,

  kMenuFences,
  kMenuFencesOff,
  kMenuFencesOn1,
  kMenuFencesOn2,
  kMenuFencesOn3,

  kMenuPlacement,
  kMenuPlacementOff,
  kMenuPlacementOn1,
  kMenuPlacementOn2,
  kMenuPlacementOn3,

  kMenuObjects,
  kMenuObjectsOff,
  kMenuObjectsOn1,
  kMenuObjectsOn2,
  kMenuObjectsOn3,

  kMenuBiomes,
  kMenuBiomesOff,
  kMenuBiomesOn1,
  kMenuBiomesOn2,
  kMenuBiomesOn3,

  kMenuTiles,
  kMenuTilesOff,
  kMenuTilesOn1,
  kMenuTilesOn2,
  kMenuTilesOn3,

  kMenuPlayer,
  kMenuPlayerOff,
  kMenuPlayerOn1,
  kMenuPlayerOn2,
  kMenuPlayerOn3,


  kMenuShaderWirebound,
  kMenuShadersOff,
  kMenuShadersOn1,
  kMenuShadersOn2,
  kMenuShadersOn3,

  kMenuDesk,
  kMenuDeskPinBack,
  kMenuDeskPinPoint,
  kMenuArrowSelect,
  kMenuArrowSelected,

  kMenuSave,
  kMenuLoad,

  kModeModeText,

  kPressedKey,

  kSettingsDesk,
  kSettingsDeskPinBack,
  kSettingsDeskPinPoint,

  kSettingsResolutionLabel,
  kSettingsResolutionLeft,
  kSettingsResolutionRight,
  kSettingsResolution,
  kSettingsFullScreenOff,
  kSettingsFullScreenOn1,
  kSettingsFullScreenOn2,
  kSettingsFullScreenOn3,

  kSettingsSensitivityArea,
  kSettingsSensitivityHandle,

  kSettingsSoundArea,
  kSettingsSoundHandle,
  kSettingsSoundOff,
  kSettingsSoundOn1,
  kSettingsSoundOn2,
  kSettingsSoundOn3,

  kSettingsMusicArea,
  kSettingsMusicHandle,
  kSettingsMusicOff,
  kSettingsMusicOn1,
  kSettingsMusicOn2,
  kSettingsMusicOn3,

  kSettingsTipInfoLabel,
  kSettingsTipInfo,
  kSettingsTipInfoOff,
  kSettingsTipInfoOn1,
  kSettingsTipInfoOn2,
  kSettingsTipInfoOn3,

  kModeTextBack,
  kModeTextLabel,

  kTerrainTerrainMode,
  kTerrainUpdate,
  kTerrainFlatten,
  kTerrainFlattenOff,
  kTerrainFlattenOn1,
  kTerrainFlattenOn2,
  kTerrainFlattenOn3,

  kTerrainFalloffFill,
  kTerrainFalloffBack,
  kTerrainFalloffIcon,

  kTerrainBake,
  kTerrainReset,

  kTerrainBakeDesk,
  kTerrainBakeDeskPinBack,
  kTerrainBakeDeskPinPoint,
  kTerrainBakeAccept,
  kTerrainBakeErosionStepLabel,
  kTerrainBakeErosionStepInputText,
  kTerrainBakeErosionStepInputBack,
  kTerrainBakeWeatheringStepLabel,
  kTerrainBakeWeatheringStepInputText,
  kTerrainBakeWeatheringStepInputBack,
  kTerrainBakeHmap,
  kTerrainBakeHmapName,

  kTerrainEditDesk,
  kTerrainEditDeskPinBack,
  kTerrainEditDeskPinPoint,
  kTerrainEditAccept,
  kTerrainEditName,
  kTerrainEditNameBack,
  kTerrainColorPaletteHS,
  kTerrainColorPaletteB,
  kTerrainColorIndicator,
  kTerrainColorBrightnessCursor,
  kTerrainColorColorCursor,
  kTerrainEditHeightmap,
  kTerrainEditRandomGeneration,

  kTerrainEditLabelInvert,
  kTerrainEditLabelTiling,
  kTerrainEditLabelStrength,

  kTerrainEditTranslateLabel,
  kTerrainEditTranslateXText,
  kTerrainEditTranslateXTextSlot,
  kTerrainEditTranslateYText,
  kTerrainEditTranslateYTextSlot,
  kTerrainEditTranslateZText,
  kTerrainEditTranslateZTextSlot,

  kTerrainEditScaleLabel,
  kTerrainEditScaleXText,
  kTerrainEditScaleXTextSlot,
  kTerrainEditScaleYText,
  kTerrainEditScaleYTextSlot,
  kTerrainEditScaleZText,
  kTerrainEditScaleZTextSlot,

  kTerrainEditRotateLabel,
  kTerrainEditRotateXText,
  kTerrainEditRotateXTextSlot,
  kTerrainEditRotateYText,
  kTerrainEditRotateYTextSlot,
  kTerrainEditRotateZText,
  kTerrainEditRotateZTextSlot,

  kTerrainEditNoiseConfig,
  kTerrainEditNoiseName,
  kTerrainEditNoiseInvertOff,
  kTerrainEditNoiseInvertOn1,
  kTerrainEditNoiseInvertOn2,
  kTerrainEditNoiseInvertOn3,
  kTerrainEditNoiseTilingOff,
  kTerrainEditNoiseTilingOn1,
  kTerrainEditNoiseTilingOn2,
  kTerrainEditNoiseTilingOn3,
  kTerrainEditNoiseStrengthArea,
  kTerrainEditNoiseStrengthIcon,
  kTerrainEditNoiseHmap,


  kWaterWaterMode,
  kWaterUpdate,
  kWaterOcean,
  kWaterRiver,
  kWaterSelected,

  kWaterEditDesk,
  kWaterEditDeskPinBack,
  kWaterEditDeskPinPoint,
  kWaterEditAccept,
  kWaterEditName,
  kWaterEditNameBack,
  kWaterColorPaletteHS,
  kWaterColorPaletteB,
  kWaterColorIndicator,
  kWaterColorBrightnessCursor,
  kWaterColorColorCursor,
  kWaterEditRandomGeneration,

  kWaterEditRiverViscosityText,
  kWaterEditRiverViscosityArea,
  kWaterEditRiverViscosityIcon,
  kWaterEditRiverTransparencyText,
  kWaterEditRiverTransparencyArea,
  kWaterEditRiverTransparencyIcon,

  kWaterEditOceanVisibleOff,
  kWaterEditOceanVisibleOn1,
  kWaterEditOceanVisibleOn2,
  kWaterEditOceanVisibleOn3,
  kWaterEditOceanConfig,
  kWaterEditOceanLayerName,


  kPlacementPlacementMode,

  kPlacementTrees,
  kPlacementTreesOff,
  kPlacementTreesOn1,
  kPlacementTreesOn2,
  kPlacementTreesOn3,

  kPlacementBushes,
  kPlacementBushesOff,
  kPlacementBushesOn1,
  kPlacementBushesOn2,
  kPlacementBushesOn3,

  kPlacementTallGrass,
  kPlacementTallGrassOff,
  kPlacementTallGrassOn1,
  kPlacementTallGrassOn2,
  kPlacementTallGrassOn3,

  kPlacementUndergrowth,
  kPlacementUndergrowthOff,
  kPlacementUndergrowthOn1,
  kPlacementUndergrowthOn2,
  kPlacementUndergrowthOn3,

  kPlacementAsphalt,
  kPlacementAsphaltOff,
  kPlacementAsphaltOn1,
  kPlacementAsphaltOn2,
  kPlacementAsphaltOn3,

  kPlacemenGravel,
  kPlacementGravelOff,
  kPlacementGravelOn1,
  kPlacementGravelOn2,
  kPlacementGravelOn3,

  kPlacementSoil,
  kPlacementSoilOff,
  kPlacementSoilOn1,
  kPlacementSoilOn2,
  kPlacementSoilOn3,

  kPlacementSelected,

  kPlacementChangeMode,
  kPlacementDrawOff,
  kPlacementDrawOn1,
  kPlacementDrawOn2,
  kPlacementDrawOn3,


  kObjectsObjectsMode,
  kObjectsEditDesk,
  kObjectsEditDeskPinBack,
  kObjectsEditDeskPinPoint,
  kObjectsEditAccept,
  kObjectsEditName,
  kObjectsEditNameBack,
  kObjectsEditRandomGeneration,

  kObjectsEditModelName,
  kObjectsEditCharacteristicArea,
  kObjectsEditCharacteristicIcon,


  kBiomesBiomesMode,
  kBiomesBiomePoint,

  kBiomesEditDesk,
  kBiomesEditDeskPinBack,
  kBiomesEditDeskPinPoint,
  kBiomesEditAccept,
  kBiomesEditName,
  kBiomesEditNameBack,
  kBiomesColorPaletteHS,
  kBiomesColorPaletteB,
  kBiomesColorIndicator,
  kBiomesColorBrightnessCursor,
  kBiomesColorColorCursor,
  kBiomesEditRandomGeneration,

  kBiomesEditNoiseName,
  kBiomesEditNoiseStrengthArea,
  kBiomesEditNoiseStrengthIcon,


  kTilesTilesMode,
  kTilesLeft,
  kTilesRight,
  kTilesUp,
  kTilesDown,
  kTilesCreate,
  kTilesRemove,


  kPlayerPlayerMode,
  kPlayerHealthPoint,
  kPlayerStaminaPoint,

  kPlayerGameObjInfoDesk,
  kPlayerGameObjInfoPinBack,
  kPlayerGameObjInfoPinPoint,
  kPlayerGameObjInfoEnemy,
  kPlayerGameObjInfoFriend,
  kPlayerGameObjInfoNeutal,
  kPlayerGameObjInfoObstacle,
  kPlayerGameObjInfoName,
  kPlayerGameObjInfoCharacteristic,
  kPlayerGameObjInfoValue,

  kPlayerPhone,
  kPlayerPhoneMap,
  kPlayerMapPlayer,
  kPlayerMapEnemy,
  kPlayerMapFriend,
  kPlayerMapNeutral,
  kPlayerMapObstacle,


  kConfigWindowName,
  kConfigWindowDesk,
  kConfigWindowPinBack,
  kConfigWindowPinPoint,
  kConfigWindowHandler,
  kConfigWindowSlider,
  kConfigWindowSave,
  kConfigWindowSliderText,
  kConfigWindowSliderArea,
  kConfigWindowSliderIcon,

  kSlotsName,
  kSlotsConfig,
  kSlotsVisibleOff,
  kSlotsVisibleOn1,
  kSlotsVisibleOn2,
  kSlotsVisibleOn3,
  kSlotsSlot,
  kSlotsSlotColor,
  kSlotsRemove,
  kSlotsSelected,
  kSlotsHandler,
  kSlotsSlider,
  kSlotsBack,
  kSlotsCreate,

  kMapTomb,
  kMapFlowerRed,
  kMapFlowerWhite,
  kMapFlowerBlue,
  kMapKorn,
  kMapSeed,
  kMapLeaf,
  kMapSelected1,
  kMapSelected2,
  kMapSelected0,
  kMapEdge,

  kCautionDesk,
  kCautionText,

  kConfirmationDesk,
  kConfirmationText,
  kConfirmationAccept,
  kConfirmationDecline,

  kFileDesk,
  kFileQuestion,
  kFileTextLabel,
  kFileTextBack,
  kFileAccept,
  kFileDecline,

  kTipInfoDesk,
  kTipInfoDeskPinBack,
  kTipInfoDeskPinPoint,
  kTipInfoText,

  kError,
  kConsoleText,
  kConsoleBack,

  kShadow,
  kPivot,

  kWireframeWindow,
  kWireframeWindowFrame,
  kWireframeWindowXpos,
  kWireframeWindowXneg,
  kWireframeWindowYpos,
  kWireframeWindowYneg,
  kWireframeWindowZpos,
  kWireframeWindowZneg,

  kTextRendererCursor,
  kTextRendererPrerenderSlot,

  kCursorRectangle,
  kCursorCircle,
  kCursorLasso,
  kCursorTweak,

  kCompass,
  kCompassNorth,
  kCompassSouth,
  kCompassWest,
  kCompassEast,

  kCursorTree,
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
  kLogoBig,
  kLogoMedium,
  kLogoSmall,

  kButtonsTextKeys,
  kButtonsTextMods,

  kSelectionSprite,
  kSelectionCircle,

  kSpare1,
  kSpare2,
  kSpare3,
  kSpare4,
  kSpare5,
  kSpare6,
  kSpare7,
  kSpare8,
  kSpare9,
  kSpare10,

  kTotal,
};

inline constexpr int gVboIdSize =
    static_cast<int>(VboIdMain::kTotal)
    - static_cast<int>(VboIdMain::kMenuTerrain);

extern const std::array<std::string_view, gVboIdSize> gVboIdMainText;

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

inline UiData GetUiData(VboIdMain btn_type) {
  return {static_cast<int>(btn_type), GetVboMainOffset(btn_type), 0, nullptr};
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
