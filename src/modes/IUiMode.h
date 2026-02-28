#ifndef WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_

class TileRenderer;
class UiRenderer__Deprecated;
class TextRenderer;

class IUiMode {
 public:
  virtual ~IUiMode() = default;

  virtual void Render(TileRenderer* tile_renderer,
                      UiRenderer__Deprecated* ui_renderer) = 0;

  virtual void RenderPicking(TileRenderer* tile_renderer,
                             UiRenderer__Deprecated* ui_renderer) = 0;

  /// setup mode before switching to it
  virtual void Setup() = 0;

  /// in case of mode-menu fast switching (no mode changed)
  virtual void BindDefaultCallbacks() = 0;

  virtual void PrerenderText(TextRenderer* text_renderer) = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
