#include "UiPlacementMode.h"

#include "../common/Callbacks.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiPlacementMode::UiPlacementMode(UiSharedResources& ui_shared_resources,
                                 WindowQueue& window_queue,
                                 ModelManager& mdl_manager)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kPlacementPlacementMode}),
      btn_trees_(data::VboIdMain::kPlacementTrees,
                 [this]() {
                   SetPlacementMode(&tex_placement_trees_);
                   sp_selected_mode_.SetSelected(0);
                 }),
      btn_bushes_(data::VboIdMain::kPlacementBushes,
                  [this]() {
                    SetPlacementMode(&tex_placement_bushes_);
                    sp_selected_mode_.SetSelected(1);
                  }),
      btn_tall_grass(data::VboIdMain::kPlacementTallGrass,
                     [this]() {
                       SetPlacementMode(&tex_placement_tall_grass_);
                       sp_selected_mode_.SetSelected(2);
                     }),
      btn_undergrowth_(data::VboIdMain::kPlacementUndergrowth,
                       [this]() {
                         SetPlacementMode(&tex_placement_undergrowth_);
                         sp_selected_mode_.SetSelected(3);
                       }),
      btn_asphalt_(data::VboIdMain::kPlacementAsphalt,
                   [this]() {
                     ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
                     map_points_.SetData(&asphalt_map_points_, &asphalt_map_joints_);
                         sp_selected_mode_.SetSelected(4);
                     map_points_.UpdateJointsBuffer();
                   }),
      btn_gravel_(data::VboIdMain::kPlacemenGravel,
                  [this]() {
                     ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
                    map_points_.SetData(&gravel_map_points_, &gravel_map_joints_);
                         sp_selected_mode_.SetSelected(5);
                     map_points_.UpdateJointsBuffer();
                  }),
      btn_soil_(data::VboIdMain::kPlacementSoil,
                [this]() {
                     ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
                  map_points_.SetData(&soil_map_points_, &soil_map_joints_);
                         sp_selected_mode_.SetSelected(6);
                     map_points_.UpdateJointsBuffer();
                }),
      btn_change_mode_(data::VboIdMain::kPlacementChangeMode,
                       [this]() { TogglePlacement(); }),
      ui_selection_(ui_shared_resources),
      ui_event_handler_({&btn_trees_, &btn_bushes_, &btn_tall_grass,
                         &btn_undergrowth_, &btn_asphalt_, &btn_gravel_,
                         &btn_soil_, &btn_change_mode_}),
      tex_placement_trees_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_bushes_(details::gTerrainSize, details::gTerrainSize,
                            GL_R8),
      tex_placement_tall_grass_(details::gTerrainSize, details::gTerrainSize,
                                GL_R8),
      tex_placement_undergrowth_(details::gTerrainSize, details::gTerrainSize,
                                 GL_R8),
      sp_selected_mode_({data::VboIdMain::kPlacementSelected}, &btn_trees_),
      mdl_manager_(mdl_manager),
      map_points_(mdl_manager),
      mouse_transform_(ui_shared_resources) {}

void UiPlacementMode::SetPlacementMode(Texture* tex_placement) {
  if (IsPreviewMode()) {
    TogglePlacement();
  }
  SetDrawTexture(tex_placement);
  map_points_.SetData(nullptr, nullptr);
  ui_selection_.SetIdBounds(details::kIdOffsetTerrain, details::kIdOffsetWater);
}

void UiPlacementMode::SetDrawTexture(Texture* tex_placement) {
  tex_cur_placement_ = tex_placement;
  ui_selection_.SetMask(*tex_cur_placement_);
}

void UiPlacementMode::TogglePlacement() {
  auto tile_renderer = ui_shared_resources_.gltf_context_.tile_renderer;
  if (IsPreviewMode()) {
    // tile_renderer->show_terrain_ = false;
  } else {
    if (tex_cur_placement_->GetId() == tex_placement_trees_.GetId()) {
      auto positions =
          tile_renderer->placement.UpdatePipeline(*tex_cur_placement_, 0);
      mdl_manager_.tree_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() == tex_placement_bushes_.GetId()) {
      auto positions =
          tile_renderer->placement.UpdatePipeline(*tex_cur_placement_, 1);
      mdl_manager_.bush_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() ==
               tex_placement_tall_grass_.GetId()) {
      auto positions =
          tile_renderer->placement.UpdatePipeline(*tex_cur_placement_, 2);
      mdl_manager_.tall_grass_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() ==
               tex_placement_undergrowth_.GetId()) {
      auto positions =
          tile_renderer->placement.UpdatePipeline(*tex_cur_placement_, 3);
      mdl_manager_.undergrowth_.SetPlacement(ui_shared_resources_, positions);
    }
    // tile_renderer->show_terrain_ = true;
  }
  preview_mode_ = !preview_mode_;
}

