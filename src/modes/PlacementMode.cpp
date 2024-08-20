#include "PlacementMode.h"

#include "../io/Window.h"
#include "../common/GlobalGlfwCallbackData.h"
#include "../core/Menu.h"
#include "../common/Vbos.h"

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
  if (action == GLFW_PRESS) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      global_data->cursor_.SwitchMode(window);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
      auto pressed_id = global_data->picking_fbo_.GetIdByMousePos(cursor_pos);
      if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        global_data->menu_.Press(pressed_id);
      } else {
        //TODO: bvh?
        if (pressed_id < details::kIdOffsetWater) {
          placement->draw_ = true;
        } else if (pressed_id == placement->btn_place_trees_.GetId()) {
          placement->PlaceTrees();
        } else if (pressed_id == placement->btn_place_bushes_.GetId()) {
          placement->PlaceBushes();
        } else if (pressed_id == placement->btn_place_tall_grass_.GetId()) {
          placement->PlaceTallGrass();
        } else if (pressed_id == placement->btn_place_undergrowth_.GetId()) {
          placement->PlaceUndergrowth();
        } else if (pressed_id == placement->slider_color_.GetTrackId()) {
          placement->slider_color_pressed_ = true;
          placement->UpdateColorSliderPos(global_data->cursor_pos_tex_norm_);
        } else if (pressed_id == placement->slider_size_.GetTrackId()) {
          placement->slider_size_pressed_ = true;
          placement->UpdateSizeSliderPos(global_data->cursor_pos_tex_norm_);
        } else if (pressed_id == placement->slider_falloff_.GetTrackId()) {
          placement->slider_falloff_pressed_ = true;
          placement->UpdateFalloffSliderPos(global_data->cursor_pos_tex_norm_);
        }
      }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      // TODO: embed to movement / add button
      placement->SwitchViewMode();
    }
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (placement->draw_) {
      placement->draw_ = false;
    } else if (placement->slider_color_pressed_) {
      placement->slider_color_pressed_ = false;
    } else if (placement->slider_size_pressed_) {
      placement->slider_size_pressed_ = false;
    } else if (placement->slider_falloff_pressed_) {
      placement->slider_falloff_pressed_ = false;
    }
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
      btn_place_trees_(
          "place trees", GetUiData(UiVboDataMainId::kPlacementTrees)),
      btn_place_bushes_(
          "place bushes", GetUiData(UiVboDataMainId::kPlacementBushes)),
      btn_place_tall_grass_(
          "place tall grass", GetUiData(UiVboDataMainId::kPlacementTallGrass)),
      btn_place_undergrowth_(
          "place undergrowth", GetUiData(UiVboDataMainId::kPlacementUndergrowth)),
      slider_color_(
          {"min size", GetUiData(UiVboDataMainId::kPlacementSliderColorWhite)},
          {"max size", GetUiData(UiVboDataMainId::kPlacementSliderColorBlack)},
          {"size slider track", GetUiData(UiVboDataMainId::kPlacementSliderTrack)},
          {"size slider handle", GetUiData(UiVboDataMainId::kPlacementSliderHandler)}),
      slider_size_(
          {"min size", GetUiData(UiVboDataMainId::kUiSliderSizeMin)},
          {"max size", GetUiData(UiVboDataMainId::kUiSliderSizeMax)},
          {"size slider track", GetUiData(UiVboDataMainId::kUiSliderSizeTrack)},
          {"size slider handle", GetUiData(UiVboDataMainId::kUiSliderSizeHandler)}),
      slider_falloff_(
          {"min falloff", GetUiData(UiVboDataMainId::kUiSliderFalloffMin)},
          {"max falloff", GetUiData(UiVboDataMainId::kUiSliderFalloffMax)},
          {"falloff slider track", GetUiData(UiVboDataMainId::kUiSliderFalloffTrack)},
          {"falloff slider handle", GetUiData(UiVboDataMainId::kUiSliderFalloffHandler)}),
      shader_draw_(paths.shader_placement_draw_comp) {
  shader_draw_.Bind();
  //TODO: not sure it represents actual starting values on UiSlider
  unsigned int radius = 100;
  float falloff = 0.5f;
  float color = 0.5f;
  shader_draw_.SetUniform("radius", radius);
  shader_draw_.SetUniform("falloff", falloff);
  shader_draw_.SetUniform("color", color);
}

//TODO: Ctrl+Z, Ctrl+Shift+Z
// falloff should me more smooth at edges (cubic interpolation?)
// IN THEORY we can use texture for this and simply change everything... idk

//TODO; GRASS placement !

