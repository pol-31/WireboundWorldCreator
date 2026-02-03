#include "UiPlayerMode.h"

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiPlayerMode::UiPlayerMode(UiSharedResources& ui_shared_resources,
                           WindowQueue& window_queue,
                           TextRenderer& text_renderer, Tile& cur_tile,
                           ModelManager& mdl_manager)
    : IUiMode(ui_shared_resources, {data::VboIdMain::kPlayerPlayerMode}),
      sp_hp_(data::VboIdMain::kPlayerHealthPoint),
      ui_map_(ui_shared_resources, window_queue),
      ui_obj_info_(
          {data::VboIdMain::kPlayerGameObjInfoDesk}, 1.0f,
          {{data::VboIdMain::kPlayerGameObjInfoPinBack, []() {}},
           {data::VboIdMain::kPlayerGameObjInfoPinPoint}},
          ui_shared_resources_, window_queue,
          {data::VboIdMain::kPlayerGameObjInfoEnemy},
          {data::VboIdMain::kPlayerGameObjInfoFriend},
          {data::VboIdMain::kPlayerGameObjInfoNeutal},
          {data::VboIdMain::kPlayerGameObjInfoObstacle},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoName}},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoCharacteristic}},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoValue}}),
      ui_selection_(ui_shared_resources),
      mdl_manager_(mdl_manager) {}

void UiPlayerMode::Setup() {
  ui_shared_resources_.glfw_context_.text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kHp),
      static_cast<int>(data::TextId::kAttackSpeed) + 1);
  BindDefaultCallbacks();
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
  auto camera = ui_shared_resources_.glfw_context_.camera;
  camera->SetPitch(45.0f);
  camera->SetOriginDist(10.0f);
}

