#include "UiGameMap.h"

UiGameMap::UiGameMap() : UiElement(data::UiId::kPlayerPhoneMap) {
  animation_.SetTranslation(glm::vec2(0.2f, 0.0f), glm::vec2(0.0f));
}

void UiGameMap::Update(float delta_time, UiLayer& ui_layer, glm::vec2 translate,
                       float rotate, glm::vec2 scale) {
  auto animated_transform = animation_.GetCurrentTransform();
  translate += position_ + animated_transform.translate;
  rotate += rotate_ + animated_transform.rotation;
  scale *= scale_ * animated_transform.scale;
  glm::mat3 transform(1.0f);
  transform *= glm::translate(transform, translate);
  transform *= glm::rotate(transform, rotate);
  transform *= glm::scale(transform, scale);
  auto quad = UiQuad::GenSprite(id_, transform, glm::vec4(1.0f));
  ui_layer.buffer_sprites.push_back(quad);
  ui_layer.scissors = GetScissorsRect();

  // mdl_manager->RenderOnMap(&sp_player_, &sp_enemy_, &sp_friend_,
  // &sp_neutral_, &sp_obstacle_);
  // sp_player_(data::UiId::kPlayerMapPlayer),
  // sp_enemy_(data::UiId::kPlayerMapEnemy),
  // sp_friend_(data::UiId::kPlayerMapFriend),
  // sp_neutral_(data::UiId::kPlayerMapNeutral),
  // sp_obstacle_(data::UiId::kPlayerMapObstacle),
}

void UiGameMap::MoveMouse(glm::vec2 mouse_pos, unsigned int hovered_id) {
  auto hovered_element = FindByID(hovered_id);
  if (hovered_ && !hovered_element) {
    hovered_ = false;
    animation_.RunForward(false);
  } else if (!hovered_ && hovered_element) {
    hovered_ = true;
    animation_.RunForward(true);
    animation_.StartAnimation();
  }
}

UiScissorRect UiGameMap::GetScissorsRect() {
  UiScissorRect scissors;
  scissors.width = 2000;
  scissors.height = 2000;
  return scissors;

  // float centre_height =
  //     (background_.GetTopBorder() + background_.GetBottomBorder()) / 2.0f +
  //     0.025f;
  // float length_y = background_.GetHeight() / 2.3f;
  // float y_ndc = centre_height - length_y / 2;
  // int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  // int height_py = int(length_y * 0.5f * gWindowHeight);
  //
  // float centre_width =
  //     (background_.GetRightBorder() + background_.GetLeftBorder()) / 2.0f;
  // float length_x = background_.GetWidth() * 0.84f;
  // float x_ndc = centre_width - length_x / 2;
  // int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
  // int width_px = int(length_x * 0.5f * gWindowWidth);
}
