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

  kSettingsKeyboard,

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

  kTerrainSizeFill,
  kTerrainSizeBack,
  kTerrainSizeIcon,

  kTerrainFalloffFill,
  kTerrainFalloffBack,
  kTerrainFalloffIcon,

  kTerrainBake,

  kTerrainSlotsName,
  kTerrainSlotsConfig,
  kTerrainVisibleOff,
  kTerrainVisibleOn1,
  kTerrainVisibleOn2,
  kTerrainVisibleOn3,
  kTerrainSlotsSlot,
  kTerrainSlotsSlotColor,
  kTerrainSlotsRemove,
  kTerrainSlotsSelected,
  kTerrainSlotsHandler,
  kTerrainSlotsSlider,
  kTerrainSlotsBack,
  kTerrainSlotsCreate,

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


  kTerrainEditLabelInvert,
  kTerrainEditLabelTiling,
  kTerrainEditLabelStrength,

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

  kTerrainWindowNoiseName,
  kTerrainWindowNoiseDesk,
  kTerrainWindowNoisePinBack,
  kTerrainWindowNoisePinPoint,
  kTerrainWindowNoiseHandler,
  kTerrainWindowNoiseSlider,
  kTerrainWindowNoiseSave,
  kTerrainWindowNoiseSliderText,
  kTerrainWindowNoiseSliderArea,
  kTerrainWindowNoiseSliderIcon,


  kWaterWaterMode,
  kWaterUpdate,
  kWaterLake,
  kWaterRiver,
  kWaterWaterfall,

  kWaterSlotsName,
  kWaterSlotsConfig,
  kWaterVisibleOff,
  kWaterVisibleOn1,
  kWaterVisibleOn2,
  kWaterVisibleOn3,
  kWaterSlotsSlot,
  kWaterSlotsSlotColor,
  kWaterSlotsRemove,
  kWaterSlotsSelected,
  kWaterSlotsHandler,
  kWaterSlotsSlider,
  kWaterSlotsBack,
  kWaterSlotsCreate,
  kWaterSlotsFlipPointEdgeFace_Back,
  kWaterSlotsFlipSelectEdit_Back,
  kWaterSlotsFlipPointEdgeFace,
  kWaterSlotsFlipSelectEdit,

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

  kWaterTypeBack,
  kWaterTypeText,
  kWaterTypePrevious,
  kWaterTypeNext,

  kWaterLayer1,
  kWaterLayer1Window,
  kWaterLayer1PinBack,
  kWaterLayer1PinPoint,
  kWaterLayer1Text,
  kWaterLayer1Off,
  kWaterLayer1On1,
  kWaterLayer1On2,
  kWaterLayer1On3,

  kWater1ScaleLabel,
  kWater1ScaleFill,
  kWater1ScaleBack,
  kWater1ScaleHandler,

  kWater1FetchLabel,
  kWater1FetchFill,
  kWater1FetchBack,
  kWater1FetchHandler,

  kWater1SpreadBlendLabel,
  kWater1SpreadBlendFill,
  kWater1SpreadBlendBack,
  kWater1SpreadBlendHandler,

  kWater1SwellLabel,
  kWater1SwellFill,
  kWater1SwellBack,
  kWater1SwellHandler,

  kWater1PeakEnhancementLabel,
  kWater1PeakEnhancementFill,
  kWater1PeakEnhancementBack,
  kWater1PeakEnhancementHandler,

  kWater1ShortWavesFadeLabel,
  kWater1ShortWavesFadeFill,
  kWater1ShortWavesFadeBack,
  kWater1ShortWavesFadeHandler,

  kWater1LambdaLabel,
  kWater1LambdaFill,
  kWater1LambdaBack,
  kWater1LambdaHandler,


  kWaterLayer2,
  kWaterLayer2Window,
  kWaterLayer2PinBack,
  kWaterLayer2PinPoint,
  kWaterLayer2Text,
  kWaterLayer2Off,
  kWaterLayer2On1,
  kWaterLayer2On2,
  kWaterLayer2On3,

  kWater2ScaleLabel,
  kWater2ScaleFill,
  kWater2ScaleBack,
  kWater2ScaleHandler,

  kWater2FetchLabel,
  kWater2FetchFill,
  kWater2FetchBack,
  kWater2FetchHandler,

  kWater2SpreadBlendLabel,
  kWater2SpreadBlendFill,
  kWater2SpreadBlendBack,
  kWater2SpreadBlendHandler,

  kWater2SwellLabel,
  kWater2SwellFill,
  kWater2SwellBack,
  kWater2SwellHandler,

  kWater2PeakEnhancementLabel,
  kWater2PeakEnhancementFill,
  kWater2PeakEnhancementBack,
  kWater2PeakEnhancementHandler,

  kWater2ShortWavesFadeLabel,
  kWater2ShortWavesFadeFill,
  kWater2ShortWavesFadeBack,
  kWater2ShortWavesFadeHandler,

  kWater2LambdaLabel,
  kWater2LambdaFill,
  kWater2LambdaBack,
  kWater2LambdaHandler,


  kWaterLayer3,
  kWaterLayer3Window,
  kWaterLayer3PinBack,
  kWaterLayer3PinPoint,
  kWaterLayer3Text,
  kWaterLayer3Off,
  kWaterLayer3On1,
  kWaterLayer3On2,
  kWaterLayer3On3,

  kWater3ScaleLabel,
  kWater3ScaleFill,
  kWater3ScaleBack,
  kWater3ScaleHandler,

  kWater3FetchLabel,
  kWater3FetchFill,
  kWater3FetchBack,
  kWater3FetchHandler,

  kWater3SpreadBlendLabel,
  kWater3SpreadBlendFill,
  kWater3SpreadBlendBack,
  kWater3SpreadBlendHandler,

  kWater3SwellLabel,
  kWater3SwellFill,
  kWater3SwellBack,
  kWater3SwellHandler,

  kWater3PeakEnhancementLabel,
  kWater3PeakEnhancementFill,
  kWater3PeakEnhancementBack,
  kWater3PeakEnhancementHandler,

  kWater3ShortWavesFadeLabel,
  kWater3ShortWavesFadeFill,
  kWater3ShortWavesFadeBack,
  kWater3ShortWavesFadeHandler,

  kWater3LambdaLabel,
  kWater3LambdaFill,
  kWater3LambdaBack,
  kWater3LambdaHandler,


  kRoadsRoadsMode,
  kRoadsAsphalt,
  kRoadsGravel,
  kRoadsSoil,

  kRoadsSlotsName,
  kRoadsSlotsConfig,
  kRoadsVisibleOff,
  kRoadsVisibleOn1,
  kRoadsVisibleOn2,
  kRoadsVisibleOn3,
  kRoadsSlotsSlot,
  kRoadsSlotsSlotColor,
  kRoadsSlotsRemove,
  kRoadsSlotsSelected,
  kRoadsSlotsHandler,
  kRoadsSlotsSlider,
  kRoadsSlotsBack,
  kRoadsSlotsCreate,
  kRoadsSlotsFlipPointEdgeFace_Back,
  kRoadsSlotsFlipSelectEdit_Back,
  kRoadsSlotsFlipPointEdgeFace,
  kRoadsSlotsFlipSelectEdit,

  kRoadsEditDesk,
  kRoadsEditPinBack,
  kRoadsEditPinPoint,
  kRoadsEditAccept,
  kRoadsEditName,
  kRoadsEditNameBack,
  kRoadsColorPaletteHS,
  kRoadsColorPaletteB,
  kRoadsColorIndicator,
  kRoadsColorBrightnessCursor,
  kRoadsColorColorCursor,

  kRoadsTypeBack,
  kRoadsTypeText,
  kRoadsTypePrevious,
  kRoadsTypeNext,


  kFencesFencesMode,
  kFencesPicket,
  kFencesChainLink,
  kFencesWooden,

  kFencesSlotsName,
  kFencesSlotsConfig,
  kFencesVisibleOff,
  kFencesVisibleOn1,
  kFencesVisibleOn2,
  kFencesVisibleOn3,
  kFencesSlotsSlot,
  kFencesSlotsSlotColor,
  kFencesSlotsRemove,
  kFencesSlotsSelected,
  kFencesSlotsHandler,
  kFencesSlotsSlider,
  kFencesSlotsBack,
  kFencesSlotsCreate,
  kFencesSlotsFlipPointEdgeFace_Back,
  kFencesSlotsFlipSelectEdit_Back,
  kFencesSlotsFlipPointEdgeFace,
  kFencesSlotsFlipSelectEdit,

  kFencesEditDesk,
  kFencesEditPinBack,
  kFencesEditPinPoint,
  kFencesEditAccept,
  kFencesEditName,
  kFencesEditNameBack,
  kFencesColorPaletteHS,
  kFencesColorPaletteB,
  kFencesColorIndicator,
  kFencesColorBrightnessCursor,
  kFencesColorColorCursor,

  kFencesTypeBack,
  kFencesTypeText,
  kFencesTypePrevious,
  kFencesTypeNext,


  kPlacementPlacementMode,
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

  kPlacementChangeMode,

  kPlacementDrawOff,
  kPlacementDrawOn1,
  kPlacementDrawOn2,
  kPlacementDrawOn3,


  kObjectsObjectsMode,
  kObjectsMavka,
  kObjectsVodyaniy,
  kObjectsChugaister,
  kObjectsPedestal,
  kObjectsCampfire,
  kObjectsRoadSign,
  kObjectsHuman,

  kBiomesBiomesMode,

  kBiomesWindDirArea,
  kBiomesWindDirHandle,
  kBiomesWindDirIcon,

  kBiomesSunDirArea,
  kBiomesSunDirHandle,
  kBiomesSunDirIcon,

  kBiomesTimeArea,
  kBiomesTimeHandle,
  kBiomesTimeIcon,

  kBiomesSlotsName,
  kBiomesSlotsConfig,
  kBiomesVisibleOff,
  kBiomesVisibleOn1,
  kBiomesVisibleOn2,
  kBiomesVisibleOn3,
  kBiomesSlotsSlot,
  kBiomesSlotsSlotColor,
  kBiomesSlotsRemove,
  kBiomesSlotsSelected,
  kBiomesSlotsHandler,
  kBiomesSlotsSlider,
  kBiomesSlotsBack,
  kBiomesSlotsCreate,
  kBiomesSlotsFlipPointEdgeFace_Back,
  kBiomesSlotsFlipSelectEdit_Back,
  kBiomesSlotsFlipPointEdgeFace,
  kBiomesSlotsFlipSelectEdit,

  kBiomesEditDesk,
  kBiomesEditPinBack,
  kBiomesEditPinPoint,
  kBiomesEditAccept,
  kBiomesEditName,
  kBiomesEditNameBack,
  kBiomesColorPaletteHS,
  kBiomesColorPaletteB,
  kBiomesColorIndicator,
  kBiomesColorBrightnessCursor,
  kBiomesColorColorCursor,

  kBiomesEditWindFill,
  kBiomesEditWindBack,
  kBiomesEditWindHandler,
  kBiomesEditWindIcon,

  kBiomesEditSunFill,
  kBiomesEditSunBack,
  kBiomesEditSunHandler,
  kBiomesEditSunIcon,

  kBiomesEditTimeFill,
  kBiomesEditTimeBack,
  kBiomesEditTimeHandler,
  kBiomesEditTimeIcon,

  kBiomesEditPrecipitationFill,
  kBiomesEditPrecipitationBack,
  kBiomesEditPrecipitationHandler,
  kBiomesEditPrecipitationIcon,

  kBiomesEditTemperatureFill,
  kBiomesEditTemperatureBack,
  kBiomesEditTemperatureHandler,
  kBiomesEditTemperatureIcon,

  kBiomesEditCloudFill,
  kBiomesEditCloudBack,
  kBiomesEditCloudHandler,
  kBiomesEditCloudIcon,


  kTilesTilesMode,
  kTilesLeft,
  kTilesRight,
  kTilesUp,
  kTilesDown,

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

  kHealthPoint,
  kStaminaPoint,

  kPhone,

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
