#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_

#include <random>

#include "../common/BaseInstanceData.h"
#include "../common/models/ModelManager.h"
#include "../core/TileRenderer.h"
#include "../core/Ui.h"
#include "../core/UiConfigWindow.h"
#include "../core/WindowQueue.h"
#include "IUiEdit.h"
#include "UiEditShared.h"
#include "UiSharedResources.h"

struct ObjectTraits {
  float hp = 100.0f;
  float speed = 1.0f;
  float attack = 1.0f;
  float attack_speed = 1.0f;
};

class UiEditObjects : public IUiEdit {
 public:
  using Base = IUiEdit;
  UiEditObjects(UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue, TextRenderer& text_renderer,
                std::vector<BaseInstanceData>& base_instances,
                const int& selected_id, ModelManager& mdl_manager);

  UiEditObjects(UiEditObjects&& other) noexcept;

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

  [[nodiscard]] ObjectTraits& GetInstanceData() noexcept;

  void RenderGraph();

  void Reset() override;

 private:
  void MergeLayers();

  void RenderModelConfig();

  void RenderPickingModelConfig();

  std::vector<ObjectTraits> instances_;
  const int& selected_id_;
  std::vector<BaseInstanceData>& base_instances_;

  std::mt19937 random_generator_;

  UiDynamicSprite accept_;
  UiTextInput name_;
  UiDynamicSprite random_generate_;

  UiEditConfigSlTxt value_config_;

  UiEventHandler<static_cast<int>(
                     data::VboIdMain::kObjectsEditCharacteristicIcon) -
                 static_cast<int>(data::VboIdMain::kObjectsEditDesk) + 1>
      ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
  ModelManager& mdl_manager_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITOBJECTS_H_
