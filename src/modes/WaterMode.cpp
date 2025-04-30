#include "WaterMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"

#include "../common/ArbitraryGraph.h"

void WaterModeScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer_.DownScale();
  } else {
    global_data->tile_renderer_.UpScale();
  }
}

void WaterModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto water = dynamic_cast<WaterMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  //TODO: refactor to "if(!cond) return"
  //TODO: change order (based on usage frequency)
  global_data->camera_.ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id < details::kIdOffsetUi && water->do_add_points_) {
          //TODO; make it more explicit
          /// because water offset id goes after terrain ids
          if (pressed_id < details::kIdOffsetWater) {
            water->AddNewPoint(pressed_id);
          }
        } else if (pressed_id == water->btn_bake_lake_.GetId()) {
          water->BakeLake();
        } else if (pressed_id == water->btn_bake_river_.GetId()) {
          water->BakeRiver();
        } else if (pressed_id == water->btn_bake_waterfall_.GetId()) {
          water->BakeWaterfall();
        } else if (pressed_id == water->btn_create_.GetId()) {
          water->Create(pressed_id);
        } else if (pressed_id == water->btn_remove_.GetId()) {
          water->Remove();
        } else if (pressed_id == water->btn_update_.GetId()) {
          water->UpdateOcean(); // TODO: update ocean (only ocean by now
        }
        // have no effect if pressed_id doesn't match
        water->ocean_layer_config_1_.Press(pressed_id);
        water->ocean_layer_config_2_.Press(pressed_id);
        water->ocean_layer_config_3_.Press(pressed_id);
      }
    }
  } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
    //Release
    water->ocean_layer_config_1_.Release();
    water->ocean_layer_config_2_.Release();
    water->ocean_layer_config_3_.Release();
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void WaterModeKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  /*  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
        glfwGetWindowUserPointer(window));
    auto terrain = dynamic_cast<TerrainMode*>(global_data->cur_mode_);
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
      } else if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera_.SpeedUp();
      } else if (key == GLFW_KEY_BACKSPACE) {
        global_data->terrain_.ClearPoints();
      } else if (key == GLFW_KEY_ENTER) {
        global_data->terrain_.Bake();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = true;
      }
    } else if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_LEFT_SHIFT) {
        global_data->camera_.SlowDown();
      } else if (key == GLFW_KEY_TAB) {
        global_data->tab_pressed_ = false;
      }
    }*/
}

WaterMode::WaterMode(
    SharedResources& shared_resources,
    const Paths& paths, const TextRenderer& text_renderer)
    : IEditMode(shared_resources),
      points_shader_(paths.shader_points_polygon_vert,
                     paths.shader_points_polygon_frag),
      btn_bake_lake_(vbos::VboIdMain::kWaterLake, vbos::VboIdText::kBakeAsALake),
      btn_bake_river_(vbos::VboIdMain::kWaterRiver, vbos::VboIdText::kBakeAsARiver),
      btn_bake_waterfall_(vbos::VboIdMain::kWaterWaterfall, vbos::VboIdText::kBakeAsAWaterfall),
      btn_create_(vbos::VboIdMain::kAddNew, vbos::VboIdText::kAddNew),
      btn_remove_(vbos::VboIdMain::kRemove, vbos::VboIdText::kRemoveSelected),
      btn_update_(vbos::VboIdMain::kWaterUpdate, vbos::VboIdText::kNone),
      slots_(water_data_.GetSizeRef(),
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNextSlot},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kPreviousSlot},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNone},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNone},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNone},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNone},
             {vbos::VboIdMain::kWireboundLogo, vbos::VboIdText::kNone},
             cur_points_data_idx_, text_renderer),
      ocean_layer_config_1_(
          {vbos::VboIdMain::kWater1SliderScaleFill, vbos::VboIdText::kPreviousRow},
          {vbos::VboIdMain::kWater1SliderScaleWheel, vbos::VboIdText::kNone},
          1000.0f,
          {vbos::VboIdMain::kWater1SliderFetchFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderFetchWheel, vbos::VboIdText::kNone},
          100000.0f,
          {vbos::VboIdMain::kWater1SliderSpreadBlendFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderSpreadBlendWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater1SliderSwellFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderSwellWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater1SliderPeakEnhancementFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderPeakEnhancementWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater1SliderShortWavesFadeFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderShortWavesFadeWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater1SliderLambdaFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater1SliderLambdaWheel, vbos::VboIdText::kNone},
          1.0f),
      ocean_layer_config_2_(
          {vbos::VboIdMain::kWater2SliderScaleFill, vbos::VboIdText::kPreviousPage},
          {vbos::VboIdMain::kWater2SliderScaleWheel, vbos::VboIdText::kNone},
          1000.0f,
          {vbos::VboIdMain::kWater2SliderFetchFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderFetchWheel, vbos::VboIdText::kNone},
          100000.0f,
          {vbos::VboIdMain::kWater2SliderSpreadBlendFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderSpreadBlendWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater2SliderSwellFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderSwellWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater2SliderPeakEnhancementFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderPeakEnhancementWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater2SliderShortWavesFadeFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderShortWavesFadeWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater2SliderLambdaFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater2SliderLambdaWheel, vbos::VboIdText::kNone},
          1.0f),
      ocean_layer_config_3_(
          {vbos::VboIdMain::kWater3SliderScaleFill, vbos::VboIdText::kNextPage},
          {vbos::VboIdMain::kWater3SliderScaleWheel, vbos::VboIdText::kNone},
          1000.0f,
          {vbos::VboIdMain::kWater3SliderFetchFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderFetchWheel, vbos::VboIdText::kNone},
          100000.0f,
          {vbos::VboIdMain::kWater3SliderSpreadBlendFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderSpreadBlendWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater3SliderSwellFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderSwellWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater3SliderPeakEnhancementFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderPeakEnhancementWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater3SliderShortWavesFadeFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderShortWavesFadeWheel, vbos::VboIdText::kNone},
          1.0f,
          {vbos::VboIdMain::kWater3SliderLambdaFill, vbos::VboIdText::kNone},
          {vbos::VboIdMain::kWater3SliderLambdaWheel, vbos::VboIdText::kNone},
          1.0f) {
  Init();
}

