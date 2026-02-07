#include "UiWaterMode.h"

#include "../common/Callbacks.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "../renderers/UiRenderer.h"

UiWaterMode::UiWaterMode(UiRenderData& render_data,
                         UiSlots& ui_slots, WindowQueue& window_queue,
                         TextRenderer& text_renderer,
                         UiEditSlots& ui_edit_slots,
                         UiEditConfigSlCfg& value_config_ocean,
                         UiEditConfigSlTxt& value_config_river,
                         UiConfigWindow& ui_config_window,
                         ModelManager& mdl_manager)
    : sp_mode_(data::VboIdMain::kWaterWaterMode),
      btn_bake_ocean_(data::VboIdMain::kWaterOcean,
                      [this]() {
                        sp_selected_mode_.SetSelected(0);
                        bool is_ocean = true;
                        ui_slots_.Setup(&ui_edit_ocean_, [this, is_ocean] {
                          this->OnSelectedSlotChanged(is_ocean);
                        });
                      }),
      btn_bake_river_(data::VboIdMain::kWaterRiver,
                      [this] {
                        sp_selected_mode_.SetSelected(1);
                        bool is_ocean = false;
                        ui_slots_.Setup(&ui_edit_river_, [this, is_ocean] {
                          this->OnSelectedSlotChanged(is_ocean);
                        });
                      }),
      map_points_(mdl_manager),
      btn_update_(data::VboIdMain::kWaterUpdate, [this] { UpdateRivers(); }),
      sp_selected_mode_({data::VboIdMain::kWaterSelected}, &btn_bake_ocean_),
      ui_slots_(ui_slots),
      ui_edit_ocean_(render_data, text_renderer, ui_edit_slots,
                     value_config_ocean, ui_config_window),
      ui_edit_river_(render_data, ui_edit_slots, value_config_river),
      ui_selection_(render_data),
      mouse_transform_(render_data),
      ui_event_handler_({&btn_update_, &btn_bake_ocean_, &btn_bake_river_}) {}

void UiWaterMode::OnSelectedSlotChanged(bool is_ocean) {
  std::vector<MapPoint>* points = nullptr;
  std::vector<glm::uvec2>* joints = nullptr;
  if (ui_slots_.GetSelectedSlotId() != -1) {
    if (is_ocean) {
      points = &ui_edit_ocean_.GetInstanceData().map_points;
    } else {
      points = &ui_edit_river_.GetInstanceData().map_points;
      joints = &ui_edit_river_.GetInstanceData().map_joints;
    }
    ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  } else {
    ui_selection_.SetIdBounds(details::kIdOffsetTerrain,
                              details::kIdOffsetWater);
  }
  map_points_.SetData(points, joints);
}

void UiWaterMode::Setup() {
  ui_selection_.SetIdBounds(details::kIdOffsetObjects, details::kIdOffsetUi);
  ui_selection_.SetModeForce(SelectionMode::kRectangle);  // before Press()
  btn_bake_ocean_.Press();
  BindDefaultCallbacks();
}

void UiWaterMode::PrerenderText(TextRenderer* text_renderer) {
  text_renderer->PrerenderModeText(
      static_cast<int>(data::TextId::kScaleWater),
      static_cast<int>(data::TextId::kWaterRadiusFlat) + 1);
}

void UiWaterMode::BindDefaultCallbacks() {
  glfwSetScrollCallback(gWindow, water::ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, water::MouseButtonCallback);
  glfwSetKeyCallback(gWindow, water::KeyCallback);
  glfwSetCursorPosCallback(gWindow, nullptr);
}

void UiWaterMode::Render(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->Render();
  ui_selection_.Render();
  if (ui_slots_.GetSelectedSlotId() != -1) {
    auto color = ui_slots_.GetInstanceBaseData()->color;
    map_points_.RenderPoints(color);
    auto map_scale =
        render_data.glfw_context_.tile_renderer->cur_tile_.map_scale;
    map_points_.RenderJoints(render_data.glfw_context_.tile_renderer
      ->cur_tile_.map_terrain_height, map_scale, color);
  } else {
    ui_selection_.RenderOnSurface(
        &render_data.glfw_context_.tile_renderer->cur_tile_
             .map_ocean_surface_);
  }

  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_.Bind();

  sp_mode_.Render();
  btn_bake_ocean_.Render();
  btn_bake_river_.Render();
  btn_update_.Render();
  sp_selected_mode_.Render();

  ui_slots_.Render();

}

