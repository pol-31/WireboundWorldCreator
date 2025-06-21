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
  data::TextId text_id;

  // parent id to take transform from it
  std::size_t parent_id_{0};

  // useful for e.g. slider to update centre/length of interactive area
  UiBase* ui{nullptr};
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

  kMenuDesk,
  kMenuArrowSelect,
  kMenuArrowSelected,

  kMenuSave,
  kMenuLoad,


  kSettingsDesk,

  kSettingsResolutionLabel,
  kSettingsResolution,
  kSettingsFullScreenOff,
  kSettingsFullScreenOn1,
  kSettingsFullScreenOn2,
  kSettingsFullScreenOn3,

  kSettingsSensitivityArea,
  kSettingsSensitivityIcon,

  kSettingsKeyboard,

  kSettingsSoundArea,
  kSettingsSoundIcon,
  kSettingsSoundOff,
  kSettingsSoundOn1,
  kSettingsSoundOn2,
  kSettingsSoundOn3,

  kSettingsMusicArea,
  kSettingsMusicIcon,
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
  kCompass,
  kCursorTree,
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
  kLogoBig,
  kLogoMedium,
  kLogoSmall,

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
  kTerrainBakeDesk,
  kTerrainBakeClose,
  kTerrainBakeAccept,
  kTerrainBakeErosionStepLabel,
  kTerrainBakeErosinoStepInput,
  kTerrainBakeWeatheringStepLabel,
  kTerrainBakeWeatheringStepInput,
  kTerrainBakeHeightmap,

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

  kTerrainEditDesk,
  kTerrainEditAccept,
  kTerrainEditClose,
  kTerrainEditName,
  kTerrainEditNameBack,
  kTerrainColorPalette,
  kTerrainColorIndicator,
  kTerrainColorBrightnessCursor,
  kTerrainColorColorCursor,
  kTerrainEditHeightmap,
  kTerrainEditRandomGeneration,

  kTerrainEditTranslateXText,
  kTerrainEditTranslateYText,
  kTerrainEditTranslateZText,
  kTerrainEditScaleXText,
  kTerrainEditScaleYText,
  kTerrainEditScaleZText,
  kTerrainEditRotateXText,
  kTerrainEditRotateYText,
  kTerrainEditRotateZText,

  kTerrainEditLabelInvert,

  kTerrainEditNoise1Config,
  kTerrainEditNoise1Name,
  kTerrainEditNoise1InvertOff,
  kTerrainEditNoise1InvertOn1,
  kTerrainEditNoise1InvertOn2,
  kTerrainEditNoise1InvertOn3,
  kTerrainEditNoise1StrengthText,
  kTerrainEditNoise1StrengthArea,
  kTerrainEditNoise1StrengthIcon,

  kTerrainEditNoise2Config,
  kTerrainEditNoise2Name,
   kTerrainEditNoise2InvertOff,
  kTerrainEditNoise2InvertOn1,
  kTerrainEditNoise2InvertOn2,
  kTerrainEditNoise2InvertOn3,
  kTerrainEditNoise2StrengthText,
  kTerrainEditNoise2StrengthArea,
  kTerrainEditNoise2StrengthIcon,

  kTerrainEditNoise3Config,
  kTerrainEditNoise3Name,
  kTerrainEditNoise3InvertOff,
  kTerrainEditNoise3InvertOn1,
  kTerrainEditNoise3InvertOn2,
  kTerrainEditNoise3InvertOn3,
  kTerrainEditNoise3StrengthText,
  kTerrainEditNoise3StrengthArea,
  kTerrainEditNoise3StrengthIcon,

  kTerrainEditNoise4Config,
  kTerrainEditNoise4Name,
  kTerrainEditNoise4InvertOff,
  kTerrainEditNoise4InvertOn1,
  kTerrainEditNoise4InvertOn2,
  kTerrainEditNoise4InvertOn3,
  kTerrainEditNoise4StrengthText,
  kTerrainEditNoise4StrengthArea,
  kTerrainEditNoise4StrengthIcon,

  kTerrainEditNoise5Config,
  kTerrainEditNoise5Name,
  kTerrainEditNoise5InvertOff,
  kTerrainEditNoise5InvertOn1,
  kTerrainEditNoise5InvertOn2,
  kTerrainEditNoise5InvertOn3,
  kTerrainEditNoise5StrengthText,
  kTerrainEditNoise5StrengthArea,
  kTerrainEditNoise5StrengthIcon,

  kTerrainEditNoise6Config,
  kTerrainEditNoise6Name,
  kTerrainEditNoise6InvertOff,
  kTerrainEditNoise6InvertOn1,
  kTerrainEditNoise6InvertOn2,
  kTerrainEditNoise6InvertOn3,
  kTerrainEditNoise6StrengthText,
  kTerrainEditNoise6StrengthArea,
  kTerrainEditNoise6StrengthIcon,

  kTerrainEditNoise7Config,
  kTerrainEditNoise7Name,
  kTerrainEditNoise7InvertOff,
  kTerrainEditNoise7InvertOn1,
  kTerrainEditNoise7InvertOn2,
  kTerrainEditNoise7InvertOn3,
  kTerrainEditNoise7StrengthText,
  kTerrainEditNoise7StrengthArea,
  kTerrainEditNoise7StrengthIcon,

  kTerrainEditNoise8Config,
  kTerrainEditNoise8Name,
  kTerrainEditNoise8InvertOff,
  kTerrainEditNoise8InvertOn1,
  kTerrainEditNoise8InvertOn2,
  kTerrainEditNoise8InvertOn3,
  kTerrainEditNoise8StrengthText,
  kTerrainEditNoise8StrengthArea,
  kTerrainEditNoise8StrengthIcon,

  kTerrainNoisePerlinScaleText,
  kTerrainNoisePerlinScaleArea,
  kTerrainNoisePerlinScaleIcon,
  kTerrainNoisePerlinSeedText,
  kTerrainNoisePerlinSeedArea,
  kTerrainNoisePerlinSeedIcon,

  kTerrainNoiseCellularScaleText,
  kTerrainNoiseCellularScaleArea,
  kTerrainNoiseCellularScaleIcon,
  kTerrainNoiseCellularJitterText,
  kTerrainNoiseCellularJitterArea,
  kTerrainNoiseCellularJitterIcon,
  kTerrainNoiseCellularSeedText,
  kTerrainNoiseCellularSeedArea,
  kTerrainNoiseCellularSeedIcon,

  kTerrainNoiseMetaballsScaleText,
  kTerrainNoiseMetaballsScaleArea,
  kTerrainNoiseMetaballsScaleIcon,
  kTerrainNoiseMetaballsJitterText,
  kTerrainNoiseMetaballsJitterArea,
  kTerrainNoiseMetaballsJitterIcon,
  kTerrainNoiseMetaballsPhaseText,
  kTerrainNoiseMetaballsPhaseArea,
  kTerrainNoiseMetaballsPhaseIcon,
  kTerrainNoiseMetaballsSeedText,
  kTerrainNoiseMetaballsSeedArea,
  kTerrainNoiseMetaballsSeedIcon,

  kTerrainNoiseFbmGridScaleText,
  kTerrainNoiseFbmGridScaleArea,
  kTerrainNoiseFbmGridScaleIcon,
  kTerrainNoiseFbmGridOctavesText,
  kTerrainNoiseFbmGridOctavesArea,
  kTerrainNoiseFbmGridOctavesIcon,
  kTerrainNoiseFbmGridShiftText,
  kTerrainNoiseFbmGridShiftArea,
  kTerrainNoiseFbmGridShiftIcon,
  kTerrainNoiseFbmGridTimeShiftText,
  kTerrainNoiseFbmGridTimeShiftArea,
  kTerrainNoiseFbmGridTimeShiftIcon,
  kTerrainNoiseFbmGridGainText,
  kTerrainNoiseFbmGridGainArea,
  kTerrainNoiseFbmGridGainIcon,
  kTerrainNoiseFbmGridLacunarityText,
  kTerrainNoiseFbmGridLacunarityArea,
  kTerrainNoiseFbmGridLacunarityIcon,
  kTerrainNoiseFbmGridTranslateText,
  kTerrainNoiseFbmGridTranslateArea,
  kTerrainNoiseFbmGridTranslateIcon,
  kTerrainNoiseFbmGridWarpStrengthText,
  kTerrainNoiseFbmGridWarpStrengthArea,
  kTerrainNoiseFbmGridWarpStrengthIcon,
  kTerrainNoiseFbmGridOctaveFactorText,
  kTerrainNoiseFbmGridOctaveFactorArea,
  kTerrainNoiseFbmGridOctaveFactorIcon,
  kTerrainNoiseFbmGridSeedText,
  kTerrainNoiseFbmGridSeedArea,
  kTerrainNoiseFbmGridSeedIcon,

  kTerrainNoiseFbmMultiScaleText,
  kTerrainNoiseFbmMultiScaleArea,
  kTerrainNoiseFbmMultiScaleIcon,
  kTerrainNoiseFbmMultiLacunarityText,
  kTerrainNoiseFbmMultiLacunarityArea,
  kTerrainNoiseFbmMultiLacunarityIcon,
  kTerrainNoiseFbmMultiOctavesText,
  kTerrainNoiseFbmMultiOctavesArea,
  kTerrainNoiseFbmMultiOctavesIcon,
  kTerrainNoiseFbmMultiPhaseText,
  kTerrainNoiseFbmMultiPhaseArea,
  kTerrainNoiseFbmMultiPhaseIcon,
  kTerrainNoiseFbmMultiSeedText,
  kTerrainNoiseFbmMultiSeedArea,
  kTerrainNoiseFbmMultiSeedIcon,

  kTerrainNoiseFbmdPerlinScaleText,
  kTerrainNoiseFbmdPerlinScaleArea,
  kTerrainNoiseFbmdPerlinScaleIcon,
  kTerrainNoiseFbmdPerlinOctavesText,
  kTerrainNoiseFbmdPerlinOctavesArea,
  kTerrainNoiseFbmdPerlinOctavesIcon,
  kTerrainNoiseFbmdPerlinShiftText,
  kTerrainNoiseFbmdPerlinShiftArea,
  kTerrainNoiseFbmdPerlinShiftIcon,
  kTerrainNoiseFbmdPerlinTransformText,
  kTerrainNoiseFbmdPerlinTransformArea,
  kTerrainNoiseFbmdPerlinTransformIcon,
  kTerrainNoiseFbmdPerlinGainText,
  kTerrainNoiseFbmdPerlinGainArea,
  kTerrainNoiseFbmdPerlinGainIcon,
  kTerrainNoiseFbmdPerlinLacunarityText,
  kTerrainNoiseFbmdPerlinLacunarityArea,
  kTerrainNoiseFbmdPerlinLacunarityIcon,
  kTerrainNoiseFbmdPerlinSlopenessText,
  kTerrainNoiseFbmdPerlinSlopenessArea,
  kTerrainNoiseFbmdPerlinSlopenessIcon,
  kTerrainNoiseFbmdPerlinOctaveFactorText,
  kTerrainNoiseFbmdPerlinOctaveFactorArea,
  kTerrainNoiseFbmdPerlinOctaveFactorIcon,
  kTerrainNoiseFbmdPerlinNegativeText,
  kTerrainNoiseFbmdPerlinNegativeOff,
  kTerrainNoiseFbmdPerlinNegativeOn1,
  kTerrainNoiseFbmdPerlinNegativeOn2,
  kTerrainNoiseFbmdPerlinNegativeOn3,
  kTerrainNoiseFbmdPerlinSeedText,
  kTerrainNoiseFbmdPerlinSeedArea,
  kTerrainNoiseFbmdPerlinSeedIcon,

  kTerrainNoiseFbmWarpScaleText,
  kTerrainNoiseFbmWarpScaleArea,
  kTerrainNoiseFbmWarpScaleIcon,
  kTerrainNoiseFbmWarpFactorsText,
  kTerrainNoiseFbmWarpFactorsArea,
  kTerrainNoiseFbmWarpFactorsIcon,
  kTerrainNoiseFbmWarpOctavesText,
  kTerrainNoiseFbmWarpOctavesArea,
  kTerrainNoiseFbmWarpOctavesIcon,
  kTerrainNoiseFbmWarpShiftText,
  kTerrainNoiseFbmWarpShiftArea,
  kTerrainNoiseFbmWarpShiftIcon,
  kTerrainNoiseFbmWarpTimeShiftText,
  kTerrainNoiseFbmWarpTimeShiftArea,
  kTerrainNoiseFbmWarpTimeShiftIcon,
  kTerrainNoiseFbmWarpGainText,
  kTerrainNoiseFbmWarpGainArea,
  kTerrainNoiseFbmWarpGainIcon,
  kTerrainNoiseFbmWarpLacunarityText,
  kTerrainNoiseFbmWarpLacunarityArea,
  kTerrainNoiseFbmWarpLacunarityIcon,
  kTerrainNoiseFbmWarpSlopenessText,
  kTerrainNoiseFbmWarpSlopenessArea,
  kTerrainNoiseFbmWarpSlopenessIcon,
  kTerrainNoiseFbmWarpOctaveFactorText,
  kTerrainNoiseFbmWarpOctaveFactorArea,
  kTerrainNoiseFbmWarpOctaveFactorIcon,
  kTerrainNoiseFbmWarpNegativeOff,
  kTerrainNoiseFbmWarpNegativeOn1,
  kTerrainNoiseFbmWarpNegativeOn2,
  kTerrainNoiseFbmWarpNegativeOn3,
  kTerrainNoiseFbmWarpSeedText,
  kTerrainNoiseFbmWarpSeedArea,
  kTerrainNoiseFbmWarpSeedIcon,
  kTerrainNoiseFbmWarpQText,
  kTerrainNoiseFbmWarpQArea,
  kTerrainNoiseFbmWarpQIcon,
  kTerrainNoiseFbmWarpRText,
  kTerrainNoiseFbmWarpRArea,
  kTerrainNoiseFbmWarpRIcon,

  kTerrainNoiseFbmPerlinWarpScaleText,
  kTerrainNoiseFbmPerlinWarpScaleArea,
  kTerrainNoiseFbmPerlinWarpScaleIcon,
  kTerrainNoiseFbmPerlinWarpFactorsText,
  kTerrainNoiseFbmPerlinWarpFactorsArea,
  kTerrainNoiseFbmPerlinWarpFactorsIcon,
  kTerrainNoiseFbmPerlinWarpOctavesText,
  kTerrainNoiseFbmPerlinWarpOctavesArea,
  kTerrainNoiseFbmPerlinWarpOctavesIcon,
  kTerrainNoiseFbmPerlinWarpShiftText,
  kTerrainNoiseFbmPerlinWarpShiftArea,
  kTerrainNoiseFbmPerlinWarpShiftIcon,
  kTerrainNoiseFbmPerlinWarpTimeShiftText,
  kTerrainNoiseFbmPerlinWarpTimeShiftArea,
  kTerrainNoiseFbmPerlinWarpTimeShiftIcon,
  kTerrainNoiseFbmPerlinWarpGainText,
  kTerrainNoiseFbmPerlinWarpGainArea,
  kTerrainNoiseFbmPerlinWarpGainIcon,
  kTerrainNoiseFbmPerlinWarpLacunarityText,
  kTerrainNoiseFbmPerlinWarpLacunarityArea,
  kTerrainNoiseFbmPerlinWarpLacunarityIcon,
  kTerrainNoiseFbmPerlinWarpSlopenessText,
  kTerrainNoiseFbmPerlinWarpSlopenessArea,
  kTerrainNoiseFbmPerlinWarpSlopenessIcon,
  kTerrainNoiseFbmPerlinWarpOctaveFactorText,
  kTerrainNoiseFbmPerlinWarpOctaveFactorArea,
  kTerrainNoiseFbmPerlinWarpOctaveFactorIcon,
  kTerrainNoiseFbmPerlinWarpNegativeOff,
  kTerrainNoiseFbmPerlinWarpNegativeOn1,
  kTerrainNoiseFbmPerlinWarpNegativeOn2,
  kTerrainNoiseFbmPerlinWarpNegativeOn3,
  kTerrainNoiseFbmPerlinWarpSeedText,
  kTerrainNoiseFbmPerlinWarpSeedArea,
  kTerrainNoiseFbmPerlinWarpSeedIcon,
  kTerrainNoiseFbmPerlinWarpQText,
  kTerrainNoiseFbmPerlinWarpQArea,
  kTerrainNoiseFbmPerlinWarpQIcon,
  kTerrainNoiseFbmPerlinWarpRText,
  kTerrainNoiseFbmPerlinWarpRArea,
  kTerrainNoiseFbmPerlinWarpRIcon,


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
  kWaterSlotsFlipPointEdge_Back,
  kWaterSlotsFlipSelectEdit_Back,
  kWaterSlotsFlipPointEdge,
  kWaterSlotsFlipSelectEdit,

  kWaterEditDesk,
  kWaterEditAccept,
  kWaterEditClose,
  kWaterEditName,
  kWaterEditNameBack,
  kWaterColorPalette,
  kWaterColorIndicator,
  kWaterColorBrightnessCursor,
  kWaterColorColorCursor,

  kWaterTypeBack,
  kWaterTypeText,
  kWaterTypePrevious,
  kWaterTypeNext,

  kWaterLayer1,
  kWaterLayer1Window,
  kWaterLayer1Text,
  kWaterLayer1Off,
  kWaterLayer1On1,
  kWaterLayer1On2,
  kWaterLayer1On3,

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


  kWaterLayer2,
  kWaterLayer2Window,
  kWaterLayer2Text,
  kWaterLayer2Off,
  kWaterLayer2On1,
  kWaterLayer2On2,
  kWaterLayer2On3,

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


  kWaterLayer3,
  kWaterLayer3Window,
  kWaterLayer3Text,
  kWaterLayer3Off,
  kWaterLayer3On1,
  kWaterLayer3On2,
  kWaterLayer3On3,

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
  kRoadsSlotsFlipPointEdge_Back,
  kRoadsSlotsFlipSelectEdit_Back,
  kRoadsSlotsFlipPointEdge,
  kRoadsSlotsFlipSelectEdit,

  kRoadsEditDesk,
  kRoadsEditAccept,
  kRoadsEditClose,
  kRoadsEditName,
  kRoadsEditNameBack,
  kRoadsColorPalette,
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
  kFencesSlotsFlipPointEdge_Back,
  kFencesSlotsFlipSelectEdit_Back,
  kFencesSlotsFlipPointEdge,
  kFencesSlotsFlipSelectEdit,

  kFencesEditDesk,
  kFencesEditAccept,
  kFencesEditClose,
  kFencesEditName,
  kFencesEditNameBack,
  kFencesColorPalette,
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
  kBiomesSlotsFlipPointEdge_Back,
  kBiomesSlotsFlipSelectEdit_Back,
  kBiomesSlotsFlipPointEdge,
  kBiomesSlotsFlipSelectEdit,

  kBiomesEditDesk,
  kBiomesEditAccept,
  kBiomesEditClose,
  kBiomesEditName,
  kBiomesEditNameBack,
  kBiomesColorPalette,
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

  kConfirmationDesk,
  kConfirmationTextLabel,
  kConfirmationClose,
  kConfirmationAccept,
  kConfirmationDecline,

  kTipInfoDesk,
  kTipInfoText,

  kFileDesk,
  kFileText,
  kFileTextInput,
  kFileAccept,
  kFileClose,

  kSpareText1,
  kSpareText2,
  kSpareText3,
  kSpareText4,
  kSpareText5,
  kSpareText6,
  kSpareText7,
  kSpareText8,
  kSpareText9,
  kSpareText10,

  kFullSCreen,
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
