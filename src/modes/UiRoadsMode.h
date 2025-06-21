#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_

#include <glad/glad.h>

#include "IUiMode.h"
#include "../core/UiSlots.h"

class UiRoadsMode final : public IUiMode {
 public:
  UiRoadsMode(UiSharedResources& ui_shared_resources);

  void Render() override;
  void RenderPicking() override;
  data::TextId Hover(std::uint32_t global_id) override;

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
  UiStaticSprite btn_create_;
  UiStaticSprite btn_remove_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIROADSMODE_H_
