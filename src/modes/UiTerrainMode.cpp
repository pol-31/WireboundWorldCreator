#include "UiTerrainMode.h"

#include <GLFW/glfw3.h>

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"
#include "../common/Callbacks.h"

UiTerrainMode::UiTerrainMode(
    UiSharedResources& ui_shared_resources,
    UiSlots& ui_slots,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    Tile& cur_tile,
    UiConfigWindow& ui_config_window)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kTerrainTerrainMode}),
      btn_update_(data::VboIdMain::kTerrainUpdate,
                  [this]() {
                    std::cout << "btn_update?" << std::endl;
                    ui_bake_.Bake(1000, 1000, 0.1f);
                  }),
      btn_reset_(data::VboIdMain::kTerrainFlatten,
                [this]() {
                  std::cerr << "-- reset --" << std::endl;
                  this->Reset();
                }),
      tg_flatten_(
          {data::VboIdMain::kTerrainFlattenOff,
           [this]() {
             std::cout << "toggle smoothness" << std::endl;
           }},
          {data::VboIdMain::kTerrainFlattenOn1},
          {data::VboIdMain::kTerrainFlattenOn2},
          {data::VboIdMain::kTerrainFlattenOn3}
          ),
      btn_bake_(data::VboIdMain::kTerrainBake,
                [this]() {
                  this->ui_bake_.Show();
                }),
      ui_bake_(
          cur_tile,
          {data::VboIdMain::kTerrainBakeDesk},
          1.0f,
          {{data::VboIdMain::kTerrainBakeDeskPinBack, []() {}},
           {data::VboIdMain::kTerrainBakeDeskPinPoint}},
          ui_shared_resources_,
          window_queue,
          text_renderer,
          {data::VboIdMain::kTerrainBakeAccept},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeErosionStepLabel},
          data::TextId::kErosion},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeErosionStepInputText},
           {data::VboIdMain::kTerrainBakeErosionStepInputBack}},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepLabel},
           data::TextId::kWeathering},
          {text_renderer,
           {data::VboIdMain::kTerrainBakeWeatheringStepInputText},
           {data::VboIdMain::kTerrainBakeWeatheringStepInputBack}}),
      layers_{},
      ui_slots_(ui_slots),
      ui_edit_(cur_tile, ui_shared_resources, window_queue,
               text_renderer, layers_,
               ui_slots_.GetSelectedIdRef(), ui_config_window),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_event_handler_({
          &btn_update_, &btn_reset_,
          &tg_flatten_, &btn_bake_/*, &ui_slots_*/}) {
}

void UiTerrainMode::Setup() {
  ui_slots_.Setup(&layers_, &ui_edit_);
  BindDefaultCallbacks();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f); // to update camera vectors
  ui_selection_.SetIdBounds(details::kIdOffsetTerrain, details::kIdOffsetWater);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
}

void UiTerrainMode::BindDefaultCallbacks() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, terrain::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, terrain::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, terrain::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

int UiTerrainMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiTerrainMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kStrength) + 1;
}

void UiTerrainMode::RenderWorld() {
  if (ui_slots_.GetSelectedSlotId() == -1) {
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer->Render();
  } else {
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
    ->RenderUiTerrain(ui_edit_.GetInstanceData().data.hmap);
  }
}

void UiTerrainMode::RenderPickingWorld() {
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->RenderPicking();
}

void UiTerrainMode::Render() {
  //TODO: should we call it RenderUi() and RenderTerrain()?
  ui_edit_.RenderGraph(); // should be first (terrain render before ui render)
  ui_selection_.Render();
  ui_selection_.RenderOnSurface(
      &ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_
           .map_terrain_height);

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  btn_reset_.Render();
  sp_mode_.Render();

  btn_update_.Render();
  btn_bake_.Render();

  tg_flatten_.Render();

  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.global_glfw_callback_data_.windows->Render();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->Update(1.0f); // const pos
}

void UiTerrainMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  btn_reset_.RenderPicking();
  sp_mode_.RenderPicking();

  btn_update_.RenderPicking();
  btn_bake_.RenderPicking();

  tg_flatten_.RenderPicking();

  ui_slots_.RenderPicking();
  auto& ui_layer_wireframe =
      ui_shared_resources_.global_glfw_callback_data_
          .ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderPickingLayerWireframe();
  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
}

void UiTerrainMode::CancelTransform() {
  std::cout << "CancelTransform" << std::endl;
  mouse_transform_.CancelTransform();
  BindDefaultCallbacks();
}

void UiTerrainMode::ApplyTransform() {
  std::cout << "ApplyTransform" << std::endl;
  mouse_transform_.ApplyTransform();
  BindDefaultCallbacks();
}

void UiTerrainMode::Reset() {
  ui_selection_.Stop(glm::vec2{0.0f});
  ui_bake_.ForceHide();
  ui_slots_.Reset();
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->cur_tile_.ResetTerrain();
  mouse_transform_.Reset();
  ui_edit_.Reset();
}

