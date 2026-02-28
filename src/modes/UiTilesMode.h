#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_

#include "../core/Ui.h"
#include "../core/WindowQueue.h"
#include "../ui/UiRenderData.h"
#include "IUiMode.h"

class UiTilesMode final : public IUiMode {
 public:
  explicit UiTilesMode(UiRenderData& render_data, WindowQueue& window_queue);

  void Render(TileRenderer* tile_renderer,
              UiRenderer__Deprecated* ui_renderer) override;

  void RenderPicking(TileRenderer* tile_renderer,
                     UiRenderer__Deprecated* ui_renderer) override;

  void Setup() override;

  void BindDefaultCallbacks() override;

  void PrerenderText(TextRenderer* text_renderer) override;

 protected:
  UiSprite sp_mode_;
  UiSprite btn_up_;
  UiSprite btn_down_;
  UiSprite btn_left_;
  UiSprite btn_right_;

  UiEventHandler ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITILESMODE_H_
