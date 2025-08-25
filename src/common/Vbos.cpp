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
  "kCompass",
  "kCursorTree",
  "kCross",
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
  "kTerrainSlotsFlipPointEdgeFace_Back",
  "kTerrainSlotsFlipPointEdgeFace",

  "kTerrainBakeDesk",
  "kTerrainBakeDeskPinBack",
  "kTerrainBakeDeskPinPoint",
  "kTerrainBakeAccept",
  "kTerrainBakeErosionStepLabel",
  "kTerrainBakeErosionStepInput",
  "kTerrainBakeWeatheringStepLabel",
  "kTerrainBakeWeatheringStepInput",
  "kTerrainBakeHmap",
  "kTerrainBakeNmap",
  "kTerrainBakeAo",
  "kTerrainBakeSlopemap",
  "kTerrainBakeSplatmap",
  "kTerrainBakeErosionThermal",
  "kTerrainBakeErosionHydraulic",
  "kTerrainBakeWaterAccum",
  "kTerrainBakeWaterFlow",


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

  "kTerrainEditTranslateXText",
  "kTerrainEditTranslateXTextSlot",
  "kTerrainEditTranslateYText",
  "kTerrainEditTranslateYTextSlot",
  "kTerrainEditTranslateZText",
  "kTerrainEditTranslateZTextSlot",
  "kTerrainEditScaleXText",
  "kTerrainEditScaleXTextSlot",
  "kTerrainEditScaleYText",
  "kTerrainEditScaleYTextSlot",
  "kTerrainEditScaleZText",
  "kTerrainEditScaleZTextSlot",
  "kTerrainEditRotateXText",
  "kTerrainEditRotateXTextSlot",
  "kTerrainEditRotateYText",
  "kTerrainEditRotateYTextSlot",
  "kTerrainEditRotateZText",
  "kTerrainEditRotateZTextSlot",

  "kTerrainEditLabelInvert",
  "kTerrainEditLabelTiling",
  "kTerrainEditLabelStrength",

  "kTerrainEditNoise1Config",
  "kTerrainEditNoise1Name",
  "kTerrainEditNoise1InvertOff",
  "kTerrainEditNoise1InvertOn1",
  "kTerrainEditNoise1InvertOn2",
  "kTerrainEditNoise1InvertOn3",
  "kTerrainEditNoise1TilingOff",
  "kTerrainEditNoise1TilingOn1",
  "kTerrainEditNoise1TilingOn2",
  "kTerrainEditNoise1TilingOn3",
  "kTerrainEditNoise1StrengthArea",
  "kTerrainEditNoise1StrengthIcon",
  "kTerrainEditNoise1Hmap",

  "kTerrainEditNoise2Config",
  "kTerrainEditNoise2Name",
  "kTerrainEditNoise2InvertOff",
  "kTerrainEditNoise2InvertOn1",
  "kTerrainEditNoise2InvertOn2",
  "kTerrainEditNoise2InvertOn3",
  "kTerrainEditNoise2TilingOff",
  "kTerrainEditNoise2TilingOn1",
  "kTerrainEditNoise2TilingOn2",
  "kTerrainEditNoise2TilingOn3",
  "kTerrainEditNoise2StrengthArea",
  "kTerrainEditNoise2StrengthIcon",
  "kTerrainEditNoise2Hmap",

  "kTerrainEditNoise3Config",
  "kTerrainEditNoise3Name",
  "kTerrainEditNoise3InvertOff",
  "kTerrainEditNoise3InvertOn1",
  "kTerrainEditNoise3InvertOn2",
  "kTerrainEditNoise3InvertOn3",
  "kTerrainEditNoise3TilingOff",
  "kTerrainEditNoise3TilingOn1",
  "kTerrainEditNoise3TilingOn2",
  "kTerrainEditNoise3TilingOn3",
  "kTerrainEditNoise3StrengthArea",
  "kTerrainEditNoise3StrengthIcon",
  "kTerrainEditNoise3Hmap",

  "kTerrainEditNoise4Config",
  "kTerrainEditNoise4Name",
  "kTerrainEditNoise4InvertOff",
  "kTerrainEditNoise4InvertOn1",
  "kTerrainEditNoise4InvertOn2",
  "kTerrainEditNoise4InvertOn3",
  "kTerrainEditNoise4TilingOff",
  "kTerrainEditNoise4TilingOn1",
  "kTerrainEditNoise4TilingOn2",
  "kTerrainEditNoise4TilingOn3",
  "kTerrainEditNoise4StrengthArea",
  "kTerrainEditNoise4StrengthIcon",
  "kTerrainEditNoise4Hmap",

  "kTerrainEditNoise5Config",
  "kTerrainEditNoise5Name",
  "kTerrainEditNoise5InvertOff",
  "kTerrainEditNoise5InvertOn1",
  "kTerrainEditNoise5InvertOn2",
  "kTerrainEditNoise5InvertOn3",
  "kTerrainEditNoise5TilingOff",
  "kTerrainEditNoise5TilingOn1",
  "kTerrainEditNoise5TilingOn2",
  "kTerrainEditNoise5TilingOn3",
  "kTerrainEditNoise5StrengthArea",
  "kTerrainEditNoise5StrengthIcon",
  "kTerrainEditNoise5Hmap",

  "kTerrainEditNoise6Config",
  "kTerrainEditNoise6Name",
  "kTerrainEditNoise6InvertOff",
  "kTerrainEditNoise6InvertOn1",
  "kTerrainEditNoise6InvertOn2",
  "kTerrainEditNoise6InvertOn3",
  "kTerrainEditNoise6TilingOff",
  "kTerrainEditNoise6TilingOn1",
  "kTerrainEditNoise6TilingOn2",
  "kTerrainEditNoise6TilingOn3",
  "kTerrainEditNoise6StrengthArea",
  "kTerrainEditNoise6StrengthIcon",
  "kTerrainEditNoise6Hmap",

  "kTerrainEditNoise7Config",
  "kTerrainEditNoise7Name",
  "kTerrainEditNoise7InvertOff",
  "kTerrainEditNoise7InvertOn1",
  "kTerrainEditNoise7InvertOn2",
  "kTerrainEditNoise7InvertOn3",
  "kTerrainEditNoise7TilingOff",
  "kTerrainEditNoise7TilingOn1",
  "kTerrainEditNoise7TilingOn2",
  "kTerrainEditNoise7TilingOn3",
  "kTerrainEditNoise7StrengthArea",
  "kTerrainEditNoise7StrengthIcon",
  "kTerrainEditNoise7Hmap",

  "kTerrainEditNoise8Config",
  "kTerrainEditNoise8Name",
  "kTerrainEditNoise8InvertOff",
  "kTerrainEditNoise8InvertOn1",
  "kTerrainEditNoise8InvertOn2",
  "kTerrainEditNoise8InvertOn3",
  "kTerrainEditNoise8TilingOff",
  "kTerrainEditNoise8TilingOn1",
  "kTerrainEditNoise8TilingOn2",
  "kTerrainEditNoise8TilingOn3",
  "kTerrainEditNoise8StrengthArea",
  "kTerrainEditNoise8StrengthIcon",
  "kTerrainEditNoise8Hmap",

  "kTerrainNoisePerlinName",
  "kTerrainNoisePerlinDesk",
  "kTerrainNoisePerlinPinBack",
  "kTerrainNoisePerlinPinPoint",
  "kTerrainNoisePerlinSave",
  "kTerrainNoisePerlinScaleXText",
  "kTerrainNoisePerlinScaleXArea",
  "kTerrainNoisePerlinScaleXIcon",
  "kTerrainNoisePerlinScaleYText",
  "kTerrainNoisePerlinScaleYArea",
  "kTerrainNoisePerlinScaleYIcon",
  "kTerrainNoisePerlinSeedText",
  "kTerrainNoisePerlinSeedArea",
  "kTerrainNoisePerlinSeedIcon",

  "kTerrainNoiseCellularName",
  "kTerrainNoiseCellularDesk",
  "kTerrainNoiseCellularPinBack",
  "kTerrainNoiseCellularPinPoint",
  "kTerrainNoiseCellularSave",
  "kTerrainNoiseCellularScaleXText",
  "kTerrainNoiseCellularScaleXArea",
  "kTerrainNoiseCellularScaleXIcon",
  "kTerrainNoiseCellularScaleYText",
  "kTerrainNoiseCellularScaleYArea",
  "kTerrainNoiseCellularScaleYIcon",
  "kTerrainNoiseCellularJitterText",
  "kTerrainNoiseCellularJitterArea",
  "kTerrainNoiseCellularJitterIcon",
  "kTerrainNoiseCellularSeedText",
  "kTerrainNoiseCellularSeedArea",
  "kTerrainNoiseCellularSeedIcon",

  "kTerrainNoiseMetaballsName",
  "kTerrainNoiseMetaballsDesk",
  "kTerrainNoiseMetaballsPinBack",
  "kTerrainNoiseMetaballsPinPoint",
  "kTerrainNoiseMetaballsSave",
  "kTerrainNoiseMetaballsScaleXText",
  "kTerrainNoiseMetaballsScaleXArea",
  "kTerrainNoiseMetaballsScaleXIcon",
  "kTerrainNoiseMetaballsScaleYText",
  "kTerrainNoiseMetaballsScaleYArea",
  "kTerrainNoiseMetaballsScaleYIcon",
  "kTerrainNoiseMetaballsJitterText",
  "kTerrainNoiseMetaballsJitterArea",
  "kTerrainNoiseMetaballsJitterIcon",
  "kTerrainNoiseMetaballsSeedText",
  "kTerrainNoiseMetaballsSeedArea",
  "kTerrainNoiseMetaballsSeedIcon",

  "kTerrainNoiseFbmGridName",
  "kTerrainNoiseFbmGridDesk",
  "kTerrainNoiseFbmGridPinBack",
  "kTerrainNoiseFbmGridPinPoint",
  "kTerrainNoiseFbmGridSave",
  "kTerrainNoiseFbmGridScaleXText",
  "kTerrainNoiseFbmGridScaleXArea",
  "kTerrainNoiseFbmGridScaleXIcon",
  "kTerrainNoiseFbmGridScaleYText",
  "kTerrainNoiseFbmGridScaleYArea",
  "kTerrainNoiseFbmGridScaleYIcon",
  "kTerrainNoiseFbmGridOctavesText",
  "kTerrainNoiseFbmGridOctavesArea",
  "kTerrainNoiseFbmGridOctavesIcon",
  "kTerrainNoiseFbmGridShiftText",
  "kTerrainNoiseFbmGridShiftArea",
  "kTerrainNoiseFbmGridShiftIcon",
  "kTerrainNoiseFbmGridGainText",
  "kTerrainNoiseFbmGridGainArea",
  "kTerrainNoiseFbmGridGainIcon",
  "kTerrainNoiseFbmGridLacunarityText",
  "kTerrainNoiseFbmGridLacunarityArea",
  "kTerrainNoiseFbmGridLacunarityIcon",
  "kTerrainNoiseFbmGridWarpStrengthText",
  "kTerrainNoiseFbmGridWarpStrengthArea",
  "kTerrainNoiseFbmGridWarpStrengthIcon",
  "kTerrainNoiseFbmGridOctaveFactorText",
  "kTerrainNoiseFbmGridOctaveFactorArea",
  "kTerrainNoiseFbmGridOctaveFactorIcon",
  "kTerrainNoiseFbmGridSeedText",
  "kTerrainNoiseFbmGridSeedArea",
  "kTerrainNoiseFbmGridSeedIcon",

  "kTerrainNoiseFbmMultiName",
  "kTerrainNoiseFbmMultiDesk",
  "kTerrainNoiseFbmMultiPinBack",
  "kTerrainNoiseFbmMultiPinPoint",
  "kTerrainNoiseFbmMultiSave",
  "kTerrainNoiseFbmMultiScaleXText",
  "kTerrainNoiseFbmMultiScaleXArea",
  "kTerrainNoiseFbmMultiScaleXIcon",
  "kTerrainNoiseFbmMultiScaleYText",
  "kTerrainNoiseFbmMultiScaleYArea",
  "kTerrainNoiseFbmMultiScaleYIcon",
  "kTerrainNoiseFbmMultiLacunarityText",
  "kTerrainNoiseFbmMultiLacunarityArea",
  "kTerrainNoiseFbmMultiLacunarityIcon",
  "kTerrainNoiseFbmMultiOctavesText",
  "kTerrainNoiseFbmMultiOctavesArea",
  "kTerrainNoiseFbmMultiOctavesIcon",
  "kTerrainNoiseFbmMultiSeedText",
  "kTerrainNoiseFbmMultiSeedArea",
  "kTerrainNoiseFbmMultiSeedIcon",

  "kTerrainNoiseFbmdPerlinName",
  "kTerrainNoiseFbmdPerlinDesk",
  "kTerrainNoiseFbmdPerlinPinBack",
  "kTerrainNoiseFbmdPerlinPinPoint",
  "kTerrainNoiseFbmdPerlinSave",
  "kTerrainNoiseFbmdPerlinScaleXText",
  "kTerrainNoiseFbmdPerlinScaleXArea",
  "kTerrainNoiseFbmdPerlinScaleXIcon",
  "kTerrainNoiseFbmdPerlinScaleYText",
  "kTerrainNoiseFbmdPerlinScaleYArea",
  "kTerrainNoiseFbmdPerlinScaleYIcon",
  "kTerrainNoiseFbmdPerlinOctavesText",
  "kTerrainNoiseFbmdPerlinOctavesArea",
  "kTerrainNoiseFbmdPerlinOctavesIcon",
  "kTerrainNoiseFbmdPerlinGainText",
  "kTerrainNoiseFbmdPerlinGainArea",
  "kTerrainNoiseFbmdPerlinGainIcon",
  "kTerrainNoiseFbmdPerlinLacunarityText",
  "kTerrainNoiseFbmdPerlinLacunarityArea",
  "kTerrainNoiseFbmdPerlinLacunarityIcon",
  "kTerrainNoiseFbmdPerlinSlopenessText",
  "kTerrainNoiseFbmdPerlinSlopenessArea",
  "kTerrainNoiseFbmdPerlinSlopenessIcon",
  "kTerrainNoiseFbmdPerlinOctaveFactorText",
  "kTerrainNoiseFbmdPerlinOctaveFactorArea",
  "kTerrainNoiseFbmdPerlinOctaveFactorIcon",
  "kTerrainNoiseFbmdPerlinNegativeText",
  "kTerrainNoiseFbmdPerlinNegativeOff",
  "kTerrainNoiseFbmdPerlinNegativeOn1",
  "kTerrainNoiseFbmdPerlinNegativeOn2",
  "kTerrainNoiseFbmdPerlinNegativeOn3",
  "kTerrainNoiseFbmdPerlinSeedText",
  "kTerrainNoiseFbmdPerlinSeedArea",
  "kTerrainNoiseFbmdPerlinSeedIcon",

  "kTerrainNoiseFbmWarpName",
  "kTerrainNoiseFbmWarpDesk",
  "kTerrainNoiseFbmWarpPinBack",
  "kTerrainNoiseFbmWarpPinPoint",
  "kTerrainNoiseFbmWarpSave",
  "kTerrainNoiseFbmWarpScaleXText",
  "kTerrainNoiseFbmWarpScaleXArea",
  "kTerrainNoiseFbmWarpScaleXIcon",
  "kTerrainNoiseFbmWarpScaleYText",
  "kTerrainNoiseFbmWarpScaleYArea",
  "kTerrainNoiseFbmWarpScaleYIcon",
  "kTerrainNoiseFbmWarpOctavesText",
  "kTerrainNoiseFbmWarpOctavesArea",
  "kTerrainNoiseFbmWarpOctavesIcon",
  "kTerrainNoiseFbmWarpGainText",
  "kTerrainNoiseFbmWarpGainArea",
  "kTerrainNoiseFbmWarpGainIcon",
  "kTerrainNoiseFbmWarpLacunarityText",
  "kTerrainNoiseFbmWarpLacunarityArea",
  "kTerrainNoiseFbmWarpLacunarityIcon",
  "kTerrainNoiseFbmWarpSlopenessText",
  "kTerrainNoiseFbmWarpSlopenessArea",
  "kTerrainNoiseFbmWarpSlopenessIcon",
  "kTerrainNoiseFbmWarpOctaveFactorText",
  "kTerrainNoiseFbmWarpOctaveFactorArea",
  "kTerrainNoiseFbmWarpOctaveFactorIcon",
  "kTerrainNoiseFbmWarpNegativeText",
  "kTerrainNoiseFbmWarpNegativeOff",
  "kTerrainNoiseFbmWarpNegativeOn1",
  "kTerrainNoiseFbmWarpNegativeOn2",
  "kTerrainNoiseFbmWarpNegativeOn3",
  "kTerrainNoiseFbmWarpSeedText",
  "kTerrainNoiseFbmWarpSeedArea",
  "kTerrainNoiseFbmWarpSeedIcon",
  "kTerrainNoiseFbmWarpQText",
  "kTerrainNoiseFbmWarpQArea",
  "kTerrainNoiseFbmWarpQIcon",
  "kTerrainNoiseFbmWarpRText",
  "kTerrainNoiseFbmWarpRArea",
  "kTerrainNoiseFbmWarpRIcon",

  "kTerrainNoiseFbmPerlinWarpName",
  "kTerrainNoiseFbmPerlinWarpDesk",
  "kTerrainNoiseFbmPerlinWarpPinBack",
  "kTerrainNoiseFbmPerlinWarpPinPoint",
  "kTerrainNoiseFbmPerlinWarpSave",
  "kTerrainNoiseFbmPerlinWarpScaleXText",
  "kTerrainNoiseFbmPerlinWarpScaleXArea",
  "kTerrainNoiseFbmPerlinWarpScaleXIcon",
  "kTerrainNoiseFbmPerlinWarpScaleYText",
  "kTerrainNoiseFbmPerlinWarpScaleYArea",
  "kTerrainNoiseFbmPerlinWarpScaleYIcon",
  "kTerrainNoiseFbmPerlinWarpOctavesText",
  "kTerrainNoiseFbmPerlinWarpOctavesArea",
  "kTerrainNoiseFbmPerlinWarpOctavesIcon",
  "kTerrainNoiseFbmPerlinWarpGainText",
  "kTerrainNoiseFbmPerlinWarpGainArea",
  "kTerrainNoiseFbmPerlinWarpGainIcon",
  "kTerrainNoiseFbmPerlinWarpLacunarityText",
  "kTerrainNoiseFbmPerlinWarpLacunarityArea",
  "kTerrainNoiseFbmPerlinWarpLacunarityIcon",
  "kTerrainNoiseFbmPerlinWarpSlopenessText",
  "kTerrainNoiseFbmPerlinWarpSlopenessArea",
  "kTerrainNoiseFbmPerlinWarpSlopenessIcon",
  "kTerrainNoiseFbmPerlinWarpOctaveFactorText",
  "kTerrainNoiseFbmPerlinWarpOctaveFactorArea",
  "kTerrainNoiseFbmPerlinWarpOctaveFactorIcon",
  "kTerrainNoiseFbmPerlinWarpNegativeText",
  "kTerrainNoiseFbmPerlinWarpNegativeOff",
  "kTerrainNoiseFbmPerlinWarpNegativeOn1",
  "kTerrainNoiseFbmPerlinWarpNegativeOn2",
  "kTerrainNoiseFbmPerlinWarpNegativeOn3",
  "kTerrainNoiseFbmPerlinWarpSeedText",
  "kTerrainNoiseFbmPerlinWarpSeedArea",
  "kTerrainNoiseFbmPerlinWarpSeedIcon",
  "kTerrainNoiseFbmPerlinWarpQText",
  "kTerrainNoiseFbmPerlinWarpQArea",
  "kTerrainNoiseFbmPerlinWarpQIcon",
  "kTerrainNoiseFbmPerlinWarpRText",
  "kTerrainNoiseFbmPerlinWarpRArea",
  "kTerrainNoiseFbmPerlinWarpRIcon",


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

  "kConfirmationDesk",
  "kConfirmationTextLabel",
  "kConfirmationAccept",
  "kConfirmationDecline",

  "kTipInfoDesk",
  "kTipInfoDeskPinBack",
  "kTipInfoDeskPinPoint",
  "kTipInfoText",

  "kFileDesk",
  "kFileText",
  "kFileTextInput",
  "kFileAccept",

  "kError",
  "kConsole",

  "kSpareText1",
  "kSpareText2",
  "kSpareText3",
  "kSpareText4",
  "kSpareText5",
  "kSpareText6",
  "kSpareText7",
  "kSpareText8",
  "kSpareText9",
  "kSpareText10",
  "kFullScreen",
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

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,

    0.0,0.0,1.0,
    0.0,0.0,1.0,
    0.0,0.0,1.0,
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
    0.100000,-0.100000,0.125000,0.683594,
    0.100000,0.100000,0.125000,0.714844,
    -0.100000,-0.100000,0.062500,0.683594,
    -0.100000,0.100000,0.062500,0.714844,

    // kMenuTerrainOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuTerrainOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuTerrainOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuTerrainOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuWater
    0.100000,-0.100000,0.875000,0.578125,
    0.100000,0.100000,0.875000,0.609375,
    -0.100000,-0.100000,0.812500,0.578125,
    -0.100000,0.100000,0.812500,0.609375,

    // kMenuWaterOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuWaterOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuWaterOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuWaterOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuRoads
    0.100000,-0.100000,0.062500,0.617188,
    0.100000,0.100000,0.062500,0.648438,
    -0.100000,-0.100000,0.000000,0.617188,
    -0.100000,0.100000,0.000000,0.648438,

    // kMenuRoadsOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuRoadsOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuRoadsOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuRoadsOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuFences
    0.100000,-0.100000,0.312500,0.671875,
    0.100000,0.100000,0.312500,0.703125,
    -0.100000,-0.100000,0.250000,0.671875,
    -0.100000,0.100000,0.250000,0.703125,

    // kMenuFencesOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuFencesOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuFencesOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuFencesOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuPlacement
    0.100000,-0.100000,0.312500,0.578125,
    0.100000,0.100000,0.312500,0.609375,
    -0.100000,-0.100000,0.250000,0.578125,
    -0.100000,0.100000,0.250000,0.609375,

    // kMenuPlacementOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuPlacementOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuPlacementOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuPlacementOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuObjects
    0.100000,-0.100000,0.500000,0.656250,
    0.100000,0.100000,0.500000,0.687500,
    -0.100000,-0.100000,0.437500,0.656250,
    -0.100000,0.100000,0.437500,0.687500,

    // kMenuObjectsOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuObjectsOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuObjectsOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuObjectsOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuBiomes
    0.100000,-0.100000,0.718750,0.703125,
    0.100000,0.100000,0.718750,0.734375,
    -0.100000,-0.100000,0.656250,0.703125,
    -0.100000,0.100000,0.656250,0.734375,

    // kMenuBiomesOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuBiomesOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuBiomesOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuBiomesOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuTiles
    0.100000,-0.100000,0.812500,0.648438,
    0.100000,0.100000,0.812500,0.679688,
    -0.100000,-0.100000,0.750000,0.648438,
    -0.100000,0.100000,0.750000,0.679688,

    // kMenuTilesOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuTilesOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuTilesOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuTilesOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuShaderWirebound
    0.100000,-0.100000,0.562500,0.562500,
    0.100000,0.100000,0.562500,0.593750,
    -0.100000,-0.100000,0.500000,0.562500,
    -0.100000,0.100000,0.500000,0.593750,

    // kMenuShadersOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kMenuShadersOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kMenuShadersOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kMenuShadersOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kMenuDesk
    0.250000,-0.250000,0.406250,0.734375,
    0.250000,0.250000,0.406250,0.812500,
    -0.250000,-0.250000,0.250000,0.734375,
    -0.250000,0.250000,0.250000,0.812500,

    // kMenuDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kMenuDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kMenuArrowSelect
    0.037500,-0.225000,0.859375,0.507812,
    0.037500,0.225000,0.859375,0.578125,
    -0.037500,-0.225000,0.835938,0.507812,
    -0.037500,0.225000,0.835938,0.578125,

    // kMenuArrowSelected
    0.025000,-0.275000,1.000000,0.507812,
    0.025000,0.275000,1.000000,0.593750,
    -0.025000,-0.275000,0.984375,0.507812,
    -0.025000,0.275000,0.984375,0.593750,

    // kMenuSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kMenuLoad
    0.100000,-0.100000,0.062500,0.648438,
    0.100000,0.100000,0.062500,0.679688,
    -0.100000,-0.100000,0.000000,0.648438,
    -0.100000,0.100000,0.000000,0.679688,

    // kSettingsDesk
    0.150000,-0.150000,0.656250,0.703125,
    0.150000,0.150000,0.656250,0.750000,
    -0.150000,-0.150000,0.562500,0.703125,
    -0.150000,0.150000,0.562500,0.750000,

    // kSettingsDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kSettingsDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kSettingsResolutionLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSettingsResolutionLeft
    0.100000,-0.100000,0.062500,0.679688,
    0.100000,0.100000,0.062500,0.710938,
    -0.100000,-0.100000,0.000000,0.679688,
    -0.100000,0.100000,0.000000,0.710938,

    // kSettingsResolutionRight
    0.100000,-0.100000,0.625000,0.578125,
    0.100000,0.100000,0.625000,0.609375,
    -0.100000,-0.100000,0.562500,0.578125,
    -0.100000,0.100000,0.562500,0.609375,

    // kSettingsResolution
    0.100000,-0.100000,1.000000,0.625000,
    0.100000,0.100000,1.000000,0.656250,
    -0.100000,-0.100000,0.937500,0.625000,
    -0.100000,0.100000,0.937500,0.656250,

    // kSettingsFullScreenOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kSettingsFullScreenOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kSettingsFullScreenOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kSettingsFullScreenOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kSettingsSensitivityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kSettingsSensitivityHandle
    0.100000,-0.100000,0.375000,0.640625,
    0.100000,0.100000,0.375000,0.671875,
    -0.100000,-0.100000,0.312500,0.640625,
    -0.100000,0.100000,0.312500,0.671875,

    // kSettingsKeyboard
    0.100000,-0.100000,0.562500,0.656250,
    0.100000,0.100000,0.562500,0.687500,
    -0.100000,-0.100000,0.500000,0.656250,
    -0.100000,0.100000,0.500000,0.687500,

    // kSettingsSoundArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kSettingsSoundHandle
    0.100000,-0.100000,1.000000,0.593750,
    0.100000,0.100000,1.000000,0.625000,
    -0.100000,-0.100000,0.937500,0.593750,
    -0.100000,0.100000,0.937500,0.625000,

    // kSettingsSoundOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kSettingsSoundOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kSettingsSoundOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kSettingsSoundOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kSettingsMusicArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kSettingsMusicHandle
    0.100000,-0.100000,0.625000,0.640625,
    0.100000,0.100000,0.625000,0.671875,
    -0.100000,-0.100000,0.562500,0.640625,
    -0.100000,0.100000,0.562500,0.671875,

    // kSettingsMusicOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kSettingsMusicOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kSettingsMusicOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kSettingsMusicOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kSettingsTipInfoLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSettingsTipInfo
    0.100000,-0.100000,0.937500,0.625000,
    0.100000,0.100000,0.937500,0.656250,
    -0.100000,-0.100000,0.875000,0.625000,
    -0.100000,0.100000,0.875000,0.656250,

    // kSettingsTipInfoOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kSettingsTipInfoOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kSettingsTipInfoOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kSettingsTipInfoOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kModeTextBack
    0.125000,-0.050000,0.734375,0.734375,
    0.125000,0.050000,0.734375,0.750000,
    -0.125000,-0.050000,0.656250,0.734375,
    -0.125000,0.050000,0.656250,0.750000,

    // kModeTextLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kCompass
    0.200000,-0.200000,0.687500,0.750000,
    0.200000,0.200000,0.687500,0.812500,
    -0.200000,-0.200000,0.562500,0.750000,
    -0.200000,0.200000,0.562500,0.812500,

    // kCursorTree
    0.025000,-0.025000,0.750000,0.585938,
    0.025000,0.025000,0.750000,0.593750,
    -0.025000,-0.025000,0.734375,0.585938,
    -0.025000,0.025000,0.734375,0.593750,

    // kCross
    0.100000,-0.100000,0.968750,0.687500,
    0.100000,0.100000,0.968750,0.718750,
    -0.100000,-0.100000,0.906250,0.687500,
    -0.100000,0.100000,0.906250,0.718750,

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
    0.200000,-0.200000,0.937500,0.718750,
    0.200000,0.200000,0.937500,0.781250,
    -0.200000,-0.200000,0.812500,0.718750,
    -0.200000,0.200000,0.812500,0.781250,

    // kLogoMedium
    0.100000,-0.100000,0.562500,0.562500,
    0.100000,0.100000,0.562500,0.593750,
    -0.100000,-0.100000,0.500000,0.562500,
    -0.100000,0.100000,0.500000,0.593750,

    // kLogoSmall
    0.025000,-0.025000,0.343750,0.568359,
    0.025000,0.025000,0.343750,0.576172,
    -0.025000,-0.025000,0.328125,0.568359,
    -0.025000,0.025000,0.328125,0.576172,

    // kTerrainTerrainMode
    0.100000,-0.100000,0.125000,0.683594,
    0.100000,0.100000,0.125000,0.714844,
    -0.100000,-0.100000,0.062500,0.683594,
    -0.100000,0.100000,0.062500,0.714844,

    // kTerrainUpdate
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainFlatten
    0.100000,-0.100000,0.312500,0.609375,
    0.100000,0.100000,0.312500,0.640625,
    -0.100000,-0.100000,0.250000,0.609375,
    -0.100000,0.100000,0.250000,0.640625,

    // kTerrainFlattenOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainFlattenOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainFlattenOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainFlattenOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainSizeFill
    0.050000,-0.200000,0.656250,0.515625,
    0.050000,0.200000,0.656250,0.578125,
    -0.050000,-0.200000,0.625000,0.515625,
    -0.050000,0.200000,0.625000,0.578125,

    // kTerrainSizeBack
    0.050000,-0.200000,0.625000,0.515625,
    0.050000,0.200000,0.625000,0.578125,
    -0.050000,-0.200000,0.593750,0.515625,
    -0.050000,0.200000,0.593750,0.578125,

    // kTerrainSizeIcon
    0.100000,-0.100000,0.625000,0.609375,
    0.100000,0.100000,0.625000,0.640625,
    -0.100000,-0.100000,0.562500,0.609375,
    -0.100000,0.100000,0.562500,0.640625,

    // kTerrainFalloffFill
    0.050000,-0.200000,0.656250,0.515625,
    0.050000,0.200000,0.656250,0.578125,
    -0.050000,-0.200000,0.625000,0.515625,
    -0.050000,0.200000,0.625000,0.578125,

    // kTerrainFalloffBack
    0.050000,-0.200000,0.625000,0.515625,
    0.050000,0.200000,0.625000,0.578125,
    -0.050000,-0.200000,0.593750,0.515625,
    -0.050000,0.200000,0.593750,0.578125,

    // kTerrainFalloffIcon
    0.100000,-0.100000,0.250000,0.671875,
    0.100000,0.100000,0.250000,0.703125,
    -0.100000,-0.100000,0.187500,0.671875,
    -0.100000,0.100000,0.187500,0.703125,

    // kTerrainBake
    0.100000,-0.100000,1.000000,0.750000,
    0.100000,0.100000,1.000000,0.781250,
    -0.100000,-0.100000,0.937500,0.750000,
    -0.100000,0.100000,0.937500,0.781250,

    // kTerrainSlotsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainSlotsConfig
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainVisibleOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainVisibleOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainVisibleOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainVisibleOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainSlotsSlot
    0.175000,-0.075000,0.109375,0.742188,
    0.175000,0.075000,0.109375,0.765625,
    -0.175000,-0.075000,0.000000,0.742188,
    -0.175000,0.075000,0.000000,0.765625,

    // kTerrainSlotsSlotColor
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kTerrainSlotsRemove
    0.037500,-0.037500,0.929688,0.566406,
    0.037500,0.037500,0.929688,0.578125,
    -0.037500,-0.037500,0.906250,0.566406,
    -0.037500,0.037500,0.906250,0.578125,

    // kTerrainSlotsSelected
    0.025000,-0.025000,0.328125,0.568359,
    0.025000,0.025000,0.328125,0.576172,
    -0.025000,-0.025000,0.312500,0.568359,
    -0.025000,0.025000,0.312500,0.576172,

    // kTerrainSlotsHandler
    0.025000,-0.050000,0.750000,0.570312,
    0.025000,0.050000,0.750000,0.585938,
    -0.025000,-0.050000,0.734375,0.570312,
    -0.025000,0.050000,0.734375,0.585938,

    // kTerrainSlotsSlider
    0.050000,-0.400000,0.687500,0.453125,
    0.050000,0.400000,0.687500,0.578125,
    -0.050000,-0.400000,0.656250,0.453125,
    -0.050000,0.400000,0.656250,0.578125,

    // kTerrainSlotsBack
    0.250000,-0.400000,0.562500,0.687500,
    0.250000,0.400000,0.562500,0.812500,
    -0.250000,-0.400000,0.406250,0.687500,
    -0.250000,0.400000,0.406250,0.812500,

    // kTerrainSlotsCreate
    0.037500,-0.037500,0.882812,0.566406,
    0.037500,0.037500,0.882812,0.578125,
    -0.037500,-0.037500,0.859375,0.566406,
    -0.037500,0.037500,0.859375,0.578125,

    // kTerrainSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kTerrainSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.250000,0.554688,
    0.075000,0.075000,0.250000,0.578125,
    -0.075000,-0.075000,0.203125,0.554688,
    -0.075000,0.075000,0.203125,0.578125,

    // kTerrainBakeDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kTerrainBakeDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainBakeDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainBakeAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kTerrainBakeErosionStepLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeErosionStepInput
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainBakeWeatheringStepLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeWeatheringStepInput
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainBakeHmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeNmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeAo
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeSlopemap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeSplatmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeErosionThermal
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeErosionHydraulic
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeWaterAccum
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainBakeWaterFlow
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kTerrainEditDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainEditDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainEditAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kTerrainEditName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNameBack
    0.153125,-0.050000,0.205078,0.750000,
    0.153125,0.050000,0.205078,0.765625,
    -0.153125,-0.050000,0.109375,0.750000,
    -0.153125,0.050000,0.109375,0.765625,

    // kTerrainColorPaletteHS
    0.100000,-0.100000,0.906250,0.687500,
    0.100000,0.100000,0.906250,0.718750,
    -0.100000,-0.100000,0.843750,0.687500,
    -0.100000,0.100000,0.843750,0.718750,

    // kTerrainColorPaletteB
    0.100000,-0.025000,0.843750,0.710938,
    0.100000,0.025000,0.843750,0.718750,
    -0.100000,-0.025000,0.781250,0.710938,
    -0.100000,0.025000,0.781250,0.718750,

    // kTerrainColorIndicator
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kTerrainColorBrightnessCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kTerrainColorColorCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kTerrainEditHeightmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditRandomGeneration
    0.075000,-0.075000,0.734375,0.570312,
    0.075000,0.075000,0.734375,0.593750,
    -0.075000,-0.075000,0.687500,0.570312,
    -0.075000,0.075000,0.687500,0.593750,

    // kTerrainEditTranslateXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditTranslateXTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditTranslateYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditTranslateYTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditTranslateZText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditTranslateZTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditScaleXTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditScaleYTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditScaleZText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditScaleZTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditRotateXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditRotateXTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditRotateYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditRotateYTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditRotateZText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditRotateZTextSlot
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kTerrainEditLabelInvert
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditLabelTiling
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditLabelStrength
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise1Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise1Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise1InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise1InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise1InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise1InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise1TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise1TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise1TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise1TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise1StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise1StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise1Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise2Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise2Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise2InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise2InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise2InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise2InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise2TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise2TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise2TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise2TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise2StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise2StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise2Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise3Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise3Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise3InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise3InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise3InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise3InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise3TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise3TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise3TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise3TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise3StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise3StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise3Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise4Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise4Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise4InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise4InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise4InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise4InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise4TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise4TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise4TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise4TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise4StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise4StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise4Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise5Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise5Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise5InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise5InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise5InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise5InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise5TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise5TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise5TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise5TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise5StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise5StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise5Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise6Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise6Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise6InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise6InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise6InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise6InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise6TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise6TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise6TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise6TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise6StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise6StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise6Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise7Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise7Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise7InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise7InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise7InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise7InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise7TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise7TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise7TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise7TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise7StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise7StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise7Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise8Config
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kTerrainEditNoise8Name
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainEditNoise8InvertOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise8InvertOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise8InvertOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise8InvertOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise8TilingOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainEditNoise8TilingOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainEditNoise8TilingOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainEditNoise8TilingOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainEditNoise8StrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainEditNoise8StrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainEditNoise8Hmap
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoisePerlinName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoisePerlinDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoisePerlinPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoisePerlinPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoisePerlinSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoisePerlinScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoisePerlinScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoisePerlinScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoisePerlinScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoisePerlinScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoisePerlinScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoisePerlinSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoisePerlinSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoisePerlinSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseCellularName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseCellularDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseCellularPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseCellularPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseCellularSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseCellularScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseCellularScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseCellularScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseCellularScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseCellularScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseCellularScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseCellularJitterText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseCellularJitterArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseCellularJitterIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseCellularSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseCellularSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseCellularSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseMetaballsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseMetaballsDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseMetaballsPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseMetaballsPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseMetaballsSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseMetaballsScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseMetaballsScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseMetaballsScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseMetaballsScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseMetaballsScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseMetaballsScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseMetaballsJitterText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseMetaballsJitterArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseMetaballsJitterIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseMetaballsSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseMetaballsSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseMetaballsSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseFbmGridPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseFbmGridPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseFbmGridSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseFbmGridScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridOctavesText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridOctavesArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridOctavesIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridShiftText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridShiftArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridShiftIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridGainText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridGainArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridGainIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridLacunarityText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridLacunarityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridLacunarityIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridWarpStrengthText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridWarpStrengthArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridWarpStrengthIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridOctaveFactorText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridOctaveFactorArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridOctaveFactorIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmGridSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmGridSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmGridSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmMultiName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseFbmMultiPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseFbmMultiPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseFbmMultiSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseFbmMultiScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmMultiScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmMultiScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmMultiScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmMultiLacunarityText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiLacunarityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmMultiLacunarityIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmMultiOctavesText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiOctavesArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmMultiOctavesIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmMultiSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmMultiSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmMultiSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseFbmdPerlinPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseFbmdPerlinPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseFbmdPerlinSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseFbmdPerlinScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinOctavesText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinOctavesArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinOctavesIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinGainText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinGainArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinGainIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinLacunarityText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinLacunarityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinLacunarityIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinSlopenessText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinSlopenessArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinSlopenessIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinOctaveFactorText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinOctaveFactorArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinOctaveFactorIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmdPerlinNegativeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinNegativeOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainNoiseFbmdPerlinNegativeOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainNoiseFbmdPerlinNegativeOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainNoiseFbmdPerlinNegativeOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainNoiseFbmdPerlinSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmdPerlinSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmdPerlinSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseFbmWarpPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseFbmWarpPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseFbmWarpSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseFbmWarpScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpOctavesText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpOctavesArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpOctavesIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpGainText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpGainArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpGainIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpLacunarityText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpLacunarityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpLacunarityIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpSlopenessText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpSlopenessArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpSlopenessIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpOctaveFactorText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpOctaveFactorArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpOctaveFactorIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpNegativeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpNegativeOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainNoiseFbmWarpNegativeOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainNoiseFbmWarpNegativeOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainNoiseFbmWarpNegativeOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainNoiseFbmWarpSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpQText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpQArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpQIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmWarpRText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmWarpRArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmWarpRIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kTerrainNoiseFbmPerlinWarpPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTerrainNoiseFbmPerlinWarpPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTerrainNoiseFbmPerlinWarpSave
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kTerrainNoiseFbmPerlinWarpScaleXText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpScaleXArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpScaleXIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpScaleYText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpScaleYArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpScaleYIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpOctavesText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpOctavesArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpOctavesIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpGainText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpGainArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpGainIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpLacunarityText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpLacunarityArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpLacunarityIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpSlopenessText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpSlopenessArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpSlopenessIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpOctaveFactorText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpOctaveFactorArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpOctaveFactorIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpNegativeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpNegativeOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kTerrainNoiseFbmPerlinWarpNegativeOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kTerrainNoiseFbmPerlinWarpNegativeOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kTerrainNoiseFbmPerlinWarpNegativeOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kTerrainNoiseFbmPerlinWarpSeedText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpSeedArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpSeedIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpQText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpQArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpQIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kTerrainNoiseFbmPerlinWarpRText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kTerrainNoiseFbmPerlinWarpRArea
    0.200000,-0.050000,0.937500,0.781250,
    0.200000,0.050000,0.937500,0.796875,
    -0.200000,-0.050000,0.812500,0.781250,
    -0.200000,0.050000,0.812500,0.796875,

    // kTerrainNoiseFbmPerlinWarpRIcon
    0.050000,-0.050000,0.593750,0.562500,
    0.050000,0.050000,0.593750,0.578125,
    -0.050000,-0.050000,0.562500,0.562500,
    -0.050000,0.050000,0.562500,0.578125,

    // kWaterWaterMode
    0.100000,-0.100000,0.875000,0.578125,
    0.100000,0.100000,0.875000,0.609375,
    -0.100000,-0.100000,0.812500,0.578125,
    -0.100000,0.100000,0.812500,0.609375,

    // kWaterUpdate
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kWaterLake
    0.100000,-0.100000,0.875000,0.578125,
    0.100000,0.100000,0.875000,0.609375,
    -0.100000,-0.100000,0.812500,0.578125,
    -0.100000,0.100000,0.812500,0.609375,

    // kWaterRiver
    0.100000,-0.100000,0.937500,0.578125,
    0.100000,0.100000,0.937500,0.609375,
    -0.100000,-0.100000,0.875000,0.578125,
    -0.100000,0.100000,0.875000,0.609375,

    // kWaterWaterfall
    0.100000,-0.100000,0.812500,0.570312,
    0.100000,0.100000,0.812500,0.601562,
    -0.100000,-0.100000,0.750000,0.570312,
    -0.100000,0.100000,0.750000,0.601562,

    // kWaterSlotsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterSlotsConfig
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kWaterVisibleOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kWaterVisibleOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kWaterVisibleOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kWaterVisibleOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kWaterSlotsSlot
    0.175000,-0.075000,0.109375,0.742188,
    0.175000,0.075000,0.109375,0.765625,
    -0.175000,-0.075000,0.000000,0.742188,
    -0.175000,0.075000,0.000000,0.765625,

    // kWaterSlotsSlotColor
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kWaterSlotsRemove
    0.037500,-0.037500,0.929688,0.566406,
    0.037500,0.037500,0.929688,0.578125,
    -0.037500,-0.037500,0.906250,0.566406,
    -0.037500,0.037500,0.906250,0.578125,

    // kWaterSlotsSelected
    0.025000,-0.025000,0.328125,0.568359,
    0.025000,0.025000,0.328125,0.576172,
    -0.025000,-0.025000,0.312500,0.568359,
    -0.025000,0.025000,0.312500,0.576172,

    // kWaterSlotsHandler
    0.025000,-0.050000,0.750000,0.570312,
    0.025000,0.050000,0.750000,0.585938,
    -0.025000,-0.050000,0.734375,0.570312,
    -0.025000,0.050000,0.734375,0.585938,

    // kWaterSlotsSlider
    0.050000,-0.400000,0.687500,0.453125,
    0.050000,0.400000,0.687500,0.578125,
    -0.050000,-0.400000,0.656250,0.453125,
    -0.050000,0.400000,0.656250,0.578125,

    // kWaterSlotsBack
    0.250000,-0.400000,0.562500,0.687500,
    0.250000,0.400000,0.562500,0.812500,
    -0.250000,-0.400000,0.406250,0.687500,
    -0.250000,0.400000,0.406250,0.812500,

    // kWaterSlotsCreate
    0.037500,-0.037500,0.882812,0.566406,
    0.037500,0.037500,0.882812,0.578125,
    -0.037500,-0.037500,0.859375,0.566406,
    -0.037500,0.037500,0.859375,0.578125,

    // kWaterSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kWaterSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kWaterSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.250000,0.554688,
    0.075000,0.075000,0.250000,0.578125,
    -0.075000,-0.075000,0.203125,0.554688,
    -0.075000,0.075000,0.203125,0.578125,

    // kWaterSlotsFlipSelectEdit
    0.025000,-0.100000,0.062500,0.554688,
    0.025000,0.100000,0.062500,0.585938,
    -0.025000,-0.100000,0.046875,0.554688,
    -0.025000,0.100000,0.046875,0.585938,

    // kWaterEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kWaterEditDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kWaterEditDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kWaterEditAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kWaterEditName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterEditNameBack
    0.153125,-0.050000,0.205078,0.750000,
    0.153125,0.050000,0.205078,0.765625,
    -0.153125,-0.050000,0.109375,0.750000,
    -0.153125,0.050000,0.109375,0.765625,

    // kWaterColorPaletteHS
    0.100000,-0.100000,0.906250,0.687500,
    0.100000,0.100000,0.906250,0.718750,
    -0.100000,-0.100000,0.843750,0.687500,
    -0.100000,0.100000,0.843750,0.718750,

    // kWaterColorPaletteB
    0.100000,-0.025000,0.843750,0.710938,
    0.100000,0.025000,0.843750,0.718750,
    -0.100000,-0.025000,0.781250,0.710938,
    -0.100000,0.025000,0.781250,0.718750,

    // kWaterColorIndicator
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kWaterColorBrightnessCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kWaterColorColorCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kWaterTypeBack
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kWaterTypeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterTypePrevious
    0.100000,-0.100000,0.187500,0.652344,
    0.100000,0.100000,0.187500,0.683594,
    -0.100000,-0.100000,0.125000,0.652344,
    -0.100000,0.100000,0.125000,0.683594,

    // kWaterTypeNext
    0.100000,-0.100000,0.125000,0.621094,
    0.100000,0.100000,0.125000,0.652344,
    -0.100000,-0.100000,0.062500,0.621094,
    -0.100000,0.100000,0.062500,0.652344,

    // kWaterLayer1
    0.100000,-0.100000,0.906250,0.656250,
    0.100000,0.100000,0.906250,0.687500,
    -0.100000,-0.100000,0.843750,0.656250,
    -0.100000,0.100000,0.843750,0.687500,

    // kWaterLayer1Window
    0.200000,-0.200000,0.812500,0.750000,
    0.200000,0.200000,0.812500,0.812500,
    -0.200000,-0.200000,0.687500,0.750000,
    -0.200000,0.200000,0.687500,0.812500,

    // kWaterLayer1PinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kWaterLayer1PinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kWaterLayer1Text
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterLayer1Off
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kWaterLayer1On1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kWaterLayer1On2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kWaterLayer1On3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kWater1ScaleLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1ScaleFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1ScaleBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1ScaleHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1FetchLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1FetchFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1FetchBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1FetchHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1SpreadBlendLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1SpreadBlendFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1SpreadBlendBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1SpreadBlendHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1SwellLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1SwellFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1SwellBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1SwellHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1PeakEnhancementLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1PeakEnhancementFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1PeakEnhancementBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1PeakEnhancementHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1ShortWavesFadeLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1ShortWavesFadeFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1ShortWavesFadeBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1ShortWavesFadeHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater1LambdaLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater1LambdaFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater1LambdaBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater1LambdaHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWaterLayer2
    0.100000,-0.100000,0.968750,0.656250,
    0.100000,0.100000,0.968750,0.687500,
    -0.100000,-0.100000,0.906250,0.656250,
    -0.100000,0.100000,0.906250,0.687500,

    // kWaterLayer2Window
    0.200000,-0.200000,0.812500,0.750000,
    0.200000,0.200000,0.812500,0.812500,
    -0.200000,-0.200000,0.687500,0.750000,
    -0.200000,0.200000,0.687500,0.812500,

    // kWaterLayer2PinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kWaterLayer2PinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kWaterLayer2Text
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterLayer2Off
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kWaterLayer2On1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kWaterLayer2On2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kWaterLayer2On3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kWater2ScaleLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2ScaleFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2ScaleBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2ScaleHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2FetchLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2FetchFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2FetchBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2FetchHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2SpreadBlendLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2SpreadBlendFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2SpreadBlendBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2SpreadBlendHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2SwellLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2SwellFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2SwellBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2SwellHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2PeakEnhancementLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2PeakEnhancementFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2PeakEnhancementBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2PeakEnhancementHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2ShortWavesFadeLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2ShortWavesFadeFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2ShortWavesFadeBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2ShortWavesFadeHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater2LambdaLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater2LambdaFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater2LambdaBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater2LambdaHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWaterLayer3
    0.100000,-0.100000,0.125000,0.652344,
    0.100000,0.100000,0.125000,0.683594,
    -0.100000,-0.100000,0.062500,0.652344,
    -0.100000,0.100000,0.062500,0.683594,

    // kWaterLayer3Window
    0.200000,-0.200000,0.812500,0.750000,
    0.200000,0.200000,0.812500,0.812500,
    -0.200000,-0.200000,0.687500,0.750000,
    -0.200000,0.200000,0.687500,0.812500,

    // kWaterLayer3PinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kWaterLayer3PinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kWaterLayer3Text
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWaterLayer3Off
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kWaterLayer3On1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kWaterLayer3On2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kWaterLayer3On3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kWater3ScaleLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3ScaleFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3ScaleBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3ScaleHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3FetchLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3FetchFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3FetchBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3FetchHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3SpreadBlendLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3SpreadBlendFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3SpreadBlendBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3SpreadBlendHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3SwellLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3SwellFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3SwellBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3SwellHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3PeakEnhancementLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3PeakEnhancementFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3PeakEnhancementBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3PeakEnhancementHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3ShortWavesFadeLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3ShortWavesFadeFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3ShortWavesFadeBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3ShortWavesFadeHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kWater3LambdaLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kWater3LambdaFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kWater3LambdaBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kWater3LambdaHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kRoadsRoadsMode
    0.100000,-0.100000,0.062500,0.617188,
    0.100000,0.100000,0.062500,0.648438,
    -0.100000,-0.100000,0.000000,0.617188,
    -0.100000,0.100000,0.000000,0.648438,

    // kRoadsAsphalt
    0.100000,-0.100000,0.062500,0.617188,
    0.100000,0.100000,0.062500,0.648438,
    -0.100000,-0.100000,0.000000,0.617188,
    -0.100000,0.100000,0.000000,0.648438,

    // kRoadsGravel
    0.100000,-0.100000,0.812500,0.617188,
    0.100000,0.100000,0.812500,0.648438,
    -0.100000,-0.100000,0.750000,0.617188,
    -0.100000,0.100000,0.750000,0.648438,

    // kRoadsSoil
    0.100000,-0.100000,0.250000,0.609375,
    0.100000,0.100000,0.250000,0.640625,
    -0.100000,-0.100000,0.187500,0.609375,
    -0.100000,0.100000,0.187500,0.640625,

    // kRoadsSlotsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kRoadsSlotsConfig
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kRoadsVisibleOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kRoadsVisibleOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kRoadsVisibleOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kRoadsVisibleOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kRoadsSlotsSlot
    0.175000,-0.075000,0.109375,0.742188,
    0.175000,0.075000,0.109375,0.765625,
    -0.175000,-0.075000,0.000000,0.742188,
    -0.175000,0.075000,0.000000,0.765625,

    // kRoadsSlotsSlotColor
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kRoadsSlotsRemove
    0.037500,-0.037500,0.929688,0.566406,
    0.037500,0.037500,0.929688,0.578125,
    -0.037500,-0.037500,0.906250,0.566406,
    -0.037500,0.037500,0.906250,0.578125,

    // kRoadsSlotsSelected
    0.025000,-0.025000,0.328125,0.568359,
    0.025000,0.025000,0.328125,0.576172,
    -0.025000,-0.025000,0.312500,0.568359,
    -0.025000,0.025000,0.312500,0.576172,

    // kRoadsSlotsHandler
    0.025000,-0.050000,0.750000,0.570312,
    0.025000,0.050000,0.750000,0.585938,
    -0.025000,-0.050000,0.734375,0.570312,
    -0.025000,0.050000,0.734375,0.585938,

    // kRoadsSlotsSlider
    0.050000,-0.400000,0.687500,0.453125,
    0.050000,0.400000,0.687500,0.578125,
    -0.050000,-0.400000,0.656250,0.453125,
    -0.050000,0.400000,0.656250,0.578125,

    // kRoadsSlotsBack
    0.250000,-0.400000,0.562500,0.687500,
    0.250000,0.400000,0.562500,0.812500,
    -0.250000,-0.400000,0.406250,0.687500,
    -0.250000,0.400000,0.406250,0.812500,

    // kRoadsSlotsCreate
    0.037500,-0.037500,0.882812,0.566406,
    0.037500,0.037500,0.882812,0.578125,
    -0.037500,-0.037500,0.859375,0.566406,
    -0.037500,0.037500,0.859375,0.578125,

    // kRoadsSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kRoadsSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kRoadsSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.250000,0.554688,
    0.075000,0.075000,0.250000,0.578125,
    -0.075000,-0.075000,0.203125,0.554688,
    -0.075000,0.075000,0.203125,0.578125,

    // kRoadsSlotsFlipSelectEdit
    0.025000,-0.100000,0.062500,0.554688,
    0.025000,0.100000,0.062500,0.585938,
    -0.025000,-0.100000,0.046875,0.554688,
    -0.025000,0.100000,0.046875,0.585938,

    // kRoadsEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kRoadsEditPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kRoadsEditPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kRoadsEditAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kRoadsEditName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kRoadsEditNameBack
    0.153125,-0.050000,0.205078,0.750000,
    0.153125,0.050000,0.205078,0.765625,
    -0.153125,-0.050000,0.109375,0.750000,
    -0.153125,0.050000,0.109375,0.765625,

    // kRoadsColorPaletteHS
    0.100000,-0.100000,0.906250,0.687500,
    0.100000,0.100000,0.906250,0.718750,
    -0.100000,-0.100000,0.843750,0.687500,
    -0.100000,0.100000,0.843750,0.718750,

    // kRoadsColorPaletteB
    0.100000,-0.025000,0.843750,0.710938,
    0.100000,0.025000,0.843750,0.718750,
    -0.100000,-0.025000,0.781250,0.710938,
    -0.100000,0.025000,0.781250,0.718750,

    // kRoadsColorIndicator
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kRoadsColorBrightnessCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kRoadsColorColorCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kRoadsTypeBack
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kRoadsTypeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kRoadsTypePrevious
    0.100000,-0.100000,0.187500,0.652344,
    0.100000,0.100000,0.187500,0.683594,
    -0.100000,-0.100000,0.125000,0.652344,
    -0.100000,0.100000,0.125000,0.683594,

    // kRoadsTypeNext
    0.100000,-0.100000,0.125000,0.621094,
    0.100000,0.100000,0.125000,0.652344,
    -0.100000,-0.100000,0.062500,0.621094,
    -0.100000,0.100000,0.062500,0.652344,

    // kFencesFencesMode
    0.100000,-0.100000,0.312500,0.671875,
    0.100000,0.100000,0.312500,0.703125,
    -0.100000,-0.100000,0.250000,0.671875,
    -0.100000,0.100000,0.250000,0.703125,

    // kFencesPicket
    0.100000,-0.100000,0.312500,0.671875,
    0.100000,0.100000,0.312500,0.703125,
    -0.100000,-0.100000,0.250000,0.671875,
    -0.100000,0.100000,0.250000,0.703125,

    // kFencesChainLink
    0.100000,-0.100000,0.375000,0.671875,
    0.100000,0.100000,0.375000,0.703125,
    -0.100000,-0.100000,0.312500,0.671875,
    -0.100000,0.100000,0.312500,0.703125,

    // kFencesWooden
    0.100000,-0.100000,0.625000,0.671875,
    0.100000,0.100000,0.625000,0.703125,
    -0.100000,-0.100000,0.562500,0.671875,
    -0.100000,0.100000,0.562500,0.703125,

    // kFencesSlotsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFencesSlotsConfig
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kFencesVisibleOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kFencesVisibleOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kFencesVisibleOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kFencesVisibleOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kFencesSlotsSlot
    0.175000,-0.075000,0.109375,0.742188,
    0.175000,0.075000,0.109375,0.765625,
    -0.175000,-0.075000,0.000000,0.742188,
    -0.175000,0.075000,0.000000,0.765625,

    // kFencesSlotsSlotColor
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kFencesSlotsRemove
    0.037500,-0.037500,0.929688,0.566406,
    0.037500,0.037500,0.929688,0.578125,
    -0.037500,-0.037500,0.906250,0.566406,
    -0.037500,0.037500,0.906250,0.578125,

    // kFencesSlotsSelected
    0.025000,-0.025000,0.328125,0.568359,
    0.025000,0.025000,0.328125,0.576172,
    -0.025000,-0.025000,0.312500,0.568359,
    -0.025000,0.025000,0.312500,0.576172,

    // kFencesSlotsHandler
    0.025000,-0.050000,0.750000,0.570312,
    0.025000,0.050000,0.750000,0.585938,
    -0.025000,-0.050000,0.734375,0.570312,
    -0.025000,0.050000,0.734375,0.585938,

    // kFencesSlotsSlider
    0.050000,-0.400000,0.687500,0.453125,
    0.050000,0.400000,0.687500,0.578125,
    -0.050000,-0.400000,0.656250,0.453125,
    -0.050000,0.400000,0.656250,0.578125,

    // kFencesSlotsBack
    0.250000,-0.400000,0.562500,0.687500,
    0.250000,0.400000,0.562500,0.812500,
    -0.250000,-0.400000,0.406250,0.687500,
    -0.250000,0.400000,0.406250,0.812500,

    // kFencesSlotsCreate
    0.037500,-0.037500,0.882812,0.566406,
    0.037500,0.037500,0.882812,0.578125,
    -0.037500,-0.037500,0.859375,0.566406,
    -0.037500,0.037500,0.859375,0.578125,

    // kFencesSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kFencesSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kFencesSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.250000,0.554688,
    0.075000,0.075000,0.250000,0.578125,
    -0.075000,-0.075000,0.203125,0.554688,
    -0.075000,0.075000,0.203125,0.578125,

    // kFencesSlotsFlipSelectEdit
    0.025000,-0.100000,0.062500,0.554688,
    0.025000,0.100000,0.062500,0.585938,
    -0.025000,-0.100000,0.046875,0.554688,
    -0.025000,0.100000,0.046875,0.585938,

    // kFencesEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kFencesEditPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kFencesEditPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kFencesEditAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kFencesEditName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFencesEditNameBack
    0.153125,-0.050000,0.205078,0.750000,
    0.153125,0.050000,0.205078,0.765625,
    -0.153125,-0.050000,0.109375,0.750000,
    -0.153125,0.050000,0.109375,0.765625,

    // kFencesColorPaletteHS
    0.100000,-0.100000,0.906250,0.687500,
    0.100000,0.100000,0.906250,0.718750,
    -0.100000,-0.100000,0.843750,0.687500,
    -0.100000,0.100000,0.843750,0.718750,

    // kFencesColorPaletteB
    0.100000,-0.025000,0.843750,0.710938,
    0.100000,0.025000,0.843750,0.718750,
    -0.100000,-0.025000,0.781250,0.710938,
    -0.100000,0.025000,0.781250,0.718750,

    // kFencesColorIndicator
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kFencesColorBrightnessCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kFencesColorColorCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kFencesTypeBack
    0.100000,-0.050000,0.812500,0.601562,
    0.100000,0.050000,0.812500,0.617188,
    -0.100000,-0.050000,0.750000,0.601562,
    -0.100000,0.050000,0.750000,0.617188,

    // kFencesTypeText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFencesTypePrevious
    0.100000,-0.100000,0.187500,0.652344,
    0.100000,0.100000,0.187500,0.683594,
    -0.100000,-0.100000,0.125000,0.652344,
    -0.100000,0.100000,0.125000,0.683594,

    // kFencesTypeNext
    0.100000,-0.100000,0.125000,0.621094,
    0.100000,0.100000,0.125000,0.652344,
    -0.100000,-0.100000,0.062500,0.621094,
    -0.100000,0.100000,0.062500,0.652344,

    // kPlacementPlacementMode
    0.100000,-0.100000,0.312500,0.578125,
    0.100000,0.100000,0.312500,0.609375,
    -0.100000,-0.100000,0.250000,0.578125,
    -0.100000,0.100000,0.250000,0.609375,

    // kPlacementColorFill
    0.050000,-0.200000,0.656250,0.515625,
    0.050000,0.200000,0.656250,0.578125,
    -0.050000,-0.200000,0.625000,0.515625,
    -0.050000,0.200000,0.625000,0.578125,

    // kPlacementColorBack
    0.050000,-0.200000,0.625000,0.515625,
    0.050000,0.200000,0.625000,0.578125,
    -0.050000,-0.200000,0.593750,0.515625,
    -0.050000,0.200000,0.593750,0.578125,

    // kPlacementColorIcon
    0.100000,-0.100000,0.781250,0.687500,
    0.100000,0.100000,0.781250,0.718750,
    -0.100000,-0.100000,0.718750,0.687500,
    -0.100000,0.100000,0.718750,0.718750,

    // kPlacementSizeFill
    0.050000,-0.200000,0.656250,0.515625,
    0.050000,0.200000,0.656250,0.578125,
    -0.050000,-0.200000,0.625000,0.515625,
    -0.050000,0.200000,0.625000,0.578125,

    // kPlacementSizeBack
    0.050000,-0.200000,0.625000,0.515625,
    0.050000,0.200000,0.625000,0.578125,
    -0.050000,-0.200000,0.593750,0.515625,
    -0.050000,0.200000,0.593750,0.578125,

    // kPlacementSizeIcon
    0.100000,-0.100000,0.625000,0.609375,
    0.100000,0.100000,0.625000,0.640625,
    -0.100000,-0.100000,0.562500,0.609375,
    -0.100000,0.100000,0.562500,0.640625,

    // kPlacementFalloffFill
    0.050000,-0.200000,0.656250,0.515625,
    0.050000,0.200000,0.656250,0.578125,
    -0.050000,-0.200000,0.625000,0.515625,
    -0.050000,0.200000,0.625000,0.578125,

    // kPlacementFalloffBack
    0.050000,-0.200000,0.625000,0.515625,
    0.050000,0.200000,0.625000,0.578125,
    -0.050000,-0.200000,0.593750,0.515625,
    -0.050000,0.200000,0.593750,0.578125,

    // kPlacementFalloffIcon
    0.100000,-0.100000,0.250000,0.671875,
    0.100000,0.100000,0.250000,0.703125,
    -0.100000,-0.100000,0.187500,0.671875,
    -0.100000,0.100000,0.187500,0.703125,

    // kPlacementTrees
    0.100000,-0.100000,0.312500,0.578125,
    0.100000,0.100000,0.312500,0.609375,
    -0.100000,-0.100000,0.250000,0.578125,
    -0.100000,0.100000,0.250000,0.609375,

    // kPlacementTreesOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kPlacementTreesOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kPlacementTreesOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kPlacementTreesOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kPlacementBushes
    0.100000,-0.100000,0.796875,0.718750,
    0.100000,0.100000,0.796875,0.750000,
    -0.100000,-0.100000,0.734375,0.718750,
    -0.100000,0.100000,0.734375,0.750000,

    // kPlacementBushesOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kPlacementBushesOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kPlacementBushesOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kPlacementBushesOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kPlacementTallGrass
    0.100000,-0.100000,0.437500,0.656250,
    0.100000,0.100000,0.437500,0.687500,
    -0.100000,-0.100000,0.375000,0.656250,
    -0.100000,0.100000,0.375000,0.687500,

    // kPlacementTallGrassOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kPlacementTallGrassOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kPlacementTallGrassOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kPlacementTallGrassOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kPlacementUndergrowth
    0.100000,-0.100000,0.375000,0.578125,
    0.100000,0.100000,0.375000,0.609375,
    -0.100000,-0.100000,0.312500,0.578125,
    -0.100000,0.100000,0.312500,0.609375,

    // kPlacementUndergrowthOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kPlacementUndergrowthOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kPlacementUndergrowthOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kPlacementUndergrowthOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kPlacementChangeMode
    0.100000,-0.100000,0.375000,0.609375,
    0.100000,0.100000,0.375000,0.640625,
    -0.100000,-0.100000,0.312500,0.609375,
    -0.100000,0.100000,0.312500,0.640625,

    // kPlacementDrawOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kPlacementDrawOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kPlacementDrawOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kPlacementDrawOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kObjectsObjectsMode
    0.100000,-0.100000,0.500000,0.656250,
    0.100000,0.100000,0.500000,0.687500,
    -0.100000,-0.100000,0.437500,0.656250,
    -0.100000,0.100000,0.437500,0.687500,

    // kObjectsMavka
    0.100000,-0.100000,0.250000,0.640625,
    0.100000,0.100000,0.250000,0.671875,
    -0.100000,-0.100000,0.187500,0.640625,
    -0.100000,0.100000,0.187500,0.671875,

    // kObjectsVodyaniy
    0.100000,-0.100000,0.687500,0.578125,
    0.100000,0.100000,0.687500,0.609375,
    -0.100000,-0.100000,0.625000,0.578125,
    -0.100000,0.100000,0.625000,0.609375,

    // kObjectsChugaister
    0.100000,-0.100000,0.359375,0.703125,
    0.100000,0.100000,0.359375,0.734375,
    -0.100000,-0.100000,0.296875,0.703125,
    -0.100000,0.100000,0.296875,0.734375,

    // kObjectsPedestal
    0.100000,-0.100000,0.437500,0.625000,
    0.100000,0.100000,0.437500,0.656250,
    -0.100000,-0.100000,0.375000,0.625000,
    -0.100000,0.100000,0.375000,0.656250,

    // kObjectsCampfire
    0.100000,-0.100000,1.000000,0.718750,
    0.100000,0.100000,1.000000,0.750000,
    -0.100000,-0.100000,0.937500,0.718750,
    -0.100000,0.100000,0.937500,0.750000,

    // kObjectsRoadSign
    0.100000,-0.100000,0.187500,0.621094,
    0.100000,0.100000,0.187500,0.652344,
    -0.100000,-0.100000,0.125000,0.621094,
    -0.100000,0.100000,0.125000,0.652344,

    // kObjectsHuman
    0.100000,-0.100000,0.500000,0.656250,
    0.100000,0.100000,0.500000,0.687500,
    -0.100000,-0.100000,0.437500,0.656250,
    -0.100000,0.100000,0.437500,0.687500,

    // kBiomesBiomesMode
    0.100000,-0.100000,0.718750,0.703125,
    0.100000,0.100000,0.718750,0.734375,
    -0.100000,-0.100000,0.656250,0.703125,
    -0.100000,0.100000,0.656250,0.734375,

    // kBiomesWindDirArea
    0.100000,-0.100000,0.562500,0.593750,
    0.100000,0.100000,0.562500,0.625000,
    -0.100000,-0.100000,0.500000,0.593750,
    -0.100000,0.100000,0.500000,0.625000,

    // kBiomesWindDirHandle
    0.100000,-0.100000,0.750000,0.593750,
    0.100000,0.100000,0.750000,0.625000,
    -0.100000,-0.100000,0.687500,0.593750,
    -0.100000,0.100000,0.687500,0.625000,

    // kBiomesWindDirIcon
    0.100000,-0.100000,0.437500,0.562500,
    0.100000,0.100000,0.437500,0.593750,
    -0.100000,-0.100000,0.375000,0.562500,
    -0.100000,0.100000,0.375000,0.593750,

    // kBiomesSunDirArea
    0.100000,-0.100000,0.562500,0.593750,
    0.100000,0.100000,0.562500,0.625000,
    -0.100000,-0.100000,0.500000,0.593750,
    -0.100000,0.100000,0.500000,0.625000,

    // kBiomesSunDirHandle
    0.100000,-0.100000,0.750000,0.593750,
    0.100000,0.100000,0.750000,0.625000,
    -0.100000,-0.100000,0.687500,0.593750,
    -0.100000,0.100000,0.687500,0.625000,

    // kBiomesSunDirIcon
    0.100000,-0.100000,0.125000,0.589844,
    0.100000,0.100000,0.125000,0.621094,
    -0.100000,-0.100000,0.062500,0.589844,
    -0.100000,0.100000,0.062500,0.621094,

    // kBiomesTimeArea
    0.100000,-0.100000,0.562500,0.593750,
    0.100000,0.100000,0.562500,0.625000,
    -0.100000,-0.100000,0.500000,0.593750,
    -0.100000,0.100000,0.500000,0.625000,

    // kBiomesTimeHandle
    0.100000,-0.100000,0.750000,0.593750,
    0.100000,0.100000,0.750000,0.625000,
    -0.100000,-0.100000,0.687500,0.593750,
    -0.100000,0.100000,0.687500,0.625000,

    // kBiomesTimeIcon
    0.100000,-0.100000,0.250000,0.578125,
    0.100000,0.100000,0.250000,0.609375,
    -0.100000,-0.100000,0.187500,0.578125,
    -0.100000,0.100000,0.187500,0.609375,

    // kBiomesSlotsName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kBiomesSlotsConfig
    0.100000,-0.100000,0.687500,0.671875,
    0.100000,0.100000,0.687500,0.703125,
    -0.100000,-0.100000,0.625000,0.671875,
    -0.100000,0.100000,0.625000,0.703125,

    // kBiomesVisibleOff
    0.100000,-0.100000,0.171875,0.714844,
    0.100000,0.100000,0.171875,0.746094,
    -0.100000,-0.100000,0.109375,0.714844,
    -0.100000,0.100000,0.109375,0.746094,

    // kBiomesVisibleOn1
    0.100000,-0.100000,0.234375,0.714844,
    0.100000,0.100000,0.234375,0.746094,
    -0.100000,-0.100000,0.171875,0.714844,
    -0.100000,0.100000,0.171875,0.746094,

    // kBiomesVisibleOn2
    0.100000,-0.100000,0.062500,0.710938,
    0.100000,0.100000,0.062500,0.742188,
    -0.100000,-0.100000,0.000000,0.710938,
    -0.100000,0.100000,0.000000,0.742188,

    // kBiomesVisibleOn3
    0.100000,-0.100000,0.296875,0.703125,
    0.100000,0.100000,0.296875,0.734375,
    -0.100000,-0.100000,0.234375,0.703125,
    -0.100000,0.100000,0.234375,0.734375,

    // kBiomesSlotsSlot
    0.175000,-0.075000,0.109375,0.742188,
    0.175000,0.075000,0.109375,0.765625,
    -0.175000,-0.075000,0.000000,0.742188,
    -0.175000,0.075000,0.000000,0.765625,

    // kBiomesSlotsSlotColor
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kBiomesSlotsRemove
    0.037500,-0.037500,0.929688,0.566406,
    0.037500,0.037500,0.929688,0.578125,
    -0.037500,-0.037500,0.906250,0.566406,
    -0.037500,0.037500,0.906250,0.578125,

    // kBiomesSlotsSelected
    0.025000,-0.025000,0.328125,0.568359,
    0.025000,0.025000,0.328125,0.576172,
    -0.025000,-0.025000,0.312500,0.568359,
    -0.025000,0.025000,0.312500,0.576172,

    // kBiomesSlotsHandler
    0.025000,-0.050000,0.750000,0.570312,
    0.025000,0.050000,0.750000,0.585938,
    -0.025000,-0.050000,0.734375,0.570312,
    -0.025000,0.050000,0.734375,0.585938,

    // kBiomesSlotsSlider
    0.050000,-0.400000,0.687500,0.453125,
    0.050000,0.400000,0.687500,0.578125,
    -0.050000,-0.400000,0.656250,0.453125,
    -0.050000,0.400000,0.656250,0.578125,

    // kBiomesSlotsBack
    0.250000,-0.400000,0.562500,0.687500,
    0.250000,0.400000,0.562500,0.812500,
    -0.250000,-0.400000,0.406250,0.687500,
    -0.250000,0.400000,0.406250,0.812500,

    // kBiomesSlotsCreate
    0.037500,-0.037500,0.882812,0.566406,
    0.037500,0.037500,0.882812,0.578125,
    -0.037500,-0.037500,0.859375,0.566406,
    -0.037500,0.037500,0.859375,0.578125,

    // kBiomesSlotsFlipPointEdgeFace_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kBiomesSlotsFlipSelectEdit_Back
    0.037500,-0.037500,0.906250,0.566406,
    0.037500,0.037500,0.906250,0.578125,
    -0.037500,-0.037500,0.882812,0.566406,
    -0.037500,0.037500,0.882812,0.578125,

    // kBiomesSlotsFlipPointEdgeFace
    0.075000,-0.075000,0.250000,0.554688,
    0.075000,0.075000,0.250000,0.578125,
    -0.075000,-0.075000,0.203125,0.554688,
    -0.075000,0.075000,0.203125,0.578125,

    // kBiomesSlotsFlipSelectEdit
    0.025000,-0.100000,0.062500,0.554688,
    0.025000,0.100000,0.062500,0.585938,
    -0.025000,-0.100000,0.046875,0.554688,
    -0.025000,0.100000,0.046875,0.585938,

    // kBiomesEditDesk
    0.800000,-0.600000,0.500000,0.812500,
    0.800000,0.600000,0.500000,1.000000,
    -0.800000,-0.600000,0.000000,0.812500,
    -0.800000,0.600000,0.000000,1.000000,

    // kBiomesEditPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kBiomesEditPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kBiomesEditAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kBiomesEditName
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kBiomesEditNameBack
    0.153125,-0.050000,0.205078,0.750000,
    0.153125,0.050000,0.205078,0.765625,
    -0.153125,-0.050000,0.109375,0.750000,
    -0.153125,0.050000,0.109375,0.765625,

    // kBiomesColorPaletteHS
    0.100000,-0.100000,0.906250,0.687500,
    0.100000,0.100000,0.906250,0.718750,
    -0.100000,-0.100000,0.843750,0.687500,
    -0.100000,0.100000,0.843750,0.718750,

    // kBiomesColorPaletteB
    0.100000,-0.025000,0.843750,0.710938,
    0.100000,0.025000,0.843750,0.718750,
    -0.100000,-0.025000,0.781250,0.710938,
    -0.100000,0.025000,0.781250,0.718750,

    // kBiomesColorIndicator
    0.150000,-0.012500,0.203125,0.746094,
    0.150000,0.012500,0.203125,0.750000,
    -0.150000,-0.012500,0.109375,0.746094,
    -0.150000,0.012500,0.109375,0.750000,

    // kBiomesColorBrightnessCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kBiomesColorColorCursor
    0.012500,-0.012500,0.937500,0.574219,
    0.012500,0.012500,0.937500,0.578125,
    -0.012500,-0.012500,0.929688,0.574219,
    -0.012500,0.012500,0.929688,0.578125,

    // kBiomesEditWindFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditWindBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditWindHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditWindIcon
    0.100000,-0.100000,0.437500,0.562500,
    0.100000,0.100000,0.437500,0.593750,
    -0.100000,-0.100000,0.375000,0.562500,
    -0.100000,0.100000,0.375000,0.593750,

    // kBiomesEditSunFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditSunBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditSunHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditSunIcon
    0.100000,-0.100000,0.125000,0.589844,
    0.100000,0.100000,0.125000,0.621094,
    -0.100000,-0.100000,0.062500,0.589844,
    -0.100000,0.100000,0.062500,0.621094,

    // kBiomesEditTimeFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditTimeBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditTimeHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditTimeIcon
    0.100000,-0.100000,0.250000,0.578125,
    0.100000,0.100000,0.250000,0.609375,
    -0.100000,-0.100000,0.187500,0.578125,
    -0.100000,0.100000,0.187500,0.609375,

    // kBiomesEditPrecipitationFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditPrecipitationBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditPrecipitationHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditPrecipitationIcon
    0.100000,-0.100000,0.875000,0.625000,
    0.100000,0.100000,0.875000,0.656250,
    -0.100000,-0.100000,0.812500,0.625000,
    -0.100000,0.100000,0.812500,0.656250,

    // kBiomesEditTemperatureFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditTemperatureBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditTemperatureHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditTemperatureIcon
    0.100000,-0.100000,0.062500,0.585938,
    0.100000,0.100000,0.062500,0.617188,
    -0.100000,-0.100000,0.000000,0.585938,
    -0.100000,0.100000,0.000000,0.617188,

    // kBiomesEditCloudFill
    0.100000,-0.050000,0.937500,0.609375,
    0.100000,0.050000,0.937500,0.625000,
    -0.100000,-0.050000,0.875000,0.609375,
    -0.100000,0.050000,0.875000,0.625000,

    // kBiomesEditCloudBack
    0.100000,-0.050000,0.875000,0.609375,
    0.100000,0.050000,0.875000,0.625000,
    -0.100000,-0.050000,0.812500,0.609375,
    -0.100000,0.050000,0.812500,0.625000,

    // kBiomesEditCloudHandler
    0.100000,-0.100000,0.437500,0.593750,
    0.100000,0.100000,0.437500,0.625000,
    -0.100000,-0.100000,0.375000,0.593750,
    -0.100000,0.100000,0.375000,0.625000,

    // kBiomesEditCloudIcon
    0.100000,-0.100000,0.718750,0.703125,
    0.100000,0.100000,0.718750,0.734375,
    -0.100000,-0.100000,0.656250,0.703125,
    -0.100000,0.100000,0.656250,0.734375,

    // kTilesTilesMode
    0.100000,-0.100000,0.812500,0.648438,
    0.100000,0.100000,0.812500,0.679688,
    -0.100000,-0.100000,0.750000,0.648438,
    -0.100000,0.100000,0.750000,0.679688,

    // kTilesLeft
    0.100000,-0.100000,0.187500,0.652344,
    0.100000,0.100000,0.187500,0.683594,
    -0.100000,-0.100000,0.125000,0.652344,
    -0.100000,0.100000,0.125000,0.683594,

    // kTilesRight
    0.100000,-0.100000,0.125000,0.621094,
    0.100000,0.100000,0.125000,0.652344,
    -0.100000,-0.100000,0.062500,0.621094,
    -0.100000,0.100000,0.062500,0.652344,

    // kTilesUp
    0.100000,-0.100000,0.625000,0.578125,
    0.100000,0.100000,0.625000,0.609375,
    -0.100000,-0.100000,0.562500,0.578125,
    -0.100000,0.100000,0.562500,0.609375,

    // kTilesDown
    0.100000,-0.100000,0.062500,0.679688,
    0.100000,0.100000,0.062500,0.710938,
    -0.100000,-0.100000,0.000000,0.679688,
    -0.100000,0.100000,0.000000,0.710938,

    // kMapTomb
    0.075000,-0.125000,0.046875,0.546875,
    0.075000,0.125000,0.046875,0.585938,
    -0.075000,-0.125000,0.000000,0.546875,
    -0.075000,0.125000,0.000000,0.585938,

    // kMapFlowerRed
    0.075000,-0.075000,0.109375,0.566406,
    0.075000,0.075000,0.109375,0.589844,
    -0.075000,-0.075000,0.062500,0.566406,
    -0.075000,0.075000,0.062500,0.589844,

    // kMapFlowerWhite
    0.075000,-0.075000,0.156250,0.566406,
    0.075000,0.075000,0.156250,0.589844,
    -0.075000,-0.075000,0.109375,0.566406,
    -0.075000,0.075000,0.109375,0.589844,

    // kMapFlowerBlue
    0.075000,-0.075000,0.984375,0.570312,
    0.075000,0.075000,0.984375,0.593750,
    -0.075000,-0.075000,0.937500,0.570312,
    -0.075000,0.075000,0.937500,0.593750,

    // kMapKorn
    0.050000,-0.150000,0.281250,0.531250,
    0.050000,0.150000,0.281250,0.578125,
    -0.050000,-0.150000,0.250000,0.531250,
    -0.050000,0.150000,0.250000,0.578125,

    // kMapSeed
    0.075000,-0.075000,0.203125,0.554688,
    0.075000,0.075000,0.203125,0.578125,
    -0.075000,-0.075000,0.156250,0.554688,
    -0.075000,0.075000,0.156250,0.578125,

    // kMapLeaf
    0.050000,-0.050000,0.312500,0.562500,
    0.050000,0.050000,0.312500,0.578125,
    -0.050000,-0.050000,0.281250,0.562500,
    -0.050000,0.050000,0.281250,0.578125,

    // kMapSelected1
    0.050000,-0.050000,0.375000,0.562500,
    0.050000,0.050000,0.375000,0.578125,
    -0.050000,-0.050000,0.343750,0.562500,
    -0.050000,0.050000,0.343750,0.578125,

    // kMapSelected2
    0.037500,-0.037500,0.835938,0.566406,
    0.037500,0.037500,0.835938,0.578125,
    -0.037500,-0.037500,0.812500,0.566406,
    -0.037500,0.037500,0.812500,0.578125,

    // kMapSelected0
    0.012500,-0.012500,0.937500,0.570312,
    0.012500,0.012500,0.937500,0.574219,
    -0.012500,-0.012500,0.929688,0.570312,
    -0.012500,0.012500,0.929688,0.574219,

    // kMapEdge
    0.050000,-0.006250,0.343750,0.576172,
    0.050000,0.006250,0.343750,0.578125,
    -0.050000,-0.006250,0.312500,0.576172,
    -0.050000,0.006250,0.312500,0.578125,

    // kConfirmationDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kConfirmationTextLabel
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kConfirmationAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kConfirmationDecline
    0.100000,-0.100000,0.187500,0.683594,
    0.100000,0.100000,0.187500,0.714844,
    -0.100000,-0.100000,0.125000,0.683594,
    -0.100000,0.100000,0.125000,0.714844,

    // kTipInfoDesk
    0.200000,-0.200000,0.812500,0.750000,
    0.200000,0.200000,0.812500,0.812500,
    -0.200000,-0.200000,0.687500,0.750000,
    -0.200000,0.200000,0.687500,0.812500,

    // kTipInfoDeskPinBack
    0.100000,-0.100000,0.500000,0.625000,
    0.100000,0.100000,0.500000,0.656250,
    -0.100000,-0.100000,0.437500,0.625000,
    -0.100000,0.100000,0.437500,0.656250,

    // kTipInfoDeskPinPoint
    0.100000,-0.100000,0.562500,0.625000,
    0.100000,0.100000,0.562500,0.656250,
    -0.100000,-0.100000,0.500000,0.625000,
    -0.100000,0.100000,0.500000,0.656250,

    // kTipInfoText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFileDesk
    0.400000,-0.200000,0.750000,0.937500,
    0.400000,0.200000,0.750000,1.000000,
    -0.400000,-0.200000,0.500000,0.937500,
    -0.400000,0.200000,0.500000,1.000000,

    // kFileText
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFileTextInput
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFileAccept
    0.100000,-0.100000,1.000000,0.781250,
    0.100000,0.100000,1.000000,0.812500,
    -0.100000,-0.100000,0.937500,0.781250,
    -0.100000,0.100000,0.937500,0.812500,

    // kError
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kConsole
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText1
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText2
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText3
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText4
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText5
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText6
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText7
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText8
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText9
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kSpareText10
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,

    // kFullScreen
    1.000000,-1.000000,1.000000,0.000000,
    1.000000,1.000000,1.000000,1.000000,
    -1.000000,-1.000000,0.000000,0.000000,
    -1.000000,1.000000,0.000000,1.000000,


};

} // namespace vbos
