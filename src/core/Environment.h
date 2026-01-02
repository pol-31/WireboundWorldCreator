#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_ENVIRONMENT_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_ENVIRONMENT_H_

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../common/Texture.h"

class Environment {
 public:
  Environment();

  ~Environment() { DeInit(); }

  void Update();

  void SetEnvironment(float wind_speed, float wind_angle,
                      glm::vec3 sun_direction, glm::vec3 sun_color);

  [[nodiscard]] float GetFps() const noexcept;

 private:
  struct UboData {
    glm::vec3 sun_color = glm::vec3(1.0f);
    float _pad1 = 0.0f;
    glm::vec3 sun_direction = glm::vec3(-1.0f);
    float wind_speed = 0.0f;
    glm::vec2 wind_velocity = glm::vec2(0.0f);
    float time = 0.0f;
    float delta_time = 0.01f;
  };

  void Init();

  void DeInit();

  GLuint ubo_id_ = 0;

  Texture wind_texture_;
  float wind_speed_ = 0.0f;
  float wind_angle_ = 0.0f;

  double time_last_ = 0.0;
  float time_delta_ = 0.0f;

  glm::vec3 sun_color_ = glm::vec3(1.0);
  glm::vec3 sun_direction_ = glm::vec3(-1.0);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_ENVIRONMENT_H_
