#include "MouseTransform.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "OpenGlUtility.h"

MouseTransform::MouseTransform(UiRenderData& render_data)
    : render_data_(render_data),
      vertices_transform_shader_(
          "../shaders/generate_shaders/VerticesTransform.comp", {}) {}

void MouseTransform::UpdateStartAngle() {
  auto mouse_pos = render_data_.glfw_context_.cursor_pos_;
  auto offset = GetWorldOffset(mouse_pos.x, mouse_pos.y);
  zero_angle_ = std::atan2(offset.y, offset.x) - zero_angle_;
  last_angle_ = 0.0f;
}

float MouseTransform::UpdateAngle(float xpos, float ypos) {
  auto offset = GetWorldOffset(xpos, ypos);
  float angle = std::atan2(offset.y, offset.x) - zero_angle_;
  float deltaAngle = angle - last_angle_;
  last_angle_ = angle;
  if (deltaAngle > glm::pi<float>()) {
    deltaAngle -= glm::two_pi<float>();
  }
  if (deltaAngle < -glm::pi<float>()) {
    deltaAngle += glm::two_pi<float>();
  }
  return deltaAngle;
}

void MouseTransform::InitTransform(glm::vec3 translate, glm::quat rotate,
                                   glm::vec3 scale) {
  axis_ = glm::vec3(1.0f);
  Reset(translate, rotate, scale);
  UpdateStartAngle();
  auto mouse_pos = render_data_.glfw_context_.cursor_pos_;
  auto offset = GetWorldOffset(mouse_pos.x, mouse_pos.y);
  zero_scale_length_ = glm::length(offset);
}

void MouseTransform::SetAxis(int key, bool mod_shift) {
  if (mod_shift & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      axis_ = {0.0f, 1.0f, 1.0f};
    } else if (key == GLFW_KEY_Y) {
      axis_ = {1.0f, 0.0f, 1.0f};
    } else if (key == GLFW_KEY_Z) {
      axis_ = {1.0f, 1.0f, 0.0f};
    }
  } else {
    if (key == GLFW_KEY_X) {
      axis_ = {1.0f, 0.0f, 0.0f};
    } else if (key == GLFW_KEY_Y) {
      axis_ = {0.0f, 1.0f, 0.0f};
    } else if (key == GLFW_KEY_Z) {
      axis_ = {0.0f, 0.0f, 1.0f};
    }
  }
}

void MouseTransform::RotateSelected(double xpos, double ypos) {
  float map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  Camera* camera = render_data_.glfw_context_.camera;
  glm::vec3 axis = axis_;
  if (axis == glm::vec3(1.0f)) {
    auto camera_pos = camera->GetPosition();
    auto obj_pos = cur_translate_ * map_scale;
    axis = glm::normalize(obj_pos - camera_pos);
  }
  float delta_angle = UpdateAngle(xpos, ypos);
  glm::quat value = glm::angleAxis(delta_angle, axis);
  cur_rotate_ = glm::normalize(value * cur_rotate_);
}

void MouseTransform::ScaleSelected(double xpos, double ypos) {
  auto offset = GetWorldOffset(xpos, ypos);
  float distance_to_centre = glm::length(offset);
  float bias = 0.0001;
  float distance = std::max(bias, distance_to_centre);  // to avoid / 0.0f
  float value = glm::clamp(distance / zero_scale_length_, 0.0001f, 100.0f);
  // add 1.0f to avoid op with 0 scale (bad), affect only specified axis
  cur_scale_ =
      glm::clamp(prev_scale_ * (glm::vec3(1.0f) - axis_ + axis_ * value),
                 glm::vec3(-200.0f), glm::vec3(200.0f));
}

void MouseTransform::TranslateSelected(double xpos, double ypos) {
  auto value = GetWorldOffsetTranslate(xpos, ypos);
  cur_translate_ = glm::clamp(prev_translate_ + value, glm::vec3(-200.0f),
                              glm::vec3(200.0f));
}

void MouseTransform::TranslateSelectedMapPoints(
    std::vector<MapPoint>& map_points, double xpos, double ypos) {
  auto value = GetWorldOffsetTranslate(xpos, ypos);
  cur_translate_ = glm::clamp(glm::vec3(value.x, -value.y, value.z),
                              glm::vec3(-200.0f), glm::vec3(200.0f));

  float min_x = 32.0f;
  float min_y = 32.0f;
  float max_x = -32.0f;
  float max_y = -32.0f;
  for (auto& p : map_points) {
    if (!p.selected) {
      continue;
    }
    min_x = glm::min(min_x, p.position.x);
    max_x = glm::max(max_x, p.position.x);
    min_y = glm::min(min_y, p.position.y);
    max_y = glm::max(max_y, p.position.y);
  }
  float min_translate_x = -32.0f - min_x;
  float max_translate_x = 32.0f - max_x;
  float min_translate_y = -32.0f - min_y;
  float max_translate_y = 32.0f - max_y;

  // -1 to get delta value
  glm::vec3 translate = cur_translate_ - prev_translate_;
  translate.x = glm::clamp(translate.x, min_translate_x, max_translate_x);
  translate.y = glm::clamp(translate.y, min_translate_y, max_translate_y);

  for (auto& p : map_points) {
    if (!p.selected) {
      continue;
    }
    p.position += glm::vec2(translate.x, translate.y);
  }
  prev_translate_ += translate;
}