void UiPlayerMode::BindDefaultCallbacks() {
  double xpos, ypos;
  glfwGetCursorPos(gWindow, &xpos, &ypos);
  lastX = xpos;
  lastY = ypos;
  glfwSetScrollCallback(gWindow, player::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, player::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, player::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

void UiPlayerMode::Render() {
  auto camera = ui_shared_resources_.glfw_context_.camera;
  auto map_scale =
      ui_shared_resources_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  glm::vec3 camera_pos;
  if (mdl_manager_.player_.IsFpv()) {
    camera_pos = mdl_manager_.player_.GetFpv().GetPosition();
  } else {
    camera_pos = mdl_manager_.player_.GetPosition();
  }
  camera->SetOrigin(camera_pos * map_scale);
  camera->MoveRotateViewOriginDist(0.0f);  // update camera vectors after origin
  camera->Update();

  ui_shared_resources_.glfw_context_.tile_renderer->RenderInGame(camera);
  mdl_manager_.Update();
  mdl_manager_.Render();

  ui_selection_.Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_.Bind();
  sp_mode_.Render();
  sp_hp_.Render();
  ui_map_.Render(&mdl_manager_);
  ui_shared_resources_.glfw_context_.windows->Render();
}

void UiPlayerMode::RenderPicking() {
  ui_shared_resources_.glfw_context_.tile_renderer->RenderPicking();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_mode_.RenderPicking();
  sp_hp_.RenderPicking();
  ui_map_.RenderPicking();
  ui_shared_resources_.glfw_context_.windows->RenderPicking();
  mdl_manager_.RenderPicking();
}

void UiPlayerMode::HandleSelection(const std::set<GLuint>& selected_ids) {
  int selected_num = 0;
  const ModelData* mdl_single_selected = nullptr;
  bool enemy_selected = false;
  bool friend_selected = false;
  bool neutral_selected = false;
  bool obstacle_selected = false;
  auto it = selected_ids.find(
    mdl_manager_.player_.GetId() || mdl_manager_.player_.GetFpv().GetId());
  if (it != selected_ids.end()) {
    std::cout << "player selected" << std::endl;
  }
  for (int i = 0; i < mdl_manager_.creatures_.size(); ++i) {
    auto it = selected_ids.find(mdl_manager_.creatures_[i].GetId());
    if (it != selected_ids.end()) {
      mdl_manager_.creatures_[i].Select();
      const auto model = mdl_manager_.creatures_[i].GetModelData();
      if (++selected_num == 1) {
        mdl_single_selected = model;
      }
      /*if (model->category == ModelData::Category::kEnemy) {
        enemy_selected = true;
      } else if (model->category == ModelData::Category::kFriend) {
        friend_selected = true;
      } else if (model->category == ModelData::Category::kNeutral) {
        neutral_selected = true;
      } else {  // kObstacle
        obstacle_selected = true;
      }*/
    } else {
      mdl_manager_.creatures_[i].DeSelect();
    }
  }
  for (int i = 0; i < mdl_manager_.fpvs_.size(); ++i) {
    auto it = selected_ids.find(mdl_manager_.fpvs_[i].GetId());
    if (it != selected_ids.end()) {
      mdl_manager_.fpvs_[i].Select();
      const auto model = mdl_manager_.fpvs_[i].GetModelData();
      if (++selected_num == 1) {
        mdl_single_selected = model;
      }
    } else {
      mdl_manager_.fpvs_[i].DeSelect();
    }
  }
  if (selected_num == 1) {
    ui_obj_info_.Show(enemy_selected, friend_selected, neutral_selected,
                      obstacle_selected, mdl_single_selected);
  } else if (selected_num != 0) {
    ui_obj_info_.Show(enemy_selected, friend_selected, neutral_selected,
                      obstacle_selected, selected_num);
  }
}

namespace player {

/// scroll -> scale map
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto player = dynamic_cast<UiPlayerMode*>(*glfw_context->cur_mode);
  if (player->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    glfw_context->windows->Scroll(pressed_id, yoffset);  // ui
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto player = dynamic_cast<UiPlayerMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    lastX = xpos;
    lastY = ypos;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
      std::cout << "Pressed id: " << pressed_id << std::endl;
      if (glfw_context->windows->Press(pressed_id)) {
        return;  // ui handled
      }
      glfwSetCursorPosCallback(gWindow, player::CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow,
                                 player::MouseButtonCallback_Selection);
      player->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_, mod_ctrl,
                                  mod_shift);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Mmb);
    }
  } else {  // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      glfw_context->windows->Release();
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      glfwSetCursorPosCallback(gWindow, nullptr);  /// restore
    }
  }
}

/// smt already pressed
void MouseButtonCallback_Selection(GLFWwindow* window, int button, int action,
                                   int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto player = dynamic_cast<UiPlayerMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    player->BindDefaultCallbacks();
    auto selected_ids =
        player->ui_selection_.StopIntoSet(glfw_context->cursor_pos_tex_norm_);
    player->HandleSelection(selected_ids);
  } else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
    player->ui_selection_.NextMode();
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto player = dynamic_cast<UiPlayerMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  if (action != GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      if (mod_shift) {
        glfwSetWindowShouldClose(window, true);
        return;
      }
      if (glfw_context->windows->GetSize() == 0) {
        glfw_context->ui_confirmation->Show(
            data::TextId::kConfirmationExit,
            [] { glfwSetWindowShouldClose(gWindow, true); });
        return;
      }
    }
  }
  auto& player_ = player->mdl_manager_.player_;
  player_.ProcessMovement(key, action);
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_1) player_.SwitchToHuman();
    else if (key == GLFW_KEY_2) player_.SwitchToFpv();
  }
}

/// select
void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto player = dynamic_cast<UiPlayerMode*>(*glfw_context->cur_mode);
  player->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

/// move the camera (not height, only pos around the Z axis)
void CursorPosCallback_Mmb(GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  Camera* camera = GetGlfwContext(window)->camera;
  camera->MoveRotateViewOriginDist(xoffset);  // const pitch
}

}  // namespace player
