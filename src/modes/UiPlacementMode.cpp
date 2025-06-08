#include "UiPlacementMode.h"

#include "../io/Window.h"
#include "../core/Menu.h"
#include "../io/Cameras.h"
#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../common/ShadersBinding.h"

void UiPlacementMode::ScrollCallback(
    GLFWwindow* window, double xoffset, double yoffset) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  if (yoffset < 0.0f) {
    global_data->tile_renderer->DownScale();
  } else {
    global_data->tile_renderer->UpScale();
  }
}

void UiPlacementMode::MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  auto global_data = reinterpret_cast<GlobalGlfwCallbackData*>(
      glfwGetWindowUserPointer(window));
  auto placement = dynamic_cast<UiPlacementMode*>(*global_data->cur_mode);
  glm::dvec2 cursor_pos = global_data->cursor_pos_;
  global_data->camera->ProcessMouseKey(button, action, mods);
  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    auto pressed_id = global_data->picking_fbo->GetIdByMousePos(cursor_pos);
    if (global_data->menu->Press(pressed_id)) {
      return;
    }
    if (pressed_id < details::kIdOffsetWater) {
      placement->draw_ = true;
      placement->last_modified_point_ = -1;
    }
    placement->ui_event_handler_.Press(pressed_id);
  } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
    if (placement->draw_) {
      placement->draw_ = false;
      GLuint black = 0;
      glClearTexImage(placement->new_draw_layer_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
    }
    placement->ui_event_handler_.Release();
  }
}

void UiPlacementMode::KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {}

