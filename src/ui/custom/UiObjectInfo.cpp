#include "UiObjectInfo.h"
//
// UiObjectInfo::UiObjectInfo(UiSprite&& sprite, float size_scale, UiToggle2&&
// pin,
//                            UiRenderData& render_data,
//                            WindowQueue& window_queue,
//
//                            UiSprite&& sp_enemy, UiSprite&& sp_friend,
//                            UiSprite&& sp_neutral, UiSprite&& sp_obstacle,
//                            UiText&& txt_name, UiText&& txt_characteristic,
//                            UiText&& txt_value)
//     : UiWindowAppear(std::move(sprite), size_scale, std::move(pin),
//                      render_data, window_queue),
//       sp_enemy_(std::move(sp_enemy)),
//       sp_friend_(std::move(sp_friend)),
//       sp_neutral_(std::move(sp_neutral)),
//       sp_obstacle_(std::move(sp_obstacle)),
//       txt_name_(std::move(txt_name)),
//       txt_characteristic_(std::move(txt_characteristic)),
//       txt_value_(std::move(txt_value)),
//       ui_event_handler_({&pin_}),
//       hierarchy_(&background_,
//                  {&pin_, &sp_enemy_, &sp_friend_, &sp_neutral_,
//                  &sp_obstacle_,
//                   &txt_name_, &txt_characteristic_, &txt_value_}) {
//   speed_ = 2.0f;
// }
//
// void UiObjectInfo::Show(bool enemy_selected, bool friend_selected,
//                         bool neutral_selected, bool obstacle_selected,
//                         int selected_num) {
//   enemy_selected_ = enemy_selected;
//   friend_selected_ = friend_selected;
//   neutral_selected_ = neutral_selected;
//   obstacle_selected_ = obstacle_selected;
//   txt_name_.SetText("Pavlushas (a lot)");
//   selected_num_ = selected_num;
//   render_params_.clear();
//   render_params_.emplace_back("amount", selected_num);
//   Show();
// }
//
// void UiObjectInfo::Show(bool enemy_selected, bool friend_selected,
//                         bool neutral_selected, bool obstacle_selected,
//                         const ModelData* mdl_data) {
//   enemy_selected_ = enemy_selected;
//   friend_selected_ = friend_selected;
//   neutral_selected_ = neutral_selected;
//   obstacle_selected_ = obstacle_selected;
//   txt_name_.SetText("Pavlusha");
//   selected_num_ = 1;
//   mdl_data_ = mdl_data;
//   render_params_.clear();
//   render_params_.emplace_back("hp", mdl_data_->hp);
//   render_params_.emplace_back("speed", mdl_data_->speed);
//   render_params_.emplace_back("attack", mdl_data_->attack);
//   render_params_.emplace_back("attack speed", mdl_data_->attack_speed);
//   Show();
// }
