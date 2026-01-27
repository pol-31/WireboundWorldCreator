#ifndef WIREBOUNDWORLDCREATOR_UIMENU_H
#define WIREBOUNDWORLDCREATOR_UIMENU_H

#include "Ui.h"
#include "UiComplex.h"

class UiMenu final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiMenu(UiSharedResources& ui_shared_resources, TextRenderer& text_renderer,
         WindowQueue& window_queue, IUiMode* terrain_mode, IUiMode* water_mode,
         IUiMode* placement_mode, IUiMode* objects_mode, IUiMode* biomes_mode,
         IUiMode* tiles_mode, IUiMode* player_mode, IUiMode*& cur_mode);

  bool Render() override;

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  void SetMode(int id);

  void BindCallbacks();

 private:
  void SetCircleTransform();

  static void SetSpTransform(UiSprite& sp, int id, int total_num, float radius);

  static void SetTg4Transform(UiToggle4& sp, int id, int total_num,
                              float radius);

  UiSprite btn_terrain_;
  UiSprite btn_water_;
  UiSprite btn_placement_;
  UiSprite btn_objects_;
  UiSprite btn_biomes_;
  UiSprite btn_tiles_;
  UiSprite btn_player_;

  UiToggle4 tg_terrain_;
  UiToggle4 tg_water_;
  UiToggle4 tg_placement_;
  UiToggle4 tg_objects_;
  UiToggle4 tg_biomes_;
  UiToggle4 tg_tiles_;

  UiSprite btn_shader_wirebound_;
  UiToggle4 toggle_shaders_;

  UiSprite arrow_select_;
  UiSprite arrow_selected_;
  UiSprite save_data_;
  UiSprite load_data_;

  UiTextInput text_filename_;

  UiEventHandler ui_event_handler_;
  UiHierarchy hierarchy_;

  float arrow_selected_angle_ = 0;

  UiTextMenuId txt_mode_;

  IUiMode*& cur_mode_;
  std::array<IUiMode*, 7> modes_;
};

namespace menu {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

}  // namespace menu

#endif  // WIREBOUNDWORLDCREATOR_UIMENU_H
