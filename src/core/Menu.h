#ifndef WIREBOUNDWORLDCREATOR_SRC_MENU_H_
#define WIREBOUNDWORLDCREATOR_SRC_MENU_H_

#include "../modes/IUiMode.h"
#include "Ui.h"
#include "UiComplex.h"
#include "../common/TextRenderer.h"

class UiSharedResources;

/// here we switch modes
class Menu {
 public:

  //TODO: includes settings

  Menu(UiSharedResources& ui_shared_resources,
       WindowQueue& window_queue,
       TextRenderer& text_renderer,
       IUiMode* terrain_mode,
       IUiMode* water_mode,
       IUiMode* roads_mode,
       IUiMode* fences_mode,
       IUiMode* placement_mode,
       IUiMode* objects_mode,
       IUiMode* biomes_mode,
       IUiMode* tiles_mode,
       IUiMode*& cur_mode);

  // TODO: on switching reset all unsaved in previous, update

  void Show();

  void Hide();

  void Render(bool show);

  void RenderPicking(bool show);

  data::TextId Hover(bool show, uint32_t global_id);

  bool Press(uint32_t global_id);

  void Release();

  void Parse();

  void Serialize();

  /// why public: for manual switching (e.g. at init)
  void SetMode(int id);

  void BindCallbacks();

 private:
  static void ScrollCallback(
      GLFWwindow* window, double xoffset, double yoffset);

  static void MouseButtonCallback(
      GLFWwindow* window, int button, int action, int mods);

  static void KeyCallback(
      GLFWwindow* window, int key, int scancode, int action, int mods);

  UiSharedResources& ui_shared_resources_;
  TextRenderer& text_renderer_;

  // for Show() & Hide()
  WindowQueue& window_queue_;

  UiTabMenu ui_tab_menu_;

  IUiMode*& cur_mode_;
  // radians
  float arrow_select_angle_ = 0;
  float arrow_selected_angle_ = 0;

  std::array<IUiMode*, 8> modes_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MENU_H_
