#include "MouseTransform.h"

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "OpenGlUtility.h"

MouseTransform::MouseTransform(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      vertices_transform_shader_(
          "../shaders/generate_shaders/VerticesTransform.comp") {}

void MouseTransform::UpdateStartAngle() {
  auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_;
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

void MouseTransform::InitTransform(
   glm::vec3 translate, glm::quat rotate, glm::vec3 scale) {
  axis_ = glm::vec3(1.0f);
  Reset(translate, rotate, scale);
  UpdateStartAngle();
  auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_;
  auto offset = GetWorldOffset(mouse_pos.x, mouse_pos.y);
  zero_scale_length_ = glm::length(offset);
}

void MouseTransform::SetAxis(int key, bool mod_shift) {
  if (mod_shift & GLFW_MOD_SHIFT) {
    if (key == GLFW_KEY_X) {
      axis_= {0.0f, 1.0f, 1.0f};
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
  float map_scale = ui_shared_resources_.glfw_context_.tile_renderer
    ->cur_tile_.map_scale;
  Camera* camera = ui_shared_resources_.glfw_context_.camera;
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
  cur_scale_ = glm::clamp(prev_scale_ * (glm::vec3(1.0f) - axis_ + axis_ * value),
                       glm::vec3(-200.0f), glm::vec3(200.0f));
}

void MouseTransform::TranslateSelected(double xpos, double ypos) {
  auto value = GetWorldOffsetTranslate(xpos, ypos);
  cur_translate_ =
      glm::clamp(prev_translate_ + value, glm::vec3(-200.0f),
        glm::vec3(200.0f));
}

void MouseTransform::TranslateSelectedMapPoints(
  std::vector<MapPoint>& map_points, double xpos, double ypos) {
  auto value = GetWorldOffsetTranslate(xpos, ypos);
  cur_translate_ =
      glm::clamp(value, glm::vec3(-200.0f),
        glm::vec3(200.0f));

  GLuint min_x = 512;
  GLuint min_y = 512;
  GLuint max_x = 512;
  GLuint max_y = 512;
  for (auto& p : map_points) {
    if (!p.selected) {
      continue;
    }
    GLuint x = p.position >> 10;
    GLuint y = p.position & 1023;
    min_x = glm::min(min_x, x);
    max_x = glm::max(max_x, x);
    min_y = glm::min(min_y, y);
    max_y = glm::max(max_y, y);
  }
  float min_translate_x = -static_cast<float>(min_x) / 16.0f;
  float max_translate_x = static_cast<float>(1024 - max_x - 1) / 16.0f;
  float min_translate_y = -static_cast<float>(min_y - 1) / 16.0f;
  float max_translate_y = static_cast<float>(1024 - max_y - 1) / 16.0f;

  // -1 to get delta value
  glm::vec3 translate = cur_translate_ - prev_translate_;
  translate.x = glm::clamp(translate.x, min_translate_x, max_translate_x);
  translate.y = glm::clamp(translate.y, min_translate_y, max_translate_y);
  auto delta_x = static_cast<int>(translate.x * 16.0f);
  auto delta_y = static_cast<int>(translate.y * 16.0f);

  for (auto& p : map_points) {
    if (!p.selected) {
      continue;
    }
    p.position += delta_x * 1024 + delta_y;
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
    glm::vec2 obj_translate = {(p.position >> 10) / 16.0f - 32.0f,
                        (p.position & 1023) / 16.0f - 32.0f};
    obj_translate -= translate;
    auto fx = static_cast<int>(obj_translate.x * 16.0f + 512.0f);
    auto fy = static_cast<int>(obj_translate.y * 16.0f + 512.0f);
    p.position = fx * 1024 + fy;
  }
}

void MouseTransform::TranslateSelectedVerticesUp(
    double xpos, double ypos, const Texture& selection_mask, bool smooth) {
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
  utility::BindImageTexture(
    0, ui_shared_resources_.glfw_context_.tile_renderer
    ->cur_tile_.map_terrain_height, GL_READ_WRITE);
  utility::BindImageTexture(1, selection_mask, GL_READ_ONLY);
  glUniform1f(0, value_y);
  float falloff = 1.0f;
  if (smooth) {
    falloff = 10.0f;
  }
  glUniform1f(1, falloff);
  GLuint workGroupSizeX = (1024 + 15) / 16;
  GLuint workGroupSizeY = (1024 + 15) / 16;
  glDispatchCompute(workGroupSizeX, workGroupSizeY, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  utility::UnBindImageTexture(
    0, ui_shared_resources_.glfw_context_.tile_renderer
    ->cur_tile_.map_terrain_height, GL_READ_WRITE);
  utility::UnBindImageTexture(1, selection_mask, GL_READ_ONLY);
}

void MouseTransform::Reset(
    glm::vec3 translate, glm::quat rotate, glm::vec3 scale) {
  prev_value_y_ = 0;
  prev_translate_ = translate;
  prev_rotate_ = rotate;
  prev_scale_ = scale;
  cur_translate_ = translate;
  cur_rotate_ = rotate;
  cur_scale_ = scale;
}

glm::vec2 MouseTransform::GetWorldOffset(float xpos, float ypos) {
  float map_scale = ui_shared_resources_.glfw_context_.tile_renderer
    ->cur_tile_.map_scale;
  Camera* camera = ui_shared_resources_.glfw_context_.camera;

  glm::vec4 object_centre_3d
    = glm::vec4(cur_translate_.x, cur_translate_.y, cur_translate_.z, 1.0f);
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
