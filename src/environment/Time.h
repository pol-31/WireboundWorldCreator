#ifndef WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_TIME_H_
#define WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_TIME_H_

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE

class Time {
 public:
  explicit Time(GLuint ubo_id);

  void Update();

  [[nodiscard]] float GetLastTime() const noexcept {
    return last_time_;
  }

  [[nodiscard]] float GetDeltaTime() const noexcept {
    return delta_time_;
  }

  [[nodiscard]] float GetFps() const noexcept;

 private:
  void Init();

  double last_time_;
  float delta_time_;
  GLuint ubo_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_ENVIRONMENT_TIME_H_
