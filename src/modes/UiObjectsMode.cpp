#include "UiObjectsMode.h"

#include "../common/Callbacks.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer__Deprecated.h"

UiObjectsMode::UiObjectsMode(UiRenderData& render_data, UiSlots& ui_slots,
                             WindowQueue& window_queue,
                             TextRenderer& text_renderer,
                             UiEditSlots& ui_edit_slots,
                             UiEditConfigSlTxt& value_config,
                             ModelManager& mdl_manager)
    : sp_mode_(data::UiId::kObjectsObjectsMode),
      ui_selection_(render_data),
      mouse_transform_(render_data),
      ui_slots_(ui_slots),
      ui_edit_(render_data, ui_edit_slots, value_config, mdl_manager),
      map_points_(mdl_manager),
      mdl_manager_(mdl_manager) {}

void UiObjectsMode::Setup() {
  UpdateModelsList();
  ui_slots_.Setup(&ui_edit_, [this] {
    std::vector<MapPoint>* points = nullptr;
    std::vector<glm::uvec2>* joints = nullptr;
    std::vector<glm::quat>* rotates = nullptr;
    std::vector<glm::vec3>* scales = nullptr;
    if (ui_slots_.GetSelectedSlotId() != -1) {
      points = &ui_edit_.GetInstanceData().map_points;
      joints = &ui_edit_.GetInstanceData().map_joints;
      rotates = &ui_edit_.GetInstanceData().rotations;
      scales = &ui_edit_.GetInstanceData().scales;
    }
    map_points_.SetData(points, joints, rotates, scales);
    map_points_.UpdateJointsBuffer();
  });
  BindDefaultCallbacks();
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
}

void UiObjectsMode::PrerenderText(TextRenderer* text_renderer) {
  text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kHp),
      static_cast<int>(data::TextId::kAttackSpeed) + 1);
}

void UiObjectsMode::BindDefaultCallbacks() {
  glfwSetScrollCallback(gWindow, objects::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, objects::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, objects::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

void UiObjectsMode::Render(TileRenderer* tile_renderer,
                           UiRenderer__Deprecated* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->Render();
  ui_selection_.Render();
  if (ui_slots_.GetSelectedSlotId() != -1) {
    auto color = ui_slots_.GetInstanceBaseData()->color;
    map_points_.RenderPoints(color);
    auto map_scale =
        render_data.glfw_context_.tile_renderer->cur_tile_.map_scale;
    map_points_.RenderJoints(
        render_data.glfw_context_.tile_renderer->cur_tile_.map_terrain_height,
        map_scale, color);
  }
  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_.Bind();
  sp_mode_.Render();

  ui_slots_.Render();
}

void UiObjectsMode::RenderPicking(TileRenderer* tile_renderer,
                                  UiRenderer__Deprecated* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->RenderPicking();
  map_points_.RenderPickingPoints();
  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  ui_slots_.RenderPicking();
}

void UiObjectsMode::UpdateModelsList() {
  ui_slots_.Reset();
  ui_edit_.SetModels(mdl_manager_.GetLoadedModels());
}

void UiObjectsMode::HandleSelection(const std::set<GLuint>& selected_ids) {
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

void UiObjectsMode::CancelTransform() {
  BindDefaultCallbacks();
  auto map_points = map_points_.GetPoints();
  auto rotates = map_points_.GetRotates();
  auto scales = map_points_.GetScales();
  mouse_transform_.TranslateSelectedMapPointsBack(*map_points);
  mouse_transform_.RotateSelectedMapPointsBack(*map_points, *rotates);
  mouse_transform_.ScaleSelectedMapPointsBack(*map_points, *scales);
  map_points_.UpdateJointsBuffer();
}

void UiObjectsMode::ApplyTransform() { BindDefaultCallbacks(); }

namespace objects {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  if (objects->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    objects->ui_slots_.Scroll(pressed_id, yoffset);
    glfw_context->windows->Scroll(pressed_id, yoffset);
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        objects->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      objects->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_, mod_ctrl,
                                   mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id > details::kIdOffsetObjects &&
          objects->anything_selected_ && pressed_id < details::kIdOffsetUi) {
        objects->map_points_.AddJoints(pressed_id);
      } else if (pressed_id < details::kIdOffsetWater) {
        return objects->map_points_.AddPoint(pressed_id);
      }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      callbacks::SetCameraCallbacks(mod_shift);
    }
  } else {  // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      glfw_context->windows->Release();
      objects->ui_slots_.Release();
    }
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    return;
  }
  if (key == GLFW_KEY_ESCAPE) {
    if (mod_shift) {
      glfwSetWindowShouldClose(window, true);
    } else if (glfw_context->windows->GetSize() == 0) {
      glfw_context->ui_confirmation->Show(data::TextId::kConfirmationExit, [] {
        glfwSetWindowShouldClose(gWindow, true);
      });
    }
  } else if (key == GLFW_KEY_1) {
    objects->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    objects->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    objects->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    objects->ui_selection_.SetMode(SelectionMode::kTweak);
  }
  if (!objects->anything_selected_) {
    return;
  }
  if (key == GLFW_KEY_G) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  } else if (key == GLFW_KEY_S) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
  } else if (key == GLFW_KEY_R) {
    BindCallbacksTransform();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
  }
}

/// --- transform ---

void BindCallbacksTransform() {
  glfwSetScrollCallback(gWindow, nullptr);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
  glfwSetKeyCallback(gWindow, KeyCallbackTransform);
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->mouse_transform_.InitTransform();
}

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    objects->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    // TODO: NextAxis();
  }
}

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      return objects->CancelTransform();
    case GLFW_KEY_ENTER:
      return objects->ApplyTransform();
    case GLFW_KEY_G:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
      return objects->CancelTransform();
    case GLFW_KEY_S:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_S);
      return objects->CancelTransform();
    case GLFW_KEY_R:
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_R);
      return objects->CancelTransform();
  }
  objects->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
}

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  objects->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos, ypos);
  objects->map_points_.UpdateJointsBuffer();
}

void CursorPosCallback_R(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  auto rotates = objects->map_points_.GetRotates();
  objects->mouse_transform_.RotateSelectedMapPoints(*map_points, *rotates, xpos,
                                                    ypos);
}

void CursorPosCallback_S(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  auto map_points = objects->map_points_.GetPoints();
  auto scales = objects->map_points_.GetScales();
  objects->mouse_transform_.ScaleSelectedMapPoints(*map_points, *scales, xpos,
                                                   ypos);
}

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  objects->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto objects = dynamic_cast<UiObjectsMode*>(*glfw_context->cur_mode);
  //  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  if (action == GLFW_PRESS) {
    objects->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    objects->BindDefaultCallbacks();
    if (objects->map_points_.GetPoints()) {
      auto selected_ids = objects->ui_selection_.StopIntoSet(
          glfw_context->cursor_pos_tex_norm_);
      objects->HandleSelection(selected_ids);
    }
  }
}

}  // namespace objects