void WaterMode::Init() {
  //TODO: idk - looks like this is picking...
  glGenVertexArrays(1, &points_vao_);
  glBindVertexArray(points_vao_);
  glGenBuffers(1, &points_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo_);
  glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(GLuint), // TODO: 64 is max
               nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);

  /// DUCK!
  //    glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(GLuint),
  //                          reinterpret_cast<void*>(0));
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint),
                         reinterpret_cast<void*>(0));
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  points_shader_.Bind();
  glUniform1i(shader::kGraphHeightMap, 0);
}

void WaterMode::Render() {
  shared_resources_.tile_renderer_.Render();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_bake_lake_.Render();
  btn_bake_river_.Render();
  btn_bake_waterfall_.Render();
  btn_create_.Render();
  btn_remove_.Render();
  btn_update_.Render();
  slots_.Render();

  if (cur_points_data_idx_ != -1) {
    RenderPoints();
  }

  ocean_layer_config_1_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);
  ocean_layer_config_2_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);
  ocean_layer_config_3_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);

//  if (need_to_update_uniforms_) {
//    shader_draw_.Bind();
//    glUniform1f(shader::kPlacementColor, slider_color_.GetProgress());
//    // TODO: koef to Details.h
//    auto radius = static_cast<unsigned int>(slider_size_.GetProgress() * 100.0f);
//    glUniform1ui(shader::kPlacementRadius, radius);
//    glUniform1f(shader::kPlacementFalloff, slider_falloff_.GetProgress());
//    glUseProgram(0);
    //    need_to_update_uniforms_ = false;
//  }
}

void WaterMode::UpdateOcean() {
  if (!ocean_layer_config_1_.Modified() &&
      !ocean_layer_config_2_.Modified() &&
      !ocean_layer_config_3_.Modified()) {
    return;
  }
  OceanTraits traits = {
      ocean_layer_config_1_.GetOceanLayerTraits(),
      ocean_layer_config_2_.GetOceanLayerTraits(),
      ocean_layer_config_3_.GetOceanLayerTraits()
  };
  shared_resources_.tile_renderer_.UpdateOcean(traits);
}

