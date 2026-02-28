#include "UiSettings.h"

// UiSettings::UiSettings()
//     : UiElement(data::UiId::kSettingsDesk),
//       resolution_label_(std::move(resolution_label)),
//       resolution_left_(std::move(resolution_left)),
//       resolution_right_(std::move(resolution_right)),
//       resolution_(std::move(resolution)),
//       toggle_fullscreen_(std::move(toggle_fullscreen)),
//       sensitivity_(std::move(sensitivity)),
//       sound_(std::move(sound)),
//       toggle_sound_(std::move(toggle_sound)),
//       music_(std::move(music)),
//       toggle_music_(std::move(toggle_music)),
//       tip_info_label_(std::move(tip_info_label)),
//       tip_info_(std::move(tip_info)),
//       toggle_tip_info_(std::move(toggle_tip_info)) {
//   animation_.SetTranslation(glm::vec2(0.0f), glm::vec2(0.2f, 0.0f));
// }

// ui_settings_({data::VboIdMain::kSettingsDesk}, 2.0f,
//                    {{data::VboIdMain::kSettingsDeskPinBack, []() {}},
//                     {data::VboIdMain::kSettingsDeskPinPoint}},
//                    render_data_, windows_,
//                    LocalTransform{glm::vec2{0.0f}, 1.0f, 0.0f},
//                    LocalTransform{glm::vec2{0.2f, 0.0f}, 1.0f, 0.0f},
//                    {data::VboIdMain::kSettingsResolutionLabel},
//                    {data::VboIdMain::kSettingsResolutionLeft},
//                    {data::VboIdMain::kSettingsResolutionRight},
//                    {data::VboIdMain::kSettingsResolution},
//                    {{data::VboIdMain::kSettingsFullScreenOff},
//                     {data::VboIdMain::kSettingsFullScreenOn1},
//                     {data::VboIdMain::kSettingsFullScreenOn2},
//                     {data::VboIdMain::kSettingsFullScreenOn3}},
//                    UiSliderH2{
//                        {data::VboIdMain::kSettingsSensitivityArea},
//                        {data::VboIdMain::kSettingsSensitivityHandle},
//                    },
//                    UiSliderH2{
//                        {data::VboIdMain::kSettingsSoundArea},
//                        {data::VboIdMain::kSettingsSoundHandle},
//                    },
//                    {{data::VboIdMain::kSettingsSoundOff},
//                     {data::VboIdMain::kSettingsSoundOn1},
//                     {data::VboIdMain::kSettingsSoundOn2},
//                     {data::VboIdMain::kSettingsSoundOn3}},
//                    UiSliderH2{
//                        {data::VboIdMain::kSettingsMusicArea},
//                        {data::VboIdMain::kSettingsMusicHandle},
//                    },
//                    {{data::VboIdMain::kSettingsMusicOff},
//                     {data::VboIdMain::kSettingsMusicOn1},
//                     {data::VboIdMain::kSettingsMusicOn2},
//                     {data::VboIdMain::kSettingsMusicOn3}},
//                    {data::VboIdMain::kSettingsTipInfoLabel},
//                    {data::VboIdMain::kSettingsTipInfo},
//                    {{data::VboIdMain::kSettingsTipInfoOff},
//                     {data::VboIdMain::kSettingsTipInfoOn1},
// {data::VboIdMain::kSettingsTipInfoOn2},
// {data::VboIdMain::kSettingsTipInfoOn3}}),

// bool UiSettings::Render() {
//   auto mouse_pos = render_data_.glfw_context_.cursor_pos_tex_norm_;
//   resolution_left_.Render();
//   resolution_right_.Render();
//   resolution_.Render();
//   toggle_fullscreen_.Render();
//   sensitivity_.Render(mouse_pos);
//   sound_.Render(mouse_pos);
//   toggle_sound_.Render();
//   music_.Render(mouse_pos);
//   toggle_music_.Render();
//   tip_info_.Render();
//   toggle_tip_info_.Render();
//   render_data_.glfw_context_.text_renderer->RenderText(
//       resolution_label_, "resolution", 0.05f, glm::vec2{0.0f});
//   render_data_.glfw_context_.text_renderer->RenderText(
//       tip_info_label_, "show tips", 0.05f, glm::vec2{0.0f});
// }