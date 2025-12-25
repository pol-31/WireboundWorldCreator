#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITWATER_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITWATER_H_

#include <random>

#include "IUiEdit.h"
#include "../core/Ui.h"
#include "../core/TileRenderer.h"
#include "UiSharedResources.h"
#include "../core/WindowQueue.h"
#include "../core/UiConfigWindow.h"
#include "UiEditShared.h"
#include "UiWaterConfig.h"
#include "../common/BaseInstanceData.h"

//TODO:
// ocean instance: 1 instance as 1 layer.... let's refactor.....
// river instance: 1 source as 1 layer

//TODO: different kWaterEditDesk

class UiEditOcean : public IUiEdit {
 public:
  using Base = IUiEdit;
  UiEditOcean(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      std::vector<BaseInstanceData>& base_instances,
      const int& selected_id,
      UiConfigWindow& ui_ocean_config);

  UiEditOcean(UiEditOcean&& other) noexcept;

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  void Generate();

  void RandomGenerate();

  [[nodiscard]] OceanTraits& GetInstanceData() noexcept;

  void RenderGraph();

  void Reset() override;

 private:
  void MergeLayers();

  void RenderOceanConfig();

  void RenderPickingOceanConfig();

  std::vector<OceanTraits> instances_;
  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;

  std::mt19937 random_generator_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;
  UiDynamicSprite random_generate_;

  UiEditConfigOcean ocean_layer_config_;
  UiConfigWindow& ui_ocean_config_;

  std::array<OceanLayerConfig, 3> ocean_layers_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kWaterEditOceanLayerName) -
      static_cast<int>(data::VboIdMain::kWaterEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

struct RiverTraits {
  float transparency;
  float viscosity;
};

class UiEditRiver : public IUiEdit {
 public:
  using Base = IUiEdit;
  UiEditRiver(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer,
      std::vector<BaseInstanceData>& base_instances,
      const int& selected_id);

  UiEditRiver(UiEditRiver&& other) noexcept;

  void HideAll() override;

  void CreateInstance() override;

  void UpdateConfig() override;

  void SetInstanceId(int id) override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Render() override;

  void RenderPicking() override;

  void Generate();

  void RandomGenerate();

  [[nodiscard]] RiverTraits& GetInstanceData() noexcept;

  void RenderGraph();

  void Reset() override;

 private:
  void MergeLayers();

  void RenderRiverConfig();

  void RenderPickingRiverConfig();

  std::vector<RiverTraits> instances_;
  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;

  std::mt19937 random_generator_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;
  UiDynamicSprite random_generate_;

  UiEditConfigSlTxt river_layer_config_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kWaterEditOceanLayerName) -
      static_cast<int>(data::VboIdMain::kWaterEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITWATER_H_
