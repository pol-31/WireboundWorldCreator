#include "Time.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../common/OpenGlUtility.h"

Time::Time(GLuint ubo_id)
    : last_time_(0.0f),
      delta_time_(0.0f),
      ubo_id_(ubo_id) {
  Init();
}

void Time::Init() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  last_time_ = glfwGetTime();
  Update();
}

//TODO: need to update in the Render Loop
void Time::Update() {
  float time_info[2];
  time_info[0] = static_cast<float>(last_time_);
  time_info[1] = delta_time_;
  delta_time_ = static_cast<float>(glfwGetTime() - last_time_);
  last_time_ = glfwGetTime();
  utility::UpdateUbo(ubo_id_, 0, 2 * sizeof(float), time_info);
}

float Time::GetFps() const noexcept {
  float epsilon = 1e-6f;
  return 1.0f / (delta_time_ + epsilon);
}
