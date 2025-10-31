#include "TerrainGrid.h"

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"
#include "../renderers/UiRenderer.h"
#include "../core/UiLayerWireframe.h"

TerrainGrid::TerrainGrid(
    UiSharedResources& ui_shared_resources,
    UiEditTerrain& ui_edit_terrain,
    std::vector<TerrainInstanceData>& instances,
    const UiSliderV3& slider_size,
    const UiSliderV3& slider_falloff)
    : ui_shared_resources_(ui_shared_resources),
      ui_edit_terrain_(ui_edit_terrain),
      instances_(instances),
      tex_selection_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_potential_selection_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      slider_size_(slider_size),
      slider_falloff_(slider_falloff),
      selection_mask_blured_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      vertices_transform_shader_("../shaders/generate_shaders/VerticesTransform.comp"),
      selection_blur_shader_("../shaders/generate_shaders/SelectionBlur.comp") {}

TerrainGrid::TerrainGrid(TerrainGrid&& other)
    : ui_shared_resources_(other.ui_shared_resources_),
      ui_edit_terrain_(other.ui_edit_terrain_),
      instances_(other.instances_),
      tex_selection_(std::move(other.tex_selection_)),
      tex_potential_selection_(std::move(other.tex_potential_selection_)),
      slider_size_(other.slider_size_),
      slider_falloff_(other.slider_falloff_),
      start_is_end_(other.start_is_end_),
      selected_vertices_(std::move(other.selected_vertices_)),
      pressed_(other.pressed_),
      mouse_check_point_(other.mouse_check_point_),
      selected_slot_id_(other.selected_slot_id_),
      vertices_transform_shader_(std::move(other.vertices_transform_shader_)) {}

void TerrainGrid::CreateGraph() {
  if (instances_.size() >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_.size();
    ClearSelection();
    TerrainInstanceData instance_data;
    instance_data.name = {};
    instance_data.color = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
    instance_data.do_show = true;
    instance_data.type_id = 0; // TODO: unused
    instance_data.data = NoiseTerrainData{};
    // default scale, rotate, translate, do_tiling, do_invert
//    instance_data.hmap = Texture32F(details::gTerrainSize, GL_R32F);
    instance_data.heights.fill(0.0f);
    instances_.push_back(std::move(instance_data));
  }
}

void TerrainGrid::SelectGraph(int slot_id) {
  if (slot_id == selected_slot_id_) {
    std::cout << "already selected" << std::endl;
    return;
  }
  if (slot_id >= instances_.size()) {
    throw "select non-existent slot id";
  }
  selected_slot_id_ = slot_id;
  ClearSelection();
}

bool TerrainGrid::RemoveGraph(int slot_id) {
  if (slot_id >= instances_.size()) {
    throw "remove non-existent graph id";
  }
  if (selected_slot_id_ > slot_id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == slot_id) {
    selected_slot_id_ = -1;
    ClearSelection();
  }
  instances_.erase(instances_.begin() + slot_id);
  std::cout << "*slot removed " << slot_id << std::endl;
  return true;
}

void TerrainGrid::UpdateMousePotentialSelection(glm::vec2 mouse_pos) {
  GLuint black = 0;
  glClearTexImage(tex_potential_selection_.GetId(), 0, GL_RED,
                  GL_UNSIGNED_BYTE, &black);
 /* ui_shared_resources_.shader_terrain_selection_.Bind();
  glBindImageTexture(
      0, tex_potential_selection_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
  glm::uvec2 point{};
  glUniform1ui(2, static_cast<GLuint>(slider_size_.GetProgress()));
  glUniform1f(3, slider_falloff_.GetProgress());
  GLuint id = ui_shared_resources_.global_glfw_callback_data_.
              picking_fbo->GetIdByMousePos(mouse_pos);
  glm::uvec2 pos(id >> 10, id & 1023);
  glUniform2uiv(0, 1, glm::value_ptr(pos));
  glUniform2uiv(1, 1, glm::value_ptr(pos));
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);*/
}

