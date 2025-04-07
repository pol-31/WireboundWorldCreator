#include "PlacementMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"
#include "../common/ShadersBinding.h"

//TODO: "size"/"radius" namin inconsistencies for slider

void PlacementModeScrollCallback(
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

void PlacementModeMouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<PlacementMode*>(global_data->cur_mode_);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera_.ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (global_data->menu_.Press(pressed_id)) {
        return;
      }
      if (pressed_id < details::kIdOffsetWater) {
        placement->draw_ = true;
        placement->last_modified_point_ = -1;
      } else if (pressed_id == placement->btn_place_trees_.GetId()) {
        placement->PlaceTrees();
      } else if (pressed_id == placement->btn_place_bushes_.GetId()) {
        placement->PlaceBushes();
      } else if (pressed_id == placement->btn_place_tall_grass_.GetId()) {
        placement->PlaceTallGrass();
      } else if (pressed_id == placement->btn_place_undergrowth_.GetId()) {
        placement->PlaceUndergrowth();
      } else if (pressed_id == placement->slider_color_.GetTrackId()) {
        placement->need_to_update_uniforms_ = true;
        placement->slider_color_.Press();
      } else if (pressed_id == placement->slider_size_.GetTrackId()) {
        placement->need_to_update_uniforms_ = true;
        placement->slider_size_.Press();
      } else if (pressed_id == placement->slider_falloff_.GetTrackId()) {
        placement->need_to_update_uniforms_ = true;
        placement->slider_falloff_.Press();
      }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      // TODO: embed to movement / add button
      placement->SwitchViewMode();
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (placement->draw_) {
      placement->draw_ = false;
      GLuint black = 0;
      glClearTexImage(placement->new_draw_layer_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
    }
    /// release all (anyway we couldn't press on slider and another one simult)
    placement->need_to_update_uniforms_ = false;
    placement->slider_color_.Release();
    placement->slider_size_.Release();
    placement->slider_falloff_.Release();
  }
}

//TODO: multiple water areas. GLFW_KEY_0-9 to switch (chosen highlighted in shader)

// TODO: possible more keys to press (now se use src/io/Window.h WasdKeyCallback
void PlacementModeKeyCallback(
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

PlacementMode::PlacementMode(SharedResources& shared_resources,
                             const Paths& paths)
    : IEditMode(shared_resources),
      btn_place_trees_(vbos::VboIdMain::kPlacementTrees, vbos::VboIdText::kPlaceTrees),
      btn_place_bushes_(vbos::VboIdMain::kPlacementBushes, vbos::VboIdText::kPlaceBushes),
      btn_place_tall_grass_(vbos::VboIdMain::kPlacementTallGrass,
                            vbos::VboIdText::kPlaceTallGrass),
      btn_place_undergrowth_(vbos::VboIdMain::kPlacementUndergrowth,
                             vbos::VboIdText::kPlaceUndergrowth),
      slider_color_(
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderColorFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderColorSlow, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderColorModerate, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderColorFast, vbos::VboIdText::kNone}),
      slider_size_(
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderSizeFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderSizeSlow, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderSizeModerate, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderSizeFast, vbos::VboIdText::kNone}),
      slider_falloff_(
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderFalloffFill, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderFalloffSlow, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderFalloffModerate, vbos::VboIdText::kNone},
          UiStaticSprite{vbos::VboIdMain::kPlacementSliderFalloffFast, vbos::VboIdText::kNone}),
      shader_draw_(paths.shader_placement_draw_comp),
      last_modified_placement_(
          shared_resources_.tile_.map_placement_trees.GetId()),
      new_draw_layer_(1024, 1024, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE) {
  shader_draw_.Bind();
  //TODO: not sure it represents actual starting values on UiSlider
  unsigned int radius = 100;
  float falloff = 1.0f;
  float color = 1.0f;
  glUniform1ui(shader::kPlacementRadius, radius);
  glUniform1f(shader::kPlacementFalloff, falloff);
  glUniform1f(shader::kPlacementColor, color);
}

//TODO: Ctrl+Z, Ctrl+Shift+Z
//TODO: Ctrl+Z, Ctrl+Shift+Z
//TODO: Ctrl+Z, Ctrl+Shift+Z
/**
for implementing of undo/redo - (let's keep it simple) we store 8 full canvas
snaps and then when switch mode, we can compress to png each time like 2 rgba textures
to reduce size, so total it's 8 mb for each mode, which we can compress...


 ... or 10 opengl textures, so we store aabb for each draw and blit onto new texture
 */

