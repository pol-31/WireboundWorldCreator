#include "Vbos.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace data {

std::array<SpriteTc, static_cast<int>(SpriteId::kTotal)> gUiSprites{};
std::array<UiTransform, static_cast<int>(UiId::kTotal)> gUiTransform{};

constexpr std::array<std::string_view, static_cast<size_t>(UiId::kTotal)>
    gUiNames = {
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

        "kMenuPlayer",
        "kMenuPlayerOff",
        "kMenuPlayerOn1",
        "kMenuPlayerOn2",
        "kMenuPlayerOn3",

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

        "kTerrainFalloffFill",
        "kTerrainFalloffBack",
        "kTerrainFalloffIcon",

        "kTerrainBake",
        "kTerrainReset",

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

        "kTerrainEditLabelInvert",
        "kTerrainEditLabelTiling",
        "kTerrainEditLabelStrength",

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

        "kWaterWaterMode",
        "kWaterUpdate",
        "kWaterOcean",
        "kWaterRiver",
        "kWaterSelected",

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
        "kWaterEditRandomGeneration",

        "kWaterEditRiverViscosityText",
        "kWaterEditRiverViscosityArea",
        "kWaterEditRiverViscosityIcon",
        "kWaterEditRiverTransparencyText",
        "kWaterEditRiverTransparencyArea",
        "kWaterEditRiverTransparencyIcon",

        "kWaterEditOceanVisibleOff",
        "kWaterEditOceanVisibleOn1",
        "kWaterEditOceanVisibleOn2",
        "kWaterEditOceanVisibleOn3",
        "kWaterEditOceanConfig",
        "kWaterEditOceanLayerName",

        "kPlacementPlacementMode",

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

        "kPlacementAsphalt",
        "kPlacementAsphaltOff",
        "kPlacementAsphaltOn1",
        "kPlacementAsphaltOn2",
        "kPlacementAsphaltOn3",

        "kPlacemenGravel",
        "kPlacementGravelOff",
        "kPlacementGravelOn1",
        "kPlacementGravelOn2",
        "kPlacementGravelOn3",

        "kPlacementSoil",
        "kPlacementSoilOff",
        "kPlacementSoilOn1",
        "kPlacementSoilOn2",
        "kPlacementSoilOn3",

        "kPlacementSelected",

        "kPlacementChangeMode",
        "kPlacementDrawOff",
        "kPlacementDrawOn1",
        "kPlacementDrawOn2",
        "kPlacementDrawOn3",

        "kObjectsObjectsMode",
        "kObjectsEditDesk",
        "kObjectsEditDeskPinBack",
        "kObjectsEditDeskPinPoint",
        "kObjectsEditAccept",
        "kObjectsEditName",
        "kObjectsEditNameBack",
        "kObjectsEditRandomGeneration",

        "kObjectsEditModelName",
        "kObjectsEditCharacteristicArea",
        "kObjectsEditCharacteristicIcon",

        "kBiomesBiomesMode",
        "kBiomesBiomePoint",

        "kBiomesEditDesk",
        "kBiomesEditDeskPinBack",
        "kBiomesEditDeskPinPoint",
        "kBiomesEditAccept",
        "kBiomesEditName",
        "kBiomesEditNameBack",
        "kBiomesColorPaletteHS",
        "kBiomesColorPaletteB",
        "kBiomesColorIndicator",
        "kBiomesColorBrightnessCursor",
        "kBiomesColorColorCursor",
        "kBiomesEditRandomGeneration",

        "kBiomesEditNoiseName",
        "kBiomesEditNoiseStrengthArea",
        "kBiomesEditNoiseStrengthIcon",

        "kTilesTilesMode",
        "kTilesLeft",
        "kTilesRight",
        "kTilesUp",
        "kTilesDown",
        "kTilesCreate",
        "kTilesRemove",

        "kPlayerPlayerMode",
        "kPlayerHealthPoint",
        "kPlayerStaminaPoint",

        "kPlayerGameObjInfoDesk",
        "kPlayerGameObjInfoPinBack",
        "kPlayerGameObjInfoPinPoint",
        "kPlayerGameObjInfoEnemy",
        "kPlayerGameObjInfoFriend",
        "kPlayerGameObjInfoNeutal",
        "kPlayerGameObjInfoObstacle",
        "kPlayerGameObjInfoName",
        "kPlayerGameObjInfoCharacteristic",
        "kPlayerGameObjInfoValue",

        "kPlayerPhone",
        "kPlayerPhoneMap",
        "kPlayerMapPlayer",
        "kPlayerMapEnemy",
        "kPlayerMapFriend",
        "kPlayerMapNeutral",
        "kPlayerMapObstacle",

        "kConfigWindowName",
        "kConfigWindowDesk",
        "kConfigWindowPinBack",
        "kConfigWindowPinPoint",
        "kConfigWindowHandler",
        "kConfigWindowSlider",
        "kConfigWindowSave",
        "kConfigWindowSliderText",
        "kConfigWindowSliderArea",
        "kConfigWindowSliderIcon",

        "kSlotsName",
        "kSlotsConfig",
        "kSlotsVisibleOff",
        "kSlotsVisibleOn1",
        "kSlotsVisibleOn2",
        "kSlotsVisibleOn3",
        "kSlotsSlot",
        "kSlotsSlotColor",
        "kSlotsRemove",
        "kSlotsSelected",
        "kSlotsHandler",
        "kSlotsSlider",
        "kSlotsBack",
        "kSlotsCreate",

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

        "kCursorRectangle",
        "kCursorCircle",
        "kCursorLasso",
        "kCursorTweak",

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

        "kButtonsTextKeys",
        "kButtonsTextMods",

        "kSelectionSprite",
        "kSelectionCircle",

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

constexpr std::array<std::string_view, static_cast<size_t>(UiId::kTotal)>
    gSpriteNames = {
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

        "kMenuPlayer",
        "kMenuPlayerOff",
        "kMenuPlayerOn1",
        "kMenuPlayerOn2",
        "kMenuPlayerOn3",

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

        "kTerrainFalloffFill",
        "kTerrainFalloffBack",
        "kTerrainFalloffIcon",

        "kTerrainBake",
        "kTerrainReset",

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

        "kTerrainEditLabelInvert",
        "kTerrainEditLabelTiling",
        "kTerrainEditLabelStrength",

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

        "kWaterWaterMode",
        "kWaterUpdate",
        "kWaterOcean",
        "kWaterRiver",
        "kWaterSelected",

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
        "kWaterEditRandomGeneration",

        "kWaterEditRiverViscosityText",
        "kWaterEditRiverViscosityArea",
        "kWaterEditRiverViscosityIcon",
        "kWaterEditRiverTransparencyText",
        "kWaterEditRiverTransparencyArea",
        "kWaterEditRiverTransparencyIcon",

        "kWaterEditOceanVisibleOff",
        "kWaterEditOceanVisibleOn1",
        "kWaterEditOceanVisibleOn2",
        "kWaterEditOceanVisibleOn3",
        "kWaterEditOceanConfig",
        "kWaterEditOceanLayerName",

        "kPlacementPlacementMode",

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

        "kPlacementAsphalt",
        "kPlacementAsphaltOff",
        "kPlacementAsphaltOn1",
        "kPlacementAsphaltOn2",
        "kPlacementAsphaltOn3",

        "kPlacemenGravel",
        "kPlacementGravelOff",
        "kPlacementGravelOn1",
        "kPlacementGravelOn2",
        "kPlacementGravelOn3",

        "kPlacementSoil",
        "kPlacementSoilOff",
        "kPlacementSoilOn1",
        "kPlacementSoilOn2",
        "kPlacementSoilOn3",

        "kPlacementSelected",

        "kPlacementChangeMode",
        "kPlacementDrawOff",
        "kPlacementDrawOn1",
        "kPlacementDrawOn2",
        "kPlacementDrawOn3",

        "kObjectsObjectsMode",
        "kObjectsEditDesk",
        "kObjectsEditDeskPinBack",
        "kObjectsEditDeskPinPoint",
        "kObjectsEditAccept",
        "kObjectsEditName",
        "kObjectsEditNameBack",
        "kObjectsEditRandomGeneration",

        "kObjectsEditModelName",
        "kObjectsEditCharacteristicArea",
        "kObjectsEditCharacteristicIcon",

        "kBiomesBiomesMode",
        "kBiomesBiomePoint",

        "kBiomesEditDesk",
        "kBiomesEditDeskPinBack",
        "kBiomesEditDeskPinPoint",
        "kBiomesEditAccept",
        "kBiomesEditName",
        "kBiomesEditNameBack",
        "kBiomesColorPaletteHS",
        "kBiomesColorPaletteB",
        "kBiomesColorIndicator",
        "kBiomesColorBrightnessCursor",
        "kBiomesColorColorCursor",
        "kBiomesEditRandomGeneration",

        "kBiomesEditNoiseName",
        "kBiomesEditNoiseStrengthArea",
        "kBiomesEditNoiseStrengthIcon",

        "kTilesTilesMode",
        "kTilesLeft",
        "kTilesRight",
        "kTilesUp",
        "kTilesDown",
        "kTilesCreate",
        "kTilesRemove",

        "kPlayerPlayerMode",
        "kPlayerHealthPoint",
        "kPlayerStaminaPoint",

        "kPlayerGameObjInfoDesk",
        "kPlayerGameObjInfoPinBack",
        "kPlayerGameObjInfoPinPoint",
        "kPlayerGameObjInfoEnemy",
        "kPlayerGameObjInfoFriend",
        "kPlayerGameObjInfoNeutal",
        "kPlayerGameObjInfoObstacle",
        "kPlayerGameObjInfoName",
        "kPlayerGameObjInfoCharacteristic",
        "kPlayerGameObjInfoValue",

        "kPlayerPhone",
        "kPlayerPhoneMap",
        "kPlayerMapPlayer",
        "kPlayerMapEnemy",
        "kPlayerMapFriend",
        "kPlayerMapNeutral",
        "kPlayerMapObstacle",

        "kConfigWindowName",
        "kConfigWindowDesk",
        "kConfigWindowPinBack",
        "kConfigWindowPinPoint",
        "kConfigWindowHandler",
        "kConfigWindowSlider",
        "kConfigWindowSave",
        "kConfigWindowSliderText",
        "kConfigWindowSliderArea",
        "kConfigWindowSliderIcon",

        "kSlotsName",
        "kSlotsConfig",
        "kSlotsVisibleOff",
        "kSlotsVisibleOn1",
        "kSlotsVisibleOn2",
        "kSlotsVisibleOn3",
        "kSlotsSlot",
        "kSlotsSlotColor",
        "kSlotsRemove",
        "kSlotsSelected",
        "kSlotsHandler",
        "kSlotsSlider",
        "kSlotsBack",
        "kSlotsCreate",

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

        "kCursorRectangle",
        "kCursorCircle",
        "kCursorLasso",
        "kCursorTweak",

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

        "kButtonsTextKeys",
        "kButtonsTextMods",

        "kSelectionSprite",
        "kSelectionCircle",

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

UiId UiIdFromString(std::string_view str) {
  for (size_t i = 0; i < gUiNames.size(); ++i) {
    if (gUiNames[i] == str) {
      return static_cast<UiId>(i);
    }
  }
  throw std::runtime_error("Unknown UiId: " + std::string(str));
}

std::string_view ToString(UiId id) { return gUiNames[static_cast<size_t>(id)]; }

const std::string gUiTransformPath = "../assets/UiTransform.txt";

void ReadUiTransform() {
  std::ifstream in_file(gUiTransformPath);
  if (!in_file.is_open()) {
    throw std::runtime_error("Unable to open UiTransform file");
  }
  std::string name;
  UiTransform t;
  while (in_file >> name >> t.position.x >> t.position.y >> t.scale.x >>
         t.scale.y >> t.rotation) {
    UiId id = UiIdFromString(name);
    gUiTransform[static_cast<size_t>(id)] = t;
  }
  if (!in_file.eof()) {
    throw std::runtime_error("Malformed UiTransform file");
  }
}

void WriteUiTransform() {
  std::ofstream out_file(gUiTransformPath, std::ios::trunc);
  if (!out_file.is_open()) {
    throw std::runtime_error("Unable to open UiTransform file for writing");
  }
  out_file << std::fixed << std::setprecision(6);
  for (size_t i = 0; i < gUiTransform.size(); ++i) {
    const UiTransform& t = gUiTransform[i];
    out_file << ToString(static_cast<UiId>(i)) << ' ' << t.position.x << ' '
             << t.position.y << ' ' << t.scale.x << ' ' << t.scale.y << ' '
             << t.rotation << '\n';
  }
}

SpriteId UiSpriteIdFromString(std::string_view str) {
  for (size_t i = 0; i < gSpriteNames.size(); ++i) {
    if (gSpriteNames[i] == str) {
      return static_cast<SpriteId>(i);
    }
  }
  throw std::runtime_error("Unknown UiId: " + std::string(str));
}

std::string_view ToString(SpriteId id) {
  return gSpriteNames[static_cast<size_t>(id)];
}

const std::string gUiSpritesPath = "../assets/UiSprites.txt";

void ReadUiSprites() {
  std::ifstream in_file(gUiSpritesPath);
  if (!in_file.is_open()) {
    throw std::runtime_error("Unable to open UiSprites file");
  }
  std::string name;
  SpriteTc t;
  while (in_file >> name >> t.lb.x >> t.lb.y >> t.rt.x >> t.rt.y) {
    SpriteId id = UiSpriteIdFromString(name);
    gUiSprites[static_cast<size_t>(id)] = t;
  }
  if (!in_file.eof()) {
    throw std::runtime_error("Malformed UiSprites file");
  }
}

}  // namespace data
