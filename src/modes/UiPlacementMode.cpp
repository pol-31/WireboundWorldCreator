#include "UiPlacementMode.h"

#include "../common/Callbacks.h"
#include "../common/GraphBakeConfig.h"
#include "../common/OpenGLUtility.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiPlacementMode::UiPlacementMode(UiRenderData& render_data,
                                 UiSlots& ui_slots, WindowQueue& window_queue,
                                 UiEditSlots& ui_edit_slots,
                                 UiEditConfigSlTxt& value_config,
                                 ModelManager& mdl_manager)
    : sp_mode_(data::VboIdMain::kPlacementPlacementMode),
      btn_trees_(data::VboIdMain::kPlacementTrees,
                 [this]() { SetPlacementMode(GetPlacementTree(), 0); }),
      btn_bushes_(data::VboIdMain::kPlacementBushes,
                  [this]() { SetPlacementMode(GetPlacementBushes(), 1); }),
      btn_tall_grass(
          data::VboIdMain::kPlacementTallGrass,
          [this]() { SetPlacementMode(GetPlacementTallGrass(), 2); }),
      btn_undergrowth_(
          data::VboIdMain::kPlacementUndergrowth,
          [this]() { SetPlacementMode(GetPlacementUndergrowth(), 3); }),
      btn_change_mode_(data::VboIdMain::kPlacementChangeMode,
                       [this]() { TogglePlacement(); }),
      ui_selection_(render_data),
      ui_event_handler_({&btn_trees_, &btn_bushes_, &btn_tall_grass,
                         &btn_undergrowth_, &btn_change_mode_}),
      sp_selected_mode_({data::VboIdMain::kPlacementSelected}, &btn_trees_),
      mdl_manager_(mdl_manager),
      map_points_(mdl_manager),
      ui_slots_(ui_slots),
      ui_edit_(render_data, ui_edit_slots, value_config),
      render_data_(render_data),
      mouse_transform_(render_data) {}

void UiPlacementMode::SetPlacementMode(Texture* tex_placement, int id) {
  if (IsPreviewMode()) {
    TogglePlacement();
  }
  tex_cur_placement_ = tex_placement;
  ui_selection_.SetMask(*tex_cur_placement_);
  ui_slots_.DeSelect();
  ui_selection_.SetIdBounds(details::kIdOffsetTerrain, details::kIdOffsetWater);
  sp_selected_mode_.SetSelected(id);
}

void UiPlacementMode::TogglePlacement() {
  if (IsPreviewMode()) {
    // tile_renderer->show_terrain_ = false;
  } else {
    // tile_renderer->show_terrain_ = true;
  }
  preview_mode_ = !preview_mode_;
  auto tile_renderer = render_data_.glfw_context_.tile_renderer;
  tile_renderer->UpdatePipeline();
  Tile& tile = tile_renderer->cur_tile_;
  mdl_manager_.tree_.SetPlacement(render_data_, tile.placement_trees_);
  mdl_manager_.bush_.SetPlacement(render_data_, tile.placement_bushes_);
  mdl_manager_.tall_grass_.SetPlacement(render_data_,
                                        tile.placement_tall_grass_);
  mdl_manager_.undergrowth_.SetPlacement(render_data_,
                                         tile.placement_undergrowth_);
}

void UiPlacementMode::OnSelectedSlotChanged() {
  std::vector<MapPoint>* points = nullptr;
  std::vector<glm::uvec2>* joints = nullptr;
  if (ui_slots_.GetSelectedSlotId() != -1) {
    points = &ui_edit_.GetInstanceData().map_points;
    joints = &ui_edit_.GetInstanceData().map_joints;
  }
  map_points_.SetData(points, joints);
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  map_points_.UpdateJointsBuffer();
}

void UiPlacementMode::Setup() {
  ui_slots_.Setup(&ui_edit_, [this] { this->OnSelectedSlotChanged(); });
  BindDefaultCallbacks();
  btn_trees_.Press();
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
}

void UiPlacementMode::PrerenderText(TextRenderer* text_renderer) {
  text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kPlacementCurveAmplitude),
      static_cast<int>(data::TextId::kPlacementRadiusFlat) + 1);
}

