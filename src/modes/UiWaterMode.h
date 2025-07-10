#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../common/Vbos.h"
#include "../core/UiComplex.h"
#include "../core/UiSlots.h"
#include "../common/Paths.h"

class UiWaterMode final : public IUiMode {
 public:
  explicit UiWaterMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      const Paths& paths);

  void Render() override;
  void RenderPicking() override;
  data::TextId Hover(std::uint32_t global_id) override;

  /**
   * Lake: fill everything with height = max_height_ inside the polygon nd
   *   beyond if height allows
   * River: inside the polygon all heights = max_height_, while outside
   *   we bake wrt terrain height;
   * Waterfall: always bake wrt terrain height
   */
  void BakeLake();
  void BakeRiver();
  void BakeWaterfall();

  void BindCallbacks() override;

 protected:
  enum class WaterType {
    kLake,
    kRiver,
    kWaterfall,
  };

  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  OceanTraits CollectOceanTraits() {
    return {ocean_layer_config_1_.GetOceanLayerTraits(),
            ocean_layer_config_2_.GetOceanLayerTraits(),
            ocean_layer_config_3_.GetOceanLayerTraits()
    };
  }

  void ReBake();

  void UpdateOcean();

  bool ConfigModified();

  UiStaticSprite btn_bake_lake_;
  UiStaticSprite btn_bake_river_;
  UiStaticSprite btn_bake_waterfall_;
  UiStaticSprite btn_update_;
  //UiSlots slots_;

  UiWaterLayerConfig ocean_layer_config_1_;
  UiWaterLayerConfig ocean_layer_config_2_;
  UiWaterLayerConfig ocean_layer_config_3_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERMODE_H_
