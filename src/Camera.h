#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera {
 public:
  Camera() {
    InitUbo();
    updateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
  };

  ~Camera() {
    DeInitUbo();
  }

  void UpdateViewMatrix() const {
    glm::mat4 view_mat = glm::lookAt(Position, Position + Front, Up);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
  void UpdateProjectionMatrix() const { // TODO: should be called from WindowSizeCallback
    glm::mat4 proj_mat = glm::perspective(
        glm::radians(45.0f), static_cast<float>(details::kWindowWidth)
                                 / static_cast<float>(details::kWindowHeight),
        0.01f, 1000.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(proj_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  [[nodiscard]] float GetSpeed() const {
    return MovementSpeed;
  }

  void MoveForward(float velocity) {
    Position += Front * velocity;
  }
  void MoveBackward(float velocity) {
    Position -= Front * velocity;
  }
  void MoveLeft(float velocity) {
    Position -= Right * velocity;
  }
  void MoveRight(float velocity) {
    Position += Right * velocity;
  }

  void SpeedUp() {
    MovementSpeed = speed_2_;
  }
  void SlowDown() {
    MovementSpeed = speed_1_;
  }

  void ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    /// constrain pitch
    if (Pitch > 89.0f) {
      Pitch = 89.0f;
    } else if (Pitch < -89.0f) {
      Pitch = -89.0f;
    }

    updateCameraVectors();
  }

 private:
  void InitUbo() {
    glGenBuffers(1, &ubo_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_);
  }

  void DeInitUbo() {
    glDeleteBuffers(GL_UNIFORM_BUFFER, &ubo_);
  }

  void updateCameraVectors() {
    glm::vec3 front;
    front.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
    front.y = glm::sin(glm::radians(Pitch));
    front.z = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }

  glm::vec3 Position{0.0f, 0.0f, 0.0f};
  glm::vec3 Front{0.0f, 0.0f, -1.0f};
  glm::vec3 Up{0.0f, 1.0f, 0.0f};
  glm::vec3 Right{0.0f};
  glm::vec3 WorldUp{Up};

  float Yaw{-90.0f};
  float Pitch{0.0f};

  float speed_1_{1.0f};
  float speed_2_{10.0f};
  float MovementSpeed{speed_1_};
  float MouseSensitivity{0.1f};

  GLuint ubo_{0};
};
#endif