void MouseTransform::TranslateSelectedMapPointsBack(
    std::vector<MapPoint>& map_points) {
  glm::vec2 translate = glm::vec2(prev_translate_.x, prev_translate_.y);
  for (auto& p : map_points) {
    if (!p.selected) {
      continue;
    }
    p.position = p.position - translate;
  }
}

void MouseTransform::RotateSelectedMapPoints(
    const std::vector<MapPoint>& map_points, std::vector<glm::quat>& rotates,
    double xpos, double ypos) {
  RotateSelected(xpos, ypos);
  glm::quat delta = cur_rotate_ * glm::inverse(prev_rotate_);
  for (int i = 0; i < map_points.size(); ++i) {
    if (!map_points[i].selected) {
      continue;
    }
    rotates[i] = delta * rotates[i];
  }
  prev_rotate_ = cur_rotate_;
}

void MouseTransform::RotateSelectedMapPointsBack(
    const std::vector<MapPoint>& map_points, std::vector<glm::quat>& rotates) {
  for (int i = 0; i < map_points.size(); ++i) {
    if (!map_points[i].selected) {
      continue;
    }
    rotates[i] = glm::inverse(prev_rotate_) * rotates[i];
  }
}

void MouseTransform::ScaleSelectedMapPoints(
    const std::vector<MapPoint>& map_points, std::vector<glm::vec3>& scales,
    double xpos, double ypos) {
  ScaleSelected(xpos, ypos);
  glm::vec3 delta = cur_scale_ / (prev_scale_ * prev_scale_);
  for (int i = 0; i < map_points.size(); ++i) {
    if (!map_points[i].selected) {
      continue;
    }
    scales[i] = delta * scales[i];
  }
  prev_scale_ = cur_scale_ / prev_scale_;
}

void MouseTransform::ScaleSelectedMapPointsBack(
    const std::vector<MapPoint>& map_points, std::vector<glm::vec3>& scales) {
  for (int i = 0; i < map_points.size(); ++i) {
    if (!map_points[i].selected) {
      continue;
    }
    scales[i] = scales[i] / prev_scale_;
  }
}

void MouseTransform::TranslateSelectedVerticesUp(
    double xpos, double ypos, const Texture& hmap,
    const Texture& selection_mask) {
  auto value = GetWorldOffsetTranslate(xpos, ypos);

  auto value_y = value.y - prev_value_y_;
  prev_value_y_ = value.y;
  if (value_y == 0.0f) {
    return;
  }
  double prev_time_ = last_update_time_;
  double cur_time = glfwGetTime();
  auto time_diff = cur_time - prev_time_;
  if (time_diff < 0.5f) {
    return;
  }
  vertices_transform_shader_.Bind();
  hmap.BindImage(0, GL_READ_WRITE);
  selection_mask.BindImage(1, GL_READ_ONLY);
  glUniform1f(0, value_y);
  // always smoothing, so affect "in-between" vertices too
  float falloff = 10.0f;
  glUniform1f(1, falloff);
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void MouseTransform::Reset(glm::vec3 translate, glm::quat rotate,
                           glm::vec3 scale) {
  prev_value_y_ = 0;
  prev_translate_ = translate;
  prev_rotate_ = rotate;
  prev_scale_ = scale;
  cur_translate_ = translate;
  cur_rotate_ = rotate;
  cur_scale_ = scale;
}

glm::vec2 MouseTransform::GetWorldOffset(float xpos, float ypos) {
  float map_scale =
      render_data_.glfw_context_.tile_renderer->cur_tile_.map_scale;
  Camera* camera = render_data_.glfw_context_.camera;

  glm::vec4 object_centre_3d =
      glm::vec4(cur_translate_.x, cur_translate_.y, cur_translate_.z, 1.0f);
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  auto view = camera->GetViewMatrix(1.0f);
  auto projection = camera->GetProjMatrix();
  auto mvp = projection * view * model;

  glm::vec4 clipPos = mvp * object_centre_3d;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  glm::vec2 delta(
      (xpos / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
      ndc.y - ((gWindowHeight - ypos) / (gWindowHeight / 2.0f) - 1.0f));
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  return {delta.x, factor * delta.y};
}

glm::vec3 MouseTransform::GetWorldOffsetTranslate(float xpos, float ypos) {
  float xoffset = (xpos - lastX) / 1.0f;
  float yoffset = (lastY - ypos) / 1.0f;

  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);

  float cursor_diff = glm::length(glm::vec2{xoffset, yoffset});
  auto camera_dir_right = glfw_context->camera->GetDirectionRight();
  auto camera_dir_top = glfw_context->camera->GetDirectionUp();
  auto translate = camera_dir_right * xoffset * cursor_diff /
                       static_cast<float>(gWindowWidth) +
                   camera_dir_top * yoffset * cursor_diff /
                       static_cast<float>(gWindowHeight);

  return translate * axis_;
}
