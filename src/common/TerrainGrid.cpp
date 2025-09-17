#include "TerrainGrid.h"

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

TerrainGrid::TerrainGrid(
    UiSharedResources& ui_shared_resources,
    UiEditTerrain& ui_edit_terrain,
    std::vector<TerrainInstanceData>& instances,
    int& instances_size,
    const UiSliderV& slider_size,
    const UiSliderV& slider_falloff)
    : ui_shared_resources_(ui_shared_resources),
      ui_edit_terrain_(ui_edit_terrain),
      instances_(instances),
      instances_size_(instances_size),
      tex_selection_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      tex_potential_selection_(details::gTerrainSize, details::gTerrainSize, GL_R8),
      slider_size_(slider_size),
      slider_falloff_(slider_falloff),\
      layer_(data::VboIdMain::kSpareText1) {
  Init();
}

TerrainGrid::TerrainGrid(TerrainGrid&& other)
    : ui_shared_resources_(other.ui_shared_resources_),
      ui_edit_terrain_(other.ui_edit_terrain_),
      instances_(other.instances_),
      instances_size_(other.instances_size_),
      tex_selection_(std::move(other.tex_selection_)),
      tex_potential_selection_(std::move(other.tex_potential_selection_)),
      slider_size_(other.slider_size_),
      slider_falloff_(other.slider_falloff_),
      layer_(std::move(other.layer_)),
      start_is_end_(other.start_is_end_),
      selected_vertices_(std::move(other.selected_vertices_)),
      pressed_(other.pressed_),
      mouse_check_point_(other.mouse_check_point_),
      selected_slot_id_(other.selected_slot_id_),
      layer_tex_(std::move(other.layer_tex_)),
      layer_fbo_(other.layer_fbo_) {
  other.layer_fbo_ = 0;
}

void TerrainGrid::Init() {
  instances_.resize(gMaxLayers);

  //TODO: fboDepth unhandled
  GLuint fbo_tex, fboDepth;
  int fboWidth = gWindowWidth / 4, fboHeight = gWindowHeight / 4; // Preview resolution

  // Create color texture
  glGenTextures(1, &fbo_tex);
  glBindTexture(GL_TEXTURE_2D, fbo_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create depth buffer
  glGenRenderbuffers(1, &fboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // Create framebuffer
  glGenFramebuffers(1, &layer_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, fbo_tex, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, fboDepth);

  // Check completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "FBO not complete!" << std::endl;
  }
  layer_tex_ = Texture(fbo_tex, 256, 256, GL_RGBA8);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TerrainGrid::DeInit() {
  glDeleteFramebuffers(1, &layer_fbo_);
}

void TerrainGrid::CreateGraph() {
  if (instances_size_ >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_size_;
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
    instances_[instances_size_++] = std::move(instance_data);
  }
}

void TerrainGrid::SelectGraph(int slot_id) {
  if (slot_id == selected_slot_id_) {
    std::cout << "already selected" << std::endl;
    return;
  }
  if (slot_id >= instances_size_) {
    throw "select non-existent slot id";
  }
  selected_slot_id_ = slot_id;
  ClearSelection();
}

bool TerrainGrid::RemoveGraph(int slot_id) {
  if (slot_id >= instances_size_) {
    throw "remove non-existent graph id";
  }
  if (selected_slot_id_ > slot_id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == slot_id) {
    selected_slot_id_ = -1;
    ClearSelection();
  }
  --instances_size_;
  instances_.erase(instances_.begin() + slot_id);
  std::cout << "*slot removed " << slot_id << std::endl;
  return true;
}

void TerrainGrid::UpdateMousePotentialSelection(glm::vec2 mouse_pos) {
  GLuint black = 0;
  glClearTexImage(tex_potential_selection_.GetId(), 0, GL_RED,
                  GL_UNSIGNED_BYTE, &black);
  ui_shared_resources_.shader_terrain_selection_.Bind();
  glBindImageTexture(
      0, tex_potential_selection_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, GL_R8);
  glm::uvec2 point{};
  glUniform1ui(2, static_cast<GLuint>(slider_size_.GetProgress()));
  glUniform1f(3, slider_falloff_.GetProgress());
  GLuint id = ui_shared_resources_.global_glfw_callback_data_.
              picking_fbo->GetIdByMousePos(mouse_pos);
  glm::uvec2 pos(id & 1023, id >> 10);
  glUniform2uiv(0, 1, glm::value_ptr(pos));
  glUniform2uiv(1, 1, glm::value_ptr(pos));
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}

void TerrainGrid::UpdateDownScaledWireframe() {
  glBindFramebuffer(GL_FRAMEBUFFER, layer_fbo_);
  glViewport(0, 0, gWindowWidth / 4, gWindowHeight / 4);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->terrain.RenderWireframe(&instances_[selected_slot_id_]);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight); /// restore
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

    /// wireframe left bottom
    UpdateDownScaledWireframe();
    ui_shared_resources_.dynamic_sprite_shader_.Bind();
    glBindVertexArray(ui_shared_resources_.vao_ui_);
    glActiveTexture(GL_TEXTURE0);
    layer_tex_.Bind();
    layer_.Render();

    /// selection: mouse move, potential place
    glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.1f);
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
        ->terrain.RenderSelection(
            &instances_[selected_slot_id_], tex_potential_selection_, color);

    /// selection: already drawn, selected
    color = glm::vec3(0.2f, 0.2f, 0.8f);
    ui_shared_resources_.global_glfw_callback_data_.tile_renderer
        ->terrain.RenderSelection(
            &instances_[selected_slot_id_], tex_selection_, color);
  }
}

void TerrainGrid::RenderPicking() {
  layer_.RenderPicking();
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
  // shift and ctrl doesn't matter
  ui_shared_resources_.shader_terrain_selection_.Bind();
  glBindImageTexture(
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
                glm::value_ptr(glm::uvec2(start & 1023, start >> 10)));
  glUniform2uiv(1, 1,
                glm::value_ptr(glm::uvec2(end & 1023, end >> 10)));
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  mouse_check_point_ = mouse_pos;
  glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}

int TerrainGrid::GetSize() const noexcept {
  return instances_size_;
}

TerrainInstanceData* TerrainGrid::GetInstanceData() {
  if (selected_slot_id_ != -1) {
    return &instances_[selected_slot_id_];
  } else {
    return nullptr;
  }
}
