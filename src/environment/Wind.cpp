#include "Wind.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../common/OpenGlUtility.h"

Wind::Wind(const Paths& paths, GLuint ubo_id)
    : texture_{}/*(paths.perlin_noise)*/,
      velocity_{},
      ubo_id_(ubo_id) {
  Init();
}

void Wind::Init() {
  Update(4.8f, 20.0f);
}

float Wind::GetSpeed() const {
  return glm::length(velocity_);
}

float Wind::GetDirection() const {
  return std::atan2(velocity_.y, velocity_.x);
}

void Wind::Update(float direction, float speed) {
  velocity_.x = glm::cos(direction) * speed;
  velocity_.y = glm::sin(direction) * speed;
  /// after time (2 floats) TODO: should we somehow declare it i Details.h?
  utility::UpdateUbo(ubo_id_, 2 * sizeof(float),
                     sizeof(glm::vec2), glm::value_ptr(velocity_));
}
