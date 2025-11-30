#include "UiPlayerMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Camera.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"

UiPlayerMode::UiPlayerMode(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    Tile& cur_tile,
    ModelManager& mdl_manager)
    : IUiMode(
          ui_shared_resources,
          {data::VboIdMain::kMenuPlayer}),
      sp_hp_(data::VboIdMain::kPlayerHealthPoint),
//      sp_map_(data::VboIdMain::kTerrainUpdate2),
      sp_phone_(data::VboIdMain::kPlayerPhoneMap),
      ui_obj_info_(
          {data::VboIdMain::kPlayerGameObjInfoDesk},
          1.0f,
          {{data::VboIdMain::kPlayerGameObjInfoPinBack, [](){}},
           {data::VboIdMain::kPlayerGameObjInfoPinPoint}},
          ui_shared_resources_,
          window_queue,
          {data::VboIdMain::kPlayerGameObjInfoEnemy},
          {data::VboIdMain::kPlayerGameObjInfoFriend},
          {data::VboIdMain::kPlayerGameObjInfoNeutal},
          {data::VboIdMain::kPlayerGameObjInfoObstacle},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoName}},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoCharacteristic},
           data::TextId::kNotYet},
          {text_renderer, {data::VboIdMain::kPlayerGameObjInfoValue},
           data::TextId::kNotYet}),
      ui_selection_(ui_shared_resources),
      mdl_manager_(mdl_manager) {}

void UiPlayerMode::Setup() {
  BindDefaultCallbacks();
  ui_selection_.SetIdBounds(details::kIdOffsetWater, details::kIdOffsetFences);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);
  auto camera = ui_shared_resources_.global_glfw_callback_data_.camera;
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


int UiPlayerMode::GetPrerenderTextIdStart() const noexcept {
  return static_cast<int>(data::TextId::kScaleTerrain); //todo;
}

int UiPlayerMode::GetPrerenderTextIdEnd() const noexcept {
  return static_cast<int>(data::TextId::kStrength) + 1; //todo;
}

void UiPlayerMode::Render() {
  mdl_manager_.Update();
  mdl_manager_.Render();

  ui_selection_.Render();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
//  sprite_mode_.Render();
  sp_hp_.Render();
//  sp_map_.Render();
  sp_phone_.Render();
  ui_shared_resources_.global_glfw_callback_data_.windows->Render();
}

void UiPlayerMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
//  sprite_mode_.RenderPicking();
  sp_hp_.RenderPicking();
//  sp_map_.RenderPicking();
  sp_phone_.RenderPicking();
  ui_shared_resources_.global_glfw_callback_data_.windows->RenderPicking();
  mdl_manager_.RenderPicking();
}

void UiPlayerMode::HandleSelection() {
  const auto& tex_selected = ui_selection_.GetMask();
  std::vector<uint8_t> selected_pixels(
      tex_selected.GetHeight() * tex_selected.GetWidth());
  tex_selected.Bind();
  glGetTexImage(
      GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, selected_pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  int selected_num = 0;

  const ModelData* mdl_single_selected = nullptr;
  bool enemy_selected = false;
  bool friend_selected = false;
  bool neutral_selected = false;
  bool obstacle_selected = false;

  for (auto& m : mdl_manager_.creatures_) {
    auto position = m.GetPosition();
    int x = static_cast<int>(position.x * 16.0f + 512.0f);
    int y = static_cast<int>(position.z * 16.0f + 512.0f);
    if (selected_pixels[y * 1024 + x] > 0) {
      m.Select();
      const auto model = m.GetModelData();
      if (++selected_num == 1) {
        mdl_single_selected = model;
      }
      if (model->category == ModelData::Category::kEnemy) {
        enemy_selected = true;
      } else if (model->category == ModelData::Category::kFriend) {
        friend_selected = true;
      } else if (model->category == ModelData::Category::kNeutral) {
        neutral_selected = true;
      } else { // kObstacle
        obstacle_selected = true;
      }
    } else {
      m.DeSelect();
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
void ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto player = dynamic_cast<UiPlayerMode*>(*global_data->cur_mode);
  if (player->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    global_data->windows->Scroll(pressed_id, yoffset); // ui
    return;
  }
  if (yoffset < 0.0f) { // map
    global_data->tile_renderer->cur_tile_.DownScale();
  } else {
    global_data->tile_renderer->cur_tile_.UpScale();
  }
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto player = dynamic_cast<UiPlayerMode*>(*global_data->cur_mode);
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
      if (global_data->windows->Press(pressed_id)) {
        return; // ui handled
      }
      glfwSetCursorPosCallback(gWindow, player::CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, player::MouseButtonCallback_Selection);
      player->ui_selection_.Start(global_data->cursor_pos_tex_norm_, mod_ctrl,
                                  mod_shift);
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Mmb);
    }
  } else { // GLFW_RELEASE
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      global_data->windows->Release();
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      glfwSetCursorPosCallback(gWindow, nullptr); /// restore
    }
  }
}

/// smt already pressed
void MouseButtonCallback_Selection(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto player = dynamic_cast<UiPlayerMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    player->BindDefaultCallbacks();
    player->ui_selection_.Stop(cursor_pos);
    player->HandleSelection();
  } else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
    player->ui_selection_.NextMode();
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto player = dynamic_cast<UiPlayerMode*>(*global_data->cur_mode);
  global_data->ui_renderer->Press(key, action);

  bool mod_ctrl = (mods & GLFW_MOD_CONTROL);
  bool mod_shift = (mods & GLFW_MOD_SHIFT);
  player->mdl_manager_.player_.UpdateMods(mod_ctrl, mod_shift);
  if (action == GLFW_PRESS) {
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
    } else if (key == GLFW_KEY_W) {
      player->mdl_manager_.player_.SetMoveForward();
    } else if (key == GLFW_KEY_A) {
      player->mdl_manager_.player_.SetMoveLeft();
    } else if (key == GLFW_KEY_S) {
      player->mdl_manager_.player_.SetMoveBackward();
    } else if (key == GLFW_KEY_D) {
      player->mdl_manager_.player_.SetMoveRight();
    } else if (key == GLFW_KEY_SPACE) {
      player->mdl_manager_.player_.Jump(5.0f);
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      player->mdl_manager_.player_.ResetMoveForward();
    } else if (key == GLFW_KEY_A) {
      player->mdl_manager_.player_.ResetMoveLeft();
    } else if (key == GLFW_KEY_S) {
      player->mdl_manager_.player_.ResetMoveBackward();
    } else if (key == GLFW_KEY_D) {
      player->mdl_manager_.player_.ResetMoveRight();
    }
  }
}

/// select
void CursorPosCallback_Lmb(
    GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(global_data_void_ptr);
  auto player = dynamic_cast<UiPlayerMode*>(*global_data->cur_mode);
  player->ui_selection_.Update(global_data->cursor_pos_tex_norm_);
}

/// move the camera (not height, only pos around the Z axis)
void CursorPosCallback_Mmb(
    GLFWwindow* window, double xpos, double ypos) {
  float xoffset = (xpos - lastX) / 0.05f;
  float yoffset = (lastY - ypos) / 0.05f;

  lastX = xpos;
  lastY = ypos;

  void* global_data = glfwGetWindowUserPointer(window);
  Camera* camera = reinterpret_cast<GlobalGlfwCallbackData*>(global_data)->camera;
  camera->MoveRotateViewOriginDist(xoffset); // const pitch
}

} // namespace player