void TerrainGrid::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Select(mouse_pos);
  }

  UpdateMousePotentialSelection(mouse_pos);

  if (selected_slot_id_ != -1) {
    /// selected layer wireframe
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
        ->terrain.RenderWireframe(&instances_[selected_slot_id_]);
    glm::vec4 layer_pivot(instances_[selected_slot_id_].translate.x,
                          instances_[selected_slot_id_].translate.y,
                          instances_[selected_slot_id_].translate.z,
                          1.0f);
    auto color_invert =
        glm::vec4(1.0f) - instances_[selected_slot_id_].color;
    color_invert.w = 1.0f;
    ui_shared_resources_.global_glfw_callback_data_.ui_renderer
        ->RenderWorldOrigin(layer_pivot, color_invert);

    /// wireframe left bottom
    auto& ui_layer_wireframe =
        ui_shared_resources_.global_glfw_callback_data_
            .ui_renderer->GetUiLayerWireframe();
    ui_layer_wireframe.RenderLayerWireframe(&instances_[selected_slot_id_]);

/* REMOVED
 * /// selection: mouse move, potential place
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
        ->terrain.RenderSelection(
            &instances_[selected_slot_id_], tex_potential_selection_, color);*/

    /// selection: already drawn, selected
    //TODO: test in UiSelection
/*    glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.1f);
    color = glm::vec3(0.2f, 0.2f, 0.8f);
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
        ->terrain.RenderSelection(
            &instances_[selected_slot_id_], tex_selection_, color);*/
  }
}

void TerrainGrid::RenderPicking() {
  auto& ui_layer_wireframe =
      ui_shared_resources_.global_glfw_callback_data_
          .ui_renderer->GetUiLayerWireframe();
  ui_layer_wireframe.RenderPickingLayerWireframe();
}

GLuint TerrainGrid::ProjectCursorOnGrid(glm::vec2 mouse_pos) {
  return ui_shared_resources_.global_glfw_callback_data_.
      picking_fbo->GetIdByMousePos(mouse_pos);
}

void TerrainGrid::Press(glm::vec2 mouse_pos,
           bool shift_pressed, bool ctrl_pressed) {
  pressed_ = true;
  if (shift_pressed) {
    /// nothing, start point = prev end
    start_is_end_ = false;
  } else if (ctrl_pressed) {
    start_is_end_ = true;
    //TODO: update the starting point
  } else {
    ClearSelection();
    start_is_end_ = true;
  }
  Select(mouse_pos);
}

void TerrainGrid::ClearSelection() {
  GLuint black = 0;
  glClearTexImage(tex_selection_.GetId(), 0, GL_RED, GL_UNSIGNED_BYTE, &black);
}

void TerrainGrid::Release() {
  pressed_ = false;
}

void TerrainGrid::Select(glm::vec2 mouse_pos) {
  //TODO: only if mode == kCircle or kTweak
  // shift and ctrl doesn't matter
  ui_shared_resources_.shader_terrain_selection_.Bind();
  /*glBindImageTexture(
      0, tex_selection_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
  glm::uvec2 point{};
  glUniform1ui(2, static_cast<GLuint>(slider_size_.GetProgress()));
  glUniform1f(3, slider_falloff_.GetProgress());
  GLuint start = ui_shared_resources_.global_glfw_callback_data_.
                 picking_fbo->GetIdByMousePos(mouse_check_point_);
  GLuint end = ui_shared_resources_.global_glfw_callback_data_.
               picking_fbo->GetIdByMousePos(mouse_pos);
  if (start_is_end_) {
    start = end;
    start_is_end_ = false;
  }
  glUniform2uiv(0, 1,
                glm::value_ptr(glm::uvec2(start >> 10, start & 1023)));
  glUniform2uiv(1, 1,
                glm::value_ptr(glm::uvec2(end >> 10, start & 1023)));
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  mouse_check_point_ = mouse_pos;*/
  glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}

int TerrainGrid::GetSize() const noexcept {
  return instances_.size();
}

TerrainInstanceData* TerrainGrid::GetInstanceData() {
  if (selected_slot_id_ != -1) {
    return &instances_[selected_slot_id_];
  } else {
    return nullptr;
  }
}

void TerrainGrid::SelectVertices(const std::vector<GLuint>& points) {
  selected_vertices_.insert(points.begin(), points.end());
}

void TerrainGrid::SetSelectionMask(const Texture* mask) {
  selection_mask_ = mask;
}

void TerrainGrid::MoveSelected(float value) {
  if (!selection_mask_ || selected_slot_id_ == -1) {
    return;
  }
  vertices_transform_shader_.Bind();
  utility::BindImageTexture(0, instances_[selected_slot_id_].data.hmap, GL_READ_WRITE);
  utility::BindImageTexture(1, *selection_mask_, GL_READ_ONLY);
  glUniform1f(0, value);
  glUniform1f(1, slider_falloff_.GetProgress());
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  utility::UnBindImageTexture(0, instances_[selected_slot_id_].data.hmap, GL_READ_WRITE);
  utility::UnBindImageTexture(1, *selection_mask_, GL_READ_ONLY);
}
