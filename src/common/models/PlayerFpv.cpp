#include "PlayerFpv.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../core/TileRenderer.h"
#include "../../io/Camera.h"
#include "ModelLoader.h"

PlayerFpv::PlayerFpv(UiSharedResources& ui_shared_resources)
    : PlayerBase(ui_shared_resources) {
  id_ = gEntityIdManager.PlayerFpvId;
}

void PlayerFpv::Render(UiSharedResources& ui_shared_resources) {
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_.Bind();
  model_data_->BindTextures();
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

void PlayerFpv::ProcessMovement(int key, int action) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_W)
      SetMoveForward(true);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(true);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(true);
    else if (key == GLFW_KEY_D)
      SetMoveRight(true);
    else if (key == GLFW_KEY_Q)
      SetRotateCcw(true);
    else if (key == GLFW_KEY_E)
      SetRotateCw(true);
    else if (key == GLFW_KEY_SPACE)
      SetThrustUp(true);
    else if (key == GLFW_KEY_LEFT_CONTROL)
      SetThrustDown(true);
    else if (key == GLFW_KEY_X) {
      // disarm
      throttle_ = 0.0f;
      yaw_rate_ = 0.0f;
      pitch_rate_ = 0.0f;
      roll_rate_ = 0.0f;
    }
  } else if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W)
      SetMoveForward(false);
    else if (key == GLFW_KEY_S)
      SetMoveBackward(false);
    else if (key == GLFW_KEY_A)
      SetMoveLeft(false);
    else if (key == GLFW_KEY_D)
      SetMoveRight(false);
    else if (key == GLFW_KEY_Q)
      SetRotateCcw(false);
    else if (key == GLFW_KEY_E)
      SetRotateCw(false);
    else if (key == GLFW_KEY_SPACE)
      SetThrustUp(false);
    else if (key == GLFW_KEY_LEFT_CONTROL)
      SetThrustDown(false);
  }
}

void PlayerFpv::ResetRotation() {
  throttle_ = 0.0f;
  yaw_rate_ = 0.0f;
  pitch_rate_ = 0.0f;
  roll_rate_ = 0.0f;
  angular_velocity = glm::vec3(0.0f);
  euler_angles = glm::vec3(0.0f);
}

