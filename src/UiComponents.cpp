#include "UiComponents.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "Colors.h"

void UiCollisionSprite::ComputeAabb() {
  glm::vec2 translation = pos_[0];
  glm::vec2 scale = pos_[1];

  aabb_ = TexCoordsAabb(-1.0f, 1.0f, 1.0f, -1.0f); // left right top bottom

  //TODO; looks like simple matrix multiplication
  aabb_.left = aabb_.left * scale.x + translation.x;
  aabb_.right = aabb_.right * scale.x + translation.x;
  aabb_.top = aabb_.top * scale.y + translation.y;
  aabb_.bottom = aabb_.bottom * scale.y + translation.y;
}

void UiCollisionSprite::Render(const Shader& already_bind_shader) {
  Bind(already_bind_shader);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void UiCollisionSprite::Bind(const Shader& already_bind_shader) {
  already_bind_shader.SetUniformMat2v(
      "transform", 1, false, glm::value_ptr(pos_));
  sprite_.Bind(); // here bind both tex_id_ and tex_coords_
}

// vao, vbo of single rectangular texture as well as shader
void UiSlots::Render(const Shader& shader) {
  shader.Bind();
  shader.SetUniform("brightness", 1.0f);
  btn_prev_.Render(shader);
  btn_next_.Render(shader);

  /// if not exist - grey color; if active - full brightness,
  /// otherwise a half
  if (point_sets_.size() >= 5) {
    for (int i = start_idx_; i < start_idx_ + 5; ++i) {
      if (i == selected_) {
        shader.SetUniform("brightness", 1.0f);
      } else {
        shader.SetUniform("brightness", 0.5f);
      }
      int pos_in_slots = i % 5;
      shader.SetUniformVec4(
          "color", 1, glm::value_ptr(slots_[pos_in_slots].color));
      slots_[pos_in_slots].sprite.Render(shader);
    }
  } else {
    // here start_idx always == 0
    std::array<glm::vec4, 5> slots_colors({colors::kLightGrey});
    for (int i = 0; i < slots_.size(); ++i) {
      slots_colors[i] = slots_[i].color;
    }
    for (int i = 0; i < 5; ++i) {
      if (i == selected_) {
        shader.SetUniform("brightness", 1.0f);
      } else {
        shader.SetUniform("brightness", 0.5f);
      }
      shader.SetUniformVec4("color", 1, glm::value_ptr(slots_colors[i]));
      slots_[i].sprite.Render(shader);
    }
  }
}

// this function called from GlfwCallback on GLFW_MOUSE_LEFT && GLFW_PRESS
/// returns idx of point_set
int UiSlots::Press(glm::vec2 pos) {
  if (btn_next_.Contains(pos)) {
    if (point_sets_.size() - start_idx_ > 5) {
      ++start_idx_;
    }
    selected_ = -1; // reset current_point_set
  } else if (btn_prev_.Contains(pos)) {
    if (start_idx_ > 0) {
      --start_idx_;
    }
    selected_ = -1; // reset current_point_set
  } else {
    for (int i = 0; i < slots_.size(); ++i) {
      if (slots_[i].sprite.Contains(pos)) {
        selected_ = start_idx_ + i;
      }
    }
  }
  return selected_;
}

/// we can't open "raw mode" in any other way except AddNew;
/// we can't modify existing set of point, but only its graph representation;

/// how graph is crated:
/// water (assume it's circle-like) is one 1:1 graph (may be looped)


/// we it only for adding a new point set, not a point to already existent,
/// because if we baked, we already have all needed points (for each
/// terrain grid), while unbaked just can't be saved - consequently modified
void UiSlots::Add(PointSetType new_point_set) {
  selected_ = point_sets_.size();
  point_sets_.push_back(new_point_set);
  //TODO: static cast...
  start_idx_ = std::max(point_sets_.size() - 5,
                        static_cast<std::vector<PointSetType>::size_type>(0));
}

void UiSlots::Remove(int idx) {
  if (point_sets_.size() < 1) {
    return;
  }
  if (idx >= 0 && idx < point_sets_.size()) {
    point_sets_.erase(point_sets_.begin() + idx);
  }
  start_idx_ = std::max(point_sets_.size() - 5,
                        static_cast<std::vector<PointSetType>::size_type>(0));
  selected_ = -1;
}

void UiSlots::InitPositions(const TexPosition& position) {
  float height = position.Height() / 7; // 5 slots + next, prev
  float offset = 0;
  btn_prev_.SetPosition({position.Top() - offset,
                         position.Top() - offset - height, 1.0f, 1.0f});
  offset += height;
  for (int i = 0; i < 5; ++i) {
    slots_[i].sprite.SetPosition(
        {position.Top() - offset,
         position.Top() - offset - height, 1.0f, 1.0f});
    offset += height;
  }
  btn_next_.SetPosition({position.Top() - offset,
                         position.Top() - offset - height, 1.0f, 1.0f});
}

void UiSlots::InitColors() {
  slots_[0].color = colors::kOrange;
  slots_[1].color = colors::kRed;
  slots_[2].color = colors::kGreen;
  slots_[3].color = colors::kBlue;
  slots_[4].color = colors::kYellow;
}

void UiSlider::Render(const Shader& shader) {
  shader.Bind();
  shader.SetUniform("brightness", 1.0f);
  slider_area_.Render(shader);
  sprite_max_threshold_.Render(shader);
  sprite_min_threshold_.Render(shader);
  sprite_slider_obj_.Render(shader);
  std::terminate(); // NOT_IMPLEMENTED
}

void UiSlider::SetProgress(float progress) {
  progress_ = progress;
  auto new_pos = sprite_slider_obj_.GetPosition();
  auto new_y = (new_pos[0].y - height) - (); // TODO simple lerp
  new_pos[0].y = new_y;
  sprite_slider_obj_.SetPosition(new_pos);
}

void UiSlider::InitPositions(const TexPosition& position) {
  float height = position.Height() / 8; // min/max threshold as 1/8
  float offset = 0;
  sprite_max_threshold_.SetPosition({position.Top() - offset,
                                     position.Top() - offset - height, 1.0f, 1.0f});
  offset += height;
  sprite_slider_obj_.SetPosition({position.Top() - offset, // temporarily here
                                  position.Top() - offset - height, 1.0f, 1.0f});
  slider_area_.SetPosition({position.Top() - offset,
                            position.Top() - offset - height, 1.0f, 1.0f});
  offset += 6*height;
  sprite_min_threshold_.SetPosition({position.Top() - offset,
                                     position.Top() - offset - height, 1.0f, 1.0f});
}
