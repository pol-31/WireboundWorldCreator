#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../common/FixedSizeQueue.h"

class Camera {
 public:
  Camera() {
    InitUbo();
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
  };

  ~Camera() {
    DeInitUbo();
  }

  void Update();

  void UpdateViewMatrix() const;

  //TODO: should be called from WindowSizeCallback
  void UpdateProjectionMatrix() const;

  [[nodiscard]] float GetSpeed() const {
    return speed_;
  }

  void SetMoveForward();
  void SetMoveBackward();
  void SetMoveLeft();
  void SetMoveRight();

  void UnSetMoveForward();
  void UnSetMoveBackward();
  void UnSetMoveLeft();
  void UnSetMoveRight();

  void SpeedUp() {
    speed_ = accelerated_speed_;
  }
  void SlowDown() {
    speed_ = normal_speed_;
  }

  void ProcessMouseMovement(float xoffset, float yoffset);

 private:
  void InitUbo();
  void DeInitUbo();

  void UpdateCameraVectors();

  void MoveLeft();
  void MoveRight();
  void MoveForward();
  void MoveBackward();

  FixedSizeQueue<std::function<void()>, 4> movement_queue_;

  // at movement callback we push to queue and remember its indx
  struct MovementQueueIdx {
    int left{-1};
    int right{-1};
    int forward{-1};
    int backward{-1};
  };
  MovementQueueIdx movement_queue_idx_{};

  glm::vec3 position_{0.0f, 0.0f, 0.0f};
  glm::vec3 direction_front_{0.0f, 0.0f, -1.0f};
  glm::vec3 direction_up_{0.0f, 1.0f, 0.0f};
  glm::vec3 direction_right_{0.0f};
  glm::vec3 direction_world_up_{direction_up_};

  float yaw_{-90.0f};
  float pitch_{0.0f};

  float normal_speed_{1.0f};
  float accelerated_speed_{10.0f};
  float speed_{normal_speed_};
  float sensitivity_{0.1f};

  GLuint ubo_{0};
};
#endif
