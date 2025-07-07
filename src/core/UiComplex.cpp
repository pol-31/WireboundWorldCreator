#include "UiComplex.h"

#include "../common/UiDebugger.h"
#include "../common/ShadersBinding.h"
#include "../common/TextRenderer.h"

UiLoading::UiLoading(
    UiStaticSprite&& sprite0,
    UiStaticSprite&& sprite10,
    UiStaticSprite&& sprite20,
    UiStaticSprite&& sprite30,
    UiStaticSprite&& sprite40,
    UiStaticSprite&& sprite50,
    UiStaticSprite&& sprite60,
    UiStaticSprite&& sprite70,
    UiStaticSprite&& sprite80,
    UiStaticSprite&& sprite90,
    UiStaticSprite&& sprite100)
    : UiBase(sprite0.GetId(), {}),
      sprites_{
          {std::move(sprite0), std::move(sprite10), std::move(sprite20),
           std::move(sprite30), std::move(sprite40), std::move(sprite50),
           std::move(sprite60), std::move(sprite70), std::move(sprite80),
           std::move(sprite90), std::move(sprite100)}} {
  for (const auto& sprite : sprites_) {
    gUiComponents[sprite.GetId() - details::kIdOffsetUi].parent_id_
        = sprites_[0].GetId();
  }
  // reset father to default
  gUiComponents[sprites_[0].GetId() - details::kIdOffsetUi].parent_id_
      = 0;
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : UiBase(std::move(other)),
      sprites_(std::move(other.sprites_)) {
  gUiComponents[sprites_[0].GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const {
  sprites_[0].RenderPicking();
}

void UiLoading::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  for (auto& sprite : sprites_) {
    sprite.UpdateTransform();
  }
}

void UiLoading::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}
/*
UiColorPalette::UiColorPalette(
    UiDynamicSprite&& palette,
    UiDynamicSprite&& cursor_color,
    UiDynamicSprite&& cursor_brightness)
    : UiBase(palette_.GetId(), {}),
      palette_(std::move(palette)),
      cursor_color_(std::move(cursor_color)),
      cursor_brightness_(std::move(cursor_brightness)) {
  gUiComponents[cursor_color_.GetId() - details::kIdOffsetUi].parent_id_
      = palette_[0].GetId();
  gUiComponents[cursor_brightness.GetId() - details::kIdOffsetUi].parent_id_
      = palette_[0].GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
}

UiColorPalette::UiColorPalette(UiColorPalette&& other) noexcept
    : UiBase(std::move(other)),
      palette_(std::move(other.palette_)),
      cursor_color_(std::move(other.cursor_color_)),
      cursor_brightness_(std::move(other.cursor_brightness_)) {
  gUiComponents[palette_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

void UiColorPalette::Render(glm::vec2 mouse_pos) {
  palette_.Render();
  cursor_color_.Render();
  cursor_brightness_.Render();
}

void UiColorPalette::RenderPicking() const {
  palette_.RenderPicking();
}

[[nodiscard]] data::TextId UiColorPalette::Hover(std::uint32_t id) {
  //
}

void UiColorPalette::Press() {
  pressed_ = true;
}

void UiColorPalette::Release() {
  pressed_ = false;
}

bool UiColorPalette::Scroll(GLuint id, float yoffset) {
  //
}

void UiColorPalette::SetParentTransform(LocalTransform transform) {
  palette_.SetParentTransform(transform);
  cursor_color_.SetParentTransform(transform);
  cursor_brightness_.SetParentTransform(transform);
}

glm::vec3 UiColorPalette::GetRbgColor() const {
  float r = 0, g = 0, b = 0;
  float H = hue_ * 6.0f;
  int i = static_cast<int>(H);
  float f = H - i;

  float p = brightness_ * (1.0f - saturation_);
  float q = brightness_ * (1.0f - saturation_ * f);
  float t = brightness_ * (1.0f - saturation_ * (1.0f - f));

  switch (i % 6) {
    case 0: r = brightness_; g = t; b = p; break;
    case 1: r = q; g = brightness_; b = p; break;
    case 2: r = p; g = brightness_; b = t; break;
    case 3: r = p; g = q; b = brightness_; break;
    case 4: r = t; g = p; b = brightness_; break;
    case 5: r = brightness_; g = p; b = q; break;
  }

  return {r, g, b};
}

void UiColorPalette::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  palette_.UpdateTransform();
  cursor_color_.UpdateTransform();
  cursor_brightness_.UpdateTransform();
}

void UiColorPalette::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}*/

UiWindowBase::UiWindowBase(
    UiDynamicSprite&& sprite,
    float size_scale, UiSharedResources& ui_shared_resources)
    : UiBase(sprite.GetId(), {}),
      sprite_(std::move(sprite)),
      ui_shared_resources_(ui_shared_resources),
      speed_(4.0f),
      size_scale_(size_scale) {
  sprite_.SetScale(size_scale_);
}

UiWindowBase::UiWindowBase(UiWindowBase&& other) noexcept
    : UiBase(std::move(other)),
      sprite_(std::move(other.sprite_)),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_),
      size_scale_(other.size_scale_),
      ui_shared_resources_(other.ui_shared_resources_) {}

