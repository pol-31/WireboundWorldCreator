#include "Menu.h"

#include <glad/glad.h>

#include "../modes/UiSharedResources.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"
#include "../core/TileRenderer.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"

void Menu::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto menu = dynamic_cast<Menu*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
//    menu->ui_tab_menu_.Scroll(pressed_id, yoffset);
    global_data->windows->Scroll(pressed_id, yoffset);
  }
}

void Menu::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
//    global_data->menu->Press(pressed_id);
    global_data->windows->Press(pressed_id);
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
//    global_data->menu->Release();
    global_data->windows->Release();
  }
}

void Menu::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

Menu::Menu(
    UiSharedResources& ui_shared_resources,
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
    IUiMode*& cur_mode)
    : ui_shared_resources_(ui_shared_resources),
      text_renderer_(text_renderer),
      window_queue_(window_queue),
      modes_({terrain_mode, water_mode, roads_mode, fences_mode,
              placement_mode, objects_mode, biomes_mode, tiles_mode}),
      cur_mode_(cur_mode),
      /// text is first, then modes, then vision, then shaders (Menu::Press)
      /// TODO; more description
      /// first three are text, so not used as simple rendering, but only for text
      ui_tab_menu_(
          {data::VboIdMain::kMenuDesk},
          1.0f,
          {{data::VboIdMain::kMenuDeskPinBack, [](){}},
           {data::VboIdMain::kMenuDeskPinPoint}},
          ui_shared_resources_,
          window_queue,
          {data::VboIdMain::kMenuTerrain,
           [this]() {
             this->SetMode(0);
           }},
          {data::VboIdMain::kMenuWater,
           [this]() {
             this->SetMode(1);
           }},
          {data::VboIdMain::kMenuRoads,
           [this]() {
             this->SetMode(2);
           }},
          {data::VboIdMain::kMenuFences,
           [this]() {
             this->SetMode(3);
           }},
          {data::VboIdMain::kMenuPlacement,
           [this]() {
             this->SetMode(4);
           }},
          {data::VboIdMain::kMenuObjects,
           [this]() {
             this->SetMode(5);
           }},
          {data::VboIdMain::kMenuBiomes,
           [this]() {
             this->SetMode(6);
           }},
          {data::VboIdMain::kMenuTiles,
           [this]() {
             this->SetMode(7);
           }},
          {{data::VboIdMain::kMenuTerrainOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = !state;
            }},
           {data::VboIdMain::kMenuTerrainOn1},
           {data::VboIdMain::kMenuTerrainOn2},
           {data::VboIdMain::kMenuTerrainOn3}
          },
          {{data::VboIdMain::kMenuWaterOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_ = !state;
            }},
           {data::VboIdMain::kMenuWaterOn1},
           {data::VboIdMain::kMenuWaterOn2},
           {data::VboIdMain::kMenuWaterOn3}
          },
          {{data::VboIdMain::kMenuRoadsOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_ = !state;
            }},
           {data::VboIdMain::kMenuRoadsOn1},
           {data::VboIdMain::kMenuRoadsOn2},
           {data::VboIdMain::kMenuRoadsOn3}
          },
          {{data::VboIdMain::kMenuFencesOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_ = !state;
            }},
           {data::VboIdMain::kMenuFencesOn1},
           {data::VboIdMain::kMenuFencesOn2},
           {data::VboIdMain::kMenuFencesOn3}
          },
          {{data::VboIdMain::kMenuPlacementOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_ = !state;
            }},
           {data::VboIdMain::kMenuPlacementOn1},
           {data::VboIdMain::kMenuPlacementOn2},
           {data::VboIdMain::kMenuPlacementOn3}
          },
          {{data::VboIdMain::kMenuObjectsOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_ = !state;
            }},
           {data::VboIdMain::kMenuObjectsOn1},
           {data::VboIdMain::kMenuObjectsOn2},
           {data::VboIdMain::kMenuObjectsOn3}
          },
          {{data::VboIdMain::kMenuBiomesOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_ = !state;
            }},
           {data::VboIdMain::kMenuBiomesOn1},
           {data::VboIdMain::kMenuBiomesOn2},
           {data::VboIdMain::kMenuBiomesOn3}
          },
          {{data::VboIdMain::kMenuTilesOff,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_ = !state;
            }},
           {data::VboIdMain::kMenuTilesOn1},
           {data::VboIdMain::kMenuTilesOn2},
           {data::VboIdMain::kMenuTilesOn3}
          },
          {data::VboIdMain::kMenuShaderWirebound},
          {{data::VboIdMain::kMenuShadersOff,
            [this]() {
              std::cout << "toggle shaders" << std::endl;
            }},
           {data::VboIdMain::kMenuShadersOn1},
           {data::VboIdMain::kMenuShadersOn2},
           {data::VboIdMain::kMenuShadersOn3}
          },
          {data::VboIdMain::kMenuArrowSelect},
          {data::VboIdMain::kMenuArrowSelected},
          {data::VboIdMain::kMenuSave},
          {data::VboIdMain::kMenuLoad}) {}

void Menu::Show() {
  ui_tab_menu_.Show();
}

void Menu::Hide() {
  ui_tab_menu_.Hide();
}

void Menu::SetMode(int id) {
  cur_mode_ = modes_[id];
  arrow_selected_angle_ =
      static_cast<float>(id) * 2.0f * glm::pi<float>() / 8.0f;
  ui_tab_menu_.SetSelectedArrow(arrow_selected_angle_);
  cur_mode_->BindCallbacks();
  cur_mode_->PrerenderText();
}

void Menu::Render(bool show) {
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();

  auto mouse_pos = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;

  if (show) {
//    ui_tab_menu_.Render(show, arrow_select_angle_, arrow_selected_angle_);
  } else {
//    Release();
  }
}

void Menu::RenderPicking(bool show) {
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  /// first three for text (we don;t need picking for it)
  if (show) {
    ui_tab_menu_.RenderPicking();
  }
}

void Menu::Hover() {
  auto local_id = ui_shared_resources_.global_glfw_callback_data_.hovered_id
                  - details::kIdOffsetUi;
  if (local_id < 40) {
    // intentionally local_id as int
    arrow_select_angle_ =
        static_cast<float>(local_id / 5) * 2.0f * glm::pi<float>() / 8.0f;
    ui_tab_menu_.SetSelectArrow(arrow_select_angle_);
  }
}

bool Menu::Press(uint32_t global_id) {
  ui_tab_menu_.Press(global_id);
  return false;
}

void Menu::Release() {
  ui_tab_menu_.Release();
}

void Menu::Parse() {}

void Menu::Serialize() {}

void Menu::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}
