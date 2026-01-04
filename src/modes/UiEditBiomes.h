#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_

#include <random>

#include "../common/BaseInstanceData.h"
#include "../common/MapPoint.h"
#include "../core/TileRenderer.h"
#include "../core/Ui.h"
#include "../core/WindowQueue.h"
#include "IUiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"

struct BiomeTraits {
  std::vector<MapPoint> map_points;
  float wind_speed = .0f;
  float wind_angle = .0f;
  glm::vec3 sun_direction = glm::vec3(-1.0f);
  glm::vec3 sun_color = glm::vec3(1.0f);
};

class UiEditBiomes : public IUiEdit {
 public:
  using Base = IUiEdit;
  UiEditBiomes(UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue, TextRenderer& text_renderer,
               std::vector<BaseInstanceData>& base_instances,
               const int& selected_id);

  UiEditBiomes(UiEditBiomes&& other) noexcept;

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

  [[nodiscard]] BiomeTraits& GetInstanceData() noexcept;

  void RenderGraph();

  void Reset() override;

 private:
  void MergeLayers();

  void RenderModelConfig();

  void RenderPickingModelConfig();

  std::vector<BiomeTraits> instances_;
  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;

  std::mt19937 random_generator_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;
  UiDynamicSprite random_generate_;

  UiEditConfigSlTxt value_config_;

  UiEventHandler<static_cast<int>(
                     data::VboIdMain::kBiomesEditNoiseStrengthIcon) -
                 static_cast<int>(data::VboIdMain::kBiomesEditDesk) + 1>
      ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITBIOMES_H_