void PlayerFpv::Update(UiSharedResources& ui_shared_resources) {
  if (state_ == State::kOnPlayer) {
    return;
  }
  const float stiffness = 1000.0f;  // ground pushback strength
  const float damping = 0.98f;      // stops infinite spin
  const float inertia = 0.01f;
  const float linear_drag = 0.98f;
  const float g = 9.81f;
  const float mass = 2.0f;
  float angle_speed = glm::radians(5.0f);
  float dt = gDeltaTime;

  throttle_ += ((thrust_up_ ? +1.f : 0.f) - (thrust_down_ ? 1.f : 0.f)) * dt;
  yaw_rate_ +=
      ((rotate_ccw_ ? 1.f : 0.f) - (rotate_cw_ ? 1.f : 0.f)) * dt / 2.0f;
  pitch_rate_ +=
      ((move_forward_ ? 1.f : 0.f) - (move_backward_ ? 1.f : 0.f)) * dt;
  roll_rate_ += ((move_right_ ? 1.f : 0.f) - (move_left_ ? 1.f : 0.f)) * dt;
  throttle_ = glm::clamp(throttle_, 0.f, 1.f);
  yaw_rate_ = glm::clamp(yaw_rate_, -1.f, 1.f);
  pitch_rate_ = glm::clamp(pitch_rate_, -1.f, 1.f);
  roll_rate_ = glm::clamp(roll_rate_, -1.f, 1.f);

  float max_thrust = mass * g * 2.5f;  // allow 2.5g lift
  float thrust_force = throttle_ * max_thrust;
  glm::vec3 local_up = rotation_ * glm::vec3(0, 1, 0);
  float gravity = -g * mass * (position_.y / 2.0f + 1.0f);

  auto total_force = glm::vec3(0);
  auto total_torque = glm::vec3(0.0f);

  float fpv_half_height =
      (model_data_->aabb.max.y - model_data_->aabb.min.y) / 2.0f;
  glm::vec3 fpv_bottom = position_ - glm::vec3(0.0f, fpv_half_height, 0.0f);
  float arm_w = model_data_->aabb.max.x - model_data_->aabb.min.x;
  float arm_l = model_data_->aabb.max.z - model_data_->aabb.min.z;
  glm::vec3 arms[] = {{-arm_w, 0, +arm_l},
                      {+arm_w, 0, +arm_l},
                      {-arm_w, 0, -arm_l},
                      {+arm_w, 0, -arm_l}};
  float arms_num = std::size(arms);
  for (auto arm : arms) {
    glm::vec3 motorWorld = rotation_ * arm + fpv_bottom;
    float ground = ui_shared_resources.glfw_context_.tile_renderer->cur_tile_
                       .GetPositionY32(motorWorld.x, motorWorld.z);
    float motor_thrust = thrust_force / arms_num;
    float height_difference = motorWorld.y - ground;
    if (height_difference < 0.0f) {
      float penetration = ground - motorWorld.y;
      glm::vec3 reaction = glm::vec3(0, penetration * stiffness, 0);
      total_force += reaction;
      glm::vec3 r = motorWorld - position_;
      total_torque += glm::cross(r, reaction);
    } else {
      total_force += local_up * motor_thrust;
      if (height_difference > 0.001f) {  // can't rotate if on ground
        total_force.y += gravity / arms_num;
        total_torque += glm::vec3{pitch_rate_, yaw_rate_, roll_rate_} *
                        angle_speed / arms_num;
      }
    }

    /*float friction_strength = 30.0f; // how “grippy” ground is
    // glm::vec3 ground_normal = GetNormal(motorWorld.x, motorWorld.z);
    auto ground_normal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 r = motorWorld - position_;
    glm::vec3 v_motor = velocity_ + glm::cross(angular_velocity, r);
    glm::vec3 lateral = v_motor - ground_normal * glm::dot(v_motor,
    ground_normal); if(glm::length(lateral) > 0.0001f) { glm::vec3 friction =
    -glm::normalize(lateral) * friction_strength; total_force += friction;
      total_torque += glm::cross(r, friction);
    }*/
  }
  angular_velocity += total_torque / inertia * dt;
  angular_velocity *= damping;
  if (glm::length(angular_velocity) < 0.01f) {
    angular_velocity = glm::vec3(0.0f);
  }
  euler_angles += angular_velocity * dt * 0.1f;

  if (glm::length(total_force) < 1.0f) {
    total_force = glm::vec3(0.0f);
  }
  velocity_ += total_force / mass * dt;
  // velocity_ *= glm::vec3(linear_drag, 1.0f, linear_drag);
  velocity_ *= linear_drag;
  position_ += velocity_ * dt * 0.1f;
  position_ = glm::clamp(position_, -32.0f, 32.0f);
  rotation_ = glm::quat(euler_angles);
}

// TODO: Arm() Disarm() state & methods

void PlayerFpv::RenderPicking(UiSharedResources& ui_shared_resources) {
  // TODO: render capsule model instead
  UpdateAnimation();
  ui_shared_resources.shader_animated_mdl_picking_.Bind();
  glUniform1ui(1, static_cast<uint32_t>(details::kIdOffsetObjects + id_));
  auto model = GenModelMat(ui_shared_resources, 0.1f);
  glUniformMatrix4fv(0, 1, false, glm::value_ptr(model));
  model_data_->RenderModelNodes();
  glBindVertexArray(0);
}

// should be called in Render(), it updates skin ubo
void PlayerFpv::UpdateAnimation() {
  bool started_over = animator_->UpdateUbo(animation_id_, animation_time_);
  if (started_over && !animation_looped_) {
    ResetState();
  }
}

void PlayerFpv::UpdatePositionY(UiSharedResources& ui_shared_resources) {}

void PlayerFpv::Stunned() {
  return;
  if (IsRelaxed()) {
    state_ = State::kStunned;
    animation_time_ = 0.0f;
    animation_id_ = FpvAnimation::kStunned;
    animation_looped_ = false;
  }
}

void PlayerFpv::ApplyGravity(UiSharedResources& ui_shared_resources) {
  // removed
}

void PlayerFpv::ResetState() {
  if (state_ != State::kOnPlayer) {
    state_ = State::kIdle;
  }
  animation_id_ = FpvAnimation::kIdle;
  animation_time_ = 0.0f;
  animation_looped_ = true;
}

void PlayerFpv::SetAnimation(FpvAnimation type, float time, bool looped) {
  animation_id_ = type;
  animation_time_ = time;
  animation_looped_ = looped;
}

void PlayerFpv::SwitchToFpv() {
  state_ = State::kIdle;
  animation_time_ = 0.0f;
  animation_id_ = FpvAnimation::kNone;
  animation_looped_ = true;
}

void PlayerFpv::SwitchToHuman() {
  state_ = State::kOnPlayer;
  animation_time_ = 0.0f;
  animation_id_ = FpvAnimation::kIdle;
  animation_looped_ = true;
}
