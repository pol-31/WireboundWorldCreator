#include "UiFencesMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

void UiFencesMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<UiFencesMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1) &&
      fences->slots_.Scroll(pressed_id, yoffset)) {
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->DownScale();
  } else {
    global_data->tile_renderer->UpScale();
  }
}

void UiFencesMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto fences = dynamic_cast<UiFencesMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Press(pressed_id);
    } else {
      if (pressed_id < details::kIdOffsetWater) {
        std::cout << "Add point #" << pressed_id << std::endl;
        std::cout << "Coordinates are: " << (pressed_id & 1023)
                  << " and " << (pressed_id >> 10) << std::endl;
        //TODO: if water/other subtract maybe...
        fences->graphs_.Press(pressed_id);
      }
      fences->ui_event_handler_.Press(pressed_id);
      fences->slots_.Press(pressed_id);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
      global_data->menu->Release();
    } else {
      fences->ui_event_handler_.Release();
      fences->slots_.Release();
    }
  }
}

void UiFencesMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  /*  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
      } else if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SpeedUp();
      } else if (key == GLFW_KEY_BACKSPACE) {
        global_data->terrain_.ClearPoints();
      } else if (key == GLFW_KEY_ENTER) {
        global_data->terrain_.Bake();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = true;
      }
    } else if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera->SlowDown();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = false;
      }
    }*/
}

UiFencesMode::UiFencesMode(
    UiSharedResources& ui_shared_resources)
    : IUiMode(ui_shared_resources,
              UiStaticSprite{data::VboIdMain::kFencesUiFencesMode,
                             data::TextId::kNone}),
      btn_bake_picket_(data::VboIdMain::kFencesPicket, data::TextId::kNone),
      btn_bake_chain_linked_(data::VboIdMain::kFencesChainLink,
                             data::TextId::kNone),
      btn_bake_wooden_(data::VboIdMain::kFencesWooden, data::TextId::kNone),
      btn_remove_(data::VboIdMain::kFencesRemove, data::TextId::kNone),
      graphs_(ui_shared_resources_),
      slots_(
          ui_shared_resources_,
          {data::VboIdMain::kFencesSlotsHandler, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsSlider, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsBack, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsCreate, data::TextId::kNone,
           [this]() {
             this->graphs_.CreateGraph();
             ui_shared_resources_.global_glfw_callback_data_.StartCharInput(
                 graphs_.GetNamePtr(graphs_.GetSize() - 1));
           }},
          data::VboIdMain::kFencesSlotsFlipSelectEdit_Back, data::TextId::kNone,
          data::VboIdMain::kFencesSlotsFlipPointEdge_Back, data::TextId::kNone,
          {data::VboIdMain::kFencesSlotsFlipSelectEdit, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsFlipPointEdge, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsSlot, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsRemove, data::TextId::kNone},
          {data::VboIdMain::kFencesSlotsSelected, data::TextId::kNone},
          graphs_),
      ui_event_handler_({
          &btn_bake_picket_, &btn_bake_chain_linked_,
          &btn_bake_wooden_, &btn_remove_, &slots_}) {}

void UiFencesMode::Render() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_mode_.Render();
  btn_bake_picket_.Render();
  btn_bake_chain_linked_.Render();
  btn_bake_wooden_.Render();
  btn_remove_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  slots_.Render(mouse_pos);

  graphs_.Render();
}

void UiFencesMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_mode_.RenderPicking();
  btn_bake_picket_.RenderPicking();
  btn_bake_chain_linked_.RenderPicking();
  btn_bake_wooden_.RenderPicking();
  btn_remove_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  slots_.RenderPicking();
}

void UiFencesMode::Remove() {
  graphs_.Remove();
}

// auto insidePoints = GenHeightMap();
/// here water_height_map_ already contains either 0 or actual height
//    FloodFill(insidePoints);
//TODO: from points generate height map (!)
// wrt each triangle surface slope, we gen flow map (how - idk)

void UiFencesMode::BakePicket() {
  std::cout << "baked as a picket fence" << std::endl;
}

void UiFencesMode::BakeChainLinked() {
  std::cout << "baked as a chain linked fence" << std::endl;
}

void UiFencesMode::BakeWooden() {
  std::cout << "baked as a wooden fence" << std::endl;
}

void UiFencesMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
}

int UiFencesMode::Hover(std::uint32_t global_id) {
  return -1;
}