void WaterMode::RenderPoints() {
  glBindVertexArray(points_vao_);
  if (points_vbo_modified_) {
    if (cur_points_data_idx_ != -1) {
      glBindBuffer(GL_ARRAY_BUFFER, points_vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0,
                      water_data_[cur_points_data_idx_].points.size() * sizeof(GLuint),
                      water_data_[cur_points_data_idx_].points.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    points_vbo_modified_ = false;
  }
  points_shader_.Bind();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tile_.map_terrain_height.Bind();

  glUniform4fv(shader::kGraphColor, 1, glm::value_ptr(colors::kWhite));
  glPointSize(10.0f);
  glDrawArrays(GL_POINTS, 0, water_data_[cur_points_data_idx_].points.size());
  if (water_data_[cur_points_data_idx_].points.size() < 3) {
    return;
  }

  glUniform4fv(shader::kGraphColor, 1, glm::value_ptr(colors::kBlue));
  glLineWidth(3.0f);
  glDrawArrays(GL_LINE_LOOP, 0, water_data_[cur_points_data_idx_].points.size());
}

void WaterMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_bake_lake_.RenderPicking();
  btn_bake_river_.RenderPicking();
  btn_bake_waterfall_.RenderPicking();
  btn_create_.RenderPicking();
  btn_remove_.RenderPicking();
  btn_update_.RenderPicking();
  slots_.RenderPicking();
  ocean_layer_config_1_.RenderPicking();
  ocean_layer_config_2_.RenderPicking();
  ocean_layer_config_3_.RenderPicking();
}

void WaterMode::Create(GLuint id) {
  std::cout << "created new point set" << std::endl;
  do_add_points_ = true;
  //WaterType::kLake by default - anyway we'll replace it at WaterMode::Bake*()
  water_data_.EmplaceBack(WaterType::kLake, std::vector<GLuint>{});
  cur_points_data_idx_ = water_data_.Size() - 1;
  points_vbo_modified_ = true;
}

void WaterMode::Select(std::size_t idx) {
  if (idx >= water_data_.Size()) {
    std::cerr << "idx" << std::endl;
    return;
  }
  cur_points_data_idx_ = idx;
  points_vbo_modified_ = true;
  //TODO: do_add_points_ to true?
  do_add_points_ = false; //TODO: bear it all out to DeSelect()
}

// cur_points_data_idx_ always valid, because we call it from callback only
// if do_add_points_ is true, which is always true, when
// cur_points_data_idx != -1
void WaterMode::AddNewPoint(std::uint32_t id) {
  if (water_data_[cur_points_data_idx_].points.size() == 64) {
    std::cerr << "points overflow; rewriting last" << std::endl;
    water_data_[cur_points_data_idx_].points.pop_back();
  }
  for (const auto& i : water_data_[cur_points_data_idx_].points) {
    if (i == id) {
      return;
    }
  }
  points_vbo_modified_ = true;
  std::cout << "Point id: " << id << std::endl;
  water_data_[cur_points_data_idx_].points.push_back(id);
}

void WaterMode::Remove() {
  std::cout << "removed selected point set" << std::endl;
  if (do_add_points_) {
    water_data_.PopBack();
    points_vbo_modified_ = true;
    cur_points_data_idx_ = -1;
    do_add_points_ = false; //TODO: bear it all out to DeSelect()
  } else if (cur_points_data_idx_ != -1) {
    std::cout << "do you really want to erase points data #"
              << cur_points_data_idx_ << "?" << std::endl;
    // TODO: yes/no ui msg
    water_data_.Erase(std::next(water_data_.Begin(), cur_points_data_idx_));

    ReBake();
  }
}

void WaterMode::ReBake() {
  /// reset height map to starting (see declaration at Tile.h for explanation)
  shared_resources_.tile_.water_heights_ =
      shared_resources_.tile_.water_heights_init_;
  shared_resources_.tile_.map_water_height.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
               GL_UNSIGNED_BYTE, shared_resources_.tile_.water_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  for (int i = 0; i < water_data_.Size(); ++i) {
    cur_points_data_idx_ = i;
    if (water_data_[i].type == WaterType::kLake) {
      BakeLake();
    } else if (water_data_[i].type == WaterType::kRiver) {
      BakeRiver();
    } else {
      BakeWaterfall();
    }
  }
  points_vbo_modified_ = true;
  cur_points_data_idx_ = -1;
  do_add_points_ = false; //TODO: bear it all out to DeSelect()
}

void WaterMode::BakeLake() {
  std::cout << "bake as a lake" << std::endl;
  do_add_points_ = false; // no editing after baking by now
  auto control_points = GenControlPoints();
  if (control_points.empty()) {
    return;
  }
  InitStableArea(control_points);
  water_data_[cur_points_data_idx_].type = WaterType::kLake;
  FloodFill();
}

void WaterMode::BakeRiver() {
  std::cout << "bake as a river" << std::endl;
  do_add_points_ = false; // no editing after baking by now
  auto control_points = GenControlPoints();
  if (control_points.empty()) {
    return;
  }
  InitStableArea(control_points);
  water_data_[cur_points_data_idx_].type = WaterType::kRiver;
  FloodFill();
}

void WaterMode::BakeWaterfall() {
  std::cout << "bake as a waterfall" << std::endl;
  do_add_points_ = false; // no editing after baking by now
  auto control_points = GenControlPoints();
  if (control_points.empty()) {
    return;
  }
  InitMovingArea(control_points);
  water_data_[cur_points_data_idx_].type = WaterType::kWaterfall;
  FloodFill();
}

void WaterMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, WaterModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, WaterModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);

  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(gWindow));
  global_data->camera_.SetInspectCamera();
}

