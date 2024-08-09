#ifndef WIREBOUND__ASSETS_TEXT_TEXTURE_DATA_H_
#define WIREBOUND__ASSETS_TEXT_TEXTURE_DATA_H_

#include <array>

enum class UiVboDataTextId;

inline consteval std::size_t GetUiDataTextOffset(UiVboDataTextId id);

enum class UiVboDataTextId {
  kMode,
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
};

inline consteval std::size_t GetUiDataTextOffset(UiVboDataTextId id) {
  return static_cast<std::size_t>(id) * 8;
}

namespace details {

// tex coords were generated with https://github.com/pol-31/WireboundTextBaker;
inline constexpr std::array<float, 400> kUiVboDataText = {
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

} // namespace details

#endif  // WIREBOUND__ASSETS_TEXT_TEXTURE_DATA_H_
