#include "Environment.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

Environment::Environment()
    : sun_color_(1.0, 1.0, 1.0), sun_direction_(-0.4, -0.2, 0.0) {
  Init();
}

void Environment::Init() {
  glGenBuffers(1, &ubo_id_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UboData), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_id_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  Update();
}

void Environment::DeInit() { glDeleteBuffers(1, &ubo_id_); }

void Environment::Update() {
  float time_info[2];
  time_info[0] = static_cast<float>(time_last_);
  time_info[1] = time_delta_;
  double time_cur = glfwGetTime();
  time_delta_ = static_cast<float>(time_cur - time_last_);
  time_last_ = time_cur;

  glm::vec2 wind_velocity_ = {glm::cos(wind_angle_) * wind_speed_,
                              glm::sin(wind_angle_) * wind_speed_};

  UboData ubo_data;
  ubo_data.sun_color = sun_color_;
  ubo_data.sun_direction = sun_direction_;
  ubo_data.wind_speed = wind_speed_;
  ubo_data.wind_velocity = wind_velocity_;
  ubo_data.time = time_last_;
  ubo_data.delta_time = time_delta_;
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UboData), &ubo_data);
  ;
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Environment::SetEnvironment(float wind_angle, float wind_speed,
                                 glm::vec3 sun_direction, glm::vec3 sun_color) {
  wind_angle_ = wind_angle;
  wind_speed_ = wind_speed;
  sun_direction_ = sun_direction * 4.0f - glm::vec3(2);
  sun_color_ = sun_color;
}

float Environment::GetFps() const noexcept {
  float epsilon = 1e-6f;
  return 1.0f / (time_delta_ + epsilon);
}