UiPlacementMode::UiPlacementMode(UiSharedResources& ui_shared_resources,
                             const Paths& paths)
    : IUiMode(ui_shared_resources,
              UiStaticSprite{data::VboIdMain::kPlacementUiPlacementMode,
                             data::TextId::kNone}),
      btn_trees_(data::VboIdMain::kPlacementTrees, data::TextId::kNone),
      btn_bushes_(data::VboIdMain::kPlacementBushes, data::TextId::kNone),
      btn_tall_grass(data::VboIdMain::kPlacementTallGrass, data::TextId::kNone),
      btn_undergrowth_(data::VboIdMain::kPlacementUndergrowth, data::TextId::kNone),
      btn_change_mode_(data::VboIdMain::kPlacementChangeMode, data::TextId::kNone),
      slider_color_(
          UiStaticSprite{data::VboIdMain::kPlacementColorFill, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kPlacementColorBack, data::TextId::kNone},
          UiDynamicSprite{data::VboIdMain::kPlacementColorIcon,
                          data::TextId::kNone}),
      slider_size_(
          UiStaticSprite{data::VboIdMain::kPlacementSizeFill, data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kPlacementSizeBack, data::TextId::kNone},
          UiDynamicSprite{data::VboIdMain::kPlacementSizeIcon,
                          data::TextId::kNone}),
      slider_falloff_(
          UiStaticSprite{data::VboIdMain::kPlacementFalloffFill,
                                     data::TextId::kNone},
          UiStaticSprite{data::VboIdMain::kPlacementFalloffBack,
                                     data::TextId::kNone},
          UiDynamicSprite{data::VboIdMain::kPlacementFalloffIcon,
                                      data::TextId::kNone}),
      ui_event_handler_({
          &btn_trees_, &btn_bushes_, &btn_tall_grass, &btn_undergrowth_,
          &btn_change_mode_, &slider_color_, &slider_size_, &slider_falloff_}),
      shader_draw_(paths.shader_placement_draw_comp),
      new_draw_layer_(1024, 1024, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE) {
  shader_draw_.Bind();
  //TODO: not sure it represents actual starting values on UiSlider
  unsigned int radius = 100;
  float falloff = 1.0f;
  float color = 1.0f;
  glUniform1ui(shader::kPlacementRadius, radius);
  glUniform1f(shader::kPlacementFalloff, falloff);
  glUniform1f(shader::kPlacementColor, color);
  BtnTrees();
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

void UiPlacementMode::BtnChangeMode() {
  if (!preview_mode_) {
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer->placement.UpdatePipeline();
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = true;
  } else {
    PlaceLastModified();
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer->show_terrain_ = false;
  }
  preview_mode_ = !preview_mode_;
}

void UiPlacementMode::PlaceLastModified() const {
  GLuint black = 0;
  glClearTexImage(new_draw_layer_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
  glBindImageTexture(
      shader::kDrawPlacementHeightMap, last_modified_placement_, 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
  glBindImageTexture(
      shader::kDrawPlacementDrawLayer, new_draw_layer_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}

void UiPlacementMode::Render() {
  if (!preview_mode_ && draw_) {
    auto prev_modified_point = last_modified_point_;
    last_modified_point_ =
        ui_shared_resources_.global_glfw_callback_data_
            .picking_fbo->GetIdByMousePos(
            ui_shared_resources_.global_glfw_callback_data_.cursor_pos_);
    DrawPixels(prev_modified_point, last_modified_point_);
  }

  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_shader_.Bind();

  sprite_mode_.Render();
  btn_trees_.Render();
  btn_bushes_.Render();
  btn_tall_grass.Render();
  btn_undergrowth_.Render();
  btn_change_mode_.Render();

  slider_color_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_size_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
  slider_falloff_.Render(
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  slider_color_.RenderIcon();
  slider_size_.RenderIcon();
  slider_falloff_.RenderIcon();
}

void UiPlacementMode::RenderPicking() {
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.tex_ui_.Bind();
  glBindVertexArray(ui_shared_resources_.vao_ui_);

  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  sprite_mode_.RenderPicking();
  btn_trees_.RenderPicking();
  btn_bushes_.RenderPicking();
  btn_tall_grass.RenderPicking();
  btn_undergrowth_.RenderPicking();
  btn_change_mode_.RenderPicking();

  slider_color_.RenderPicking();
  slider_size_.RenderPicking();
  slider_falloff_.RenderPicking();
}

void UiPlacementMode::BtnTrees() {
  std::cout << "place trees" << std::endl;
  //TODO: we can merge this two calls
  cur_placement_mode_tex_ = &map_placement_trees_;
  last_modified_placement_ = cur_placement_mode_tex_->GetId();
  PlaceLastModified();
}
void UiPlacementMode::BtnBushes() {
  std::cout << "place bushes" << std::endl;
  cur_placement_mode_tex_ = &map_placement_bushes_;
  last_modified_placement_ = cur_placement_mode_tex_->GetId();
  PlaceLastModified();
}
void UiPlacementMode::BtnTallGrass() {
  std::cout << "place tall grass" << std::endl;
  cur_placement_mode_tex_ = &map_placement_tall_grass_;
  last_modified_placement_ = cur_placement_mode_tex_->GetId();
  PlaceLastModified();
}
void UiPlacementMode::BtnUndergrowth() {
  std::cout << "place undergrowth" << std::endl;
  cur_placement_mode_tex_ = &map_placement_undergrowth_;
  last_modified_placement_ = cur_placement_mode_tex_->GetId();
  PlaceLastModified();
}

// prev approach - by point
// cur(new) approach - by line - we check point collision with line, so
// in case when speed of mouse >> framerate, we won't skip any points
void UiPlacementMode::DrawPixels(std::uint32_t prev_id, std::uint32_t last_id) {
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
void UiPlacementMode::BindCallbacks() {
  glfwSetScrollCallback(gWindow, ScrollCallback);
  glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
  //  glfwSetKeyCallback(gWindow, KeyCallback);
  glfwSetKeyCallback(gWindow, WasdKeyCallback);
  BtnTrees();
}

int UiPlacementMode::Hover(std::uint32_t global_id) {
  return -1;
}

void UiPlacementMode::InitHeightMap(std::string_view path, Texture& texture) {
  if (!path.empty()) {
    // if float it will ignore GL_RED internally
    texture = Texture(path, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
  } else {
    texture = Texture(1024, 1024, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
  }
}
