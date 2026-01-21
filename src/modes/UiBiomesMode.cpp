#include "UiBiomesMode.h"

#include "../common/Callbacks.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiBiomesMode::UiBiomesMode(UiSharedResources& ui_shared_resources,
                           UiSlots& ui_slots, WindowQueue& window_queue,
                           TextRenderer& text_renderer,
                           UiEditSlots& ui_edit_slots,
                           UiEditConfigSlTxt& value_config,
                           ModelManager& mdl_manager)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kBiomesBiomesMode}),
      sp_biome_(data::VboIdMain::kMapTomb),
      ui_selection_(ui_shared_resources),
      mouse_transform_(ui_shared_resources),
      ui_slots_(ui_slots),
      ui_edit_(ui_shared_resources, ui_edit_slots, value_config),
      map_points_(mdl_manager) {}

void UiBiomesMode::Setup() {
  ui_shared_resources_.glfw_context_.text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kWindAngle),
      static_cast<int>(data::TextId::kSunColorB) + 1);
  ui_slots_.Setup(&ui_edit_, [this] {
    std::vector<MapPoint>* points = nullptr;
    if (ui_slots_.GetSelectedSlotId() != -1) {
      points = &ui_edit_.GetInstanceData().map_points;
    }
    map_points_.SetData(points, nullptr);
  });
  BindDefaultCallbacks();
  auto camera = ui_shared_resources_.glfw_context_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f);  // to update camera vectors
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
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

void UiBiomesMode::Render() {
  ui_shared_resources_.glfw_context_.tile_renderer->Render();
  ui_selection_.Render();
  if (ui_slots_.GetSelectedSlotId() != -1) {
    map_points_.RenderPoints(ui_slots_.GetInstanceBaseData()->color);
  }

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.shader_sp_.Bind();
  sp_mode_.Render();

  auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_;
  ui_slots_.Render(mouse_pos);

  ui_shared_resources_.glfw_context_.windows->Render();
  auto camera = ui_shared_resources_.glfw_context_.camera;
  camera->Update();  // const pos
}

void UiBiomesMode::RenderPicking() {
  ui_shared_resources_.glfw_context_.tile_renderer->RenderPicking();
  map_points_.RenderPickingPoints();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
  ui_shared_resources_.glfw_context_.windows->RenderPicking();
}

void UiBiomesMode::HandleSelection(const std::set<GLuint>& selected_ids) {
  auto map_points = map_points_.GetPoints();
  anything_selected_ = false;
  for (int i = 0; i < map_points->size(); ++i) {
    auto it = selected_ids.find(i);
    if (it != selected_ids.end()) {
      anything_selected_ = true;
      (*map_points)[i].selected = true;
    } else {
      (*map_points)[i].selected = false;
    }
  }
}

void UiBiomesMode::CancelTransform() {
  BindDefaultCallbacks();
  auto map_points = map_points_.GetPoints();
  mouse_transform_.TranslateSelectedMapPointsBack(*map_points);
}

void UiBiomesMode::ApplyTransform() { BindDefaultCallbacks(); }

namespace biomes {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  if (biomes->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    biomes->ui_slots_.Scroll(pressed_id, yoffset);
    glfw_context->windows->Scroll(pressed_id, yoffset);
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        biomes->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (biomes->anything_selected_ &&
          pressed_id > details::kIdOffsetObjects &&
          pressed_id < details::kIdOffsetUi) {
        double xpos, ypos;
        glfwGetCursorPos(gWindow, &xpos, &ypos);
        lastX = xpos;
        lastY = ypos;
        biomes->mouse_transform_.InitTransform();
        glfwSetScrollCallback(gWindow, nullptr);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbSelected);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_LmbSelected);
        glfwSetKeyCallback(gWindow, KeyCallback_LmbSelected);
        return;
      }
      if (biomes->ui_slots_.GetSelectedSlotId() != -1) {
        biomes->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_,
                                    mod_ctrl, mod_shift);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
        glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
      }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id < details::kIdOffsetWater &&
          biomes->ui_slots_.GetSelectedSlotId() != -1) {
        biomes->map_points_.AddPoint(pressed_id);
      }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow,
                                 callbacks::CursorPosCallback_MmbShift);
      } else {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_Mmb);
      }
      glfwSetMouseButtonCallback(gWindow,
                                 callbacks::MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    }
  } else {  // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      biomes->ui_slots_.Release();
      glfw_context->windows->Release();
    }
    // if GLFW_RELEASE... if not process... we haven't done anything...
  }
}

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  if (action == GLFW_PRESS) {
    biomes->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    biomes->BindDefaultCallbacks();
    auto selected_ids =
        biomes->ui_selection_.StopIntoSet(glfw_context->cursor_pos_tex_norm_);
    biomes->HandleSelection(selected_ids);
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    if (mod_shift) {
      glfwSetWindowShouldClose(window, true);
    } else if (!glfw_context->windows->GetTopWindow() &&
               glfw_context->windows->GetSize() == 0) {
      glfw_context->ui_renderer->AskForConfirmation(
          data::TextId::kConfirmationExit,
          []() { glfwSetWindowShouldClose(gWindow, true); });
    } else {
      glfw_context->windows->BtnEscape();
    }
  } else if (key == GLFW_KEY_ENTER) {
    glfw_context->windows->BtnEnter();
  } else if (key == GLFW_KEY_1) {
    biomes->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    biomes->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    biomes->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    biomes->ui_selection_.SetMode(SelectionMode::kTweak);
  }
}

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  biomes->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  auto map_points = biomes->map_points_.GetPoints();
  biomes->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos, ypos);
}

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    biomes->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    biomes->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    // TODO: NextAxis();
  }
}

void KeyCallback_LmbSelected(GLFWwindow* window, int key, int scancode,
                             int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto biomes = dynamic_cast<UiBiomesMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      return biomes->CancelTransform();
    case GLFW_KEY_ENTER:
      return biomes->ApplyTransform();
  }
  biomes->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
}

}  // namespace biomes
