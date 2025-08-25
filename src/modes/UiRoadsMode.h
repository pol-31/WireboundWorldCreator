#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../core/UiSlots.h"

class UiRoadsMode final : public IUiMode {
 public:
  UiRoadsMode(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue);

  void Render() override;
  void RenderPicking() override;

  void Create(GLuint id);

  void Remove();

  void BakeAsphalt();
  void BakeGravel();
  void BakeSoil();

  void BindCallbacks() override;

 protected:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiStaticSprite btn_bake_asphalt_;
  UiStaticSprite btn_bake_gravel_;
  UiStaticSprite btn_bake_soil_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_
