#include "UiPlacementMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../common/Callbacks.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"

UiPlacementMode::UiPlacementMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    const Paths& paths,
    ModelManager& mdl_manager)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kPlacementPlacementMode}),
      btn_trees_(data::VboIdMain::kPlacementTrees, [this]() {
        SetPlacementMode(&tex_placement_trees_);
      }),
      btn_bushes_(data::VboIdMain::kPlacementBushes, [this]() {
        SetPlacementMode(&tex_placement_bushes_);
      }),
      btn_tall_grass(data::VboIdMain::kPlacementTallGrass, [this]() {
        SetPlacementMode(&tex_placement_tall_grass_);
      }),
      btn_undergrowth_(data::VboIdMain::kPlacementUndergrowth, [this]() {
        SetPlacementMode(&tex_placement_undergrowth_);
      }),
      btn_asphalt_(data::VboIdMain::kPlacementAsphalt, [this]() {
        SetPlacementMode(&tex_placement_asphalt_);
      }),
      btn_gravel_(data::VboIdMain::kPlacemenGravel, [this]() {
        SetPlacementMode(&tex_placement_gravel_);
      }),
      btn_soil_(data::VboIdMain::kPlacementSoil, [this]() {
        SetPlacementMode(&tex_placement_soil_);
      }),
      btn_change_mode_(data::VboIdMain::kPlacementChangeMode, [this]() {
        TogglePlacement();
      }),
      ui_selection_(ui_shared_resources),
      ui_event_handler_({
          &btn_trees_, &btn_bushes_, &btn_tall_grass,
          &btn_undergrowth_, &btn_asphalt_, &btn_gravel_, &btn_soil_,
          &btn_change_mode_}),
      tex_placement_trees_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_bushes_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_tall_grass_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_undergrowth_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_asphalt_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_gravel_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_placement_soil_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      sp_selected_mode_(data::VboIdMain::kPlacementSelected),
      mdl_manager_(mdl_manager) {}

void UiPlacementMode::SetPlacementMode(Texture* tex_placement) {
  if (IsPreviewMode()) {
    TogglePlacement();
  }
  SetDrawTexture(tex_placement);
}

void UiPlacementMode::SetDrawTexture(Texture* tex_placement) {
  tex_cur_placement_ = tex_placement;
  ui_selection_.SetMask(*tex_cur_placement_);
}

void UiPlacementMode::TogglePlacement() {
  auto tile_renderer =
      ui_shared_resources_.global_glfw_callback_data_.tile_renderer;
  if (IsPreviewMode()) {
    tile_renderer->show_terrain_ = false;
  } else {
    if (tex_cur_placement_->GetId() == tex_placement_trees_.GetId()) {
      auto positions = tile_renderer->placement.UpdatePipeline(
        *tex_cur_placement_, 0);
      mdl_manager_.tree_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() == tex_placement_bushes_.GetId()) {
      auto positions = tile_renderer->placement.UpdatePipeline(
        *tex_cur_placement_, 1);
      mdl_manager_.bush_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() == tex_placement_tall_grass_.GetId()) {
      auto positions = tile_renderer->placement.UpdatePipeline(
        *tex_cur_placement_, 2);
      mdl_manager_.tall_grass_.SetPlacement(ui_shared_resources_, positions);
    } else if (tex_cur_placement_->GetId() == tex_placement_undergrowth_.GetId()) {
      auto positions = tile_renderer->placement.UpdatePipeline(
        *tex_cur_placement_, 3);
      mdl_manager_.undergrowth_.SetPlacement(ui_shared_resources_, positions);
    }
    tile_renderer->show_terrain_ = true;
  }
  preview_mode_ = !preview_mode_;
}

void UiPlacementMode::Setup() {
  BindDefaultCallbacks();
  SetPlacementMode(&tex_placement_trees_);
  ui_selection_.SetIdBounds(details::kIdOffsetWater, details::kIdOffsetFences);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->SetPosition(glm::vec3{5.0f});
  camera->SetPitch(45.0f);
  camera->SetYaw(0.0f);
  camera->SetOrigin(glm::vec3{0.0f});
  camera->MoveRotateViewOrigin(0.0f, 0.0f); // to update camera vectors
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
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->Render();
}

void UiPlacementMode::RenderPickingWorld() {
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->RenderPicking();
}

void UiPlacementMode::Render() {
  if (!IsPreviewMode()) {
    ui_selection_.Render();
    ui_selection_.RenderOnSurface(
        &ui_shared_resources_.global_glfw_callback_data_.tile_renderer
             ->cur_tile_.map_terrain_height);
  }

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();

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

  ui_shared_resources_.global_glfw_callback_data_.windows->Render();

  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
  camera->Update(1.0f); // const pos
}

void UiPlacementMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  sp_mode_.RenderPicking();
  btn_trees_.RenderPicking();
  btn_bushes_.RenderPicking();
  btn_tall_grass.RenderPicking();
  btn_undergrowth_.RenderPicking();
  btn_asphalt_.RenderPicking();
  btn_gravel_.RenderPicking();
  btn_soil_.RenderPicking();
  btn_change_mode_.RenderPicking();
  sp_selected_mode_.RenderPicking();

  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
}

namespace placement {

void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
  if (placement->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    global_data->windows->Scroll(pressed_id, yoffset);
    return;
  }
  global_data->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
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
                        placement->ui_event_handler_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      placement->ui_selection_.Start(global_data->cursor_pos_tex_norm_,
                                     mod_ctrl, mod_shift);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (mod_shift) {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_Mmb);
      } else {
        glfwSetCursorPosCallback(gWindow, callbacks::CursorPosCallback_MmbShift);
      }
      glfwSetMouseButtonCallback(gWindow, callbacks::MouseButtonCallback_Mmb_MmbShift);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    global_data->windows->Release();
    placement->ui_event_handler_.Release();
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
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
    placement->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    placement->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    placement->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    placement->ui_selection_.SetMode(SelectionMode::kTweak);
  }
}

void MouseButtonCallback_Lmb(
    GLFWwindow* window, int button, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    placement->BindDefaultCallbacks();
    placement->ui_selection_.Stop(cursor_pos);
    const auto& selection_mask = placement->ui_selection_.GetMask();
    glCopyImageSubData(
        selection_mask.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
        placement->tex_cur_placement_->GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
        details::gTerrainSize, details::gTerrainSize, 1);
  }
}

void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
  placement->ui_selection_.Update(global_data->cursor_pos_tex_norm_);
}

} // namespace placement
