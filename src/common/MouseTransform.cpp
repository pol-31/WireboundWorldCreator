#include "MouseTransform.h"

#include <iostream>

#include "../core/TileRenderer.h"
#include "../io/Camera.h"
#include "../io/Window.h"
#include "OpenGlUtility.h"

MouseTransform::MouseTransform(UiSharedResources& ui_shared_resources)
    : ui_shared_resources_(ui_shared_resources),
      vertices_transform_shader_(
          "../shaders/generate_shaders/VerticesTransform.comp") {}

void MouseTransform::UpdateStartAngle() {
  auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_;
  auto pos_3d = GetInstanceTransform();
  glm::vec4 object_centre_3d = glm::vec4(pos_3d.x, pos_3d.y, pos_3d.z, 1.0f);
  auto& glfw_context = ui_shared_resources_.gltf_context_;
  auto map_scale = glfw_context.tile_renderer->cur_tile_.map_scale;
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  auto view = glfw_context.camera->GetViewMatrix(1.0f);
  auto projection = glfw_context.camera->GetProjMatrix();
  auto mvp = projection * view * model;

  glm::vec4 clipPos = mvp * object_centre_3d;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  glm::vec2 delta(
      (mouse_pos.x / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
      ndc.y - ((gWindowHeight - mouse_pos.y) / (gWindowHeight / 2.0f) - 1.0f));
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  zero_angle_ = std::atan2(delta.y, delta.x * factor);
  last_angle_ = 0.0f;
}

float MouseTransform::UpdateAngle(float xpos, float ypos) {
  auto pos_3d = GetInstanceTransform();
  glm::vec4 object_centre_3d = glm::vec4(pos_3d.x, pos_3d.y, pos_3d.z, 1.0f);
  auto& glfw_context = ui_shared_resources_.gltf_context_;
  auto map_scale = glfw_context.tile_renderer->cur_tile_.map_scale;
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  auto view = glfw_context.camera->GetViewMatrix(1.0f);
  auto projection = glfw_context.camera->GetProjMatrix();
  auto mvp = projection * view * model;

  glm::vec4 clipPos = mvp * object_centre_3d;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  glm::vec2 delta(
      (xpos / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
      ndc.y - ((gWindowHeight - ypos) / (gWindowHeight / 2.0f) - 1.0f));
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  float angle = std::atan2(delta.y, delta.x * factor) - zero_angle_;
  float deltaAngle = angle - last_angle_;
  last_angle_ = angle;

  if (deltaAngle > glm::pi<float>()) deltaAngle -= glm::two_pi<float>();
  if (deltaAngle < -glm::pi<float>()) deltaAngle += glm::two_pi<float>();
  return deltaAngle;
}

void MouseTransform::InitTransform(glm::vec3* translate, glm::quat* rotate,
                                   glm::vec3* scale) {
  axis_ = glm::vec3(1.0f);

  if (translate) {
    temp_translate_ = *translate;
    translate_ = translate;
  }

  if (rotate) {
    UpdateStartAngle();
    temp_rotate_ = *rotate;
    rotate_ = rotate;
  }

  if (scale) {
    auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_;
    auto pos_3d = GetInstanceTransform();
    glm::vec4 object_centre_3d = glm::vec4(pos_3d.x, pos_3d.y, pos_3d.z, 1.0f);
    auto& glfw_context = ui_shared_resources_.gltf_context_;
    auto map_scale = glfw_context.tile_renderer->cur_tile_.map_scale;
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
    auto view = glfw_context.camera->GetViewMatrix(1.0f);
    auto projection = glfw_context.camera->GetProjMatrix();
    auto mvp = projection * view * model;

    glm::vec4 clipPos = mvp * object_centre_3d;
    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

    glm::vec2 delta(
        (mouse_pos.x / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
        ndc.y -
            ((gWindowHeight - mouse_pos.y) / (gWindowHeight / 2.0f) - 1.0f));
    float factor =
        static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
    glm::vec2 offset(delta.x, factor * delta.y);

    zero_scale_length_ = glm::length(offset);
    temp_scale_ = *scale;
    scale_ = scale;
  }
}

void MouseTransform::TranslateSelected(double xpos, double ypos) {
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

  glm::vec3 value = translate * GetAxis();

  auto prev_value = temp_translate_;
  *translate_ =
      glm::clamp(prev_value + value, glm::vec3(-200.0f), glm::vec3(200.0f));
}

void MouseTransform::RotateSelected(double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);

  float map_scale = glfw_context->tile_renderer->cur_tile_.map_scale;

  glm::vec3 axis = GetAxis();
  if (axis == glm::vec3(1.0f)) {
    auto camera_pos = glfw_context->camera->GetPosition();
    auto obj_pos = GetInstanceTransform() * map_scale;
    axis = glm::normalize(obj_pos - camera_pos);
  }

  float delta_angle = UpdateAngle(xpos, ypos);
  glm::quat value = glm::angleAxis(delta_angle, axis);
  *rotate_ = glm::normalize(value * *rotate_);
}

void MouseTransform::ScaleSelected(double xpos, double ypos) {
  void* global_data_void_ptr = glfwGetWindowUserPointer(gWindow);
  auto glfw_context = reinterpret_cast<GlfwContext*>(global_data_void_ptr);

  auto map_scale = glfw_context->tile_renderer->cur_tile_.map_scale;
  auto pos_3d = GetInstanceTransform();
  glm::vec4 object_centre_3d = glm::vec4(pos_3d.x, pos_3d.y, pos_3d.z, 1.0f);
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(map_scale));
  auto view = glfw_context->camera->GetViewMatrix(1.0f);
  auto projection = glfw_context->camera->GetProjMatrix();
  auto mvp = projection * view * model;
  glm::vec4 clipPos = mvp * object_centre_3d;
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

  glm::vec2 delta(
      (xpos / (gWindowWidth / 2.0f) - 1.0f) - ndc.x,
      ndc.y - ((gWindowHeight - ypos) / (gWindowHeight / 2.0f) - 1.0f));
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  glm::vec2 offset(delta.x, factor * delta.y);
  std::cout << offset.x << ' ' << offset.y << std::endl;

  float distance_to_centre = glm::length(offset);

  float bias = 0.0001;
  float distance = std::max(bias, distance_to_centre);  // to avoid / 0.0f
  float value = glm::clamp(distance / zero_scale_length_, 0.0001f, 100.0f);
  auto prev_value = temp_scale_;
  // add 1.0f to avoid op with 0 scale (bad), affect only specified axis
  *scale_ = glm::clamp(prev_value * (glm::vec3(1.0f) - axis_ + axis_ * value),
                       glm::vec3(-200.0f), glm::vec3(200.0f));
}

void MouseTransform::CancelTransform() {
  if (translate_) {
    *translate_ = temp_translate_;
  }
  if (rotate_) {
    *rotate_ = temp_rotate_;
  }
  if (scale_) {
    *scale_ = temp_scale_;
  }
  prev_value_y_ = 0;
  Reset();
}

void MouseTransform::ApplyTransform() {
  prev_value_y_ = 0;
  Reset();
}

glm::vec3 MouseTransform::GetInstanceTransform() { return *translate_; }

void MouseTransform::Reset() {
  temp_translate_ = glm::vec3(0.0f);
  temp_rotate_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  temp_scale_ = glm::vec3(0.0f);
}

void MouseTransform::TranslateSelectedVerticesUp(double xpos, double ypos,
                                                 const Texture& selection_mask,
                                                 bool smooth) {
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

  glm::vec3 value = translate * GetAxis();

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
  utility::BindImageTexture(0,
                            ui_shared_resources_.gltf_context_.tile_renderer
                                ->cur_tile_.map_terrain_height,
                            GL_READ_WRITE);
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
  utility::UnBindImageTexture(0,
                              ui_shared_resources_.gltf_context_.tile_renderer
                                  ->cur_tile_.map_terrain_height,
                              GL_READ_WRITE);
  utility::UnBindImageTexture(1, selection_mask, GL_READ_ONLY);
}
