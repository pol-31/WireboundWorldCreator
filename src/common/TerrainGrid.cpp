#include "TerrainGrid.h"

#include "../common/PickingFramebuffer.h"
#include "../core/TileRenderer.h"

TerrainGrid::TerrainGrid(
    UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources) {
  Init();
}

void TerrainGrid::CreateGraph() {
  if (instances_size_ >= gMaxLayers) {
    std::cerr << "Unable to add more graphs (data overflow)" << std::endl;
  } else {
    selected_slot_id_ = instances_size_;
    selected_vertices_.clear();
    InstanceData instance_data;
    instance_data.name = FixedSizeQueue<char, 64>{};
    instance_data.color = glm::vec3{1.0f};
    instance_data.do_show = true;
    instance_data.heights.fill(0);
    instance_data.hmap = {};
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
  selected_vertices_.clear();
  std::cout << "selected " << selected_slot_id_
            << ", not implemented" << std::endl;
}

void TerrainGrid::RemoveGraph(int slot_id) {
  if (slot_id >= instances_size_) {
    throw "remove non-existent graph id";
  }
  //TODO: deselect
  /*if (selected_slot_id_ > slot_id) {
    --selected_slot_id_;
  } else if (selected_slot_id_ == slot_id) {
    selected_slot_id_ = -1;
    selected_ids_.clear();
  }*/
  //TODO: erase
  //    instances_.erase(instances_.begin() + slot_id);
  std::cout << "*slot removed" << std::endl;
}

void TerrainGrid::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Select(mouse_pos);
  }
  ui_shared_resources_.arbitrary_graph_shader_.Bind();
  glActiveTexture(GL_TEXTURE0);
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->cur_tile_.map_terrain_height.Bind();
  glBindVertexArray(vao_);
  glPointSize(5.0f);
  glDrawArrays(GL_POINTS, 0, selected_vertices_.size());
  //TODO: render points OR edges OR faces
}

void TerrainGrid::Init() {
  instances_.resize(gMaxLayers);

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // max selected: 1024 by 1024
  glBufferData(GL_ARRAY_BUFFER, 1024 * 1024 * sizeof(GLuint),
               nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)0);

  glBindVertexArray(0);
}

void TerrainGrid::DeInit() {
  glDeleteBuffers(1, &vbo_);
  glDeleteVertexArrays(1, &vao_);
}

void TerrainGrid::UpdateVertexBuffer() {
  std::vector<GLuint> vertices{selected_vertices_.begin(), selected_vertices_.end()};
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLuint),
                  vertices.data());
}

// vec2 position -> GLuint point
std::set<GLuint> TerrainGrid::ProjectCursorOnGridRadius(
    glm::vec2 mouse_pos) {
  std::set<GLuint> vertices_total;
  //TODO: beware with mouse_pos values (0-1023?)
  for (int dy = -gRadius; dy <= gRadius; ++dy) {
    for (int dx = -gRadius; dx <= gRadius; ++dx) {
      int xi = static_cast<int>(mouse_pos.x) + dx;
      int yi = static_cast<int>(mouse_pos.y) + dy;

      // Check if still inside circle
      if (dx * dx + dy * dy > gRadius * gRadius) {
        continue;
      }

      // Clamp bounds to grid
      if (xi < 0 || xi >= 1024 || yi < 0 || yi >= 1024) {
        continue;
      }

      // (xi, yi) is inside the circle
      GLuint point = ProjectCursorOnGrid({xi, yi});
      vertices_total.insert(point);
    }
  }
  return  vertices_total;
}

std::set<GLuint> TerrainGrid::CursorOnGridRadius(GLuint point) {
  std::set<GLuint> vertices_total;
  int x = point & 1024;
  int y =  point >> 10;
  //TODO: beware with mouse_pos values (0-1023?)
  for (int dy = -gRadius; dy <= gRadius; ++dy) {
    for (int dx = -gRadius; dx <= gRadius; ++dx) {
      int xi = x + dx;
      int yi = y + dy;

      // Check if still inside circle
      if (dx * dx + dy * dy > gRadius * gRadius) {
        continue;
      }

      // Clamp bounds to grid
      if (xi < 0 || xi >= 1024 || yi < 0 || yi >= 1024) {
        continue;
      }

      // (xi, yi) is inside the circle
      GLuint point = xi + yi * 1024;
      vertices_total.insert(point);
    }
  }
  return  vertices_total;
}