void UiWaterMode::RenderPicking(
    TileRenderer* tile_renderer, UiRenderer* ui_renderer) {
  const auto& render_data = ui_renderer->GetRenderData();
  tile_renderer->RenderPicking();
  map_points_.RenderPickingPoints();

  render_data.tex_ui_.BindSampler(0);
  glBindVertexArray(render_data.vao_ui_);
  render_data.shader_sp_picking_.Bind();

  sp_mode_.RenderPicking();
  btn_bake_ocean_.RenderPicking();
  btn_bake_river_.RenderPicking();
  btn_update_.RenderPicking();

  ui_slots_.RenderPicking();
  render_data.glfw_context_.ui_renderer->ui_layer_wireframe_
    .RenderPickingLayerWireframe();
}

void UiWaterMode::HandleSelection(const std::set<GLuint>& selected_ids) {
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

void UiWaterMode::CancelTransform() {
  BindDefaultCallbacks();
  auto map_points = map_points_.GetPoints();
  mouse_transform_.TranslateSelectedMapPointsBack(*map_points);
  map_points_.UpdateJointsBuffer();
}

void UiWaterMode::ApplyTransform() { BindDefaultCallbacks(); }

constexpr int W = 1024;
constexpr int H = 1024;
constexpr float chaos = 0.5f;  // 0 = clean, 1 = wild

int idx(int x, int y) { return y * W + x; }

float Noise(int x, int y) {
  uint32_t h = x * 73856093u ^ y * 19349663u;
  h ^= h >> 13;
  return (h & 1023) / 1023.0f;
}

int FindLowestNeighbor(const std::vector<float>& h, int x, int y) {
  float h0 = h[idx(x, y)];
  int best = -1;
  float bestH = h0;

  for (int dy = -1; dy <= 1; ++dy)
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) continue;
      int nx = x + dx, ny = y + dy;
      if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;

      // float nh = h[idx(nx,ny)]
      // + chaos * Noise(nx,ny);
      float nh = h[idx(nx, ny)];

      if (nh < bestH) {
        bestH = nh;
        best = idx(nx, ny);
      }
    }
  return best;  // -1 = sink
}

void PourRiver(const std::vector<float>& terrain, std::vector<float>& flow,
               int sx, int sy) {
  int x = std::clamp(sx, 1, 1022);
  int y = std::clamp(sy, 1, 1022);

  float add_height = 1000.0f;

  for (int iter = 0; iter < 4096; ++iter) {
    x = std::clamp(x, 1, 1022);
    y = std::clamp(y, 1, 1022);
    int i = idx(x, y);
    flow[i] += add_height;

    float floodHeight = std::lerp(0.0f, 1000.0f, chaos);
    float floodSpread = std::lerp(0.0f, 300.0f, chaos);

    for (int j = -1; j <= 1; ++j) {
      for (int k = -1; k <= 1; ++k) {
        if (k == 0 && j == 0) {
          continue;
        }
        if (flow[i] + floodHeight >= flow[idx(x + k, y + j)]) {
          flow[idx(x + k, y + j)] += floodSpread;
        }
      }
    }

    int next = FindLowestNeighbor(terrain, x, y);
    if (next < 0) break;

    x = next % W;
    y = next / W;
  }
}
std::vector<uint8_t> BuildRiverMask(const std::vector<float>& flow,
                                    float threshold) {
  std::vector<uint8_t> mask(W * H, 0);
  int counter = 0;
  float min = 100000;
  float max = 0;
  for (int i = 0; i < W * H; ++i) {
    min = std::min(min, flow[i]);
    max = std::max(max, flow[i]);
    if (flow[i] >= threshold) {
      mask[i] = 255;
      ++counter;
    }
  }
  std::cout << "processed non zero " << counter << std::endl;
  std::cout << "max " << max << std::endl;
  std::cout << "min " << min << std::endl;
  return mask;
}

