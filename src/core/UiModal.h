#ifndef WIREBOUNDWORLDCREATOR_UIMODAL_H
#define WIREBOUNDWORLDCREATOR_UIMODAL_H

#include "../modes/UiSharedResources.h"
#include "Ui.h"
#include "UiText.h"

class UiConfirmation {
 public:
  UiConfirmation(UiSharedResources& ui_shared_resources,
                 TextRenderer& text_renderer);

  void Render();

  void RenderPicking();

  void Show(data::TextId text_id, std::function<void()>&& callable);

  void Hide();

  [[nodiscard]] bool IsActive() const noexcept { return active_; }

 private:
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);

  bool active_ = false;
  UiTextMenuId text_;
  UiSprite sp_accept_;
  UiSprite sp_decline_;
  UiSprite sp_background_;

  std::function<void()> callable_;
  UiHierarchy hierarchy_;
  UiSharedResources& ui_shared_resources_;
};

class UiWarning {
 public:
  UiWarning(UiSharedResources& ui_shared_resources,
            TextRenderer& text_renderer);

  void Render();

  void RenderPicking();

  void Show(data::TextId text_id);

  void Hide();

  [[nodiscard]] bool IsActive() const noexcept { return active_; }

 private:
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);

  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);

  bool active_ = false;
  UiTextMenuId text_;
  UiSprite sp_background_;
  UiHierarchy hierarchy_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_UIMODAL_H
