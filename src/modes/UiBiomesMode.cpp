#include "UiBiomesMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../common/Callbacks.h"
#include "../renderers/UiRenderer.h"

UiBiomesMode::UiBiomesMode(
    UiSharedResources& ui_shared_resources,
    UiSlots& ui_slots,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kBiomesBiomesMode}),
      sp_biome_(data::VboIdMain::kMapTomb),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_slots_(ui_slots),
      ui_edit_(ui_shared_resources, window_queue, text_renderer,
               biomes_, ui_slots_.GetSelectedIdRef()) {}

void UiBiomesMode::Setup() {
  ui_slots_.Setup(&biomes_, &ui_edit_);
  BindDefaultCallbacks();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f); // to update camera vectors
//  ui_selection_.SetIdBounds(details::kIdK, details::kIdK);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
}

void UiBiomesMode::BindDefaultCallbacks() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, biomes::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, biomes::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, biomes::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

int UiBiomesMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiBiomesMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kSunB) + 1;
}

void UiBiomesMode::RenderWorld() {
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->Render();
}

void UiBiomesMode::RenderPickingWorld() {
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->RenderPicking();
}

void UiBiomesMode::Render() {
  ui_selection_.Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  sp_mode_.Render();

  auto mouse_pos
      = ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.global_glfw_callback_data_.windows->Render();
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->Update(1.0f); // const pos
}

void UiBiomesMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
}

void UiBiomesMode::HandleSelection() {
  const auto& tex_selected = ui_selection_.GetMask();
  std::vector<uint8_t> selected_pixels(
      tex_selected.GetHeight() * tex_selected.GetWidth());
  tex_selected.Bind();
  glGetTexImage(
      GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, selected_pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  int selected_num = 0;
  /*const auto& biomes = ui_edit_.GetInstancesRef();
  for (auto& b : biomes) {
    auto position = b.GetPosition();
    int x = static_cast<int>(position.x * 16.0f + 512.0f);
    int y = static_cast<int>(position.z * 16.0f + 512.0f);
    if (selected_pixels[y * 1024 + x] > 0) {
      b.Select();
      ++selected_num;
    } else {
      b.DeSelect();
    }
  }*/
  if (selected_num > 0) {
    anything_selected_ = true;
  } else {
    anything_selected_ = false;
  }
}

void UiBiomesMode::CancelTransform() {
  std::cout << "CancelTransform" << std::endl;
  mouse_transform_.CancelTransform();
  BindDefaultCallbacks();
/*  const auto& biomes = ui_edit_.GetInstancesRef();
  for (auto& b : biomes) {
    if (b.IsSelected()) {
      auto obj_translate = b.GetPosition();
      b.SetPosition(obj_translate - prev_translate_);
      b.UpdatePositionY(ui_shared_resources_);
    }
  }*/
}

void UiBiomesMode::ApplyTransform() {
  std::cout << "ApplyTransform" << std::endl;
  mouse_transform_.ApplyTransform();
  BindDefaultCallbacks();
  ResetTransform();
}

void UiBiomesMode::ResetTransform() {
  prev_translate_ = glm::vec3(0.0f);
  cur_translate_ = glm::vec3(0.0f);
}

void UiBiomesMode::UpdateTranslateForSelected() {
  glm::vec3 translate = -prev_translate_ + cur_translate_;
  /*const auto& biomes = ui_edit_.GetInstancesRef();
  for (auto& b : biomes) {
    if (b.IsSelected()) {
      auto obj_translate = b.GetPosition();
      b.SetPosition(obj_translate + translate);
      b.UpdatePositionY(ui_shared_resources_);
    }
  }*/
  prev_translate_ = cur_translate_;
}

void UiBiomesMode::SpawnObject(GLuint pressed_id) {
  glm::vec2 object_pos = {
      (pressed_id >> 10) / 16.0f - 32.0f,
      (pressed_id & 1023) / 16.0f - 32.0f
  };
//  ui_edit_.AddInstance(ui_slots_.GetInstanceBaseData()->type_id, object_pos);
}

namespace biomes {

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  if (biomes->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  if (yoffset < 0.0f) {
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
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
      auto pressed_id =
          global_data->picking_fbo->GetIdByMousePos(cursor_pos);
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = global_data->windows->Press(pressed_id) ||
                        biomes->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (biomes->ui_slots_.GetSelectedSlotId() != -1 &&
          pressed_id < details::kIdOffsetWater) {
        biomes->SpawnObject(pressed_id);
        return;
      }
      biomes->ui_selection_.Start(global_data->cursor_pos_tex_norm_, mod_ctrl,
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
      global_data->windows->Release();
      biomes->ui_slots_.Release();
    }
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
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
    biomes->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    biomes->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    biomes->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    biomes->ui_selection_.SetMode(SelectionMode::kTweak);
  }
  if (biomes->anything_selected_ && key == GLFW_KEY_G) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  }
}

/// --- transform ---

void BindCallbacksTransform() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
  glfwSetKeyCallback(gWindow, KeyCallbackTransform);
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  biomes->ResetTransform();
  biomes->mouse_transform_.InitTransform(
      &biomes->cur_translate_, nullptr, nullptr);
}

void MouseButtonCallbackTransform(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    biomes->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    biomes->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    //TODO: NextAxis();
  }
}

void KeyCallbackTransform(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    return biomes->CancelTransform();
  } else if (key == GLFW_KEY_ENTER) {
    return biomes->ApplyTransform();
  }
  if (mods & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      biomes->mouse_transform_.SetAxis({0.0f, 1.0f, 1.0f});
    } else if (key == GLFW_KEY_Y) {
      biomes->mouse_transform_.SetAxis({1.0f, 0.0f, 1.0f});
    } else if (key == GLFW_KEY_Z) {
      biomes->mouse_transform_.SetAxis({1.0f, 1.0f, 0.0f});
    }
  } else {
    if (key == GLFW_KEY_X) {
      biomes->mouse_transform_.SetAxis({1.0f, 0.0f, 0.0f});
    } else if (key == GLFW_KEY_Y) {
      biomes->mouse_transform_.SetAxis({0.0f, 1.0f, 0.0f});
    } else if (key == GLFW_KEY_Z) {
      biomes->mouse_transform_.SetAxis({0.0f, 0.0f, 1.0f});
    }
  }
}

void CursorPosCallback_G(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  biomes->mouse_transform_.TranslateSelected(xpos, ypos);
  biomes->UpdateTranslateForSelected();
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  biomes->ui_selection_.Update(global_data->cursor_pos_tex_norm_);
}

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto biomes = dynamic_cast<UiBiomesMode*>(*global_data->cur_mode);
  //  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_PRESS) {
    biomes->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    biomes->BindDefaultCallbacks();
    biomes->ui_selection_.Stop(global_data->cursor_pos_tex_norm_);
    biomes->HandleSelection();
  }
}

} // namespace biomes