/// back_ready_==false when appearing or disappearing animation
/// returs false when disappearing fading is over
bool UiWindowBase::RenderBack(bool show) {
  ui_shared_resources_.mask_sprite_shader_.Bind();
  if (show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    back_ready_ = false;
    progress_ -= speed_ * gDeltaTime;
    if (progress_ <= 0.0f) {
      return false;
    }
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  // By default desk texture also in general tex atlas
  //    glActiveTexture(0);
  //    desk_sprite_.Bind();
//  glActiveTexture(GL_TEXTURE1);
//  ui_shared_resources_.tex_ui_mask_.Bind();
//  glUniform1f(shader::kSpriteProgress, progress_);
  ui_shared_resources_.static_sprite_shader_.Bind();
  sprite_.Render();
  glActiveTexture(GL_TEXTURE0); // go back to default
  return true;
}

void UiWindowBase::RenderPickingBack() {
  ui_shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
}

void UiWindowBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiTabMenu::UiTabMenu(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,

    UiStaticSprite&& btn_mode_terrain,
    UiStaticSprite&& btn_mode_water,
    UiStaticSprite&& btn_mode_roads,
    UiStaticSprite&& btn_mode_fences,
    UiStaticSprite&& btn_mode_placement,
    UiStaticSprite&& btn_mode_objects,
    UiStaticSprite&& btn_mode_biomes,
    UiStaticSprite&& btn_mode_tiles,

    UiToggle&& toggle_terrain,
    UiToggle&& toggle_water,
    UiToggle&& toggle_roads,
    UiToggle&& toggle_fences,
    UiToggle&& toggle_placement,
    UiToggle&& toggle_objects,
    UiToggle&& toggle_biomes,
    UiToggle&& toggle_tiles,

    UiStaticSprite&& btn_shader_wirebound,
    UiToggle&& toggle_shaders,

    UiDynamicSprite&& arrow_select,
    UiDynamicSprite&& arrow_selected,
    UiDynamicSprite&& save_data,
    UiDynamicSprite&& load_data)
    : Base(std::move(sprite), size_scale, ui_shared_resources),
      btn_mode_terrain_(std::move(btn_mode_terrain)),
      btn_mode_water_(std::move(btn_mode_water)),
      btn_mode_roads_(std::move(btn_mode_roads)),
      btn_mode_fences_(std::move(btn_mode_fences)),
      btn_mode_placement_(std::move(btn_mode_placement)),
      btn_mode_objects_(std::move(btn_mode_objects)),
      btn_mode_biomes_(std::move(btn_mode_biomes)),
      btn_mode_tiles_(std::move(btn_mode_tiles)),

      toggle_terrain_(std::move(toggle_terrain)),
      toggle_water_(std::move(toggle_water)),
      toggle_roads_(std::move(toggle_roads)),
      toggle_fences_(std::move(toggle_fences)),
      toggle_placement_(std::move(toggle_placement)),
      toggle_objects_(std::move(toggle_objects)),
      toggle_biomes_(std::move(toggle_biomes)),
      toggle_tiles_(std::move(toggle_tiles)),

      btn_shader_wirebound_(std::move(btn_shader_wirebound)),
      toggle_shaders_(std::move(toggle_shaders)),

      arrow_select_(std::move(arrow_select)),
      arrow_selected_(std::move(arrow_selected)),
      save_data_(std::move(save_data)),
      load_data_(std::move(load_data)),
      ui_event_handler_({
          &btn_mode_terrain_,
          &btn_mode_water_,
          &btn_mode_roads_,
          &btn_mode_fences_,
          &btn_mode_placement_,
          &btn_mode_objects_,
          &btn_mode_biomes_,
          &btn_mode_tiles_,

          &toggle_terrain_,
          &toggle_water_,
          &toggle_roads_,
          &toggle_fences_,
          &toggle_placement_,
          &toggle_objects_,
          &toggle_biomes_,
          &toggle_tiles_,

          &btn_shader_wirebound_,
          &toggle_shaders_,
          &save_data_,
          &load_data_,
      }) {
  gUiComponents[btn_mode_terrain_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_water_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_roads_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_fences_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_placement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_objects_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_biomes_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_mode_tiles_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[toggle_terrain_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_water_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_roads_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_fences_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_placement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_objects_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_biomes_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_tiles_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[btn_shader_wirebound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_shaders_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[arrow_select_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[arrow_selected_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[save_data_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[load_data_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  Base::UpdateTransform();
}

UiTabMenu::UiTabMenu(UiTabMenu&& other) noexcept
    : Base(std::move(other)),
      btn_mode_terrain_(std::move(other.btn_mode_terrain_)),
      btn_mode_water_(std::move(other.btn_mode_water_)),
      btn_mode_roads_(std::move(other.btn_mode_roads_)),
      btn_mode_fences_(std::move(other.btn_mode_fences_)),
      btn_mode_placement_(std::move(other.btn_mode_placement_)),
      btn_mode_objects_(std::move(other.btn_mode_objects_)),
      btn_mode_biomes_(std::move(other.btn_mode_biomes_)),
      btn_mode_tiles_(std::move(other.btn_mode_tiles_)),

      toggle_terrain_(std::move(other.toggle_terrain_)),
      toggle_water_(std::move(other.toggle_water_)),
      toggle_roads_(std::move(other.toggle_roads_)),
      toggle_fences_(std::move(other.toggle_fences_)),
      toggle_placement_(std::move(other.toggle_placement_)),
      toggle_objects_(std::move(other.toggle_objects_)),
      toggle_biomes_(std::move(other.toggle_biomes_)),
      toggle_tiles_(std::move(other.toggle_tiles_)),

      btn_shader_wirebound_(std::move(other.btn_shader_wirebound_)),
      toggle_shaders_(std::move(other.toggle_shaders_)),

      arrow_select_(std::move(other.arrow_select_)),
      arrow_selected_(std::move(other.arrow_selected_)),
      save_data_(std::move(other.save_data_)),
      load_data_(std::move(other.load_data_)),

      ui_event_handler_({
          &btn_mode_terrain_,
          &btn_mode_water_,
          &btn_mode_roads_,
          &btn_mode_fences_,
          &btn_mode_placement_,
          &btn_mode_objects_,
          &btn_mode_biomes_,
          &btn_mode_tiles_,

          &toggle_terrain_,
          &toggle_water_,
          &toggle_roads_,
          &toggle_fences_,
          &toggle_placement_,
          &toggle_objects_,
          &toggle_biomes_,
          &toggle_tiles_,

          &btn_shader_wirebound_,
          &toggle_shaders_,
          &save_data_,
          &load_data_,
      }) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiTabMenu::Render(bool show, float angle_select, float angle_selected) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  btn_mode_terrain_.Render();
  btn_mode_water_.Render();
  btn_mode_roads_.Render();
  btn_mode_fences_.Render();
  btn_mode_placement_.Render();
  btn_mode_objects_.Render();
  btn_mode_biomes_.Render();
  btn_mode_tiles_.Render();

  toggle_terrain_.Render();
  toggle_water_.Render();
  toggle_roads_.Render();
  toggle_fences_.Render();
  toggle_placement_.Render();
  toggle_objects_.Render();
  toggle_biomes_.Render();
  toggle_tiles_.Render();

  btn_shader_wirebound_.Render();

  toggle_shaders_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  arrow_select_.SetRotate(angle_select);
  arrow_selected_.SetRotate(angle_selected);

  arrow_select_.Render();
  arrow_selected_.Render();
  save_data_.Render();
  load_data_.Render();

  return stop_show;
}

void UiTabMenu::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  btn_mode_terrain_.RenderPicking();
  btn_mode_water_.RenderPicking();
  btn_mode_roads_.RenderPicking();
  btn_mode_fences_.RenderPicking();
  btn_mode_placement_.RenderPicking();
  btn_mode_objects_.RenderPicking();
  btn_mode_biomes_.RenderPicking();
  btn_mode_tiles_.RenderPicking();

  toggle_terrain_.RenderPicking();
  toggle_water_.RenderPicking();
  toggle_roads_.RenderPicking();
  toggle_fences_.RenderPicking();
  toggle_placement_.RenderPicking();
  toggle_objects_.RenderPicking();
  toggle_biomes_.RenderPicking();
  toggle_tiles_.RenderPicking();

  btn_shader_wirebound_.RenderPicking();

  toggle_shaders_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  arrow_select_.RenderPicking();
  arrow_selected_.RenderPicking();
  save_data_.RenderPicking();
  load_data_.RenderPicking();
}

void UiTabMenu::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  btn_mode_terrain_.UpdateTransform();
  btn_mode_water_.UpdateTransform();
  btn_mode_roads_.UpdateTransform();
  btn_mode_fences_.UpdateTransform();
  btn_mode_placement_.UpdateTransform();
  btn_mode_objects_.UpdateTransform();
  btn_mode_biomes_.UpdateTransform();
  btn_mode_tiles_.UpdateTransform();

  toggle_terrain_.UpdateTransform();
  toggle_water_.UpdateTransform();
  toggle_roads_.UpdateTransform();
  toggle_fences_.UpdateTransform();
  toggle_placement_.UpdateTransform();
  toggle_objects_.UpdateTransform();
  toggle_biomes_.UpdateTransform();
  toggle_tiles_.UpdateTransform();

  btn_shader_wirebound_.UpdateTransform();
  toggle_shaders_.UpdateTransform();

  arrow_select_.UpdateTransform();
  arrow_selected_.UpdateTransform();
  save_data_.UpdateTransform();
  load_data_.UpdateTransform();
}

UiSettings::UiSettings(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiToggle2&& pin,
    UiDynamicSprite&& resolution_label,
    UiDynamicSprite&& resolution_left,
    UiDynamicSprite&& resolution_right,
    UiDynamicSprite&& resolution_slot,
    UiDynamicSprite&& resolution,
    UiToggle&& toggle_fullscreen,
    UiDynamicSprite sensitivity_icon,
    UiSliderH2 sensitivity,
    UiDynamicSprite keyboard,
    UiDynamicSprite&& sound_icon,
    UiSliderH2&& sound,
    UiToggle&& toggle_sound,
    UiDynamicSprite&& music_icon,
    UiSliderH2&& music,
    UiToggle&& toggle_music,
    UiDynamicSprite&& tip_info_label,
    UiDynamicSprite&& tip_info,
    UiToggle&& toggle_tip_info)
    : Base(std::move(sprite), size_scale, ui_shared_resources,
           start_transform, end_transform, std::move(pin)),
      resolution_label_(std::move(resolution_label)),
      resolution_left_(std::move(resolution_left)),
      resolution_right_(std::move(resolution_right)),
      resolution_slot_(std::move(resolution_slot)),
      resolution_(std::move(resolution)),
      toggle_fullscreen_(std::move(toggle_fullscreen)),
      sensitivity_icon_(std::move(sensitivity_icon)),
      sensitivity_(std::move(sensitivity)),
      keyboard_(std::move(keyboard)),
      sound_icon_(std::move(sound_icon)),
      sound_(std::move(sound)),
      toggle_sound_(std::move(toggle_sound)),
      music_icon_(std::move(music_icon)),
      music_(std::move(music)),
      toggle_music_(std::move(toggle_music)),
      tip_info_label_(std::move(tip_info_label)),
      tip_info_(std::move(tip_info)),
      toggle_tip_info_(std::move(toggle_tip_info)),
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_, &keyboard_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  gUiComponents[resolution_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_left_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_right_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_slot_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[resolution_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_fullscreen_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sensitivity_icon_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sensitivity_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[keyboard_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sound_icon_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[music_icon_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[tip_info_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[tip_info_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_tip_info_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  Base::UpdateTransform();
}

UiSettings::UiSettings(UiSettings&& other) noexcept
    : Base(std::move(other)),
      resolution_label_(std::move(other.resolution_label_)),
      resolution_left_(std::move(other.resolution_left_)),
      resolution_right_(std::move(other.resolution_right_)),
      resolution_slot_(std::move(other.resolution_slot_)),
      resolution_(std::move(other.resolution_)),
      toggle_fullscreen_(std::move(other.toggle_fullscreen_)),
      sensitivity_icon_(std::move(other.sensitivity_icon_)),
      sensitivity_(std::move(other.sensitivity_)),
      keyboard_(std::move(other.keyboard_)),
      sound_icon_(std::move(other.sound_icon_)),
      sound_(std::move(other.sound_)),
      toggle_sound_(std::move(other.toggle_sound_)),
      music_icon_(std::move(other.music_icon_)),
      music_(std::move(other.music_)),
      toggle_music_(std::move(other.toggle_music_)),
      tip_info_label_(std::move(other.tip_info_label_)),
      tip_info_(std::move(other.tip_info_)),
      toggle_tip_info_(std::move(other.toggle_tip_info_)),
      ui_event_handler_({
          &pin_, &resolution_left_, &resolution_right_,
          &toggle_fullscreen_, &sensitivity_, &keyboard_,
          &sound_, &toggle_sound_, &music_, &toggle_music_,
          &tip_info_, &toggle_tip_info_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiSettings::Render() {
  bool stop_show = Base::RenderBack(hovered_);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  resolution_label_.SetParentTransform(Base::cur_transform_);
  resolution_left_.SetParentTransform(Base::cur_transform_);
  resolution_right_.SetParentTransform(Base::cur_transform_);
  resolution_slot_.SetParentTransform(Base::cur_transform_);
  resolution_.SetParentTransform(Base::cur_transform_);
  toggle_fullscreen_.SetParentTransform(Base::cur_transform_);
  sensitivity_icon_.SetParentTransform(Base::cur_transform_);
  sensitivity_.SetParentTransform(Base::cur_transform_);
  keyboard_.SetParentTransform(Base::cur_transform_);
  sound_icon_.SetParentTransform(Base::cur_transform_);
  sound_.SetParentTransform(Base::cur_transform_);
  toggle_sound_.SetParentTransform(Base::cur_transform_);
  music_icon_.SetParentTransform(Base::cur_transform_);
  music_.SetParentTransform(Base::cur_transform_);
  toggle_music_.SetParentTransform(Base::cur_transform_);
  tip_info_label_.SetParentTransform(Base::cur_transform_);
  tip_info_.SetParentTransform(Base::cur_transform_);
  toggle_tip_info_.SetParentTransform(Base::cur_transform_);

  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;

  resolution_label_.Render();
  resolution_left_.Render();
  resolution_right_.Render();
  resolution_slot_.Render();
  resolution_.Render();
  toggle_fullscreen_.Render();
  sensitivity_icon_.Render();
  sensitivity_.Render(mouse_pos);
  keyboard_.Render();
  sound_icon_.Render();
  sound_.Render(mouse_pos);
  toggle_sound_.Render();
  music_icon_.Render();
  music_.Render(mouse_pos);
  toggle_music_.Render();
  tip_info_label_.Render();
  tip_info_.Render();
  toggle_tip_info_.Render();

  music_.RenderIcon();
  sound_.RenderIcon();
  sensitivity_.RenderIcon();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(resolution_label_, "resolution", 1.0f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(tip_info_label_, "show tips", 1.0f, glm::vec2{0.0f});

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  resolution_label_.RenderPicking();
  resolution_left_.RenderPicking();
  resolution_right_.RenderPicking();
  resolution_slot_.RenderPicking();
  resolution_.RenderPicking();
  toggle_fullscreen_.RenderPicking();
  sensitivity_icon_.RenderPicking();
  sensitivity_.RenderPicking();
  keyboard_.RenderPicking();
  sound_icon_.RenderPicking();
  sound_.RenderPicking();
  toggle_sound_.RenderPicking();
  music_icon_.RenderPicking();
  music_.RenderPicking();
  toggle_music_.RenderPicking();
  tip_info_label_.RenderPicking();
  tip_info_.RenderPicking();
  toggle_tip_info_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(resolution_label_, "resolution", 1.0f, glm::vec2{0.0f});
  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(tip_info_label_, "show tips", 1.0f, glm::vec2{0.0f});
}

data::TextId UiSettings::Hover(int id) {
  data::TextId hovered_id = ui_event_handler_.Hover(id);
  if (hovered_id != data::TextId::kNone || id == Base::GetId()) {
    hovered_ = true;
  } else {
    hovered_ = false;
  }

//  hovered_ = hovered_id != data::TextId::kNotYet;
//  hovered_ = id == Base::GetId();
  //  hovered_ = id >= scale_.GetFillId() && id <= back_.GetId();
  return hovered_id;
}

void UiSettings::Press(int id) {
  ui_event_handler_.Press(id);
}

void UiSettings::Release() {
  ui_event_handler_.Release();
}

bool UiSettings::Scroll(GLuint id, float yoffset) {
  return music_.Scroll(id, yoffset) ||
         sound_.Scroll(id, yoffset) ||
         sensitivity_.Scroll(id, yoffset);
}

void UiSettings::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  resolution_label_.SetParentTransform(Base::cur_transform_);
  resolution_left_.SetParentTransform(Base::cur_transform_);
  resolution_right_.SetParentTransform(Base::cur_transform_);
  resolution_slot_.SetParentTransform(Base::cur_transform_);
  resolution_.SetParentTransform(Base::cur_transform_);
  toggle_fullscreen_.SetParentTransform(Base::cur_transform_);
  sensitivity_icon_.SetParentTransform(Base::cur_transform_);
  sensitivity_.SetParentTransform(Base::cur_transform_);
  keyboard_.SetParentTransform(Base::cur_transform_);
  sound_icon_.SetParentTransform(Base::cur_transform_);
  sound_.SetParentTransform(Base::cur_transform_);
  toggle_sound_.SetParentTransform(Base::cur_transform_);
  music_icon_.SetParentTransform(Base::cur_transform_);
  music_.SetParentTransform(Base::cur_transform_);
  toggle_music_.SetParentTransform(Base::cur_transform_);
  tip_info_label_.SetParentTransform(Base::cur_transform_);
  tip_info_.SetParentTransform(Base::cur_transform_);
  toggle_tip_info_.SetParentTransform(Base::cur_transform_);
  /// back
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  /// children
  resolution_label_.UpdateTransform();
  resolution_left_.UpdateTransform();
  resolution_right_.UpdateTransform();
  resolution_slot_.UpdateTransform();
  resolution_.UpdateTransform();
  toggle_fullscreen_.UpdateTransform();
  sensitivity_icon_.UpdateTransform();
  sensitivity_.UpdateTransform();
  keyboard_.UpdateTransform();
  sound_icon_.UpdateTransform();
  sound_.UpdateTransform();
  toggle_sound_.UpdateTransform();
  music_icon_.UpdateTransform();
  music_.UpdateTransform();
  toggle_music_.UpdateTransform();
  tip_info_label_.UpdateTransform();
  tip_info_.UpdateTransform();
  toggle_tip_info_.UpdateTransform();
}

UiConfirmation::UiConfirmation(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    UiStaticSprite&& btn_accept,
    UiStaticSprite&& btn_decline)
    : Base(std::move(sprite), size_scale, ui_shared_resources),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_decline_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  Base::UpdateTransform();
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      ui_event_handler_({&btn_accept_, &btn_decline_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiConfirmation::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  btn_accept_.Render();
  btn_decline_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  return stop_show;
}

void UiConfirmation::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
}

void UiConfirmation::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  btn_accept_.UpdateTransform();
  btn_decline_.UpdateTransform();
}

UiPopUpBase::UiPopUpBase(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiToggle2&& pin)
    : UiBase(sprite.GetId(), {}),
      sprite_(std::move(sprite)),
      start_transform_(start_transform),
      end_transform_(end_transform),
      ui_shared_resources_(ui_shared_resources),
      pin_(std::move(pin)),
      size_scale_(size_scale),
      speed_(4.0f) {
  gUiComponents[pin_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  sprite_.SetScale(size_scale);
}

UiPopUpBase::UiPopUpBase(UiPopUpBase&& other) noexcept
    : UiBase(std::move(other)),
      sprite_(std::move(other.sprite_)),
      cur_transform_(other.cur_transform_),
      start_transform_(other.start_transform_),
      end_transform_(other.end_transform_),
      speed_(other.speed_),
      progress_(other.progress_),
      back_ready_(other.back_ready_),
      size_scale_(other.size_scale_),
      ui_shared_resources_(other.ui_shared_resources_),
      pin_(std::move(other.pin_)) {}

bool UiPopUpBase::RenderBack(bool show) {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  if (pin_.TurnedOn() || show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    progress_ -= speed_ * gDeltaTime;
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  CubicInterpolation();
  sprite_.SetParentTransform(cur_transform_);
  pin_.SetParentTransform(cur_transform_);
  sprite_.Render();
  pin_.Render();
  if (progress_ == 0.0f) {
    return false;
  } else {
    return true;
  }
}

void UiPopUpBase::RenderPickingBack() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
  pin_.RenderPicking();
}

void UiPopUpBase::CubicInterpolation() {
  cur_transform_.translate =
      start_transform_.translate
      + progress_ * (end_transform_.translate - start_transform_.translate);
  cur_transform_.rotate =
      start_transform_.rotate
      + progress_ * (end_transform_.rotate - start_transform_.rotate);
  cur_transform_.scale =
      start_transform_.scale
      + progress_ * (end_transform_.scale - start_transform_.scale);
}

void UiPopUpBase::UpdateTransform() {
  auto transform = debug::gUiTransforms[
      4 * (GetId() - details::kIdOffsetUi)
  ];
  UpdateTransform(
      transform.translate.x, transform.translate.y, transform.scale);
}

UiWaterLayerConfig::UiWaterLayerConfig(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform,
    UiToggle2&& pin,
    UiDynamicSprite&& sprite_layer,
    UiSliderH&& scale,
    UiSliderH&& fetch,
    UiSliderH&& spread_blend,
    UiSliderH&& swell,
    UiSliderH&& peak_enhancement,
    UiSliderH&& short_waves_fade,
    UiSliderH&& lambda)
    : Base(std::move(sprite), size_scale, ui_shared_resources,
           start_transform, end_transform, std::move(pin)),
      sprite_layer_(std::move(sprite_layer)),
      scale_(std::move(scale)),
      fetch_(std::move(fetch)),
      spread_blend_(std::move(spread_blend)),
      swell_(std::move(swell)),
      peak_enhancement_(std::move(peak_enhancement)),
      short_waves_fade_(std::move(short_waves_fade)),
      lambda_(std::move(lambda)),
      ui_event_handler_({
          &scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }) {
  gUiComponents[sprite_layer_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[scale_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[fetch_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[spread_blend_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[swell_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[peak_enhancement_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[short_waves_fade_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[lambda_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  Base::UpdateTransform();
}

UiWaterLayerConfig::UiWaterLayerConfig(UiWaterLayerConfig&& other) noexcept
    : Base(std::move(other)),
      sprite_layer_(std::move(other.sprite_layer_)),
      scale_(std::move(other.scale_)),
      fetch_(std::move(other.fetch_)),
      spread_blend_(std::move(other.spread_blend_)),
      swell_(std::move(other.swell_)),
      peak_enhancement_(std::move(other.peak_enhancement_)),
      short_waves_fade_(std::move(other.short_waves_fade_)),
      lambda_(std::move(other.lambda_)),
      modified_(other.modified_),
      ui_event_handler_({
          &scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

// returns "stop render"
bool UiWaterLayerConfig::Render() {
  bool stop_show = Base::RenderBack(hovered_);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  scale_.SetParentTransform(Base::cur_transform_);
  fetch_.SetParentTransform(Base::cur_transform_);
  spread_blend_.SetParentTransform(Base::cur_transform_);
  swell_.SetParentTransform(Base::cur_transform_);
  peak_enhancement_.SetParentTransform(Base::cur_transform_);
  short_waves_fade_.SetParentTransform(Base::cur_transform_);
  lambda_.SetParentTransform(Base::cur_transform_);

  /// shader & transform matrix already bind by Base class
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  sprite_layer_.Render();
  scale_.Render(mouse_pos);
  fetch_.Render(mouse_pos);
  spread_blend_.Render(mouse_pos);
  swell_.Render(mouse_pos);
  peak_enhancement_.Render(mouse_pos);
  short_waves_fade_.Render(mouse_pos);
  lambda_.Render(mouse_pos);

  scale_.RenderIcon();
  fetch_.RenderIcon();
  spread_blend_.RenderIcon();
  swell_.RenderIcon();
  peak_enhancement_.RenderIcon();
  short_waves_fade_.RenderIcon();
  lambda_.RenderIcon();

  return stop_show;
}

void UiWaterLayerConfig::RenderPicking() {
  Base::RenderPickingBack();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_layer_.RenderPicking();
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();
}

void UiWaterLayerConfig::Release() {
  scale_.Release();
  fetch_.Release();
  spread_blend_.Release();
  swell_.Release();
  peak_enhancement_.Release();
  short_waves_fade_.Release();
  lambda_.Release();
}

data::TextId UiWaterLayerConfig::Hover(int id) {
  data::TextId hovered_id = ui_event_handler_.Hover(id);
  hovered_ = id == Base::GetId();
  //  hovered_ = id >= scale_.GetFillId() && id <= back_.GetId();
  return hovered_id;
}

bool UiWaterLayerConfig::Scroll(GLuint id, float yoffset) {
  return scale_.Scroll(id, yoffset) ||
         fetch_.Scroll(id, yoffset) ||
         spread_blend_.Scroll(id, yoffset) ||
         swell_.Scroll(id, yoffset) ||
         peak_enhancement_.Scroll(id, yoffset) ||
         short_waves_fade_.Scroll(id, yoffset) ||
         lambda_.Scroll(id, yoffset);
}

bool UiWaterLayerConfig::Modified() {
  if (modified_) {
    modified_ = false;
    return true;
  }
  return false;
}

OceanLayerTraits UiWaterLayerConfig::GetOceanLayerTraits() const {
  return {
      scale_.GetProgress(),
      fetch_.GetProgress(),
      spread_blend_.GetProgress(),
      swell_.GetProgress(),
      peak_enhancement_.GetProgress(),
      short_waves_fade_.GetProgress(),
      lambda_.GetProgress()
  };
}

void UiWaterLayerConfig::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  scale_.SetParentTransform(Base::cur_transform_);
  fetch_.SetParentTransform(Base::cur_transform_);
  spread_blend_.SetParentTransform(Base::cur_transform_);
  swell_.SetParentTransform(Base::cur_transform_);
  peak_enhancement_.SetParentTransform(Base::cur_transform_);
  short_waves_fade_.SetParentTransform(Base::cur_transform_);
  lambda_.SetParentTransform(Base::cur_transform_);

  /// back
  sprite_.UpdateTransform();

  /// children
  sprite_layer_.UpdateTransform();
  scale_.UpdateTransform();
  fetch_.UpdateTransform();
  spread_blend_.UpdateTransform();
  swell_.UpdateTransform();
  peak_enhancement_.UpdateTransform();
  short_waves_fade_.UpdateTransform();
  lambda_.UpdateTransform();
}

IUiEdit::IUiEdit()
    : UiBase(data::VboIdMain::kSpareText2, {}, {}) {}

UiEditStub::UiEditStub() {}

UiEditStub::UiEditStub(UiEditStub&& other) noexcept
    : IUiEdit(std::move(other)) {}

void UiEditStub::Render() {

}

void UiEditStub::RenderPicking() const {}

void UiEditStub::UpdateTransform(
    float x_translate, float y_translate, float scale) {}

void UiEditStub::UpdateTransform() {}

