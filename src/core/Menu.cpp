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
          {data::VboIdMain::kMenuDesk, data::TextId::kNotYet},
          1.0f,
          {{data::VboIdMain::kMenuDeskPinBack, data::TextId::kNotYet, [](){}},
           {data::VboIdMain::kMenuDeskPinPoint, data::TextId::kNotYet}},
          ui_shared_resources_,
          window_queue,
          {data::VboIdMain::kMenuTerrain, data::TextId::kMenuTerrain,
           [this]() {
             this->SetMode(0);
           }},
          {data::VboIdMain::kMenuWater, data::TextId::kMenuWater,
           [this]() {
             this->SetMode(1);
           }},
          {data::VboIdMain::kMenuRoads, data::TextId::kMenuRoads,
           [this]() {
             this->SetMode(2);
           }},
          {data::VboIdMain::kMenuFences, data::TextId::kMenuFences,
           [this]() {
             this->SetMode(3);
           }},
          {data::VboIdMain::kMenuPlacement, data::TextId::kMenuPlacement,
           [this]() {
             this->SetMode(4);
           }},
          {data::VboIdMain::kMenuObjects, data::TextId::kMenuObjects,
           [this]() {
             this->SetMode(5);
           }},
          {data::VboIdMain::kMenuBiomes, data::TextId::kMenuBiomes,
           [this]() {
             this->SetMode(6);
           }},
          {data::VboIdMain::kMenuTiles, data::TextId::kMenuTiles,
           [this]() {
             this->SetMode(7);
           }},
          {{data::VboIdMain::kMenuTerrainOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = !state;
            }},
           {data::VboIdMain::kMenuTerrainOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuTerrainOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuTerrainOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuWaterOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_water_ = !state;
            }},
           {data::VboIdMain::kMenuWaterOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuWaterOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuWaterOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuRoadsOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_roads_ = !state;
            }},
           {data::VboIdMain::kMenuRoadsOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuRoadsOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuRoadsOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuFencesOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_fences_ = !state;
            }},
           {data::VboIdMain::kMenuFencesOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuFencesOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuFencesOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuPlacementOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_placement_ = !state;
            }},
           {data::VboIdMain::kMenuPlacementOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuPlacementOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuPlacementOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuObjectsOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_objects_ = !state;
            }},
           {data::VboIdMain::kMenuObjectsOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuObjectsOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuObjectsOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuBiomesOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_biomes_ = !state;
            }},
           {data::VboIdMain::kMenuBiomesOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuBiomesOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuBiomesOn3, data::TextId::kNotYet}
          },
          {{data::VboIdMain::kMenuTilesOff, data::TextId::kNotYet,
            [this]() {
              bool state = this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_;
              this->ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_tiles_ = !state;
            }},
           {data::VboIdMain::kMenuTilesOn1, data::TextId::kNotYet},
           {data::VboIdMain::kMenuTilesOn2, data::TextId::kNotYet},
           {data::VboIdMain::kMenuTilesOn3, data::TextId::kNotYet}
          },
          {data::VboIdMain::kMenuShaderWirebound, data::TextId::kNotYet},
          {{data::VboIdMain::kMenuShadersOff, data::TextId::kMenuTerrain,
            [this]() {
              std::cout << "toggle shaders" << std::endl;
            }},
           {data::VboIdMain::kMenuShadersOn1, data::TextId::kMenuTerrain},
           {data::VboIdMain::kMenuShadersOn2, data::TextId::kMenuTerrain},
           {data::VboIdMain::kMenuShadersOn3, data::TextId::kMenuTerrain}
          },
          {data::VboIdMain::kMenuArrowSelect, data::TextId::kNotYet},
          {data::VboIdMain::kMenuArrowSelected, data::TextId::kNotYet},
          {data::VboIdMain::kMenuSave, data::TextId::kNotYet},
          {data::VboIdMain::kMenuLoad, data::TextId::kNotYet}) {}

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
  auto text_ids = cur_mode_->GetPrerenderTextIds();
//  text_renderer_.PrerenderModeText(text_ids.x, text_ids.y);
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

data::TextId Menu::Hover(bool show, uint32_t global_id) {
  /*if (!show) {
    return data::TextId::kNone;
  }
  auto hovered = ui_tab_menu_.Hover(global_id);
  if (hovered == data::TextId::kNone) {
    return data::TextId::kNone;
  }*/
  auto local_id = global_id - details::kIdOffsetUi;
  if (local_id < 40) {
    // intentionally local_id as int
    arrow_select_angle_ =
        static_cast<float>(local_id / 5) * 2.0f * glm::pi<float>() / 8.0f;
    ui_tab_menu_.SetSelectArrow(arrow_select_angle_);
  }
  return data::TextId::kNone;
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