std::vector<Point> WaterMode::GenControlPoints() {
  /// here we collect info only about current(selected), but
  /// all previous have been already baked by now, so it's OK
  if (cur_points_data_idx_ == -1) {
    std::cerr << "can't bake; there's no selected water object" << std::endl;
    return {};
  }
  if (water_data_[cur_points_data_idx_].points.empty()) {
    std::cerr << "can't bake; need more points" << std::endl;
    return {};
  }
  std::vector<Point> polygon(water_data_[cur_points_data_idx_].points.size());
  for (int i = 0; i < polygon.size(); ++i) {
    // cur_points_data_idx is valid, otherwise we wouldn't be here
    polygon[i].x = water_data_[cur_points_data_idx_].points[i] & 1023;
    polygon[i].y = water_data_[cur_points_data_idx_].points[i] >> 10;
  }
  if (!IsConvexPolygon(polygon)) {
    std::cerr << "The polygon isn't convex. "
                 "Height map can't be baked" << std::endl;
    return {};
  }
  std::vector<Point> control_points;
  for (int x = 0; x < 1024; x++) { // TODO: looks like this is the same part for all baking modes
    for (int y = 0; y < 1024; y++) {
      Point p = {x, y};
      if (isInsideConvexPolygon(polygon, p)) {
        max_height_ = std::max(
            max_height_, static_cast<int>(shared_resources_.tile_
                                              .terrain_heights_[y * 1024 + x]));
        control_points.push_back(p);
      }
    }
  }
  return control_points;
}

void WaterMode::InitStableArea(
    const std::vector<Point>& control_points) {
  for (auto p : control_points) {
    shared_resources_.tile_.water_heights_[p.y * 1024 + p.x] = max_height_;
  }
}

void WaterMode::InitMovingArea(
    const std::vector<Point>& control_points) {
  for (auto p : control_points) {
    shared_resources_.tile_.water_heights_[p.y * 1024 + p.x] =
        std::max(static_cast<int>(shared_resources_.tile_.water_heights_[p.y * 1024 + p.x]),
                 static_cast<int>(shared_resources_.tile_.terrain_heights_[p.y * 1024 + p.x])
                     + user_desired_river_raise_);
  }
}

// TODO: isn't it too slow (performance)?
bool WaterMode::FloodFillStablePass() {
  int total_changed = 0;
  std::uint8_t cur_water_height, near_water_height, near_terrain_height;
  int i, j;
  // except the center (because we are the centre)
  std::array<int, 8> dx = {-1, +0, +1, -1, +1, -1, +0, +1};
  std::array<int, 8> dy = {+1, +1, +1, +0, +0, -1, -1, -1};
  // skip borders, we don't want segfault; anyway they will be initialized
  // by their neighbours using dx and dy
  for (int y = 1; y < 1023; ++y) {
    for (int x = 1; x < 1023; ++x) {
      int cur_idx = y * 1024 + x;
      cur_water_height = shared_resources_.tile_.water_heights_[cur_idx];
      if (cur_water_height == 0) {
        continue;
      }
      for (int k = 0; k < 8; ++k) {
        i = dy[k];
        j = dx[k];
        int near_idx = cur_idx + i * 1024 + j;
        near_water_height = shared_resources_.tile_.water_heights_[near_idx];
        near_terrain_height = shared_resources_.tile_.terrain_heights_[near_idx];
        if (cur_water_height > near_terrain_height &&
            near_water_height < max_height_) {
          ++total_changed;
          shared_resources_.tile_.water_heights_[near_idx] = max_height_;
        }
      }
    }
  }
  std::cout << "total changed: " << total_changed << std::endl;
  return total_changed != 0;
}

