#include "UiTilesMode.h"

#include <iostream>

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../common/TextRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"

UiTilesMode::UiTilesMode(UiRenderData& render_data,
                         WindowQueue& window_queue)
    : sp_mode_(data::VboIdMain::kTilesTilesMode),
      btn_up_(data::VboIdMain::kTilesUp,
              [this] { std::cout << "btn_up" << std::endl; }),
      btn_down_(data::VboIdMain::kTilesDown,
                [this] { std::cout << "btn_down" << std::endl; }),
      btn_left_(data::VboIdMain::kTilesLeft,
                [this] { std::cout << "btn_left" << std::endl; }),
      btn_right_(data::VboIdMain::kTilesRight,
                 [this] { std::cout << "btn_right" << std::endl; }),
      ui_event_handler_({&btn_up_, &btn_down_, &btn_left_, &btn_right_}) {}

void UiTilesMode::Render(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  // sp_mode_.Render();
  // btn_up_.Render();
  // btn_down_.Render();
  // btn_left_.Render();
  // btn_right_.Render();
}

void UiTilesMode::RenderPicking(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  // sp_mode_.RenderPicking();
  // btn_up_.RenderPicking();
  // btn_down_.RenderPicking();
  // btn_left_.RenderPicking();
  // btn_right_.RenderPicking();
}

void UiTilesMode::Setup() {}

void UiTilesMode::BindDefaultCallbacks() {}

void UiTilesMode::PrerenderText(TextRenderer* text_renderer) {
  text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kScaleTerrain),
      static_cast<int>(data::TextId::kStrength) + 1);
}