void DilateMask(std::vector<uint8_t>& mask, int radius) {
  std::vector<uint8_t> copy = mask;
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x) {
      if (!copy[idx(x, y)]) continue;

      for (int dy = -radius; dy <= radius; ++dy)
        for (int dx = -radius; dx <= radius; ++dx) {
          int nx = x + dx, ny = y + dy;
          if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
          mask[idx(nx, ny)] = 255;
        }
    }
}

void UiWaterMode::UpdateRivers() {
  std::cerr << "UpdateRivers() isn't implemented" << std::endl;
  // if (ui_slots_.GetSelectedSlotId() == -1 ||
  //     ui_edit_river_.GetInstanceData().map_points.size() == 0) {
  //   std::cerr << "Unable rocessed river flood" << std::endl;
  //   return;
  // }
  //
  // Texture* mask = &render_data_.glfw_context_.tile_renderer->cur_tile_
  //                      .tex_rivers_mask_;
  // glClearTexImage(mask->GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
  // std::vector<float> flow(1024 * 1024, 0.0f);
  // for (auto point : ui_edit_river_.GetInstanceData().map_points) {
  //   auto sx = static_cast<int>(point.position.x * 16.0f + 512.0f);
  //   auto sy = static_cast<int>(point.position.y * 16.0f + 512.0f);
  //   for (int i = 0; i < 40; ++i) {
  //     PourRiver(render_data_.glfw_context_.tile_renderer->cur_tile_
  //                   .terrain_heights_,
  //               flow, sx, sy);
  //   }
  // }
  // auto mask_data = BuildRiverMask(flow, 1.0f);
  // // DilateMask(mask_data, 1.0f);
  //
  // glTextureSubImage2D(mask->GetId(), 0, 0, 0, details::gTerrainSize,
  //                     details::gTerrainSize, GL_RED, GL_UNSIGNED_BYTE,
  //                     mask_data.data());
  // std::cout << "Processed river flood" << std::endl;
  // render_data_.glfw_context_.tile_renderer
  //     ->UpdatePipeline();  // excessive?
}

namespace water {

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto glfw_context = GetGlfwContext(window);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  if (water->ui_selection_.Scroll(yoffset)) {
    return;
  }
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  if (pressed_id >= details::kIdOffsetUi &&
      pressed_id != static_cast<GLuint>(-1)) {
    water->ui_slots_.Scroll(pressed_id, yoffset);
    glfw_context->windows->Scroll(pressed_id, yoffset);
    return;
  }
  glfw_context->tile_renderer->cur_tile_.OnScroll(yoffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  glm::dvec2 cursor_pos = glfw_context->cursor_pos_;
  auto pressed_id = glfw_context->picking_fbo->GetIdByMousePos(cursor_pos);
  //  glfw_context->camera->ProcessMouseKey(button, action, mods);

  bool mod_ctrl = mods & GLFW_MOD_CONTROL;
  bool mod_shift = mods & GLFW_MOD_SHIFT;
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      std::cout << "Pressed id: " << pressed_id << std::endl;
      bool ui_handled = glfw_context->windows->Press(pressed_id) ||
                        water->ui_slots_.Press(pressed_id);
      water->ui_event_handler_.Press(pressed_id);
      if (ui_handled) {
        return;
      }
      if (water->anything_selected_ && pressed_id > details::kIdOffsetObjects &&
          pressed_id < details::kIdOffsetUi) {
        water->mouse_transform_.InitTransform();
        glfwSetScrollCallback(gWindow, nullptr);
        glfwSetCursorPosCallback(gWindow, CursorPosCallback_LmbSelected);
        glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_LmbSelected);
        glfwSetKeyCallback(gWindow, KeyCallback_LmbSelected);
        return;
      }
      water->ui_selection_.Start(glfw_context->cursor_pos_tex_norm_, mod_ctrl,
                                 mod_shift);
      glfwSetCursorPosCallback(gWindow, CursorPosCallback_Lmb);
      glfwSetMouseButtonCallback(gWindow, MouseButtonCallback_Lmb);
      glfwSetKeyCallback(gWindow, callbacks::KeyCallback_Blocked);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (pressed_id > details::kIdOffsetObjects && water->anything_selected_ &&
          pressed_id < details::kIdOffsetUi) {
        water->map_points_.AddJoints(pressed_id);
      } else if (pressed_id < details::kIdOffsetWater) {
        water->map_points_.AddPoint(pressed_id);
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
      water->ui_slots_.Release();
      glfw_context->windows->Release();
    }
    // if GLFW_RELEASE... if not process... we haven't done anything...
  }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
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
    water->ui_selection_.SetMode(SelectionMode::kRectangle);
  } else if (key == GLFW_KEY_2) {
    water->ui_selection_.SetMode(SelectionMode::kCircle);
  } else if (key == GLFW_KEY_3) {
    water->ui_selection_.SetMode(SelectionMode::kLasso);
  } else if (key == GLFW_KEY_4) {
    water->ui_selection_.SetMode(SelectionMode::kTweak);
  } else if (key == GLFW_KEY_G) {
    glfwSetScrollCallback(gWindow, nullptr);
    glfwSetMouseButtonCallback(gWindow, MouseButtonCallbackTransform);
    glfwSetKeyCallback(gWindow, KeyCallbackTransform);
    void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
    auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
    auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
    water->mouse_transform_.Reset();
    glfwSetCursorPosCallback(gWindow, CursorPosCallback_G);
  }
}

