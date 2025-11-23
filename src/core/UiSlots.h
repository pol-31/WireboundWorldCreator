#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include "Ui.h"
#include "UiComplex.h"
#include "../modes/UiTerrainWindows.h"

class UiSlots : public UiBase {
 public:
  UiSlots(Tile& cur_tile,
           UiSharedResources& ui_shared_resources,
           WindowQueue& window_queue,
           TextRenderer& text_renderer);

  UiSlots(UiSlots&& other) noexcept;
  UiSlots(const UiSlots& other) = delete;

  UiSlots& operator=(UiSlots&& other) = delete;
  UiSlots& operator=(const UiSlots& other) = delete;

  void CreateGraph();
  void SelectGraph(GLuint id);
  void RemoveGraph(GLuint id);
  [[nodiscard]] int GetSize() const noexcept;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking();

  int GetSlotId();

  int GetSelectedSlotId();

  bool Press(int pressed_id);

//  void Press() override;
//
  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform() override;

  void Reset();

  BaseInstanceData* GetInstanceBaseData();

  void SetConfigWindow(UiEditBase* ui_edit) {
    ui_edit_ = ui_edit;
  }

  [[nodiscard]] std::vector<BaseInstanceData>& GetInstancesRef() noexcept {
    return instances_;
  }

  [[nodiscard]] const int& GetSelectedIdRef() const noexcept {
    return selected_slot_id_;
  }

  /// to init values
  //TODO: need both for ui_slots_ & ui_edit_
  void AddInstance(BaseInstanceData&& data);

  static constexpr size_t gMaxLayers = 10;

 private:
  void RenderSlotsSprites();

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  std::vector<BaseInstanceData> instances_;
  int selected_slot_id_ = -1;

  UiDynamicSprite back_;
  UiDynamicSprite create_;

  UiText slot_name_;
  UiDynamicSprite slot_config_;
  UiToggle4 toggle_slot_visible_;

  UiDynamicSprite slot_back_;
  UiDynamicSprite slot_color_;
  UiDynamicSprite slot_remove_;
  UiDynamicSprite slot_selected_;

  UiEditBase* ui_edit_;
  UiHierarchy hierarchy_;
  UiWindowSlider sl_data_;
  IUiEventHandler* ui_event_handler_ = nullptr;
  UiSharedResources& ui_shared_resources_;
};

//TODO: UiEventHandler
/*template <size_t EventHandlerSize>
class UiSlotsBase : public UiBase {
 public:
  void Press() override {}
  void Release() override {
    ui_event_handler_->Release();
  }
  void Press(int id) {
    ui_event_handler_->Press(id);
  }

 protected:
  UiEventHandler<EventHandlerSize> ui_event_handler_;
};*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
