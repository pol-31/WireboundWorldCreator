#ifndef WIREBOUNDWORLDCREATOR_UIMENU_H
#define WIREBOUNDWORLDCREATOR_UIMENU_H

#include "Ui.h"
#include "UiComplex.h"

class UiMenu final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiMenu(UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
         IUiMode* terrain_mode, IUiMode* water_mode, IUiMode* placement_mode,
         IUiMode* objects_mode, IUiMode* biomes_mode, IUiMode* tiles_mode,
         IUiMode* player_mode, IUiMode*& cur_mode);

  UiMenu(UiMenu&& other) noexcept;
  UiMenu(const UiMenu& other) = delete;

  UiMenu& operator=(UiMenu&& other) = delete;
  UiMenu& operator=(const UiMenu& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  void SetSelectedArrow(int id);

  void SetMode(int id);

  void BindCallbacks();

 private:
  void SetCircleTransform();

  static void SetSpTransform(UiDynamicSprite& sp, int id, int total_num,
                             float radius);

  static void SetTg4Transform(UiToggle4& sp, int id, int total_num,
                              float radius);

  UiDynamicSprite btn_terrain_;
  UiDynamicSprite btn_water_;
  UiDynamicSprite btn_placement_;
  UiDynamicSprite btn_objects_;
  UiDynamicSprite btn_biomes_;
  UiDynamicSprite btn_tiles_;
  UiDynamicSprite btn_player_;

  UiToggle4 tg_terrain_;
  UiToggle4 tg_water_;
  UiToggle4 tg_placement_;
  UiToggle4 tg_objects_;
  UiToggle4 tg_biomes_;
  UiToggle4 tg_tiles_;

  UiDynamicSprite btn_shader_wirebound_;
  UiToggle4 toggle_shaders_;

  UiDynamicSprite arrow_select_;
  UiDynamicSprite arrow_selected_;
  UiDynamicSprite save_data_;
  UiDynamicSprite load_data_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kMenuLoad) -
                 static_cast<int>(data::VboIdMain::kMenuTerrain) + 1>
      ui_event_handler_;

  float arrow_selected_angle_ = 0;

  IUiMode*& cur_mode_;
  std::array<IUiMode*, 9> modes_;
};

namespace menu {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

}  // namespace menu

#endif  // WIREBOUNDWORLDCREATOR_UIMENU_H