void PlacementMode::SwitchViewMode() {
  if (!preview_mode_) {
    shared_resources_.tile_renderer_.UpdatePlacement();
  }
  preview_mode_ = !preview_mode_;
}

void PlacementMode::Render() {
  if (!preview_mode_) {
    shared_resources_.tile_renderer_.RenderPlacementDraw();
  } else {
    shared_resources_.tile_renderer_.Render();
  }

  if (draw_) {
    auto prev_modified_point = last_modified_point_;
    last_modified_point_ =
        shared_resources_.global_glfw_callback_data_
            .picking_fbo_.GetIdByMousePos(
                shared_resources_.global_glfw_callback_data_.cursor_pos_);
    if (prev_modified_point != last_modified_point_) {
      DrawPixels(last_modified_point_);
    }
  }

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_shader_.Bind();

  btn_place_trees_.Render();
  btn_place_bushes_.Render();
  btn_place_tall_grass_.Render();
  btn_place_undergrowth_.Render();

  if (slider_color_pressed_) {
    UpdateColorSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }
  if (slider_size_pressed_) {
    UpdateSizeSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }
  if (slider_falloff_pressed_) {
    UpdateFalloffSliderPos(
        shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  }

  slider_color_.Render(shared_resources_.static_sprite_shader_,
                            shared_resources_.slider_handle_shader_);
  slider_size_.Render(shared_resources_.static_sprite_shader_,
                               shared_resources_.slider_handle_shader_);
  slider_falloff_.Render(shared_resources_.static_sprite_shader_,
                               shared_resources_.slider_handle_shader_);
}

void PlacementMode::RenderPicking() {
  shared_resources_.tile_renderer_.RenderPickingTerrain();

  glActiveTexture(GL_TEXTURE0);
  shared_resources_.tex_ui_.Bind();
  glBindVertexArray(shared_resources_.vao_ui_);

  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_place_trees_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  btn_place_bushes_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  btn_place_tall_grass_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  btn_place_undergrowth_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  slider_color_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  slider_size_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
  slider_falloff_.RenderPicking(shared_resources_.static_sprite_picking_shader_);
}

void PlacementMode::UpdateColorSliderPos(glm::vec2 position) {
  slider_color_.UpdateSliderPos(position);
  shader_draw_.Bind();
  shader_draw_.SetUniform("color", slider_color_.GetProgress());
}

void PlacementMode::UpdateSizeSliderPos(glm::vec2 position) {
  slider_size_.UpdateSliderPos(position);
  shader_draw_.Bind();
  //TODO: koef to Details.h
  auto radius = static_cast<unsigned int>(slider_size_.GetProgress() * 100.0f);
  shader_draw_.SetUniform(
      "radius", radius);
  std::cout << "radius is " << slider_size_.GetProgress()
            << " and actual is " << radius << std::endl;
}

void PlacementMode::UpdateFalloffSliderPos(glm::vec2 position) {
  slider_falloff_.UpdateSliderPos(position);
  shader_draw_.Bind();
  shader_draw_.SetUniform("falloff", slider_falloff_.GetProgress());
}

void PlacementMode::PlaceTrees() {
  std::cout << "place trees" << std::endl;
  //TODO: we can merge this two calls
  shared_resources_.tile_.SetPlacementModeTrees();
  glBindImageTexture(
      shader_image_unit_,
      shared_resources_.tile_.map_placement_trees.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}
void PlacementMode::PlaceBushes() {
  std::cout << "place bushes" << std::endl;
  shared_resources_.tile_.SetPlacementModeBushes();
  glBindImageTexture(
      shader_image_unit_,
      shared_resources_.tile_.map_placement_bushes.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}
void PlacementMode::PlaceTallGrass() {
  std::cout << "place tall grass" << std::endl;
  shared_resources_.tile_.SetPlacementModeTallGrass();
  glBindImageTexture(
      shader_image_unit_,
      shared_resources_.tile_.map_placement_tall_grass.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}
void PlacementMode::PlaceUndergrowth() {
  std::cout << "place undergrowth" << std::endl;
  shared_resources_.tile_.SetPlacementModeUndergrowth();
  glBindImageTexture(
      shader_image_unit_,
      shared_resources_.tile_.map_placement_undergrowth.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}

void PlacementMode::DrawPixels(std::uint32_t pressed_id) {
  shader_draw_.Bind();
  glm::uvec2 point{pressed_id & 1023, pressed_id >> 10};
  shader_draw_.SetUniformVec2("point", 1, glm::value_ptr(point));

  std::cout << "draw at point " << point.x << " " << point.y << std::endl;

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