void UiPlacementMode::Setup() {
  BindDefaultCallbacks();
  SetPlacementMode(&tex_placement_trees_);
  auto camera = ui_shared_resources_.gltf_context_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f);  // to update camera vectors
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
}

void UiPlacementMode::BindDefaultCallbacks() {
  glfwSetScrollCallback(gWindow, placement::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, placement::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, placement::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

int UiPlacementMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain);
}

int UiPlacementMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kStrength) + 1;
}

void UiPlacementMode::RenderWorld() {
  ui_shared_resources_.gltf_context_.tile_renderer->Render();
}

void UiPlacementMode::RenderPickingWorld() {
  ui_shared_resources_.gltf_context_.tile_renderer->RenderPicking();
}

void UiPlacementMode::Render() {
  if (!IsPreviewMode()) {
    ui_selection_.Render();
    ui_selection_.RenderOnSurface(
        &ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_
             .map_terrain_height);
  }
  map_points_.RenderPoints(glm::vec4{1.0f});
  auto map_scale = ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_.map_scale;
  map_points_.RenderJoints(
    ui_shared_resources_.gltf_context_.tile_renderer->cur_tile_.map_terrain_height, map_scale);

  mdl_manager_.RenderPlacement();

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();

  sp_mode_.Render();
  btn_trees_.Render();
  btn_bushes_.Render();
  btn_tall_grass.Render();
  btn_undergrowth_.Render();
  btn_asphalt_.Render();
  btn_gravel_.Render();
  btn_soil_.Render();
  btn_change_mode_.Render();
  sp_selected_mode_.Render();

  ui_shared_resources_.gltf_context_.windows->Render();
  auto camera = ui_shared_resources_.gltf_context_.camera;
  camera->Update(1.0f);  // const pos
}

void UiPlacementMode::RenderPicking() {
  map_points_.RenderPickingPoints();

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_picking_.Bind();

  sp_mode_.RenderPicking();
  btn_trees_.RenderPicking();
  btn_bushes_.RenderPicking();
  btn_tall_grass.RenderPicking();
  btn_undergrowth_.RenderPicking();
  btn_asphalt_.RenderPicking();
  btn_gravel_.RenderPicking();
  btn_soil_.RenderPicking();
  btn_change_mode_.RenderPicking();

  ui_shared_resources_.gltf_context_.windows->RenderPicking();
}

void UiPlacementMode::HandleSelection(const std::set<GLuint>& selected_ids) {
  auto map_points = map_points_.GetPoints();
  anything_selected_ = false;
  for (int i = 0; i < map_points->size(); ++i) {
    auto it = selected_ids.find(details::kIdOffsetObjects + 100 + i);
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

void UiPlacementMode::ApplyTransform() {
  BindDefaultCallbacks();
}

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
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        placement->ui_event_handler_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (placement->map_points_.GetPoints()) {
        if (placement->anything_selected_ && pressed_id > details::kIdOffsetObjects &&
            pressed_id < details::kIdOffsetUi) {
          double xpos, ypos;
          glfwGetCursorPos(gWindow, &xpos, &ypos);
          lastX = xpos;
          lastY = ypos;
          placement->mouse_transform_.InitTransform();
          glfwSetScrollCallback(gWindow, nullptr);
          glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbSelected);
          glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_LmbSelected);
          glfwSetKeyCallback(gWindow, KeyCallback_LmbSelected);
          } else {
            placement->ui_selection_.Start(
            glfw_context->cursor_pos_tex_norm_, mod_ctrl, mod_shift);
            glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
            glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
            glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
          }
      } else {
        placement->ui_selection_.Start(
          glfw_context->cursor_pos_tex_norm_, mod_ctrl, mod_shift);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
        glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
      }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id > details::kIdOffsetObjects && placement->anything_selected_ &&
          pressed_id < details::kIdOffsetUi) {
        placement->map_points_.AddJoints(pressed_id);
          } else if (pressed_id < details::kIdOffsetWater) {
            return placement->map_points_.AddPoint({pressed_id, false});
          }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_MmbShift);
      } else {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_Mmb);
      }
      glfwSetMouseButtonCallback(gWindow,
                                 callbacks::MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    glfw_context->windows->Release();
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
      auto selected_ids = placement->ui_selection_.StopIntoSet(glfw_context->cursor_pos_tex_norm_);
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
  placement->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos, ypos);
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

void KeyCallback_LmbSelected(
  GLFWwindow* window, int key, int scancode, int action, int mods) {
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
