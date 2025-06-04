#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_

#include "Ui.h"

class UiSlots final : public UiTransformDbg, public UiCallable {
 public:
  UiSlots(SharedResources& shared_resources,
          UiDynamicSprite&& handler,
          UiDynamicSprite&& slider,
          UiDynamicSprite&& back,
          UiDynamicSprite&& create,
          vbos::VboIdMain flip_select_edit_back_vbo_texture, text::Id flip_select_edit_back_text_id,
          vbos::VboIdMain flip_point_edge_back_vbo_texture, text::Id flip_point_edge_back_text_id,
          UiDynamicSprite&& flip_select_edit_sprite,
          UiDynamicSprite&& flip_point_edge_sprite,
          UiDynamicSprite&& slot_back,
          UiDynamicSprite&& slot_remove,
          UiDynamicSprite&& slot_selected,
          ArbitraryGraph& graph);

  UiSlots(UiSlots&& other) noexcept;
  UiSlots(const UiSlots& other) = delete;

  UiSlots& operator=(UiSlots&& other) = delete;
  UiSlots& operator=(const UiSlots& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking();

  [[nodiscard]] size_t Hover(std::uint32_t id);

  void Press() override;

  bool Press(int id);

  size_t Hover(int id) {
    return ui_event_handler_.Hover(id);
  }

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

  void FlipSelectEdit();

  void FlipPointEdge();

 private:
  int GetSlotId();

  /// --- as a slider ---
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  void UpdateRenderData();

  void FocusOnSelected(int slot_id);

  UiDynamicSprite handler_;
  UiDynamicSprite slider_;
  UiDynamicSprite back_;
  UiDynamicSprite create_;
  UiDynamicSprite flip_select_edit_back_;
  UiDynamicSprite flip_point_edge_back_;

  UiDynamicSprite flip_select_edit_sprite_;
  UiDynamicSprite flip_point_edge_sprite_;
  UiSpriteTransformation flip_select_edit_;
  UiSpriteTransformation flip_point_edge_;

  UiDynamicSprite slot_back_;
  UiDynamicSprite slot_remove_;
  UiDynamicSprite slot_selected_;

  /// --- as a slider ---

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float length_slots_;
  static const float kTrackLengthFactor;
  static const float kSlotsLengthFactor;
  static const int kSlotsNum;

  float slot_height_{0.0f};
  int scissors_start_{0};
  int scissors_length_{0};

  /// so we could get id related to slots (0-5)
  int cur_slots_offset_{0};
  glm::vec2 start_slot_translate_{0.0f};

  // for shader bindings & mouse pos
  SharedResources& shared_resources_;

  /// --- as a complex component ---

  UiEventHandler<
      static_cast<int>(vbos::VboIdMain::kFencesSlotsFlipSelectEdit) -
      static_cast<int>(vbos::VboIdMain::kFencesSlotsSlot) + 1
      > ui_event_handler_;

  /// --- graph - related ---

  ArbitraryGraph& graph_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UISLOTS_H_
