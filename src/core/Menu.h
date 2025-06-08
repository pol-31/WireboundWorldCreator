#ifndef WIREBOUNDWORLDCREATOR_SRC_MENU_H_
#define WIREBOUNDWORLDCREATOR_SRC_MENU_H_

#include "../modes/IUiMode.h"
#include "Ui.h"
#include "../common/TextRenderer.h"

class UiSharedResources;

/// here we switch modes
class Menu {
 public:

  //TODO: includes settings

  Menu(UiSharedResources& ui_shared_resources,
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

  void Render(bool show);

  void RenderPicking();

  int Hover(uint32_t global_id);

  bool Press(uint32_t global_id);

  void Release();

  void Parse();

  void Serialize();

  /// why public: for manual switching (e.g. at init)
  void SetMode(int id);

 private:
  UiSharedResources& ui_shared_resources_;
  TextRenderer& text_renderer_;

  UiTabMenu ui_tab_menu_;
  UiSettings ui_settings_;

  bool show_settings_{false};

  IUiMode*& cur_mode_;

  std::array<IUiMode*, 8> modes_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MENU_H_
