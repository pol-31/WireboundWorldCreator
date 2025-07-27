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

  if (pressed_) {
    //TODO: draw newborn points
  }
  if (select_square_buffer_[0] != select_square_buffer_[2]) {
    ui_shared_resources_.select_square_shader_.Bind();
    glBindVertexArray(square_vao_);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, select_square_buffer_.size());
  }
}

void TerrainGrid::Init() {
  select_square_buffer_.fill(glm::vec2{0.0f});
  instances_.resize(gMaxLayers);

  GLuint vao[2];
  GLuint vbo[2];
  glGenVertexArrays(2, vao);
  glGenBuffers(2, vbo);

  vao_ = vao[0];
  square_vao_ = vao[1];
  vbo_ = vbo[0];
  square_vbo_ = vbo[1];

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // max selected: 1024 by 1024
  glBufferData(GL_ARRAY_BUFFER, 1024 * 1024 * sizeof(GLuint),
               nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), (void*)0);

  glBindVertexArray(square_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, square_vbo_);
  glBufferData(GL_ARRAY_BUFFER, select_square_buffer_.size() * sizeof(glm::vec2),
               select_square_buffer_.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TerrainGrid::DeInit() {
  GLuint vao[] = {vao_, square_vao_};
  GLuint vbo[] = {vbo_, square_vbo_};
  glDeleteVertexArrays(2, vbo);
  glDeleteBuffers(2, vao);
}

void TerrainGrid::UpdateVertexBuffer() {
  std::vector<GLuint> vertices{selected_vertices_.begin(), selected_vertices_.end()};
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLuint),
                  vertices.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TerrainGrid::UpdateSquareBuffer(glm::vec2 mouse_pos) {
  select_square_buffer_[0] =
      {(mouse_check_point_.x / gWindowWidth) * 2.0f - 1.0f,
       (1.0f - mouse_check_point_.y / gWindowHeight) * 2.0f - 1.0f};
  select_square_buffer_[1] =
      {(mouse_check_point_.x / gWindowWidth) * 2.0f - 1.0f,
       (1.0f - mouse_pos.y / gWindowHeight) * 2.0f - 1.0f};
  select_square_buffer_[2] =
      {(mouse_pos.x / gWindowWidth) * 2.0f - 1.0f,
       (1.0f - mouse_pos.y / gWindowHeight) * 2.0f - 1.0f};
  select_square_buffer_[3] =
      {(mouse_pos.x / gWindowWidth) * 2.0f - 1.0f,
       (1.0f - mouse_check_point_.y / gWindowHeight) * 2.0f - 1.0f};
  std::cout << "width " << (select_square_buffer_[0].x - select_square_buffer_[2].x)
            << ", height " << (select_square_buffer_[0].y - select_square_buffer_[2].y)
            << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, square_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * 4,
                  select_square_buffer_.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::set<GLuint> TerrainGrid::CursorOnGridRadius(GLuint point) {
  std::set<GLuint> vertices_total;
  int x = point & 1023;
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
      GLuint point = xi + (yi << 10);
      vertices_total.insert(point);
    }
  }
  std::cout << "total " << vertices_total.size() << std::endl;
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
               picking_fbo->GetIdByMousePos(pos2);
  GLuint left = std::min(start & 1023, end & 1023);
  GLuint right = std::max(start & 1023, end & 1023);
  GLuint bottom = std::min(start, end) >> 10;
  GLuint top = std::max(start, end) >> 10;
  for (int i = left; i < right; ++i) {
    vertices_total.insert((bottom << 10) + i);
  }
  // Y lower, X higher
  GLuint hor_dir = right;
  if (start < end) {
    if ((start & 1023) > (end & 1023)) {
      hor_dir = left;
    }
  } else {
    if ((start & 1023) < (end & 1023)) {
      hor_dir = left;
    }
  }
  for (int i = bottom; i < top; ++i) {
    vertices_total.insert((i << 10) + hor_dir);
  }
  return vertices_total;
}

void TerrainGrid::Press(glm::vec2 mouse_pos,
           bool shift_pressed, bool ctrl_pressed) {
  pressed_ = true;
  if (shift_pressed) {
    auto path = FindSinglePath(mouse_check_point_, mouse_pos);
    if (select_mode_ == SelectMode::kCircle) {
      // :=:=:=:=:=:
      std::set<GLuint> vertices_total;
      for (auto point : path) {
        auto vertices_new = CursorOnGridRadius(point);
        selected_vertices_.insert(vertices_new.begin(), vertices_new.end());
      }
    } else if (select_mode_ == SelectMode::kSingle) { // kSingle or kSquare
      // ._._._._._.
      selected_vertices_.insert(path.begin(), path.end());
    } else {
      FormSquare(mouse_pos);
      UpdateVertexBuffer();
      return;
      // same as on release
    }
  } else if (ctrl_pressed) {
    /// nothing
  } else {
    selected_vertices_.clear();
  }
  GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
  if (select_mode_ == SelectMode::kSingle) {
    selected_vertices_.insert(vertex_id);
  } else if (select_mode_ == SelectMode::kCircle) {
    auto new_vertices = CursorOnGridRadius(vertex_id);
    selected_vertices_.insert(new_vertices.begin(), new_vertices.end());
  }
  mouse_check_point_ = mouse_pos;
  UpdateVertexBuffer();
}

