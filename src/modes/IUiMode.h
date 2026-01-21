#ifndef WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_

#include "../core/Ui.h"
#include "UiSharedResources.h"

class IUiMode {
 public:
  IUiMode(UiSharedResources& ui_shared_resources, UiDynamicSprite&& sp_mode)
      : ui_shared_resources_(ui_shared_resources),
        sp_mode_(std::move(sp_mode)) {}

  virtual ~IUiMode() = default;

  virtual void Render() = 0;

  virtual void RenderPicking() = 0;

  /// setup mode before switching to it
  virtual void Setup() = 0;

  /// in case of mode-menu fast switching (no mode changed)
  virtual void BindDefaultCallbacks() = 0;

  UiSharedResources& ui_shared_resources_;
  UiDynamicSprite sp_mode_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IUIMODE_H_