void UiPlacementMode::BindDefaultCallbacks() {
  glfwSetScrollCallback(gWindow, placement::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, placement::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, placement::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

void UiPlacementMode::Render(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->Render();
  if (!IsPreviewMode()) {
    ui_selection_.Render();
    ui_selection_.RenderOnSurface(
        &render_data.glfw_context_.tile_renderer->cur_tile_
             .map_terrain_height);
  }
  if (ui_slots_.GetSelectedSlotId() != -1) {
    auto color = ui_slots_.GetInstanceBaseData()->color;
    map_points_.RenderPoints(color);
    auto map_scale =
        render_data.glfw_context_.tile_renderer->cur_tile_.map_scale;
    map_points_.RenderJoints(render_data.glfw_context_.tile_renderer
      ->cur_tile_.map_terrain_height, map_scale, color);
  }

  mdl_manager_.RenderPlacement();
  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_.Bind();

  sp_mode_.Render();
  btn_trees_.Render();
  btn_bushes_.Render();
  btn_tall_grass.Render();
  btn_undergrowth_.Render();
  btn_change_mode_.Render();
  if (ui_slots_.GetSelectedSlotId() == -1) {
    sp_selected_mode_.Render();
  }
  ui_slots_.Render();
}

void UiPlacementMode::RenderPicking(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->RenderPicking();
  map_points_.RenderPickingPoints();
  glActiveTexture(GL_TEXTURE0);
  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  btn_trees_.RenderPicking();
  btn_bushes_.RenderPicking();
  btn_tall_grass.RenderPicking();
  btn_undergrowth_.RenderPicking();
  btn_change_mode_.RenderPicking();
  ui_slots_.RenderPicking();
}

void UiPlacementMode::HandleSelection(const std::set<GLuint>& selected_ids) {
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

void UiPlacementMode::CancelTransform() {
  BindDefaultCallbacks();
  auto map_points = map_points_.GetPoints();
  mouse_transform_.TranslateSelectedMapPointsBack(*map_points);
  map_points_.UpdateJointsBuffer();
}

void UiPlacementMode::ApplyTransform() { BindDefaultCallbacks(); }

namespace placement {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  if (placement->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    placement->ui_slots_.Scroll(pressed_id, yoffset);
    glfw_context->windows->Scroll(pressed_id, yoffset);
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        placement->ui_event_handler_.Press(pressed_id) ||
                        placement->ui_slots_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (placement->map_points_.GetPoints()) {
        if (placement->anything_selected_ &&
            pressed_id > details::kIdOffsetObjects &&
            pressed_id < details::kIdOffsetUi) {
          placement->mouse_transform_.InitTransform();
          glfwSetScrollCallback(gWindow, nullptr);
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbSelected);
          glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_LmbSelected);
          glfwSetKeyCallback(gWindow, KeyCallback_LmbSelected);
        } else {
          placement->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_,
                                         mod_ctrl, mod_shift);
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
          glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
          glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
        }
      } else {
        placement->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_,
                                       mod_ctrl, mod_shift);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
        glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
      }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id > details::kIdOffsetObjects &&
          placement->anything_selected_ && pressed_id < details::kIdOffsetUi) {
        placement->map_points_.AddJoints(pressed_id);
      } else if (pressed_id < details::kIdOffsetWater) {
        return placement->map_points_.AddPoint(pressed_id);
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
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    glfw_context->windows->Release();
    placement->ui_slots_.Release();
    placement->ui_event_handler_.Release();
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
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
    placement->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    placement->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    placement->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    placement->ui_selection_.SetMode(SelectionMode::kTweak);
  }
}

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    placement->BindDefaultCallbacks();
    if (placement->map_points_.GetPoints()) {
      auto selected_ids = placement->ui_selection_.StopIntoSet(
          glfw_context->cursor_pos_tex_norm_);
      placement->HandleSelection(selected_ids);
    }
    placement->ui_selection_.Stop(glfw_context->cursor_pos_tex_norm_);
    const auto& selection_mask = placement->ui_selection_.GetMask();
    glCopyImageSubData(selection_mask.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
                       placement->tex_cur_placement_->GetId(), GL_TEXTURE_2D, 0,
                       0, 0, 0, details::gTerrainSize, details::gTerrainSize,
                       1);
  }
}

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  placement->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  auto map_points = placement->map_points_.GetPoints();
  placement->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos,
                                                         ypos);
  placement->map_points_.UpdateJointsBuffer();
}

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    placement->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    placement->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    // TODO: NextAxis();
  }
}

void KeyCallback_LmbSelected(GLFWwindow* window, int key, int scancode,
                             int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      return placement->CancelTransform();
    case GLFW_KEY_ENTER:
      return placement->ApplyTransform();
  }
  placement->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
}

}  // namespace placement

Texture* UiPlacementMode::GetPlacementTree() {
  return &render_data_.glfw_context_.tile_renderer->cur_tile_
              .tex_placement_trees_;
}

Texture* UiPlacementMode::GetPlacementBushes() {
  return &render_data_.glfw_context_.tile_renderer->cur_tile_
              .tex_placement_bushes_;
}

Texture* UiPlacementMode::GetPlacementTallGrass() {
  return &render_data_.glfw_context_.tile_renderer->cur_tile_
              .tex_placement_tall_grass_;
}

Texture* UiPlacementMode::GetPlacementUndergrowth() {
  return &render_data_.glfw_context_.tile_renderer->cur_tile_
              .tex_placement_undergrowth_;
}
