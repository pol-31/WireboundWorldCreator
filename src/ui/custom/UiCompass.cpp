#include "UiCompass.h"

#include <glm/gtc/constants.hpp>

#include "../../io/Camera.h"

UiCompass::UiCompass(const Camera* camera)
    : UiElement(data::UiId::kCompass), camera_(camera) {}

struct WorldDirection {
  data::UiId id;
  glm::vec2 offset;
};

void UiCompass::Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                       float rotate, glm::vec2 scale) {
  float factor =
      static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
  glm::vec2 radius = glm::vec2(0.05f, 0.05f * factor);
  float yaw = glm::radians(camera_->GetYaw());
  translate += position_;
  scale *= scale_;
  glm::mat3 transform(1.0f);

  // North (yaw), South (yaw + 180°), East (yaw + 90°), West (yaw - 90°)
  WorldDirection directions[]{
      {data::UiId::kCompassNorth,
       {radius.x * glm::cos(yaw), radius.y * glm::sin(yaw)}},
      {data::UiId::kCompassSouth,
       {radius.x * glm::cos(yaw + glm::pi<float>()),
        radius.y * glm::sin(yaw + glm::pi<float>())}},
      {data::UiId::kCompassEast,
       {radius.x * glm::cos(yaw + glm::half_pi<float>()),
        radius.y * glm::sin(yaw + glm::half_pi<float>())}},
      {data::UiId::kCompassWest,
       {radius.x * glm::cos(yaw - glm::half_pi<float>()),
        radius.y * glm::sin(yaw - glm::half_pi<float>())}},
  };
  for (auto d : directions) {
    transform *= glm::translate(glm::mat3(1.0f), translate + d.offset);
    transform *= glm::rotate(transform, rotate);
    transform *= glm::scale(transform, scale);
    auto quad = UiQuad::GenSprite(d.id, transform, glm::vec4(1.0f));
    ui_layer.buffer_sprites.push_back(quad);
  }
  // now compass itself
  rotate += yaw;
  transform *= glm::translate(glm::mat3(1.0f), translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenSprite(id_, transform, glm::vec4(1.0f));
  ui_layer.buffer_sprites.push_back(quad);
}
