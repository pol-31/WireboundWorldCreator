#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include <functional>

#include "../common/BaseInstanceData.h"
#include "Ui.h"
#include "UiComplex.h"

class IUiEdit;

class UiSlots : public UiBase {
 public:
  UiSlots(
      UiRenderData& render_data, TextRenderer& text_renderer,
      std::function<void()> on_selection = [] {});

  UiSlots(UiSlots&& other) = delete;
  UiSlots(const UiSlots& other) = delete;

  UiSlots& operator=(UiSlots&& other) = delete;
  UiSlots& operator=(const UiSlots& other) = delete;

  void Setup(IUiEdit* ui_edit, std::function<void()> on_selection = [] {});

  void CreateGraph();
  void SelectGraph(GLuint id);
  void RemoveGraph(GLuint id);
  [[nodiscard]] int GetSize() const noexcept;

  void Render();

  void RenderPicking();

  int GetHoveredSlotId();

  int GetSelectedSlotId() const;

  bool Press(int pressed_id);

  //  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform() override;

  void DeSelect();

  void Reset();

  BaseInstanceData* GetInstanceBaseData();

  /// to init values
  void AddInstance(BaseInstanceData&& data);

  [[nodiscard]] bool IsSelected() const noexcept {
    return GetSelectedSlotId() != -1;
  }

  static constexpr size_t gMaxLayers = 10;

 private:
  void RenderSlotsSprites();

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  void SetupUiHierarchy();

  std::function<void()> on_selection_;

  IUiEdit* ui_edit_ = nullptr;
  int* selected_id_ = nullptr;

  UiSprite back_;
  UiSprite create_;

  UiText slot_name_;
  UiSprite slot_config_;
  UiToggle4 toggle_slot_visible_;

  UiSprite slot_back_;
  UiSprite slot_color_;
  UiSprite slot_remove_;
  UiSprite slot_selected_;

  UiWindowSlider sl_data_;
  UiRenderData& render_data_;
  UiEventHandler ui_event_handler_;
  UiHierarchy hierarchy_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
