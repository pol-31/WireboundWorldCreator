#ifndef WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_

#include "../common/TextRenderer.h"
#include "../core/Ui.h"
#include "../core/WindowQueue.h"
#include "UiSharedResources.h"

class IUiMode {
 public:
  IUiMode(UiSharedResources& ui_shared_resources, UiDynamicSprite&& sp_mode)
      : ui_shared_resources_(ui_shared_resources),
        sp_mode_(std::move(sp_mode)) {}

  virtual void Render() = 0;

  virtual void RenderPicking() = 0;

  virtual void RenderWorld() = 0;

  virtual void RenderPickingWorld() = 0;

  /// setup mode before switching to it
  virtual void Setup() = 0;

  virtual void BindDefaultCallbacks() = 0;

  void PrerenderText() const noexcept {
    ui_shared_resources_.glfw_context_.text_renderer->PrerenderModeText(
        GetPrerenderTextIdStart(), GetPrerenderTextIdEnd());
  }

  virtual int GetPrerenderTextIdStart() const noexcept = 0;

  virtual int GetPrerenderTextIdEnd() const noexcept = 0;

  virtual std::vector<std::string> Serialize() { return {}; }

  virtual void Parse() {}

  UiSharedResources& ui_shared_resources_;
  UiDynamicSprite sp_mode_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