namespace terrain {

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  if (terrain->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    terrain->ui_slots_.Scroll(pressed_id, yoffset);
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  global_data->tile_renderer->cur_tile_.OnScroll(yoffset);
  //  terrain->slots_.UpdateTransformUniform();
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
//  global_data->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = global_data->windows->Press(pressed_id) ||
                        terrain->ui_slots_.Press(pressed_id) ||
                        terrain->ui_event_handler_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      terrain->ui_selection_.Start(
        global_data->cursor_pos_tex_norm_, mod_ctrl, mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (mod_shift) {
        terrain->ui_edit_.SetPivotPosition(pressed_id);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_RmbShift);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_RmbShift);
        glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
      }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_MmbShift);
      } else {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_Mmb);
      }
      glfwSetMouseButtonCallback(gWindow, callbacks::MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    }
  } else { // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      terrain->ui_slots_.Release();
      global_data->windows->Release();
    }
    // if GLFW_RELEASE... if not process... we haven't done anything...
  }
}

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) {
    terrain->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->ui_selection_.Stop(global_data->cursor_pos_tex_norm_);
    terrain->BindDefaultCallbacks();
  }
}

void MouseButtonCallback_RmbShift(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) { // we can press the same without release
    glfwSetCursorPosCallback(gWindow, nullptr);
    glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  } else { // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      glfwSetCursorPosCallback(gWindow, nullptr);
    }
    terrain->ui_selection_.Stop(global_data->cursor_pos_tex_norm_);
    glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
    glfwSetKeyCallback(gWindow, KeyCallback);
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    if (mod_shift) {
      glfwSetWindowShouldClose(window, true);
    } else if (!global_data->windows->GetTopWindow() &&
               global_data->windows->GetSize() == 0) {
      global_data->ui_renderer->AskForConfirmation(
          data::TextId::kConfirmationExit, []() {
            glfwSetWindowShouldClose(gWindow, true);
          });
    } else {
      global_data->windows->BtnEscape();
    }
  } else if (key == GLFW_KEY_ENTER) {
    global_data->windows->BtnEnter();
  } else if (key == GLFW_KEY_1) {
    terrain->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    terrain->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    terrain->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    terrain->ui_selection_.SetMode(SelectionMode::kTweak);
  }
  if (terrain->ui_slots_.GetSelectedSlotId() == -1) {
    if (key == GLFW_KEY_G) {
      BindCallbacksTransform(false);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_G_NonSelected);
    }
    return;
  }
  if (key == GLFW_KEY_G) {
    BindCallbacksTransform(true);
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  } else if (key == GLFW_KEY_S) {
    BindCallbacksTransform(true);
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
  } else if (key == GLFW_KEY_R) {
    BindCallbacksTransform(true);
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
  }
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->ui_selection_.Update(global_data->cursor_pos_tex_norm_);
}

void CursorPosCallback_RmbShift(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  std::cout << "Pressed id: " << pressed_id << std::endl;
  terrain->ui_edit_.SetPivotPosition(pressed_id);
}

/// ---

void BindCallbacksTransform(bool init_transform) {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
  glfwSetKeyCallback(gWindow, KeyCallbackTransform);
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  if (init_transform) {
    auto& data = terrain->ui_edit_.GetInstanceData();
    terrain->mouse_transform_.InitTransform(
        &data.translate, &data.rotate, &data.scale);
  }
}

void MouseButtonCallbackTransform(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    terrain->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    terrain->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    //TODO: NextAxis();
  }
}

void KeyCallbackTransform(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  if (mods & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      terrain->mouse_transform_.SetAxis({0.0f, 1.0f, 1.0f});
    } else if (key == GLFW_KEY_Y) {
      terrain->mouse_transform_.SetAxis({1.0f, 0.0f, 1.0f});
    } else if (key == GLFW_KEY_Z) {
      terrain->mouse_transform_.SetAxis({1.0f, 1.0f, 0.0f});
    }
  } else {
    if (key == GLFW_KEY_X) {
      terrain->mouse_transform_.SetAxis({1.0f, 0.0f, 0.0f});
    } else if (key == GLFW_KEY_Y) {
      terrain->mouse_transform_.SetAxis({0.0f, 1.0f, 0.0f});
    } else if (key == GLFW_KEY_Z) {
      terrain->mouse_transform_.SetAxis({0.0f, 0.0f, 1.0f});
    }
  }
  if (key == GLFW_KEY_ESCAPE) {
    return terrain->CancelTransform();
  } else if (key == GLFW_KEY_ENTER) {
    return terrain->ApplyTransform();
  }
  if (terrain->ui_slots_.GetSlotId() == -1) {
    if (key == GLFW_KEY_G) {
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_G_NonSelected);
      return terrain->CancelTransform();
    }
    return;
  }
  if (key == GLFW_KEY_G) {
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
    return terrain->CancelTransform();
  } else if (key == GLFW_KEY_R) {
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
    return terrain->CancelTransform();
  } else if (key == GLFW_KEY_S) {
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
    return terrain->CancelTransform();
  }
}

void CursorPosCallback_G(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->mouse_transform_.TranslateSelected(xpos, ypos);
}

// slot_id == -1 (non-selected)
void CursorPosCallback_G_NonSelected(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->mouse_transform_.TranslateSelectedVerticesUp(
      xpos, ypos, terrain->ui_selection_.GetMask(),
      terrain->tg_flatten_.TurnedOn());
}

void CursorPosCallback_R(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->mouse_transform_.RotateSelected(xpos, ypos);
}

void CursorPosCallback_S(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto terrain = dynamic_cast<UiTerrainMode*>(*global_data->cur_mode);
  terrain->mouse_transform_.ScaleSelected(xpos, ypos);
}

} // namespace terrain