bool TerrainGrid::is_inside_convex_polygon(const glm::vec2& p) {
  size_t n = select_square_buffer_.size();
  for (size_t i = 0; i < n; ++i) {
    const glm::vec2& a = select_square_buffer_[i];
    const glm::vec2& b = select_square_buffer_[(i + 1) % n];
    float cross = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    if (cross < 0) // If using clockwise winding, use >0 instead
      return false;
  }
  return true;
}

void TerrainGrid::get_vertices_inside_convex_shape() {
  // Bounding box for performance
  float min_x = 1024;
  float max_x = 0;
  float min_y = 1024;
  float max_y = 0;

  std::array<glm::uvec2, 4> polygon;
  for (int i = 0; i < select_square_buffer_.size(); ++i) {
    auto id = ui_shared_resources_.global_glfw_callback_data_
                  .picking_fbo->GetIdByMousePos(select_square_buffer_[i]);
//    polygon[i] = id;../idk;
  }
//  for (auto point : select_square_buffer_) {
//    polygon
//  }

  for (const auto& p : select_square_buffer_) {
    min_x = std::min(min_x, p.x);
    max_x = std::max(max_x, p.x);
    min_y = std::min(min_y, p.y);
    max_y = std::max(max_y, p.y);
  }
  std::terminate();
  //TODO: but we have mouse_pos, not ids...

  for (int y = std::max(0, (int)std::floor(min_y));
       y <= std::min(1023, (int)std::ceil(max_y)); ++y) {
    for (int x = std::max(0, (int)std::floor(min_x));
         x <= std::min(1023, (int)std::ceil(max_x)); ++x) {
      glm::vec2 p = { float(x) + 0.5f, float(y) + 0.5f }; // center of the grid cell
      if (is_inside_convex_polygon(p)) {
        selected_vertices_.insert((y << 10) + x);
      }
    }
  }
}

void TerrainGrid::FormSquare(glm::vec2 mouse_pos) {
  GLuint start = ui_shared_resources_.global_glfw_callback_data_.
                 picking_fbo->GetIdByMousePos(mouse_check_point_);
  GLuint end = ui_shared_resources_.global_glfw_callback_data_.
               picking_fbo->GetIdByMousePos(mouse_pos);
  GLuint left = std::min(start & 1023, end & 1023);
  GLuint right = std::max(start & 1023, end & 1023);
  GLuint bottom = std::min(start, end) >> 10;
  GLuint top = std::max(start, end) >> 10;
  for (int i = left; i < right; ++i) {
    for (int j = bottom; j < top; ++j) {
      selected_vertices_.insert((j << 10) + i);
    }
  }
//  get_vertices_inside_convex_shape();
  select_square_buffer_.fill(glm::vec2{0.0f});
  UpdateVertexBuffer();
  mouse_check_point_ = mouse_pos;
}

void TerrainGrid::Release() {
  if (!pressed_) {
    return;
  }
  pressed_ = false;
  if (select_mode_ == SelectMode::kSquare) {
    FormSquare(ui_shared_resources_.global_glfw_callback_data_.cursor_pos_);
  }
}

void TerrainGrid::Select(glm::vec2 mouse_pos) {
  // shift and ctrl doesn't matter
  GLuint vertex_id = ProjectCursorOnGrid(mouse_pos);
  if (select_mode_ == SelectMode::kSingle) {
    std::cout << selected_vertices_.size() << std::endl;
    selected_vertices_.insert(vertex_id);
  } else if (select_mode_ == SelectMode::kSquare) {
    UpdateSquareBuffer(mouse_pos);
  } else { // SelectMode::kCircle
    auto new_vertices = CursorOnGridRadius(vertex_id);
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

/**
 *
let's cheat and select regardless of mvp:
- kSquare: start - end (Decardian)
- kSingle: single point / trivial path
- kCircle: around the single point
BUT(!): draw-square/circle should be shown projected.
__extra: out text with vertex id, x, y (use kSpareTextN)
 * */

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
  return select_mode_;
}

int TerrainGrid::GetSize() const noexcept {
  return instances_size_;
}