bool WaterMode::FloodFillMovingPass() {
  int total_changed = 0;
  std::uint8_t cur_water_height, cur_terrain_height,
      near_water_height, near_terrain_height;
  int i, j;
  // except the center (because we are the centre)
  std::array<int, 8> dx = {-1, +0, +1, -1, +1, -1, +0, +1};
  std::array<int, 8> dy = {+1, +1, +1, +0, +0, -1, -1, -1};
  // skip borders, we don't want segfault; anyway they will be initialized
  // by their neighbours using dx and dy
  for (int y = 1; y < 1023; ++y) {
    for (int x = 1; x < 1023; ++x) {
      int cur_idx = y * 1024 + x;
      cur_water_height = shared_resources_.tile_.water_heights_[cur_idx];
      if (cur_water_height == 0) {
        continue;
      }
      cur_terrain_height = shared_resources_.tile_.terrain_heights_[cur_idx];
      for (int k = 0; k < 8; ++k) {
        i = dy[k];
        j = dx[k];
        int near_idx = cur_idx + i * 1024 + j;
        near_water_height = shared_resources_.tile_.water_heights_[near_idx];
        near_terrain_height = shared_resources_.tile_.terrain_heights_[near_idx];
        if (cur_water_height <= near_terrain_height) {
          continue;
        }
        GLuint new_water_height;
        if (cur_terrain_height < near_terrain_height) {
          new_water_height = cur_water_height;
        } else {
          new_water_height = near_terrain_height +
                             (cur_water_height - cur_terrain_height);
          new_water_height = std::min(int(new_water_height), int(cur_water_height));
        }
        if (near_water_height < new_water_height) {
          ++total_changed;
          shared_resources_.tile_.water_heights_[near_idx] = new_water_height;
        }
      }
    }
  }
  std::cout << "total changed: " << total_changed << std::endl;
  return total_changed != 0;
}

void WaterMode::FloodFill() {
  int iterations_counter = 0;
  //TODO: for some reasons it looks disgusting (std::cout?)
  WaterType water_type = water_data_[cur_points_data_idx_].type;
  if (water_type == WaterType::kLake) {
    std::cout << "___ Flood fill for Lake has started" << std::endl;
    while (FloodFillStablePass()) {
      std::cout << "- iteration# " << ++iterations_counter << std::endl;
    }
  } else if (water_type == WaterType::kRiver) {
    std::cout << "___ Flood fill for River has started" << std::endl;
    while (FloodFillMovingPass()) {
      std::cout << "- iteration# " << ++iterations_counter << std::endl;
    }
  } else {
    std::cout << "___ Flood fill for Waterfall has started" << std::endl;
    while (FloodFillMovingPass()) {
      std::cout << "- iteration# " << ++iterations_counter << std::endl;
    }
  }
  std::cout << "___ Flood fill is done" << std::endl;

  shared_resources_.tile_.map_water_height.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED,
               GL_UNSIGNED_BYTE, shared_resources_.tile_.water_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  max_height_ = std::numeric_limits<int>::min();
}

int WaterMode::Hover(std::uint32_t global_id) {
  int value = -1;
  if (global_id == btn_bake_lake_.GetId()) {
    value = btn_bake_lake_.Hover();
  } else if (global_id == btn_bake_river_.GetId()) {
    value = btn_bake_river_.Hover();
  } else if (global_id == btn_bake_waterfall_.GetId()) {
    value = btn_bake_waterfall_.Hover();
  } else if (global_id == btn_create_.GetId()) {
    value = btn_create_.Hover();
  } else if (global_id == btn_remove_.GetId()) {
    value = btn_remove_.Hover();
  } else if (global_id == btn_update_.GetId()) {
    value = btn_update_.Hover();
  } else if (ocean_layer_config_1_.CheckId(global_id)) {
    value = ocean_layer_config_1_.Hover(global_id);
  } else if (ocean_layer_config_2_.CheckId(global_id)) {
    value = ocean_layer_config_2_.Hover(global_id);
  } else if (ocean_layer_config_3_.CheckId(global_id)) {
    value = ocean_layer_config_3_.Hover(global_id);
  }
  if (value < -1) {
    std::cout << "here" << std::endl;
  }
  return value;
}
