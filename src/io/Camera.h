#ifndef WIREBOUNDWORLDCREATOR_SRC_IO_CAMERA_H_
#define WIREBOUNDWORLDCREATOR_SRC_IO_CAMERA_H_

#include <array>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../core/Frustum.h"

/// camera move:
/// left x-
/// right x+
/// forward y+
/// back y-

class Camera {
 public:
  Camera();

  ~Camera() { DeInitUbo(); }

  [[nodiscard]] glm::mat4 GetViewMatrix(float map_scale) const noexcept;

  [[nodiscard]] glm::mat4 GetProjMatrix() const noexcept;

  void Update();
  void UpdateMovement();

  void UpdateViewMatrix() const;

  void UpdateProjectionMatrix() const;

  void UpdateUboPos() const;

  void UpdateCameraVectors(float radius = 1.0f);

  // UiTerrainMode, other similar
  void MoveRotateViewOrigin(float xoffset, float yoffset);

  // UiPlayerMode (no pitch)
  void MoveRotateViewOriginDist(float xoffset);

  void MovePanView(float xoffset, float yoffset);

  [[nodiscard]] glm::vec3 GetDirectionRight() const noexcept {
    return direction_right_;
  }

  [[nodiscard]] glm::vec3 GetDirectionWorldUp() const noexcept {
    return direction_world_up_;
  }

  [[nodiscard]] glm::vec3 GetDirectionWorldFront() const noexcept {
    return direction_world_front_;
  }

  [[nodiscard]] glm::vec3 GetDirectionUp() const noexcept {
    return direction_up_;
  }

  [[nodiscard]] glm::vec3 GetDirectionFront() const noexcept {
    return direction_front_;
  }

  void SetYaw(float yaw) { yaw_ = yaw; }

  void SetPitch(float pitch) { pitch_ = pitch; }

  void SetPosition(glm::vec3 position) { position_ = position; }

  void SetOrigin(glm::vec3 origin) { origin_ = origin; }

  void SetOriginDist(float origin_dist) { origin_dist_ = origin_dist; }

  [[nodiscard]] float GetYaw() const noexcept { return yaw_; }

  [[nodiscard]] float GetPitch() const noexcept { return pitch_; }

  [[nodiscard]] glm::vec3 GetPosition() const noexcept { return position_; }

  [[nodiscard]] glm::vec3 GetOrigin() const noexcept { return origin_; }

  void Reset();  // for all modes except UiPlayerMode

  void HideCursor();

  void ShowCursor();

  void ProcessMovement(int key, int action);

  Frustum GetFrustum();

 protected:
  void Init();

  void InitUbo();

  void DeInitUbo();

  void SnapYaw();

  void SnapPitch();

  glm::vec3 position_;

  glm::vec3 direction_front_;
  glm::vec3 direction_up_;
  glm::vec3 direction_right_;
  glm::vec3 direction_world_up_;
  glm::vec3 direction_world_front_;

  float origin_dist_ = 10.0f;  // dist to origin
  glm::vec3 origin_ = glm::vec3(0.0f);

  float fovy_ = glm::radians(45.0f);

  float yaw_;
  float pitch_;

  float speed_;

  GLuint ubo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IO_CAMERA_H_
