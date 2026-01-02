#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include "../common/BaseInstanceData.h"
#include "Ui.h"
#include "UiComplex.h"

class IUiEdit;

class UiSlots : public UiBase {
 public:
  UiSlots(UiSharedResources& ui_shared_resources, TextRenderer& text_renderer);

  UiSlots(UiSlots&& other) noexcept;
  UiSlots(const UiSlots& other) = delete;

  UiSlots& operator=(UiSlots&& other) = delete;
  UiSlots& operator=(const UiSlots& other) = delete;

  void Setup(std::vector<BaseInstanceData>* entries, IUiEdit* ui_edit);

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

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform() override;

  void Reset();

  BaseInstanceData* GetInstanceBaseData();

  [[nodiscard]] const int& GetSelectedIdRef() const noexcept {
    return selected_slot_id_;
  }

  /// to init values
  // TODO: need both for ui_slots_ & ui_edit_
  void AddInstance(BaseInstanceData&& data);

  static constexpr size_t gMaxLayers = 10;

 private:
  void RenderSlotsSprites();

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  void SetupUiHierarchy();

  IUiEdit* ui_edit_;
  std::vector<BaseInstanceData>* entries_ = nullptr;
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

  UiHierarchy hierarchy_;
  UiWindowSlider sl_data_;
  UiSharedResources& ui_shared_resources_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kSlotsCreate) -
                 static_cast<int>(data::VboIdMain::kSlotsName) + 1>
      ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
