#include "Vbos.h"

namespace data {

extern const std::array<std::string_view, gVboIdSize> gVboIdMainText = {
    "kMenuTerrain",
    "kMenuTerrainOff",
    "kMenuTerrainOn1",
    "kMenuTerrainOn2",
    "kMenuTerrainOn3",

    "kMenuWater",
    "kMenuWaterOff",
    "kMenuWaterOn1",
    "kMenuWaterOn2",
    "kMenuWaterOn3",

    "kMenuRoads",
    "kMenuRoadsOff",
    "kMenuRoadsOn1",
    "kMenuRoadsOn2",
    "kMenuRoadsOn3",

    "kMenuFences",
    "kMenuFencesOff",
    "kMenuFencesOn1",
    "kMenuFencesOn2",
    "kMenuFencesOn3",

    "kMenuPlacement",
    "kMenuPlacementOff",
    "kMenuPlacementOn1",
    "kMenuPlacementOn2",
    "kMenuPlacementOn3",

    "kMenuObjects",
    "kMenuObjectsOff",
    "kMenuObjectsOn1",
    "kMenuObjectsOn2",
    "kMenuObjectsOn3",

    "kMenuBiomes",
    "kMenuBiomesOff",
    "kMenuBiomesOn1",
    "kMenuBiomesOn2",
    "kMenuBiomesOn3",

    "kMenuTiles",
    "kMenuTilesOff",
    "kMenuTilesOn1",
    "kMenuTilesOn2",
    "kMenuTilesOn3",

    "kMenuShaderWirebound",
    "kMenuShadersOff",
    "kMenuShadersOn1",
    "kMenuShadersOn2",
    "kMenuShadersOn3",

    "kMenuDesk",
    "kMenuDeskPinBack",
    "kMenuDeskPinPoint",
    "kMenuArrowSelect",
    "kMenuArrowSelected",

    "kMenuSave",
    "kMenuLoad",

    "kModeModeText",

    "kPressedKey",

    "kSettingsDesk",
    "kSettingsDeskPinBack",
    "kSettingsDeskPinPoint",

    "kSettingsResolutionLabel",
    "kSettingsResolutionLeft",
    "kSettingsResolutionRight",
    "kSettingsResolution",
    "kSettingsFullScreenOff",
    "kSettingsFullScreenOn1",
    "kSettingsFullScreenOn2",
    "kSettingsFullScreenOn3",

    "kSettingsSensitivityArea",
    "kSettingsSensitivityHandle",

    "kSettingsKeyboard",

    "kSettingsSoundArea",
    "kSettingsSoundHandle",
    "kSettingsSoundOff",
    "kSettingsSoundOn1",
    "kSettingsSoundOn2",
    "kSettingsSoundOn3",

    "kSettingsMusicArea",
    "kSettingsMusicHandle",
    "kSettingsMusicOff",
    "kSettingsMusicOn1",
    "kSettingsMusicOn2",
    "kSettingsMusicOn3",

    "kSettingsTipInfoLabel",
    "kSettingsTipInfo",
    "kSettingsTipInfoOff",
    "kSettingsTipInfoOn1",
    "kSettingsTipInfoOn2",
    "kSettingsTipInfoOn3",

    "kModeTextBack",
    "kModeTextLabel",

    "kTerrainTerrainMode",
    "kTerrainUpdate",
    "kTerrainFlatten",
    "kTerrainFlattenOff",
    "kTerrainFlattenOn1",
    "kTerrainFlattenOn2",
    "kTerrainFlattenOn3",

    "kTerrainSizeFill",
    "kTerrainSizeBack",
    "kTerrainSizeIcon",

    "kTerrainFalloffFill",
    "kTerrainFalloffBack",
    "kTerrainFalloffIcon",

    "kTerrainBake",

    "kTerrainSlotsName",
    "kTerrainSlotsConfig",
    "kTerrainVisibleOff",
    "kTerrainVisibleOn1",
    "kTerrainVisibleOn2",
    "kTerrainVisibleOn3",
    "kTerrainSlotsSlot",
    "kTerrainSlotsSlotColor",
    "kTerrainSlotsRemove",
    "kTerrainSlotsSelected",
    "kTerrainSlotsHandler",
    "kTerrainSlotsSlider",
    "kTerrainSlotsBack",
    "kTerrainSlotsCreate",

    "kTerrainBakeDesk",
    "kTerrainBakeDeskPinBack",
    "kTerrainBakeDeskPinPoint",
    "kTerrainBakeAccept",
    "kTerrainBakeErosionStepLabel",
    "kTerrainBakeErosionStepInputText",
    "kTerrainBakeErosionStepInputBack",
    "kTerrainBakeWeatheringStepLabel",
    "kTerrainBakeWeatheringStepInputText",
    "kTerrainBakeWeatheringStepInputBack",
    "kTerrainBakeHmap",
    "kTerrainBakeHmapName",

    "kTerrainEditDesk",
    "kTerrainEditDeskPinBack",
    "kTerrainEditDeskPinPoint",
    "kTerrainEditAccept",
    "kTerrainEditName",
    "kTerrainEditNameBack",
    "kTerrainColorPaletteHS",
    "kTerrainColorPaletteB",
    "kTerrainColorIndicator",
    "kTerrainColorBrightnessCursor",
    "kTerrainColorColorCursor",
    "kTerrainEditHeightmap",
    "kTerrainEditRandomGeneration",


    "kTerrainEditTranslateLabel",
    "kTerrainEditTranslateXText",
    "kTerrainEditTranslateXTextSlot",
    "kTerrainEditTranslateYText",
    "kTerrainEditTranslateYTextSlot",
    "kTerrainEditTranslateZText",
    "kTerrainEditTranslateZTextSlot",

    "kTerrainEditScaleLabel",
    "kTerrainEditScaleXText",
    "kTerrainEditScaleXTextSlot",
    "kTerrainEditScaleYText",
    "kTerrainEditScaleYTextSlot",
    "kTerrainEditScaleZText",
    "kTerrainEditScaleZTextSlot",

    "kTerrainEditRotateLabel",
    "kTerrainEditRotateXText",
    "kTerrainEditRotateXTextSlot",
    "kTerrainEditRotateYText",
    "kTerrainEditRotateYTextSlot",
    "kTerrainEditRotateZText",
    "kTerrainEditRotateZTextSlot",


    "kTerrainEditLabelInvert",
    "kTerrainEditLabelTiling",
    "kTerrainEditLabelStrength",

    "kTerrainEditNoiseConfig",
    "kTerrainEditNoiseName",
    "kTerrainEditNoiseInvertOff",
    "kTerrainEditNoiseInvertOn1",
    "kTerrainEditNoiseInvertOn2",
    "kTerrainEditNoiseInvertOn3",
    "kTerrainEditNoiseTilingOff",
    "kTerrainEditNoiseTilingOn1",
    "kTerrainEditNoiseTilingOn2",
    "kTerrainEditNoiseTilingOn3",
    "kTerrainEditNoiseStrengthArea",
    "kTerrainEditNoiseStrengthIcon",
    "kTerrainEditNoiseHmap",

    "kTerrainWindowNoiseName",
    "kTerrainWindowNoiseDesk",
    "kTerrainWindowNoisePinBack",
    "kTerrainWindowNoisePinPoint",
    "kTerrainWindowNoiseHandler",
    "kTerrainWindowNoiseSlider",
    "kTerrainWindowNoiseSave",
    "kTerrainWindowNoiseSliderText",
    "kTerrainWindowNoiseSliderArea",
    "kTerrainWindowNoiseSliderIcon",


    "kWaterWaterMode",
    "kWaterUpdate",
    "kWaterLake",
    "kWaterRiver",
    "kWaterWaterfall",

    "kWaterSlotsName",
    "kWaterSlotsConfig",
    "kWaterVisibleOff",
    "kWaterVisibleOn1",
    "kWaterVisibleOn2",
    "kWaterVisibleOn3",
    "kWaterSlotsSlot",
    "kWaterSlotsSlotColor",
    "kWaterSlotsRemove",
    "kWaterSlotsSelected",
    "kWaterSlotsHandler",
    "kWaterSlotsSlider",
    "kWaterSlotsBack",
    "kWaterSlotsCreate",
    "kWaterSlotsFlipPointEdgeFace_Back",
    "kWaterSlotsFlipSelectEdit_Back",
    "kWaterSlotsFlipPointEdgeFace",
    "kWaterSlotsFlipSelectEdit",

    "kWaterEditDesk",
    "kWaterEditDeskPinBack",
    "kWaterEditDeskPinPoint",
    "kWaterEditAccept",
    "kWaterEditName",
    "kWaterEditNameBack",
    "kWaterColorPaletteHS",
    "kWaterColorPaletteB",
    "kWaterColorIndicator",
    "kWaterColorBrightnessCursor",
    "kWaterColorColorCursor",

    "kWaterTypeBack",
    "kWaterTypeText",
    "kWaterTypePrevious",
    "kWaterTypeNext",

    "kWaterLayer1",
    "kWaterLayer1Window",
    "kWaterLayer1PinBack",
    "kWaterLayer1PinPoint",
    "kWaterLayer1Text",
    "kWaterLayer1Off",
    "kWaterLayer1On1",
    "kWaterLayer1On2",
    "kWaterLayer1On3",

    "kWater1ScaleLabel",
    "kWater1ScaleFill",
    "kWater1ScaleBack",
    "kWater1ScaleHandler",

    "kWater1FetchLabel",
    "kWater1FetchFill",
    "kWater1FetchBack",
    "kWater1FetchHandler",

    "kWater1SpreadBlendLabel",
    "kWater1SpreadBlendFill",
    "kWater1SpreadBlendBack",
    "kWater1SpreadBlendHandler",

    "kWater1SwellLabel",
    "kWater1SwellFill",
    "kWater1SwellBack",
    "kWater1SwellHandler",

    "kWater1PeakEnhancementLabel",
    "kWater1PeakEnhancementFill",
    "kWater1PeakEnhancementBack",
    "kWater1PeakEnhancementHandler",

    "kWater1ShortWavesFadeLabel",
    "kWater1ShortWavesFadeFill",
    "kWater1ShortWavesFadeBack",
    "kWater1ShortWavesFadeHandler",

    "kWater1LambdaLabel",
    "kWater1LambdaFill",
    "kWater1LambdaBack",
    "kWater1LambdaHandler",


    "kWaterLayer2",
    "kWaterLayer2Window",
    "kWaterLayer2PinBack",
    "kWaterLayer2PinPoint",
    "kWaterLayer2Text",
    "kWaterLayer2Off",
    "kWaterLayer2On1",
    "kWaterLayer2On2",
    "kWaterLayer2On3",

    "kWater2ScaleLabel",
    "kWater2ScaleFill",
    "kWater2ScaleBack",
    "kWater2ScaleHandler",

    "kWater2FetchLabel",
    "kWater2FetchFill",
    "kWater2FetchBack",
    "kWater2FetchHandler",

    "kWater2SpreadBlendLabel",
    "kWater2SpreadBlendFill",
    "kWater2SpreadBlendBack",
    "kWater2SpreadBlendHandler",

    "kWater2SwellLabel",
    "kWater2SwellFill",
    "kWater2SwellBack",
    "kWater2SwellHandler",

    "kWater2PeakEnhancementLabel",
    "kWater2PeakEnhancementFill",
    "kWater2PeakEnhancementBack",
    "kWater2PeakEnhancementHandler",

    "kWater2ShortWavesFadeLabel",
    "kWater2ShortWavesFadeFill",
    "kWater2ShortWavesFadeBack",
    "kWater2ShortWavesFadeHandler",

    "kWater2LambdaLabel",
    "kWater2LambdaFill",
    "kWater2LambdaBack",
    "kWater2LambdaHandler",


    "kWaterLayer3",
    "kWaterLayer3Window",
    "kWaterLayer3PinBack",
    "kWaterLayer3PinPoint",
    "kWaterLayer3Text",
    "kWaterLayer3Off",
    "kWaterLayer3On1",
    "kWaterLayer3On2",
    "kWaterLayer3On3",

    "kWater3ScaleLabel",
    "kWater3ScaleFill",
    "kWater3ScaleBack",
    "kWater3ScaleHandler",

    "kWater3FetchLabel",
    "kWater3FetchFill",
    "kWater3FetchBack",
    "kWater3FetchHandler",

    "kWater3SpreadBlendLabel",
    "kWater3SpreadBlendFill",
    "kWater3SpreadBlendBack",
    "kWater3SpreadBlendHandler",

    "kWater3SwellLabel",
    "kWater3SwellFill",
    "kWater3SwellBack",
    "kWater3SwellHandler",

    "kWater3PeakEnhancementLabel",
    "kWater3PeakEnhancementFill",
    "kWater3PeakEnhancementBack",
    "kWater3PeakEnhancementHandler",

    "kWater3ShortWavesFadeLabel",
    "kWater3ShortWavesFadeFill",
    "kWater3ShortWavesFadeBack",
    "kWater3ShortWavesFadeHandler",

    "kWater3LambdaLabel",
    "kWater3LambdaFill",
    "kWater3LambdaBack",
    "kWater3LambdaHandler",


    "kRoadsRoadsMode",
    "kRoadsAsphalt",
    "kRoadsGravel",
    "kRoadsSoil",

    "kRoadsSlotsName",
    "kRoadsSlotsConfig",
    "kRoadsVisibleOff",
    "kRoadsVisibleOn1",
    "kRoadsVisibleOn2",
    "kRoadsVisibleOn3",
    "kRoadsSlotsSlot",
    "kRoadsSlotsSlotColor",
    "kRoadsSlotsRemove",
    "kRoadsSlotsSelected",
    "kRoadsSlotsHandler",
    "kRoadsSlotsSlider",
    "kRoadsSlotsBack",
    "kRoadsSlotsCreate",
    "kRoadsSlotsFlipPointEdgeFace_Back",
    "kRoadsSlotsFlipSelectEdit_Back",
    "kRoadsSlotsFlipPointEdgeFace",
    "kRoadsSlotsFlipSelectEdit",

    "kRoadsEditDesk",
    "kRoadsEditPinBack",
    "kRoadsEditPinPoint",
    "kRoadsEditAccept",
    "kRoadsEditName",
    "kRoadsEditNameBack",
    "kRoadsColorPaletteHS",
    "kRoadsColorPaletteB",
    "kRoadsColorIndicator",
    "kRoadsColorBrightnessCursor",
    "kRoadsColorColorCursor",

    "kRoadsTypeBack",
    "kRoadsTypeText",
    "kRoadsTypePrevious",
    "kRoadsTypeNext",


    "kFencesFencesMode",
    "kFencesPicket",
    "kFencesChainLink",
    "kFencesWooden",

    "kFencesSlotsName",
    "kFencesSlotsConfig",
    "kFencesVisibleOff",
    "kFencesVisibleOn1",
    "kFencesVisibleOn2",
    "kFencesVisibleOn3",
    "kFencesSlotsSlot",
    "kFencesSlotsSlotColor",
    "kFencesSlotsRemove",
    "kFencesSlotsSelected",
    "kFencesSlotsHandler",
    "kFencesSlotsSlider",
    "kFencesSlotsBack",
    "kFencesSlotsCreate",
    "kFencesSlotsFlipPointEdgeFace_Back",
    "kFencesSlotsFlipSelectEdit_Back",
    "kFencesSlotsFlipPointEdgeFace",
    "kFencesSlotsFlipSelectEdit",

    "kFencesEditDesk",
    "kFencesEditPinBack",
    "kFencesEditPinPoint",
    "kFencesEditAccept",
    "kFencesEditName",
    "kFencesEditNameBack",
    "kFencesColorPaletteHS",
    "kFencesColorPaletteB",
    "kFencesColorIndicator",
    "kFencesColorBrightnessCursor",
    "kFencesColorColorCursor",

    "kFencesTypeBack",
    "kFencesTypeText",
    "kFencesTypePrevious",
    "kFencesTypeNext",


    "kPlacementPlacementMode",
    "kPlacementColorFill",
    "kPlacementColorBack",
    "kPlacementColorIcon",

    "kPlacementSizeFill",
    "kPlacementSizeBack",
    "kPlacementSizeIcon",

    "kPlacementFalloffFill",
    "kPlacementFalloffBack",
    "kPlacementFalloffIcon",

    "kPlacementTrees",
    "kPlacementTreesOff",
    "kPlacementTreesOn1",
    "kPlacementTreesOn2",
    "kPlacementTreesOn3",

    "kPlacementBushes",
    "kPlacementBushesOff",
    "kPlacementBushesOn1",
    "kPlacementBushesOn2",
    "kPlacementBushesOn3",

    "kPlacementTallGrass",
    "kPlacementTallGrassOff",
    "kPlacementTallGrassOn1",
    "kPlacementTallGrassOn2",
    "kPlacementTallGrassOn3",

    "kPlacementUndergrowth",
    "kPlacementUndergrowthOff",
    "kPlacementUndergrowthOn1",
    "kPlacementUndergrowthOn2",
    "kPlacementUndergrowthOn3",

    "kPlacementChangeMode",

    "kPlacementDrawOff",
    "kPlacementDrawOn1",
    "kPlacementDrawOn2",
    "kPlacementDrawOn3",


    "kObjectsObjectsMode",
    "kObjectsMavka",
    "kObjectsVodyaniy",
    "kObjectsChugaister",
    "kObjectsPedestal",
    "kObjectsCampfire",
    "kObjectsRoadSign",
    "kObjectsHuman",

    "kBiomesBiomesMode",

    "kBiomesWindDirArea",
    "kBiomesWindDirHandle",
    "kBiomesWindDirIcon",

    "kBiomesSunDirArea",
    "kBiomesSunDirHandle",
    "kBiomesSunDirIcon",

    "kBiomesTimeArea",
    "kBiomesTimeHandle",
    "kBiomesTimeIcon",

    "kBiomesSlotsName",
    "kBiomesSlotsConfig",
    "kBiomesVisibleOff",
    "kBiomesVisibleOn1",
    "kBiomesVisibleOn2",
    "kBiomesVisibleOn3",
    "kBiomesSlotsSlot",
    "kBiomesSlotsSlotColor",
    "kBiomesSlotsRemove",
    "kBiomesSlotsSelected",
    "kBiomesSlotsHandler",
    "kBiomesSlotsSlider",
    "kBiomesSlotsBack",
    "kBiomesSlotsCreate",
    "kBiomesSlotsFlipPointEdgeFace_Back",
    "kBiomesSlotsFlipSelectEdit_Back",
    "kBiomesSlotsFlipPointEdgeFace",
    "kBiomesSlotsFlipSelectEdit",

    "kBiomesEditDesk",
    "kBiomesEditPinBack",
    "kBiomesEditPinPoint",
    "kBiomesEditAccept",
    "kBiomesEditName",
    "kBiomesEditNameBack",
    "kBiomesColorPaletteHS",
    "kBiomesColorPaletteB",
    "kBiomesColorIndicator",
    "kBiomesColorBrightnessCursor",
    "kBiomesColorColorCursor",

    "kBiomesEditWindFill",
    "kBiomesEditWindBack",
    "kBiomesEditWindHandler",
    "kBiomesEditWindIcon",

    "kBiomesEditSunFill",
    "kBiomesEditSunBack",
    "kBiomesEditSunHandler",
    "kBiomesEditSunIcon",

    "kBiomesEditTimeFill",
    "kBiomesEditTimeBack",
    "kBiomesEditTimeHandler",
    "kBiomesEditTimeIcon",

    "kBiomesEditPrecipitationFill",
    "kBiomesEditPrecipitationBack",
    "kBiomesEditPrecipitationHandler",
    "kBiomesEditPrecipitationIcon",

    "kBiomesEditTemperatureFill",
    "kBiomesEditTemperatureBack",
    "kBiomesEditTemperatureHandler",
    "kBiomesEditTemperatureIcon",

    "kBiomesEditCloudFill",
    "kBiomesEditCloudBack",
    "kBiomesEditCloudHandler",
    "kBiomesEditCloudIcon",


    "kTilesTilesMode",
    "kTilesLeft",
    "kTilesRight",
    "kTilesUp",
    "kTilesDown",

    "kMapTomb",
    "kMapFlowerRed",
    "kMapFlowerWhite",
    "kMapFlowerBlue",
    "kMapKorn",
    "kMapSeed",
    "kMapLeaf",
    "kMapSelected1",
    "kMapSelected2",
    "kMapSelected0",
    "kMapEdge",

    "kCautionDesk",
    "kCautionText",

    "kConfirmationDesk",
    "kConfirmationText",
    "kConfirmationAccept",
    "kConfirmationDecline",

    "kFileDesk",
    "kFileQuestion",
    "kFileTextLabel",
    "kFileTextBack",
    "kFileAccept",
    "kFileDecline",

    "kTipInfoDesk",
    "kTipInfoDeskPinBack",
    "kTipInfoDeskPinPoint",
    "kTipInfoText",

    "kError",
    "kConsoleText",
    "kConsoleBack",

    "kShadow",
    "kPivot",

    "kWireframeWindow",
    "kWireframeWindowFrame",
    "kWireframeWindowXpos",
    "kWireframeWindowXneg",
    "kWireframeWindowYpos",
    "kWireframeWindowYneg",
    "kWireframeWindowZpos",
    "kWireframeWindowZneg",

    "kTextRendererCursor",
    "kTextRendererPrerenderSlot",

    "kCompass",
    "kCompassNorth",
    "kCompassSouth",
    "kCompassWest",
    "kCompassEast",

    "kCursorTree",
    "kLoading0",
    "kLoading1",
    "kLoading2",
    "kLoading3",
    "kLoading4",
    "kLoading5",
    "kLoading6",
    "kLoading7",
    "kLoading8",
    "kLoading9",
    "kLoading10",
    "kLogoBig",
    "kLogoMedium",
    "kLogoSmall",

    "kSpare1",
    "kSpare2",
    "kSpare3",
    "kSpare4",
    "kSpare5",
    "kSpare6",
    "kSpare7",
    "kSpare8",
    "kSpare9",
    "kSpare10",
};

// 4x the same data... nothing bad at debug (3 kb),
// otherwise there would be "stupid" uniforms
const std::array<float, gUiVboTransformSize> kUiVboDataTransform {
    // should be applied to all transforms
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
};

// +4 test
const std::array<float, gVboIdSize * 16> kUiVboDataMain = {
    // kMenuTerrain
    0.100000,-0.100000,0.343750,0.664062,
    0.100000,0.100000,0.343750,0.695312,
    -0.100000,-0.100000,0.281250,0.664062,
    -0.100000,0.100000,0.281250,0.695312,

    // kMenuTerrainOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuTerrainOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuTerrainOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuTerrainOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuWater
    0.100000,-0.100000,0.906250,0.546875,
    0.100000,0.100000,0.906250,0.578125,
    -0.100000,-0.100000,0.843750,0.546875,
    -0.100000,0.100000,0.843750,0.578125,

    // kMenuWaterOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuWaterOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuWaterOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuWaterOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuRoads
    0.100000,-0.100000,0.125000,0.593750,
    0.100000,0.100000,0.125000,0.625000,
    -0.100000,-0.100000,0.062500,0.593750,
    -0.100000,0.100000,0.062500,0.625000,

    // kMenuRoadsOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuRoadsOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuRoadsOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuRoadsOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuFences
    0.100000,-0.100000,0.468750,0.656250,
    0.100000,0.100000,0.468750,0.687500,
    -0.100000,-0.100000,0.406250,0.656250,
    -0.100000,0.100000,0.406250,0.687500,

    // kMenuFencesOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuFencesOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuFencesOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuFencesOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuPlacement
    0.100000,-0.100000,0.968750,0.554688,
    0.100000,0.100000,0.968750,0.585938,
    -0.100000,-0.100000,0.906250,0.554688,
    -0.100000,0.100000,0.906250,0.585938,

    // kMenuPlacementOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuPlacementOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuPlacementOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuPlacementOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuObjects
    0.100000,-0.100000,0.906250,0.640625,
    0.100000,0.100000,0.906250,0.671875,
    -0.100000,-0.100000,0.843750,0.640625,
    -0.100000,0.100000,0.843750,0.671875,

    // kMenuObjectsOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuObjectsOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuObjectsOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuObjectsOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuBiomes
    0.100000,-0.100000,0.843750,0.679688,
    0.100000,0.100000,0.843750,0.710938,
    -0.100000,-0.100000,0.781250,0.679688,
    -0.100000,0.100000,0.781250,0.710938,

    // kMenuBiomesOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuBiomesOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuBiomesOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuBiomesOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuTiles
    0.100000,-0.100000,0.062500,0.625000,
    0.100000,0.100000,0.062500,0.656250,
    -0.100000,-0.100000,0.000000,0.625000,
    -0.100000,0.100000,0.000000,0.656250,

    // kMenuTilesOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuTilesOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuTilesOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuTilesOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuShaderWirebound
    0.100000,-0.100000,0.375000,0.539062,
    0.100000,0.100000,0.375000,0.570312,
    -0.100000,-0.100000,0.312500,0.539062,
    -0.100000,0.100000,0.312500,0.570312,

    // kMenuShadersOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kMenuShadersOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kMenuShadersOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kMenuShadersOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kMenuDesk
    0.250000,-0.250000,0.406250,0.726562,
    0.250000,0.250000,0.406250,0.804688,
    -0.250000,-0.250000,0.250000,0.726562,
    -0.250000,0.250000,0.250000,0.804688,

    // kMenuDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kMenuDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kMenuArrowSelect
    0.037500,-0.225000,0.148438,0.472656,
    0.037500,0.225000,0.148438,0.542969,
    -0.037500,-0.225000,0.125000,0.472656,
    -0.037500,0.225000,0.125000,0.542969,

    // kMenuArrowSelected
    0.025000,-0.275000,1.000000,0.458984,
    0.025000,0.275000,1.000000,0.544922,
    -0.025000,-0.275000,0.984375,0.458984,
    -0.025000,0.275000,0.984375,0.544922,

    // kMenuSave
    0.100000,-0.100000,0.718750,0.578125,
    0.100000,0.100000,0.718750,0.609375,
    -0.100000,-0.100000,0.656250,0.578125,
    -0.100000,0.100000,0.656250,0.609375,

    // kMenuLoad
    0.100000,-0.100000,0.968750,0.632812,
    0.100000,0.100000,0.968750,0.664062,
    -0.100000,-0.100000,0.906250,0.632812,
    -0.100000,0.100000,0.906250,0.664062,

    // kModeModeText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kPressedKey
    0.075000,-0.050000,0.796875,0.539062,
    0.075000,0.050000,0.796875,0.554688,
    -0.075000,-0.050000,0.750000,0.539062,
    -0.075000,0.050000,0.750000,0.554688,

    // kSettingsDesk
    0.150000,-0.150000,0.250000,0.730469,
    0.150000,0.150000,0.250000,0.777344,
    -0.150000,-0.150000,0.156250,0.730469,
    -0.150000,0.150000,0.156250,0.777344,

    // kSettingsDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kSettingsDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kSettingsResolutionLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSettingsResolutionLeft
    0.100000,-0.100000,0.593750,0.664062,
    0.100000,0.100000,0.593750,0.695312,
    -0.100000,-0.100000,0.531250,0.664062,
    -0.100000,0.100000,0.531250,0.695312,

    // kSettingsResolutionRight
    0.100000,-0.100000,0.687500,0.546875,
    0.100000,0.100000,0.687500,0.578125,
    -0.100000,-0.100000,0.625000,0.546875,
    -0.100000,0.100000,0.625000,0.578125,

    // kSettingsResolution
    0.100000,-0.100000,0.656250,0.601562,
    0.100000,0.100000,0.656250,0.632812,
    -0.100000,-0.100000,0.593750,0.601562,
    -0.100000,0.100000,0.593750,0.632812,

    // kSettingsFullScreenOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kSettingsFullScreenOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kSettingsFullScreenOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kSettingsFullScreenOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kSettingsSensitivityArea
    0.200000,-0.050000,0.781250,0.734375,
    0.200000,0.050000,0.781250,0.750000,
    -0.200000,-0.050000,0.656250,0.734375,
    -0.200000,0.050000,0.656250,0.750000,

    // kSettingsSensitivityHandle
    0.100000,-0.100000,0.531250,0.625000,
    0.100000,0.100000,0.531250,0.656250,
    -0.100000,-0.100000,0.468750,0.625000,
    -0.100000,0.100000,0.468750,0.656250,

    // kSettingsKeyboard
    0.100000,-0.100000,0.218750,0.636719,
    0.100000,0.100000,0.218750,0.667969,
    -0.100000,-0.100000,0.156250,0.636719,
    -0.100000,0.100000,0.156250,0.667969,

    // kSettingsSoundArea
    0.200000,-0.050000,0.781250,0.734375,
    0.200000,0.050000,0.781250,0.750000,
    -0.200000,-0.050000,0.656250,0.734375,
    -0.200000,0.050000,0.656250,0.750000,

    // kSettingsSoundHandle
    0.100000,-0.100000,0.062500,0.562500,
    0.100000,0.100000,0.062500,0.593750,
    -0.100000,-0.100000,0.000000,0.562500,
    -0.100000,0.100000,0.000000,0.593750,

    // kSettingsSoundOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kSettingsSoundOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kSettingsSoundOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kSettingsSoundOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kSettingsMusicArea
    0.200000,-0.050000,0.781250,0.734375,
    0.200000,0.050000,0.781250,0.750000,
    -0.200000,-0.050000,0.656250,0.734375,
    -0.200000,0.050000,0.656250,0.750000,

    // kSettingsMusicHandle
    0.100000,-0.100000,0.843750,0.617188,
    0.100000,0.100000,0.843750,0.648438,
    -0.100000,-0.100000,0.781250,0.617188,
    -0.100000,0.100000,0.781250,0.648438,

    // kSettingsMusicOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kSettingsMusicOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kSettingsMusicOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kSettingsMusicOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kSettingsTipInfoLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSettingsTipInfo
    0.100000,-0.100000,0.593750,0.601562,
    0.100000,0.100000,0.593750,0.632812,
    -0.100000,-0.100000,0.531250,0.601562,
    -0.100000,0.100000,0.531250,0.632812,

    // kSettingsTipInfoOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kSettingsTipInfoOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kSettingsTipInfoOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kSettingsTipInfoOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kModeTextBack
    0.125000,-0.050000,0.986328,0.796875,
    0.125000,0.050000,0.986328,0.812500,
    -0.125000,-0.050000,0.908203,0.796875,
    -0.125000,0.050000,0.908203,0.812500,

    // kModeTextLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainTerrainMode
    0.100000,-0.100000,0.343750,0.664062,
    0.100000,0.100000,0.343750,0.695312,
    -0.100000,-0.100000,0.281250,0.664062,
    -0.100000,0.100000,0.281250,0.695312,

    // kTerrainUpdate
    0.100000,-0.100000,0.718750,0.578125,
    0.100000,0.100000,0.718750,0.609375,
    -0.100000,-0.100000,0.656250,0.578125,
    -0.100000,0.100000,0.656250,0.609375,

    // kTerrainFlatten
    0.100000,-0.100000,0.843750,0.585938,
    0.100000,0.100000,0.843750,0.617188,
    -0.100000,-0.100000,0.781250,0.585938,
    -0.100000,0.100000,0.781250,0.617188,

    // kTerrainFlattenOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kTerrainFlattenOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kTerrainFlattenOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kTerrainFlattenOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kTerrainSizeFill
    0.050000,-0.200000,0.500000,0.500000,
    0.050000,0.200000,0.500000,0.562500,
    -0.050000,-0.200000,0.468750,0.500000,
    -0.050000,0.200000,0.468750,0.562500,

    // kTerrainSizeBack
    0.050000,-0.200000,0.125000,0.500000,
    0.050000,0.200000,0.125000,0.562500,
    -0.050000,-0.200000,0.093750,0.500000,
    -0.050000,0.200000,0.093750,0.562500,

    // kTerrainSizeIcon
    0.100000,-0.100000,0.906250,0.578125,
    0.100000,0.100000,0.906250,0.609375,
    -0.100000,-0.100000,0.843750,0.578125,
    -0.100000,0.100000,0.843750,0.609375,

    // kTerrainFalloffFill
    0.050000,-0.200000,0.500000,0.500000,
    0.050000,0.200000,0.500000,0.562500,
    -0.050000,-0.200000,0.468750,0.500000,
    -0.050000,0.200000,0.468750,0.562500,

    // kTerrainFalloffBack
    0.050000,-0.200000,0.125000,0.500000,
    0.050000,0.200000,0.125000,0.562500,
    -0.050000,-0.200000,0.093750,0.500000,
    -0.050000,0.200000,0.093750,0.562500,

    // kTerrainFalloffIcon
    0.100000,-0.100000,0.968750,0.664062,
    0.100000,0.100000,0.968750,0.695312,
    -0.100000,-0.100000,0.906250,0.664062,
    -0.100000,0.100000,0.906250,0.695312,

    // kTerrainBake
    0.100000,-0.100000,0.970703,0.734375,
    0.100000,0.100000,0.970703,0.765625,
    -0.100000,-0.100000,0.908203,0.734375,
    -0.100000,0.100000,0.908203,0.765625,

    // kTerrainSlotsName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainSlotsConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kTerrainVisibleOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kTerrainVisibleOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kTerrainVisibleOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kTerrainVisibleOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kTerrainSlotsSlot
    0.175000,-0.075000,0.890625,0.710938,
    0.175000,0.075000,0.890625,0.734375,
    -0.175000,-0.075000,0.781250,0.710938,
    -0.175000,0.075000,0.781250,0.734375,

    // kTerrainSlotsSlotColor
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kTerrainSlotsRemove
    0.037500,-0.037500,0.046875,0.527344,
    0.037500,0.037500,0.046875,0.539062,
    -0.037500,-0.037500,0.023438,0.527344,
    -0.037500,0.037500,0.023438,0.539062,

    // kTerrainSlotsSelected
    0.025000,-0.025000,0.078125,0.531250,
    0.025000,0.025000,0.078125,0.539062,
    -0.025000,-0.025000,0.062500,0.531250,
    -0.025000,0.025000,0.062500,0.539062,

    // kTerrainSlotsHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kTerrainSlotsSlider
    0.050000,-0.400000,0.531250,0.421875,
    0.050000,0.400000,0.531250,0.546875,
    -0.050000,-0.400000,0.500000,0.421875,
    -0.050000,0.400000,0.500000,0.546875,

    // kTerrainSlotsBack
    0.250000,-0.400000,0.156250,0.656250,
    0.250000,0.400000,0.156250,0.781250,
    -0.250000,-0.400000,0.000000,0.656250,
    -0.250000,0.400000,0.000000,0.781250,

    // kTerrainSlotsCreate
    0.037500,-0.037500,0.171875,0.531250,
    0.037500,0.037500,0.171875,0.542969,
    -0.037500,-0.037500,0.148438,0.531250,
    -0.037500,0.037500,0.148438,0.542969,

    // kTerrainBakeDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kTerrainBakeDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kTerrainBakeDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kTerrainBakeAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kTerrainBakeErosionStepLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainBakeErosionStepInputText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainBakeErosionStepInputBack
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainBakeWeatheringStepLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainBakeWeatheringStepInputText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainBakeWeatheringStepInputBack
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainBakeHmap
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainBakeHmapName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kTerrainEditDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kTerrainEditDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kTerrainEditAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kTerrainEditName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditNameBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kTerrainColorPaletteHS
    0.100000,-0.100000,0.218750,0.667969,
    0.100000,0.100000,0.218750,0.699219,
    -0.100000,-0.100000,0.156250,0.667969,
    -0.100000,0.100000,0.156250,0.699219,

    // kTerrainColorPaletteB
    0.100000,-0.025000,0.968750,0.695312,
    0.100000,0.025000,0.968750,0.703125,
    -0.100000,-0.025000,0.906250,0.695312,
    -0.100000,0.025000,0.906250,0.703125,

    // kTerrainColorIndicator
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kTerrainColorBrightnessCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kTerrainColorColorCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kTerrainEditHeightmap
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditRandomGeneration
    0.075000,-0.075000,0.546875,0.546875,
    0.075000,0.075000,0.546875,0.570312,
    -0.075000,-0.075000,0.500000,0.546875,
    -0.075000,0.075000,0.500000,0.570312,

    // kTerrainEditTranslateLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditTranslateXText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditTranslateXTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditTranslateYText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditTranslateYTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditTranslateZText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditTranslateZTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditScaleLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditScaleXText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditScaleXTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditScaleYText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditScaleYTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditScaleZText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditScaleZTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditRotateLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditRotateXText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditRotateXTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditRotateYText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditRotateYTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditRotateZText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditRotateZTextSlot
    0.050000,-0.025000,1.000000,0.552734,
    0.050000,0.025000,1.000000,0.560547,
    -0.050000,-0.025000,0.968750,0.552734,
    -0.050000,0.025000,0.968750,0.560547,

    // kTerrainEditLabelInvert
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditLabelTiling
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditLabelStrength
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditNoiseConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kTerrainEditNoiseName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainEditNoiseInvertOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kTerrainEditNoiseInvertOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kTerrainEditNoiseInvertOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kTerrainEditNoiseInvertOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kTerrainEditNoiseTilingOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kTerrainEditNoiseTilingOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kTerrainEditNoiseTilingOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kTerrainEditNoiseTilingOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kTerrainEditNoiseStrengthArea
    0.200000,-0.050000,0.781250,0.734375,
    0.200000,0.050000,0.781250,0.750000,
    -0.200000,-0.050000,0.656250,0.734375,
    -0.200000,0.050000,0.656250,0.750000,

    // kTerrainEditNoiseStrengthIcon
    0.050000,-0.050000,1.000000,0.560547,
    0.050000,0.050000,1.000000,0.576172,
    -0.050000,-0.050000,0.968750,0.560547,
    -0.050000,0.050000,0.968750,0.576172,

    // kTerrainEditNoiseHmap
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainWindowNoiseName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainWindowNoiseDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainWindowNoisePinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kTerrainWindowNoisePinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kTerrainWindowNoiseHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kTerrainWindowNoiseSlider
    0.050000,-0.200000,0.562500,0.484375,
    0.050000,0.200000,0.562500,0.546875,
    -0.050000,-0.200000,0.531250,0.484375,
    -0.050000,0.200000,0.531250,0.546875,

    // kTerrainWindowNoiseSave
    0.100000,-0.100000,0.718750,0.578125,
    0.100000,0.100000,0.718750,0.609375,
    -0.100000,-0.100000,0.656250,0.578125,
    -0.100000,0.100000,0.656250,0.609375,

    // kTerrainWindowNoiseSliderText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kTerrainWindowNoiseSliderArea
    0.200000,-0.050000,0.781250,0.734375,
    0.200000,0.050000,0.781250,0.750000,
    -0.200000,-0.050000,0.656250,0.734375,
    -0.200000,0.050000,0.656250,0.750000,

    // kTerrainWindowNoiseSliderIcon
    0.050000,-0.050000,1.000000,0.560547,
    0.050000,0.050000,1.000000,0.576172,
    -0.050000,-0.050000,0.968750,0.560547,
    -0.050000,0.050000,0.968750,0.576172,

    // kWaterWaterMode
    0.100000,-0.100000,0.906250,0.546875,
    0.100000,0.100000,0.906250,0.578125,
    -0.100000,-0.100000,0.843750,0.546875,
    -0.100000,0.100000,0.843750,0.578125,

    // kWaterUpdate
    0.100000,-0.100000,0.718750,0.578125,
    0.100000,0.100000,0.718750,0.609375,
    -0.100000,-0.100000,0.656250,0.578125,
    -0.100000,0.100000,0.656250,0.609375,

    // kWaterLake
    0.100000,-0.100000,0.906250,0.546875,
    0.100000,0.100000,0.906250,0.578125,
    -0.100000,-0.100000,0.843750,0.546875,
    -0.100000,0.100000,0.843750,0.578125,

    // kWaterRiver
    0.100000,-0.100000,0.187500,0.542969,
    0.100000,0.100000,0.187500,0.574219,
    -0.100000,-0.100000,0.125000,0.542969,
    -0.100000,0.100000,0.125000,0.574219,

    // kWaterWaterfall
    0.100000,-0.100000,0.250000,0.539062,
    0.100000,0.100000,0.250000,0.570312,
    -0.100000,-0.100000,0.187500,0.539062,
    -0.100000,0.100000,0.187500,0.570312,

    // kWaterSlotsName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterSlotsConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kWaterVisibleOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kWaterVisibleOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kWaterVisibleOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kWaterVisibleOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kWaterSlotsSlot
    0.175000,-0.075000,0.890625,0.710938,
    0.175000,0.075000,0.890625,0.734375,
    -0.175000,-0.075000,0.781250,0.710938,
    -0.175000,0.075000,0.781250,0.734375,

    // kWaterSlotsSlotColor
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kWaterSlotsRemove
    0.037500,-0.037500,0.046875,0.527344,
    0.037500,0.037500,0.046875,0.539062,
    -0.037500,-0.037500,0.023438,0.527344,
    -0.037500,0.037500,0.023438,0.539062,

    // kWaterSlotsSelected
    0.025000,-0.025000,0.078125,0.531250,
    0.025000,0.025000,0.078125,0.539062,
    -0.025000,-0.025000,0.062500,0.531250,
    -0.025000,0.025000,0.062500,0.539062,

    // kWaterSlotsHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kWaterSlotsSlider
    0.050000,-0.400000,0.531250,0.421875,
    0.050000,0.400000,0.531250,0.546875,
    -0.050000,-0.400000,0.500000,0.421875,
    -0.050000,0.400000,0.500000,0.546875,

    // kWaterSlotsBack
    0.250000,-0.400000,0.156250,0.656250,
    0.250000,0.400000,0.156250,0.781250,
    -0.250000,-0.400000,0.000000,0.656250,
    -0.250000,0.400000,0.000000,0.781250,

    // kWaterSlotsCreate
    0.037500,-0.037500,0.171875,0.531250,
    0.037500,0.037500,0.171875,0.542969,
    -0.037500,-0.037500,0.148438,0.531250,
    -0.037500,0.037500,0.148438,0.542969,

    // kWaterSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kWaterSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kWaterSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.953125,0.531250,
    0.075000,0.075000,0.953125,0.554688,
    -0.075000,-0.075000,0.906250,0.531250,
    -0.075000,0.075000,0.906250,0.554688,

    // kWaterSlotsFlipSelectEdit
    0.025000,-0.100000,0.187500,0.511719,
    0.025000,0.100000,0.187500,0.542969,
    -0.025000,-0.100000,0.171875,0.511719,
    -0.025000,0.100000,0.171875,0.542969,

    // kWaterEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kWaterEditDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kWaterEditDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kWaterEditAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kWaterEditName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterEditNameBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kWaterColorPaletteHS
    0.100000,-0.100000,0.218750,0.667969,
    0.100000,0.100000,0.218750,0.699219,
    -0.100000,-0.100000,0.156250,0.667969,
    -0.100000,0.100000,0.156250,0.699219,

    // kWaterColorPaletteB
    0.100000,-0.025000,0.968750,0.695312,
    0.100000,0.025000,0.968750,0.703125,
    -0.100000,-0.025000,0.906250,0.695312,
    -0.100000,0.025000,0.906250,0.703125,

    // kWaterColorIndicator
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kWaterColorBrightnessCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kWaterColorColorCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kWaterTypeBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kWaterTypeText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterTypePrevious
    0.100000,-0.100000,0.656250,0.632812,
    0.100000,0.100000,0.656250,0.664062,
    -0.100000,-0.100000,0.593750,0.632812,
    -0.100000,0.100000,0.593750,0.664062,

    // kWaterTypeNext
    0.100000,-0.100000,0.968750,0.601562,
    0.100000,0.100000,0.968750,0.632812,
    -0.100000,-0.100000,0.906250,0.601562,
    -0.100000,0.100000,0.906250,0.632812,

    // kWaterLayer1
    0.100000,-0.100000,0.343750,0.632812,
    0.100000,0.100000,0.343750,0.664062,
    -0.100000,-0.100000,0.281250,0.632812,
    -0.100000,0.100000,0.281250,0.664062,

    // kWaterLayer1Window
    0.200000,-0.200000,0.531250,0.687500,
    0.200000,0.200000,0.531250,0.750000,
    -0.200000,-0.200000,0.406250,0.687500,
    -0.200000,0.200000,0.406250,0.750000,

    // kWaterLayer1PinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kWaterLayer1PinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kWaterLayer1Text
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterLayer1Off
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kWaterLayer1On1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kWaterLayer1On2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kWaterLayer1On3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kWater1ScaleLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1ScaleFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1ScaleBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1ScaleHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1FetchLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1FetchFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1FetchBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1FetchHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1SpreadBlendLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1SpreadBlendFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1SpreadBlendBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1SpreadBlendHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1SwellLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1SwellFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1SwellBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1SwellHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1PeakEnhancementLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1PeakEnhancementFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1PeakEnhancementBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1PeakEnhancementHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1ShortWavesFadeLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1ShortWavesFadeFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1ShortWavesFadeBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1ShortWavesFadeHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater1LambdaLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater1LambdaFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater1LambdaBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater1LambdaHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWaterLayer2
    0.100000,-0.100000,0.406250,0.632812,
    0.100000,0.100000,0.406250,0.664062,
    -0.100000,-0.100000,0.343750,0.632812,
    -0.100000,0.100000,0.343750,0.664062,

    // kWaterLayer2Window
    0.200000,-0.200000,0.531250,0.687500,
    0.200000,0.200000,0.531250,0.750000,
    -0.200000,-0.200000,0.406250,0.687500,
    -0.200000,0.200000,0.406250,0.750000,

    // kWaterLayer2PinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kWaterLayer2PinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kWaterLayer2Text
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterLayer2Off
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kWaterLayer2On1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kWaterLayer2On2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kWaterLayer2On3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kWater2ScaleLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2ScaleFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2ScaleBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2ScaleHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2FetchLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2FetchFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2FetchBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2FetchHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2SpreadBlendLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2SpreadBlendFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2SpreadBlendBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2SpreadBlendHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2SwellLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2SwellFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2SwellBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2SwellHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2PeakEnhancementLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2PeakEnhancementFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2PeakEnhancementBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2PeakEnhancementHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2ShortWavesFadeLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2ShortWavesFadeFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2ShortWavesFadeBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2ShortWavesFadeHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater2LambdaLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater2LambdaFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater2LambdaBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater2LambdaHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWaterLayer3
    0.100000,-0.100000,0.593750,0.632812,
    0.100000,0.100000,0.593750,0.664062,
    -0.100000,-0.100000,0.531250,0.632812,
    -0.100000,0.100000,0.531250,0.664062,

    // kWaterLayer3Window
    0.200000,-0.200000,0.531250,0.687500,
    0.200000,0.200000,0.531250,0.750000,
    -0.200000,-0.200000,0.406250,0.687500,
    -0.200000,0.200000,0.406250,0.750000,

    // kWaterLayer3PinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kWaterLayer3PinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kWaterLayer3Text
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWaterLayer3Off
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kWaterLayer3On1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kWaterLayer3On2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kWaterLayer3On3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kWater3ScaleLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3ScaleFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3ScaleBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3ScaleHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3FetchLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3FetchFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3FetchBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3FetchHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3SpreadBlendLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3SpreadBlendFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3SpreadBlendBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3SpreadBlendHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3SwellLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3SwellFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3SwellBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3SwellHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3PeakEnhancementLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3PeakEnhancementFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3PeakEnhancementBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3PeakEnhancementHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3ShortWavesFadeLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3ShortWavesFadeFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3ShortWavesFadeBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3ShortWavesFadeHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kWater3LambdaLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWater3LambdaFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kWater3LambdaBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kWater3LambdaHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kRoadsRoadsMode
    0.100000,-0.100000,0.125000,0.593750,
    0.100000,0.100000,0.125000,0.625000,
    -0.100000,-0.100000,0.062500,0.593750,
    -0.100000,0.100000,0.062500,0.625000,

    // kRoadsAsphalt
    0.100000,-0.100000,0.125000,0.593750,
    0.100000,0.100000,0.125000,0.625000,
    -0.100000,-0.100000,0.062500,0.593750,
    -0.100000,0.100000,0.062500,0.625000,

    // kRoadsGravel
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kRoadsSoil
    0.100000,-0.100000,0.500000,0.593750,
    0.100000,0.100000,0.500000,0.625000,
    -0.100000,-0.100000,0.437500,0.593750,
    -0.100000,0.100000,0.437500,0.625000,

    // kRoadsSlotsName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kRoadsSlotsConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kRoadsVisibleOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kRoadsVisibleOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kRoadsVisibleOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kRoadsVisibleOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kRoadsSlotsSlot
    0.175000,-0.075000,0.890625,0.710938,
    0.175000,0.075000,0.890625,0.734375,
    -0.175000,-0.075000,0.781250,0.710938,
    -0.175000,0.075000,0.781250,0.734375,

    // kRoadsSlotsSlotColor
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kRoadsSlotsRemove
    0.037500,-0.037500,0.046875,0.527344,
    0.037500,0.037500,0.046875,0.539062,
    -0.037500,-0.037500,0.023438,0.527344,
    -0.037500,0.037500,0.023438,0.539062,

    // kRoadsSlotsSelected
    0.025000,-0.025000,0.078125,0.531250,
    0.025000,0.025000,0.078125,0.539062,
    -0.025000,-0.025000,0.062500,0.531250,
    -0.025000,0.025000,0.062500,0.539062,

    // kRoadsSlotsHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kRoadsSlotsSlider
    0.050000,-0.400000,0.531250,0.421875,
    0.050000,0.400000,0.531250,0.546875,
    -0.050000,-0.400000,0.500000,0.421875,
    -0.050000,0.400000,0.500000,0.546875,

    // kRoadsSlotsBack
    0.250000,-0.400000,0.156250,0.656250,
    0.250000,0.400000,0.156250,0.781250,
    -0.250000,-0.400000,0.000000,0.656250,
    -0.250000,0.400000,0.000000,0.781250,

    // kRoadsSlotsCreate
    0.037500,-0.037500,0.171875,0.531250,
    0.037500,0.037500,0.171875,0.542969,
    -0.037500,-0.037500,0.148438,0.531250,
    -0.037500,0.037500,0.148438,0.542969,

    // kRoadsSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kRoadsSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kRoadsSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.953125,0.531250,
    0.075000,0.075000,0.953125,0.554688,
    -0.075000,-0.075000,0.906250,0.531250,
    -0.075000,0.075000,0.906250,0.554688,

    // kRoadsSlotsFlipSelectEdit
    0.025000,-0.100000,0.187500,0.511719,
    0.025000,0.100000,0.187500,0.542969,
    -0.025000,-0.100000,0.171875,0.511719,
    -0.025000,0.100000,0.171875,0.542969,

    // kRoadsEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kRoadsEditPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kRoadsEditPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kRoadsEditAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kRoadsEditName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kRoadsEditNameBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kRoadsColorPaletteHS
    0.100000,-0.100000,0.218750,0.667969,
    0.100000,0.100000,0.218750,0.699219,
    -0.100000,-0.100000,0.156250,0.667969,
    -0.100000,0.100000,0.156250,0.699219,

    // kRoadsColorPaletteB
    0.100000,-0.025000,0.968750,0.695312,
    0.100000,0.025000,0.968750,0.703125,
    -0.100000,-0.025000,0.906250,0.695312,
    -0.100000,0.025000,0.906250,0.703125,

    // kRoadsColorIndicator
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kRoadsColorBrightnessCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kRoadsColorColorCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kRoadsTypeBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kRoadsTypeText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kRoadsTypePrevious
    0.100000,-0.100000,0.656250,0.632812,
    0.100000,0.100000,0.656250,0.664062,
    -0.100000,-0.100000,0.593750,0.632812,
    -0.100000,0.100000,0.593750,0.664062,

    // kRoadsTypeNext
    0.100000,-0.100000,0.968750,0.601562,
    0.100000,0.100000,0.968750,0.632812,
    -0.100000,-0.100000,0.906250,0.601562,
    -0.100000,0.100000,0.906250,0.632812,

    // kFencesFencesMode
    0.100000,-0.100000,0.468750,0.656250,
    0.100000,0.100000,0.468750,0.687500,
    -0.100000,-0.100000,0.406250,0.656250,
    -0.100000,0.100000,0.406250,0.687500,

    // kFencesPicket
    0.100000,-0.100000,0.468750,0.656250,
    0.100000,0.100000,0.468750,0.687500,
    -0.100000,-0.100000,0.406250,0.656250,
    -0.100000,0.100000,0.406250,0.687500,

    // kFencesChainLink
    0.100000,-0.100000,0.531250,0.656250,
    0.100000,0.100000,0.531250,0.687500,
    -0.100000,-0.100000,0.468750,0.656250,
    -0.100000,0.100000,0.468750,0.687500,

    // kFencesWooden
    0.100000,-0.100000,0.843750,0.648438,
    0.100000,0.100000,0.843750,0.679688,
    -0.100000,-0.100000,0.781250,0.648438,
    -0.100000,0.100000,0.781250,0.679688,

    // kFencesSlotsName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kFencesSlotsConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kFencesVisibleOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kFencesVisibleOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kFencesVisibleOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kFencesVisibleOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kFencesSlotsSlot
    0.175000,-0.075000,0.890625,0.710938,
    0.175000,0.075000,0.890625,0.734375,
    -0.175000,-0.075000,0.781250,0.710938,
    -0.175000,0.075000,0.781250,0.734375,

    // kFencesSlotsSlotColor
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kFencesSlotsRemove
    0.037500,-0.037500,0.046875,0.527344,
    0.037500,0.037500,0.046875,0.539062,
    -0.037500,-0.037500,0.023438,0.527344,
    -0.037500,0.037500,0.023438,0.539062,

    // kFencesSlotsSelected
    0.025000,-0.025000,0.078125,0.531250,
    0.025000,0.025000,0.078125,0.539062,
    -0.025000,-0.025000,0.062500,0.531250,
    -0.025000,0.025000,0.062500,0.539062,

    // kFencesSlotsHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kFencesSlotsSlider
    0.050000,-0.400000,0.531250,0.421875,
    0.050000,0.400000,0.531250,0.546875,
    -0.050000,-0.400000,0.500000,0.421875,
    -0.050000,0.400000,0.500000,0.546875,

    // kFencesSlotsBack
    0.250000,-0.400000,0.156250,0.656250,
    0.250000,0.400000,0.156250,0.781250,
    -0.250000,-0.400000,0.000000,0.656250,
    -0.250000,0.400000,0.000000,0.781250,

    // kFencesSlotsCreate
    0.037500,-0.037500,0.171875,0.531250,
    0.037500,0.037500,0.171875,0.542969,
    -0.037500,-0.037500,0.148438,0.531250,
    -0.037500,0.037500,0.148438,0.542969,

    // kFencesSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kFencesSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kFencesSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.953125,0.531250,
    0.075000,0.075000,0.953125,0.554688,
    -0.075000,-0.075000,0.906250,0.531250,
    -0.075000,0.075000,0.906250,0.554688,

    // kFencesSlotsFlipSelectEdit
    0.025000,-0.100000,0.187500,0.511719,
    0.025000,0.100000,0.187500,0.542969,
    -0.025000,-0.100000,0.171875,0.511719,
    -0.025000,0.100000,0.171875,0.542969,

    // kFencesEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kFencesEditPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kFencesEditPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kFencesEditAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kFencesEditName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kFencesEditNameBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kFencesColorPaletteHS
    0.100000,-0.100000,0.218750,0.667969,
    0.100000,0.100000,0.218750,0.699219,
    -0.100000,-0.100000,0.156250,0.667969,
    -0.100000,0.100000,0.156250,0.699219,

    // kFencesColorPaletteB
    0.100000,-0.025000,0.968750,0.695312,
    0.100000,0.025000,0.968750,0.703125,
    -0.100000,-0.025000,0.906250,0.695312,
    -0.100000,0.025000,0.906250,0.703125,

    // kFencesColorIndicator
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kFencesColorBrightnessCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kFencesColorColorCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kFencesTypeBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kFencesTypeText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kFencesTypePrevious
    0.100000,-0.100000,0.656250,0.632812,
    0.100000,0.100000,0.656250,0.664062,
    -0.100000,-0.100000,0.593750,0.632812,
    -0.100000,0.100000,0.593750,0.664062,

    // kFencesTypeNext
    0.100000,-0.100000,0.968750,0.601562,
    0.100000,0.100000,0.968750,0.632812,
    -0.100000,-0.100000,0.906250,0.601562,
    -0.100000,0.100000,0.906250,0.632812,

    // kPlacementPlacementMode
    0.100000,-0.100000,0.968750,0.554688,
    0.100000,0.100000,0.968750,0.585938,
    -0.100000,-0.100000,0.906250,0.554688,
    -0.100000,0.100000,0.906250,0.585938,

    // kPlacementColorFill
    0.050000,-0.200000,0.500000,0.500000,
    0.050000,0.200000,0.500000,0.562500,
    -0.050000,-0.200000,0.468750,0.500000,
    -0.050000,0.200000,0.468750,0.562500,

    // kPlacementColorBack
    0.050000,-0.200000,0.125000,0.500000,
    0.050000,0.200000,0.125000,0.562500,
    -0.050000,-0.200000,0.093750,0.500000,
    -0.050000,0.200000,0.093750,0.562500,

    // kPlacementColorIcon
    0.100000,-0.100000,0.906250,0.671875,
    0.100000,0.100000,0.906250,0.703125,
    -0.100000,-0.100000,0.843750,0.671875,
    -0.100000,0.100000,0.843750,0.703125,

    // kPlacementSizeFill
    0.050000,-0.200000,0.500000,0.500000,
    0.050000,0.200000,0.500000,0.562500,
    -0.050000,-0.200000,0.468750,0.500000,
    -0.050000,0.200000,0.468750,0.562500,

    // kPlacementSizeBack
    0.050000,-0.200000,0.125000,0.500000,
    0.050000,0.200000,0.125000,0.562500,
    -0.050000,-0.200000,0.093750,0.500000,
    -0.050000,0.200000,0.093750,0.562500,

    // kPlacementSizeIcon
    0.100000,-0.100000,0.906250,0.578125,
    0.100000,0.100000,0.906250,0.609375,
    -0.100000,-0.100000,0.843750,0.578125,
    -0.100000,0.100000,0.843750,0.609375,

    // kPlacementFalloffFill
    0.050000,-0.200000,0.500000,0.500000,
    0.050000,0.200000,0.500000,0.562500,
    -0.050000,-0.200000,0.468750,0.500000,
    -0.050000,0.200000,0.468750,0.562500,

    // kPlacementFalloffBack
    0.050000,-0.200000,0.125000,0.500000,
    0.050000,0.200000,0.125000,0.562500,
    -0.050000,-0.200000,0.093750,0.500000,
    -0.050000,0.200000,0.093750,0.562500,

    // kPlacementFalloffIcon
    0.100000,-0.100000,0.968750,0.664062,
    0.100000,0.100000,0.968750,0.695312,
    -0.100000,-0.100000,0.906250,0.664062,
    -0.100000,0.100000,0.906250,0.695312,

    // kPlacementTrees
    0.100000,-0.100000,0.968750,0.554688,
    0.100000,0.100000,0.968750,0.585938,
    -0.100000,-0.100000,0.906250,0.554688,
    -0.100000,0.100000,0.906250,0.585938,

    // kPlacementTreesOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kPlacementTreesOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kPlacementTreesOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kPlacementTreesOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kPlacementBushes
    0.100000,-0.100000,0.953125,0.703125,
    0.100000,0.100000,0.953125,0.734375,
    -0.100000,-0.100000,0.890625,0.703125,
    -0.100000,0.100000,0.890625,0.734375,

    // kPlacementBushesOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kPlacementBushesOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kPlacementBushesOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kPlacementBushesOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kPlacementTallGrass
    0.100000,-0.100000,0.781250,0.640625,
    0.100000,0.100000,0.781250,0.671875,
    -0.100000,-0.100000,0.718750,0.640625,
    -0.100000,0.100000,0.718750,0.671875,

    // kPlacementTallGrassOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kPlacementTallGrassOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kPlacementTallGrassOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kPlacementTallGrassOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kPlacementUndergrowth
    0.100000,-0.100000,0.625000,0.546875,
    0.100000,0.100000,0.625000,0.578125,
    -0.100000,-0.100000,0.562500,0.546875,
    -0.100000,0.100000,0.562500,0.578125,

    // kPlacementUndergrowthOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kPlacementUndergrowthOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kPlacementUndergrowthOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kPlacementUndergrowthOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kPlacementChangeMode
    0.100000,-0.100000,0.718750,0.578125,
    0.100000,0.100000,0.718750,0.609375,
    -0.100000,-0.100000,0.656250,0.578125,
    -0.100000,0.100000,0.656250,0.609375,

    // kPlacementDrawOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kPlacementDrawOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kPlacementDrawOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kPlacementDrawOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kObjectsObjectsMode
    0.100000,-0.100000,0.906250,0.640625,
    0.100000,0.100000,0.906250,0.671875,
    -0.100000,-0.100000,0.843750,0.640625,
    -0.100000,0.100000,0.843750,0.671875,

    // kObjectsMavka
    0.100000,-0.100000,0.125000,0.625000,
    0.100000,0.100000,0.125000,0.656250,
    -0.100000,-0.100000,0.062500,0.625000,
    -0.100000,0.100000,0.062500,0.656250,

    // kObjectsVodyaniy
    0.100000,-0.100000,0.750000,0.546875,
    0.100000,0.100000,0.750000,0.578125,
    -0.100000,-0.100000,0.687500,0.546875,
    -0.100000,0.100000,0.687500,0.578125,

    // kObjectsChugaister
    0.100000,-0.100000,0.656250,0.695312,
    0.100000,0.100000,0.656250,0.726562,
    -0.100000,-0.100000,0.593750,0.695312,
    -0.100000,0.100000,0.593750,0.726562,

    // kObjectsPedestal
    0.100000,-0.100000,0.781250,0.609375,
    0.100000,0.100000,0.781250,0.640625,
    -0.100000,-0.100000,0.718750,0.609375,
    -0.100000,0.100000,0.718750,0.640625,

    // kObjectsCampfire
    0.100000,-0.100000,0.218750,0.699219,
    0.100000,0.100000,0.218750,0.730469,
    -0.100000,-0.100000,0.156250,0.699219,
    -0.100000,0.100000,0.156250,0.730469,

    // kObjectsRoadSign
    0.100000,-0.100000,0.062500,0.593750,
    0.100000,0.100000,0.062500,0.625000,
    -0.100000,-0.100000,0.000000,0.593750,
    -0.100000,0.100000,0.000000,0.625000,

    // kObjectsHuman
    0.100000,-0.100000,0.906250,0.640625,
    0.100000,0.100000,0.906250,0.671875,
    -0.100000,-0.100000,0.843750,0.640625,
    -0.100000,0.100000,0.843750,0.671875,

    // kBiomesBiomesMode
    0.100000,-0.100000,0.843750,0.679688,
    0.100000,0.100000,0.843750,0.710938,
    -0.100000,-0.100000,0.781250,0.679688,
    -0.100000,0.100000,0.781250,0.710938,

    // kBiomesWindDirArea
    0.100000,-0.100000,0.375000,0.570312,
    0.100000,0.100000,0.375000,0.601562,
    -0.100000,-0.100000,0.312500,0.570312,
    -0.100000,0.100000,0.312500,0.601562,

    // kBiomesWindDirHandle
    0.100000,-0.100000,0.562500,0.570312,
    0.100000,0.100000,0.562500,0.601562,
    -0.100000,-0.100000,0.500000,0.570312,
    -0.100000,0.100000,0.500000,0.601562,

    // kBiomesWindDirIcon
    0.100000,-0.100000,0.312500,0.539062,
    0.100000,0.100000,0.312500,0.570312,
    -0.100000,-0.100000,0.250000,0.539062,
    -0.100000,0.100000,0.250000,0.570312,

    // kBiomesSunDirArea
    0.100000,-0.100000,0.375000,0.570312,
    0.100000,0.100000,0.375000,0.601562,
    -0.100000,-0.100000,0.312500,0.570312,
    -0.100000,0.100000,0.312500,0.601562,

    // kBiomesSunDirHandle
    0.100000,-0.100000,0.562500,0.570312,
    0.100000,0.100000,0.562500,0.601562,
    -0.100000,-0.100000,0.500000,0.570312,
    -0.100000,0.100000,0.500000,0.601562,

    // kBiomesSunDirIcon
    0.100000,-0.100000,0.125000,0.562500,
    0.100000,0.100000,0.125000,0.593750,
    -0.100000,-0.100000,0.062500,0.562500,
    -0.100000,0.100000,0.062500,0.593750,

    // kBiomesTimeArea
    0.100000,-0.100000,0.375000,0.570312,
    0.100000,0.100000,0.375000,0.601562,
    -0.100000,-0.100000,0.312500,0.570312,
    -0.100000,0.100000,0.312500,0.601562,

    // kBiomesTimeHandle
    0.100000,-0.100000,0.562500,0.570312,
    0.100000,0.100000,0.562500,0.601562,
    -0.100000,-0.100000,0.500000,0.570312,
    -0.100000,0.100000,0.500000,0.601562,

    // kBiomesTimeIcon
    0.100000,-0.100000,0.843750,0.554688,
    0.100000,0.100000,0.843750,0.585938,
    -0.100000,-0.100000,0.781250,0.554688,
    -0.100000,0.100000,0.781250,0.585938,

    // kBiomesSlotsName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kBiomesSlotsConfig
    0.100000,-0.100000,0.718750,0.640625,
    0.100000,0.100000,0.718750,0.671875,
    -0.100000,-0.100000,0.656250,0.640625,
    -0.100000,0.100000,0.656250,0.671875,

    // kBiomesVisibleOff
    0.100000,-0.100000,0.281250,0.695312,
    0.100000,0.100000,0.281250,0.726562,
    -0.100000,-0.100000,0.218750,0.695312,
    -0.100000,0.100000,0.218750,0.726562,

    // kBiomesVisibleOn1
    0.100000,-0.100000,0.343750,0.695312,
    0.100000,0.100000,0.343750,0.726562,
    -0.100000,-0.100000,0.281250,0.695312,
    -0.100000,0.100000,0.281250,0.726562,

    // kBiomesVisibleOn2
    0.100000,-0.100000,0.406250,0.695312,
    0.100000,0.100000,0.406250,0.726562,
    -0.100000,-0.100000,0.343750,0.695312,
    -0.100000,0.100000,0.343750,0.726562,

    // kBiomesVisibleOn3
    0.100000,-0.100000,0.593750,0.695312,
    0.100000,0.100000,0.593750,0.726562,
    -0.100000,-0.100000,0.531250,0.695312,
    -0.100000,0.100000,0.531250,0.726562,

    // kBiomesSlotsSlot
    0.175000,-0.075000,0.890625,0.710938,
    0.175000,0.075000,0.890625,0.734375,
    -0.175000,-0.075000,0.781250,0.710938,
    -0.175000,0.075000,0.781250,0.734375,

    // kBiomesSlotsSlotColor
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kBiomesSlotsRemove
    0.037500,-0.037500,0.046875,0.527344,
    0.037500,0.037500,0.046875,0.539062,
    -0.037500,-0.037500,0.023438,0.527344,
    -0.037500,0.037500,0.023438,0.539062,

    // kBiomesSlotsSelected
    0.025000,-0.025000,0.078125,0.531250,
    0.025000,0.025000,0.078125,0.539062,
    -0.025000,-0.025000,0.062500,0.531250,
    -0.025000,0.025000,0.062500,0.539062,

    // kBiomesSlotsHandler
    0.025000,-0.050000,0.062500,0.523438,
    0.025000,0.050000,0.062500,0.539062,
    -0.025000,-0.050000,0.046875,0.523438,
    -0.025000,0.050000,0.046875,0.539062,

    // kBiomesSlotsSlider
    0.050000,-0.400000,0.531250,0.421875,
    0.050000,0.400000,0.531250,0.546875,
    -0.050000,-0.400000,0.500000,0.421875,
    -0.050000,0.400000,0.500000,0.546875,

    // kBiomesSlotsBack
    0.250000,-0.400000,0.156250,0.656250,
    0.250000,0.400000,0.156250,0.781250,
    -0.250000,-0.400000,0.000000,0.656250,
    -0.250000,0.400000,0.000000,0.781250,

    // kBiomesSlotsCreate
    0.037500,-0.037500,0.171875,0.531250,
    0.037500,0.037500,0.171875,0.542969,
    -0.037500,-0.037500,0.148438,0.531250,
    -0.037500,0.037500,0.148438,0.542969,

    // kBiomesSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kBiomesSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.023438,0.527344,
    0.037500,0.037500,0.023438,0.539062,
    -0.037500,-0.037500,0.000000,0.527344,
    -0.037500,0.037500,0.000000,0.539062,

    // kBiomesSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.953125,0.531250,
    0.075000,0.075000,0.953125,0.554688,
    -0.075000,-0.075000,0.906250,0.531250,
    -0.075000,0.075000,0.906250,0.554688,

    // kBiomesSlotsFlipSelectEdit
    0.025000,-0.100000,0.187500,0.511719,
    0.025000,0.100000,0.187500,0.542969,
    -0.025000,-0.100000,0.171875,0.511719,
    -0.025000,0.100000,0.171875,0.542969,

    // kBiomesEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kBiomesEditPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kBiomesEditPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kBiomesEditAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kBiomesEditName
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kBiomesEditNameBack
    0.100000,-0.025000,0.625000,0.578125,
    0.100000,0.025000,0.625000,0.585938,
    -0.100000,-0.025000,0.562500,0.578125,
    -0.100000,0.025000,0.562500,0.585938,

    // kBiomesColorPaletteHS
    0.100000,-0.100000,0.218750,0.667969,
    0.100000,0.100000,0.218750,0.699219,
    -0.100000,-0.100000,0.156250,0.667969,
    -0.100000,0.100000,0.156250,0.699219,

    // kBiomesColorPaletteB
    0.100000,-0.025000,0.968750,0.695312,
    0.100000,0.025000,0.968750,0.703125,
    -0.100000,-0.025000,0.906250,0.695312,
    -0.100000,0.025000,0.906250,0.703125,

    // kBiomesColorIndicator
    0.150000,-0.012500,0.250000,0.777344,
    0.150000,0.012500,0.250000,0.781250,
    -0.150000,-0.012500,0.156250,0.777344,
    -0.150000,0.012500,0.156250,0.781250,

    // kBiomesColorBrightnessCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kBiomesColorColorCursor
    0.012500,-0.012500,0.906250,0.542969,
    0.012500,0.012500,0.906250,0.546875,
    -0.012500,-0.012500,0.898438,0.542969,
    -0.012500,0.012500,0.898438,0.546875,

    // kBiomesEditWindFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditWindBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditWindHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditWindIcon
    0.100000,-0.100000,0.312500,0.539062,
    0.100000,0.100000,0.312500,0.570312,
    -0.100000,-0.100000,0.250000,0.539062,
    -0.100000,0.100000,0.250000,0.570312,

    // kBiomesEditSunFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditSunBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditSunHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditSunIcon
    0.100000,-0.100000,0.125000,0.562500,
    0.100000,0.100000,0.125000,0.593750,
    -0.100000,-0.100000,0.062500,0.562500,
    -0.100000,0.100000,0.062500,0.593750,

    // kBiomesEditTimeFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditTimeBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditTimeHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditTimeIcon
    0.100000,-0.100000,0.843750,0.554688,
    0.100000,0.100000,0.843750,0.585938,
    -0.100000,-0.100000,0.781250,0.554688,
    -0.100000,0.100000,0.781250,0.585938,

    // kBiomesEditPrecipitationFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditPrecipitationBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditPrecipitationHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditPrecipitationIcon
    0.100000,-0.100000,0.375000,0.601562,
    0.100000,0.100000,0.375000,0.632812,
    -0.100000,-0.100000,0.312500,0.601562,
    -0.100000,0.100000,0.312500,0.632812,

    // kBiomesEditTemperatureFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditTemperatureBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditTemperatureHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditTemperatureIcon
    0.100000,-0.100000,0.500000,0.562500,
    0.100000,0.100000,0.500000,0.593750,
    -0.100000,-0.100000,0.437500,0.562500,
    -0.100000,0.100000,0.437500,0.593750,

    // kBiomesEditCloudFill
    0.100000,-0.050000,0.968750,0.585938,
    0.100000,0.050000,0.968750,0.601562,
    -0.100000,-0.050000,0.906250,0.585938,
    -0.100000,0.050000,0.906250,0.601562,

    // kBiomesEditCloudBack
    0.100000,-0.050000,0.625000,0.585938,
    0.100000,0.050000,0.625000,0.601562,
    -0.100000,-0.050000,0.562500,0.585938,
    -0.100000,0.050000,0.562500,0.601562,

    // kBiomesEditCloudHandler
    0.100000,-0.100000,0.250000,0.570312,
    0.100000,0.100000,0.250000,0.601562,
    -0.100000,-0.100000,0.187500,0.570312,
    -0.100000,0.100000,0.187500,0.601562,

    // kBiomesEditCloudIcon
    0.100000,-0.100000,0.843750,0.679688,
    0.100000,0.100000,0.843750,0.710938,
    -0.100000,-0.100000,0.781250,0.679688,
    -0.100000,0.100000,0.781250,0.710938,

    // kTilesTilesMode
    0.100000,-0.100000,0.062500,0.625000,
    0.100000,0.100000,0.062500,0.656250,
    -0.100000,-0.100000,0.000000,0.625000,
    -0.100000,0.100000,0.000000,0.656250,

    // kTilesLeft
    0.100000,-0.100000,0.656250,0.632812,
    0.100000,0.100000,0.656250,0.664062,
    -0.100000,-0.100000,0.593750,0.632812,
    -0.100000,0.100000,0.593750,0.664062,

    // kTilesRight
    0.100000,-0.100000,0.968750,0.601562,
    0.100000,0.100000,0.968750,0.632812,
    -0.100000,-0.100000,0.906250,0.601562,
    -0.100000,0.100000,0.906250,0.632812,

    // kTilesUp
    0.100000,-0.100000,0.687500,0.546875,
    0.100000,0.100000,0.687500,0.578125,
    -0.100000,-0.100000,0.625000,0.546875,
    -0.100000,0.100000,0.625000,0.578125,

    // kTilesDown
    0.100000,-0.100000,0.593750,0.664062,
    0.100000,0.100000,0.593750,0.695312,
    -0.100000,-0.100000,0.531250,0.664062,
    -0.100000,0.100000,0.531250,0.695312,

    // kMapTomb
    0.075000,-0.125000,0.468750,0.523438,
    0.075000,0.125000,0.468750,0.562500,
    -0.075000,-0.125000,0.421875,0.523438,
    -0.075000,0.125000,0.421875,0.562500,

    // kMapFlowerRed
    0.075000,-0.075000,0.093750,0.539062,
    0.075000,0.075000,0.093750,0.562500,
    -0.075000,-0.075000,0.046875,0.539062,
    -0.075000,0.075000,0.046875,0.562500,

    // kMapFlowerWhite
    0.075000,-0.075000,0.421875,0.539062,
    0.075000,0.075000,0.421875,0.562500,
    -0.075000,-0.075000,0.375000,0.539062,
    -0.075000,0.075000,0.375000,0.562500,

    // kMapFlowerBlue
    0.075000,-0.075000,0.046875,0.539062,
    0.075000,0.075000,0.046875,0.562500,
    -0.075000,-0.075000,0.000000,0.539062,
    -0.075000,0.075000,0.000000,0.562500,

    // kMapKorn
    0.050000,-0.150000,1.000000,0.640625,
    0.050000,0.150000,1.000000,0.687500,
    -0.050000,-0.150000,0.968750,0.640625,
    -0.050000,0.150000,0.968750,0.687500,

    // kMapSeed
    0.075000,-0.075000,0.843750,0.531250,
    0.075000,0.075000,0.843750,0.554688,
    -0.075000,-0.075000,0.796875,0.531250,
    -0.075000,0.075000,0.796875,0.554688,

    // kMapLeaf
    0.050000,-0.050000,1.000000,0.625000,
    0.050000,0.050000,1.000000,0.640625,
    -0.050000,-0.050000,0.968750,0.625000,
    -0.050000,0.050000,0.968750,0.640625,

    // kMapSelected1
    0.050000,-0.050000,1.000000,0.607422,
    0.050000,0.050000,1.000000,0.623047,
    -0.050000,-0.050000,0.968750,0.607422,
    -0.050000,0.050000,0.968750,0.623047,

    // kMapSelected2
    0.037500,-0.037500,0.898438,0.535156,
    0.037500,0.037500,0.898438,0.546875,
    -0.037500,-0.037500,0.875000,0.535156,
    -0.037500,0.037500,0.875000,0.546875,

    // kMapSelected0
    0.012500,-0.012500,0.906250,0.539062,
    0.012500,0.012500,0.906250,0.542969,
    -0.012500,-0.012500,0.898438,0.539062,
    -0.012500,0.012500,0.898438,0.542969,

    // kMapEdge
    0.050000,-0.006250,1.000000,0.623047,
    0.050000,0.006250,1.000000,0.625000,
    -0.050000,-0.006250,0.968750,0.623047,
    -0.050000,0.006250,0.968750,0.625000,

    // kCautionDesk
    0.400000,-0.200000,0.750000,0.750000,
    0.400000,0.200000,0.750000,0.812500,
    -0.400000,-0.200000,0.500000,0.750000,
    -0.400000,0.200000,0.500000,0.812500,

    // kCautionText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kConfirmationDesk
    0.400000,-0.200000,0.750000,0.750000,
    0.400000,0.200000,0.750000,0.812500,
    -0.400000,-0.200000,0.500000,0.750000,
    -0.400000,0.200000,0.500000,0.812500,

    // kConfirmationText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kConfirmationAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kConfirmationDecline
    0.100000,-0.100000,0.406250,0.664062,
    0.100000,0.100000,0.406250,0.695312,
    -0.100000,-0.100000,0.343750,0.664062,
    -0.100000,0.100000,0.343750,0.695312,

    // kFileDesk
    0.400000,-0.200000,0.750000,0.750000,
    0.400000,0.200000,0.750000,0.812500,
    -0.400000,-0.200000,0.500000,0.750000,
    -0.400000,0.200000,0.500000,0.812500,

    // kFileQuestion
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kFileTextLabel
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kFileTextBack
    0.200000,-0.025000,0.656250,0.726562,
    0.200000,0.025000,0.656250,0.734375,
    -0.200000,-0.025000,0.531250,0.726562,
    -0.200000,0.025000,0.531250,0.734375,

    // kFileAccept
    0.100000,-0.100000,0.970703,0.765625,
    0.100000,0.100000,0.970703,0.796875,
    -0.100000,-0.100000,0.908203,0.765625,
    -0.100000,0.100000,0.908203,0.796875,

    // kFileDecline
    0.100000,-0.100000,0.406250,0.664062,
    0.100000,0.100000,0.406250,0.695312,
    -0.100000,-0.100000,0.343750,0.664062,
    -0.100000,0.100000,0.343750,0.695312,

    // kTipInfoDesk
    0.200000,-0.200000,0.531250,0.687500,
    0.200000,0.200000,0.531250,0.750000,
    -0.200000,-0.200000,0.406250,0.687500,
    -0.200000,0.200000,0.406250,0.750000,

    // kTipInfoDeskPinBack
    0.100000,-0.100000,0.906250,0.609375,
    0.100000,0.100000,0.906250,0.640625,
    -0.100000,-0.100000,0.843750,0.609375,
    -0.100000,0.100000,0.843750,0.640625,

    // kTipInfoDeskPinPoint
    0.100000,-0.100000,0.187500,0.605469,
    0.100000,0.100000,0.187500,0.636719,
    -0.100000,-0.100000,0.125000,0.605469,
    -0.100000,0.100000,0.125000,0.636719,

    // kTipInfoText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kError
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kConsoleText
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kConsoleBack
    0.400000,-0.025000,0.500000,0.804688,
    0.400000,0.025000,0.500000,0.812500,
    -0.400000,-0.025000,0.250000,0.804688,
    -0.400000,0.025000,0.250000,0.812500,

    // kShadow
    0.100000,-0.100000,0.781250,0.578125,
    0.100000,0.100000,0.781250,0.609375,
    -0.100000,-0.100000,0.718750,0.578125,
    -0.100000,0.100000,0.718750,0.609375,

    // kPivot
    0.100000,-0.100000,0.250000,0.601562,
    0.100000,0.100000,0.250000,0.632812,
    -0.100000,-0.100000,0.187500,0.601562,
    -0.100000,0.100000,0.187500,0.632812,

    // kWireframeWindow
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kWireframeWindowFrame
    0.253125,-0.200000,0.908203,0.750000,
    0.253125,0.200000,0.908203,0.812500,
    -0.253125,-0.200000,0.750000,0.750000,
    -0.253125,0.200000,0.750000,0.812500,

    // kWireframeWindowXpos
    0.050000,-0.050000,0.656250,0.531250,
    0.050000,0.050000,0.656250,0.546875,
    -0.050000,-0.050000,0.625000,0.531250,
    -0.050000,0.050000,0.625000,0.546875,

    // kWireframeWindowXneg
    0.050000,-0.050000,0.625000,0.531250,
    0.050000,0.050000,0.625000,0.546875,
    -0.050000,-0.050000,0.593750,0.531250,
    -0.050000,0.050000,0.593750,0.546875,

    // kWireframeWindowYpos
    0.050000,-0.050000,0.718750,0.531250,
    0.050000,0.050000,0.718750,0.546875,
    -0.050000,-0.050000,0.687500,0.531250,
    -0.050000,0.050000,0.687500,0.546875,

    // kWireframeWindowYneg
    0.050000,-0.050000,0.687500,0.531250,
    0.050000,0.050000,0.687500,0.546875,
    -0.050000,-0.050000,0.656250,0.531250,
    -0.050000,0.050000,0.656250,0.546875,

    // kWireframeWindowZpos
    0.050000,-0.050000,0.875000,0.531250,
    0.050000,0.050000,0.875000,0.546875,
    -0.050000,-0.050000,0.843750,0.531250,
    -0.050000,0.050000,0.843750,0.546875,

    // kWireframeWindowZneg
    0.050000,-0.050000,0.750000,0.531250,
    0.050000,0.050000,0.750000,0.546875,
    -0.050000,-0.050000,0.718750,0.531250,
    -0.050000,0.050000,0.718750,0.546875,

    // kTextRendererCursor
    0.050000,-0.050000,0.984375,0.537109,
    0.050000,0.050000,0.984375,0.552734,
    -0.050000,-0.050000,0.953125,0.537109,
    -0.050000,0.050000,0.953125,0.552734,

    // kTextRendererPrerenderSlot
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kCompass
    0.100000,-0.100000,0.281250,0.664062,
    0.100000,0.100000,0.281250,0.695312,
    -0.100000,-0.100000,0.218750,0.664062,
    -0.100000,0.100000,0.218750,0.695312,

    // kCompassNorth
    0.050000,-0.050000,1.000000,0.591797,
    0.050000,0.050000,1.000000,0.607422,
    -0.050000,-0.050000,0.968750,0.591797,
    -0.050000,0.050000,0.968750,0.607422,

    // kCompassSouth
    0.050000,-0.050000,1.000000,0.576172,
    0.050000,0.050000,1.000000,0.591797,
    -0.050000,-0.050000,0.968750,0.576172,
    -0.050000,0.050000,0.968750,0.591797,

    // kCompassWest
    0.050000,-0.050000,0.593750,0.531250,
    0.050000,0.050000,0.593750,0.546875,
    -0.050000,-0.050000,0.562500,0.531250,
    -0.050000,0.050000,0.562500,0.546875,

    // kCompassEast
    0.050000,-0.050000,1.000000,0.687500,
    0.050000,0.050000,1.000000,0.703125,
    -0.050000,-0.050000,0.968750,0.687500,
    -0.050000,0.050000,0.968750,0.703125,

    // kCursorTree
    0.025000,-0.025000,1.000000,0.544922,
    0.025000,0.025000,1.000000,0.552734,
    -0.025000,-0.025000,0.984375,0.544922,
    -0.025000,0.025000,0.984375,0.552734,

    // kLoading0
    0.400000,-0.100000,1.000000,0.968750,
    0.400000,0.100000,1.000000,1.000000,
    -0.400000,-0.100000,0.750000,0.968750,
    -0.400000,0.100000,0.750000,1.000000,

    // kLoading1
    0.400000,-0.100000,1.000000,0.937500,
    0.400000,0.100000,1.000000,0.968750,
    -0.400000,-0.100000,0.750000,0.937500,
    -0.400000,0.100000,0.750000,0.968750,

    // kLoading2
    0.400000,-0.100000,1.000000,0.906250,
    0.400000,0.100000,1.000000,0.937500,
    -0.400000,-0.100000,0.750000,0.906250,
    -0.400000,0.100000,0.750000,0.937500,

    // kLoading3
    0.400000,-0.100000,0.750000,0.875000,
    0.400000,0.100000,0.750000,0.906250,
    -0.400000,-0.100000,0.500000,0.875000,
    -0.400000,0.100000,0.500000,0.906250,

    // kLoading4
    0.400000,-0.100000,1.000000,0.875000,
    0.400000,0.100000,1.000000,0.906250,
    -0.400000,-0.100000,0.750000,0.875000,
    -0.400000,0.100000,0.750000,0.906250,

    // kLoading5
    0.400000,-0.100000,0.750000,0.843750,
    0.400000,0.100000,0.750000,0.875000,
    -0.400000,-0.100000,0.500000,0.843750,
    -0.400000,0.100000,0.500000,0.875000,

    // kLoading6
    0.400000,-0.100000,1.000000,0.843750,
    0.400000,0.100000,1.000000,0.875000,
    -0.400000,-0.100000,0.750000,0.843750,
    -0.400000,0.100000,0.750000,0.875000,

    // kLoading7
    0.400000,-0.100000,0.750000,0.812500,
    0.400000,0.100000,0.750000,0.843750,
    -0.400000,-0.100000,0.500000,0.812500,
    -0.400000,0.100000,0.500000,0.843750,

    // kLoading8
    0.400000,-0.100000,1.000000,0.812500,
    0.400000,0.100000,1.000000,0.843750,
    -0.400000,-0.100000,0.750000,0.812500,
    -0.400000,0.100000,0.750000,0.843750,

    // kLoading9
    0.400000,-0.100000,0.250000,0.781250,
    0.400000,0.100000,0.250000,0.812500,
    -0.400000,-0.100000,0.000000,0.781250,
    -0.400000,0.100000,0.000000,0.812500,

    // kLoading10
    0.400000,-0.100000,0.750000,0.906250,
    0.400000,0.100000,0.750000,0.937500,
    -0.400000,-0.100000,0.500000,0.906250,
    -0.400000,0.100000,0.500000,0.937500,

    // kLogoBig
    0.200000,-0.200000,0.781250,0.671875,
    0.200000,0.200000,0.781250,0.734375,
    -0.200000,-0.200000,0.656250,0.671875,
    -0.200000,0.200000,0.656250,0.734375,

    // kLogoMedium
    0.100000,-0.100000,0.375000,0.539062,
    0.100000,0.100000,0.375000,0.570312,
    -0.100000,-0.100000,0.312500,0.539062,
    -0.100000,0.100000,0.312500,0.570312,

    // kLogoSmall
    0.025000,-0.025000,0.093750,0.531250,
    0.025000,0.025000,0.093750,0.539062,
    -0.025000,-0.025000,0.078125,0.531250,
    -0.025000,0.025000,0.078125,0.539062,

    // kSpare1
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare2
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare3
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare4
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare5
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare6
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare7
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare8
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare9
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,

    // kSpare10
    0.250000,-0.250000,1.000000,0.000000,
    0.250000,0.250000,1.000000,1.000000,
    -0.250000,-0.250000,0.000000,0.000000,
    -0.250000,0.250000,0.000000,1.000000,
};

} // namespace vbos