void MouseButtonCallback_Lmb(GLFWwindow* window, int button, int action,
                             int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  if (action == GLFW_PRESS) {
    water->BindDefaultCallbacks();
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    water->BindDefaultCallbacks();
    if (water->ui_slots_.GetSelectedSlotId() != -1) {
      auto selected_ids =
          water->ui_selection_.StopIntoSet(glfw_context->cursor_pos_tex_norm_);
      water->HandleSelection(selected_ids);
    } else {
      water->ui_selection_.Stop(glfw_context->cursor_pos_tex_norm_);
    }
  }
}

void CursorPosCallback_Lmb(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(window);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  water->ui_selection_.Update(glfw_context->cursor_pos_tex_norm_);
}

void CursorPosCallback_LmbSelected(GLFWwindow* window, double xpos,
                                   double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  auto map_points = water->map_points_.GetPoints();
  water->mouse_transform_.TranslateSelectedMapPoints(*map_points, xpos, ypos);
  water->map_points_.UpdateJointsBuffer();
}

void MouseButtonCallback_LmbSelected(GLFWwindow* window, int button, int action,
                                     int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    water->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    water->CancelTransform();
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    // TODO: NextAxis();
  }
}

void KeyCallback_LmbSelected(GLFWwindow* window, int key, int scancode,
                             int action, int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      return water->CancelTransform();
    case GLFW_KEY_ENTER:
      return water->ApplyTransform();
  }
  water->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
}

void CursorPosCallback_G(GLFWwindow* window, double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  water->mouse_transform_.TranslateSelectedVerticesUp(
      xpos, ypos, glfw_context->tile_renderer->cur_tile_.map_ocean_surface_,
      water->ui_selection_.GetMask());
}

void MouseButtonCallbackTransform(GLFWwindow* window, int button, int action,
                                  int mods) {
  auto glfw_context = GetGlfwContext(window);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  if (action != GLFW_PRESS) {
    return;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    water->ApplyTransform();
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    water->CancelTransform();
  }
}

void KeyCallbackTransform(GLFWwindow* window, int key, int scancode, int action,
                          int mods) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);
  auto water = dynamic_cast<UiWaterMode*>(*glfw_context->cur_mode);
  glfw_context->ui_renderer->Press(key, action);
  if (action != GLFW_PRESS) {
    return;
  }
  water->mouse_transform_.SetAxis(key, mods & GLFW_MOD_SHIFT);
  if (key == GLFW_KEY_ESCAPE) {
    return water->CancelTransform();
  } else if (key == GLFW_KEY_ENTER) {
    return water->ApplyTransform();
  }
}

}  // namespace water
