#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICONFIGWINDOW_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICONFIGWINDOW_H_

#include <span>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "WindowQueue.h"
#include "UiComplex.h"
#include "../modes/UiSharedResources.h"
#include "../common/TextRenderer.h"
#include "../common/Text.h"

class UiConfigWindow final : public UiWindowAppear {
 public:
  UiConfigWindow(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  UiConfigWindow(UiConfigWindow&& other) noexcept;

  bool Render() override;

  void RenderPicking() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Press(int id) override;

  void Release() override;

  void SetNoise(std::span<float> value,
                std::span<data::TextId> text_id,
                data::TextId name_id);

 private:
  void RenderSlotsSprites(glm::vec2 mouse_pos);

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  UiTextModeId name_;
  UiDynamicSprite btn_save_;

  //  UiDynamicSprite sp_sl_track_;
  //  UiDynamicSprite sp_sl_handle_;
  UiWindowSlider sl_data_;

  UiSliderH2 config_slider_;
  UiTextModeId config_text_;

  //TODO: dublicating the size
  std::span<float> value_;
  std::span<data::TextId> text_id_;

  int sl_pressed_config_ = -1;

  //  UiEventHandler<gWidgetsNum> ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICONFIGWINDOW_H_