//TODO; GRASS placement !

void PlacementMode::SwitchViewMode() {
  if (!preview_mode_) {
    shared_resources_.tile_renderer_.UpdatePlacement();
  } else {
    PlaceLastModified();
  }
  preview_mode_ = !preview_mode_;
}

void PlacementMode::PlaceLastModified() const {
  GLuint black = 0;
  glClearTexImage(new_draw_layer_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
  glBindImageTexture(
      shader::kDrawPlacementHeightMap, last_modified_placement_, 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
  glBindImageTexture(
      shader::kDrawPlacementDrawLayer, new_draw_layer_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}

void PlacementMode::Render() {
  if (!preview_mode_) {
    shared_resources_.tile_renderer_.RenderPlacementDraw();
    if (draw_) {
      auto prev_modified_point = last_modified_point_;
      last_modified_point_ =
          shared_resources_.global_glfw_callback_data_
              .picking_fbo_.GetIdByMousePos(
                  shared_resources_.global_glfw_callback_data_.cursor_pos_);
      DrawPixels(prev_modified_point, last_modified_point_);
    }
  } else {
    shared_resources_.tile_renderer_.Render();
  }

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_place_trees_.Render();
  btn_place_bushes_.Render();
  btn_place_tall_grass_.Render();
  btn_place_undergrowth_.Render();

  shared_resources_.static_sprite_alpha_shader_.Bind();

  slider_color_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);
  slider_size_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);
  slider_falloff_.Render(
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_.y);

  if (need_to_update_uniforms_) {
    shader_draw_.Bind();
    glUniform1f(shader::kPlacementColor, slider_color_.GetProgress());
    //TODO: koef to Details.h
    auto radius = static_cast<unsigned int>(slider_size_.GetProgress() * 100.0f);
    glUniform1ui(shader::kPlacementRadius, radius);
    glUniform1f(shader::kPlacementFalloff, slider_falloff_.GetProgress());
    glUseProgram(0);
//    need_to_update_uniforms_ = false;
  }
}

void PlacementMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_place_trees_.RenderPicking();
  btn_place_bushes_.RenderPicking();
  btn_place_tall_grass_.RenderPicking();
  btn_place_undergrowth_.RenderPicking();
  slider_color_.RenderPicking();
  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();
}

void PlacementMode::PlaceTrees() {
  std::cout << "place trees" << std::endl;
  //TODO: we can merge this two calls
  shared_resources_.tile_.SetPlacementModeTrees();
  last_modified_placement_ =
      shared_resources_.tile_.map_placement_trees.GetId();
  PlaceLastModified();
}
void PlacementMode::PlaceBushes() {
  std::cout << "place bushes" << std::endl;
  shared_resources_.tile_.SetPlacementModeBushes();
  last_modified_placement_ =
      shared_resources_.tile_.map_placement_bushes.GetId();
  PlaceLastModified();
}
void PlacementMode::PlaceTallGrass() {
  std::cout << "place tall grass" << std::endl;
  shared_resources_.tile_.SetPlacementModeTallGrass();
  last_modified_placement_ =
      shared_resources_.tile_.map_placement_tall_grass.GetId();
  PlaceLastModified();
}
void PlacementMode::PlaceUndergrowth() {
  std::cout << "place undergrowth" << std::endl;
  shared_resources_.tile_.SetPlacementModeUndergrowth();
  last_modified_placement_ =
      shared_resources_.tile_.map_placement_undergrowth.GetId();
  PlaceLastModified();
}

// prev approach - by point
// cur(new) approach - by line - we check point collision with line, so
// in case when speed of mouse >> framerate, we won't skip any points
void PlacementMode::DrawPixels(std::uint32_t prev_id, std::uint32_t last_id) {
  if (prev_id == -1 || last_id == -1 || prev_id == last_id) {
    return;
  }
  shader_draw_.Bind();
  glm::uvec2 point{};
  glUniform2uiv(shader::kPlacementPointA, 1,
                glm::value_ptr(glm::uvec2(prev_id & 1023, prev_id >> 10)));
  glUniform2uiv(shader::kPlacementPointB, 1,
                glm::value_ptr(glm::uvec2(last_id & 1023, last_id >> 10)));
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

//TODO: we also bind trees mode here - so then rename to SetUp()?
void PlacementMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, PlacementModeScrollCallback);
  glfwSetMouseButtonCallback(gWindow, PlacementModeMouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, TerrainModeKeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  PlaceTrees();
}

int PlacementMode::Hover(std::uint32_t global_id) {
  return -1;
}