GLuint TerrainGrid::ProjectCursorOnGrid(glm::vec2 mouse_pos) {
  return ui_shared_resources_.global_glfw_callback_data_.
      picking_fbo->GetIdByMousePos(mouse_pos);
}

// vec2 position -> set<GLuint> points
std::set<GLuint> TerrainGrid::FindSinglePath(
      glm::vec2 pos1, glm::vec2 pos2) {
  std::set<GLuint> vertices_total;
  GLuint start = ui_shared_resources_.global_glfw_callback_data_.
                 picking_fbo->GetIdByMousePos(pos1);
  GLuint end = ui_shared_resources_.global_glfw_callback_data_.
               picking_fbo->GetIdByMousePos(pos1);

  //
  return vertices_total;
}

void TerrainGrid::Press(glm::vec2 mouse_pos,
           bool shift_pressed, bool ctrl_pressed) {
  pressed_ = true;
  return;
  if (shift_pressed) {
    std::set<GLuint> new_vertices;
    auto path = FindSinglePath(last_mouse_pos_, mouse_pos);
    if (select_mode_ == SelectMode::kCircle) {
      // :=:=:=:=:=:
      std::set<GLuint> vertices_total;
      for (auto point : path) {
        auto vertices_new = CursorOnGridRadius(point);
        new_vertices.insert(new_vertices.begin(), new_vertices.end());
      }
    } else { // kSingle or kSquare
      // ._._._._._.
      new_vertices = path;
    }
    selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  } else if (!ctrl_pressed) {
    square_last_mouse_pos_ = mouse_pos;
    selected_vertices_.clear();
  }
  GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
  if (select_mode_ == SelectMode::kSingle) {
    selected_vertices_.insert(vertex_id);
  } else if (select_mode_ == SelectMode::kCircle) {
    auto new_vertices = CursorOnGridRadius(vertex_id);
    selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  } else { // select_mode == kSquare
    last_mouse_pos_ = mouse_pos;
  }
  UpdateVertexBuffer();
}

void TerrainGrid::Release() {
  pressed_ = false;
  if (select_mode_ == SelectMode::kSquare) {
    auto new_vertices =
        ui_shared_resources_.global_glfw_callback_data_.
        picking_fbo->SelectSquare(square_last_mouse_pos_, last_mouse_pos_);
    selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  }
}

void TerrainGrid::Select(glm::vec2 mouse_pos) {
  auto new_vertices = ProjectCursorOnGridRadius(mouse_pos);
  selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  UpdateVertexBuffer();
  std::cerr << "size " << selected_vertices_.size() << std::endl;
  return;
  // shift / ctrl don't matter
  if (select_mode_ == SelectMode::kSingle) {
    GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
    selected_vertices_.insert(vertex_id);
  } else if (select_mode_ == SelectMode::kSquare) {
    square_last_mouse_pos_ = mouse_pos;
    //TODO: only DrawSquare changes its shape
  } else { // SelectMode::kCircle
    auto new_vertices = ProjectCursorOnGridRadius(mouse_pos);
    selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  }
  UpdateVertexBuffer();
  std::cerr << "size " << selected_vertices_.size() << std::endl;
}

void TerrainGrid::Select(GLuint id) {
  selected_vertices_.insert(id);
  UpdateVertexBuffer();
  std::cerr << "size " << selected_vertices_.size() << std::endl;
}

TerrainGrid::SelectMode TerrainGrid::FlipSelectMode() {
  switch (select_mode_) {
    case SelectMode::kCircle:
      select_mode_ = SelectMode::kSingle;
      break;
    case SelectMode::kSingle:
      select_mode_ = SelectMode::kSquare;
      break;
    case SelectMode::kSquare:
      select_mode_ = SelectMode::kCircle;
      break;
  }
  selected_vertices_.clear();
  return select_mode_;
}

int TerrainGrid::GetSize() const noexcept {
  return instances_size_;
}
