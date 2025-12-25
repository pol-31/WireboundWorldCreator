#include "UiWaterMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"
#include "../common/ShadersBinding.h"
#include "../common/Callbacks.h"

UiWaterMode::UiWaterMode(
    UiSharedResources& ui_shared_resources,
    UiSlots& ui_slots,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    UiConfigWindow& ui_config_window)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kWaterWaterMode}),
      btn_bake_ocean_(data::VboIdMain::kWaterOcean,
                     [this]() {
                     }),
      btn_bake_river_(data::VboIdMain::kWaterRiver,
                      [this]() {
                      }),
      btn_update_(data::VboIdMain::kWaterUpdate,
                  [this]() {
                  }),
      sp_selected_mode_(data::VboIdMain::kWaterSelected),
      ui_slots_(ui_slots),
      ui_edit_(ui_shared_resources, window_queue, text_renderer,
               instances_, ui_slots_.GetSelectedIdRef(), ui_config_window),
      ui_selection_(ui_shared_resources) {}

void UiWaterMode::Setup() {
  ui_slots_.Setup(&instances_, &ui_edit_);
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

void UiWaterMode::BindDefaultCallbacks() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, water::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, water::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, water::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

int UiWaterMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiWaterMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kLayer3) + 1;
}

void UiWaterMode::Render() {
//  ui_edit_.RenderGraph(); // should be first (terrain render before ui render)
  ui_selection_.Render();
  ui_selection_.RenderOnSurface(
      &ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_
           .map_terrain_height);
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  sp_mode_.Render();
  btn_bake_ocean_.Render();
  btn_bake_river_.Render();
  btn_update_.Render();
  sp_selected_mode_.Render();

  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.global_glfw_callback_data_.windows->Render();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->Update(1.0f); // const pos
}

void UiWaterMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  sp_mode_.RenderPicking();
  btn_bake_ocean_.RenderPicking();
  btn_bake_river_.RenderPicking();
  btn_update_.RenderPicking();
  sp_selected_mode_.RenderPicking();

  ui_slots_.RenderPicking();
  auto& ui_layer_wireframe =
      ui_shared_resources_.global_glfw_callback_data_
          .ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderPickingLayerWireframe();
  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
}

namespace water {

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
  if (water->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    water->ui_slots_.Scroll(pressed_id, yoffset);
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
  //  water->slots_.UpdateTransformUniform();
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
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
                        water->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      water->ui_selection_.Start(global_data->cursor_pos_tex_norm_, mod_ctrl,
                                   mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
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
      water->ui_slots_.Release();
      global_data->windows->Release();
    }
    // if GLFW_RELEASE... if not process... we haven't done anything...
  }
}

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) {
    water->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    water->ui_selection_.Stop(cursor_pos);
    water->BindDefaultCallbacks();
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
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
    water->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    water->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    water->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    water->ui_selection_.SetMode(SelectionMode::kTweak);
  }
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*global_data->cur_mode);
  water->ui_selection_.Update(global_data->cursor_pos_tex_norm_);
}

} // namespace water
