#include "UiComplex.h"

#include "../common/UiDebugger.h"
#include "../common/ShadersBinding.h"

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
      static_cast<UiTransformDbg*>(this);
}

UiLoading::UiLoading(UiLoading&& other) noexcept
    : UiBase(std::move(other)),
      sprites_(std::move(other.sprites_)) {
  gUiComponents[sprites_[0].GetId() - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
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
  glActiveTexture(GL_TEXTURE1);
  ui_shared_resources_.tex_ui_mask_.Bind();
  glUniform1f(shader::kSpriteProgress, progress_);
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

    UiStaticSprite&& btn_settings,
    UiStaticSprite&& btn_shader_wirebound,

    UiToggle&& toggle_shaders,

    UiDynamicSprite&& cross)
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

      btn_settings_(std::move(btn_settings)),
      btn_shader_wirebound_(std::move(btn_shader_wirebound)),
      toggle_shaders_(std::move(toggle_shaders)),
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

          &btn_settings_,
          &btn_shader_wirebound_,
          &toggle_shaders_
      }),
      cross_(std::move(cross)) {
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

  gUiComponents[btn_settings_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_shader_wirebound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_shaders_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();

  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
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

      btn_settings_(std::move(other.btn_settings_)),
      btn_shader_wirebound_(std::move(other.btn_shader_wirebound_)),
      toggle_shaders_(std::move(other.toggle_shaders_)),

      cross_(std::move(other.cross_)),
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

          &btn_settings_,
          &btn_shader_wirebound_,
          &toggle_shaders_
      }) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiTabMenu::Render(bool show) {
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

  btn_settings_.Render();
  btn_shader_wirebound_.Render();

  toggle_shaders_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

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

  btn_settings_.RenderPicking();
  btn_shader_wirebound_.RenderPicking();

  toggle_shaders_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
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

  btn_settings_.UpdateTransform();
  btn_shader_wirebound_.UpdateTransform();
  toggle_shaders_.UpdateTransform();

  cross_.UpdateTransform();
}

UiSettings::UiSettings(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    UiSliderH3&& resolution,
    UiSliderH3&& music,
    UiSliderH3&& sound,
    UiSliderH3&& sensitivity,
    UiStaticSprite&& keyboard,
    UiToggle&& toggle_music,
    UiToggle&& toggle_sound,
    UiDynamicSprite&& cross)
    : Base(std::move(sprite), size_scale, ui_shared_resources),
      resolution_(std::move(resolution)),
      music_(std::move(music)),
      sound_(std::move(sound)),
      sensitivity_(std::move(sensitivity)),
      keyboard_(std::move(keyboard)),
      toggle_music_(std::move(toggle_music)),
      toggle_sound_(std::move(toggle_sound)),
      ui_event_handler_({
          &resolution_, &music_, &sound_, &sensitivity_, &keyboard_,
          &toggle_music_, &toggle_sound_
      }),
      cross_(std::move(cross)) {
  gUiComponents[resolution_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sensitivity_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[keyboard_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_music_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[toggle_sound_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
  Base::UpdateTransform();
}

UiSettings::UiSettings(UiSettings&& other) noexcept
    : Base(std::move(other)),
      resolution_(std::move(other.resolution_)),
      music_(std::move(other.music_)),
      sound_(std::move(other.sound_)),
      sensitivity_(std::move(other.sensitivity_)),
      keyboard_(std::move(other.keyboard_)),
      toggle_music_(std::move(other.toggle_music_)),
      toggle_sound_(std::move(other.toggle_sound_)),
      cross_(std::move(other.cross_)),
      ui_event_handler_({
          &resolution_, &music_, &sound_, &sensitivity_,
          &keyboard_, &toggle_music_, &toggle_sound_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiSettings::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  resolution_.Render(mouse_pos);
  music_.Render(mouse_pos);
  sound_.Render(mouse_pos);
  sensitivity_.Render(mouse_pos);
  keyboard_.Render();

  toggle_music_.Render();
  toggle_sound_.Render();

  resolution_.RenderIcon();
  music_.RenderIcon();
  sound_.RenderIcon();
  sensitivity_.RenderIcon();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  resolution_.RenderPicking();
  music_.RenderPicking();
  sound_.RenderPicking();
  sensitivity_.RenderPicking();
  keyboard_.RenderPicking();

  toggle_music_.RenderPicking();
  toggle_sound_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

bool UiSettings::Scroll(GLuint id, float yoffset) {
  return resolution_.Scroll(id, yoffset) ||
         music_.Scroll(id, yoffset) ||
         sound_.Scroll(id, yoffset) ||
         sensitivity_.Scroll(id, yoffset);
}

void UiSettings::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  resolution_.UpdateTransform();
  music_.UpdateTransform();
  sound_.UpdateTransform();
  sensitivity_.UpdateTransform();
  keyboard_.UpdateTransform();
  toggle_music_.UpdateTransform();
  toggle_sound_.UpdateTransform();
  cross_.UpdateTransform();
}

UiConfirmation::UiConfirmation(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,

    UiStaticSprite&& btn_close,
    UiStaticSprite&& btn_accept,
    UiStaticSprite&& btn_decline,

    UiDynamicSprite&& cross)
    : Base(std::move(sprite), size_scale, ui_shared_resources),
      btn_close_(std::move(btn_close)),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      ui_event_handler_({&btn_close_, &btn_accept_, &btn_decline_}),
      cross_(std::move(cross)) {
  gUiComponents[btn_close_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[btn_decline_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[cross_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiTransformDbg*>(this);
  Base::UpdateTransform();
}

UiConfirmation::UiConfirmation(UiConfirmation&& other) noexcept
    : Base(std::move(other)),
      btn_close_(std::move(other.btn_close_)),
      btn_accept_(std::move(other.btn_accept_)),
      btn_decline_(std::move(other.btn_decline_)),
      cross_(std::move(other.cross_)),
      ui_event_handler_({&btn_close_, &btn_accept_, &btn_decline_}) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiTransformDbg*>(this);
}

// returns "stop render"
bool UiConfirmation::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();

  btn_close_.Render();
  btn_accept_.Render();
  btn_decline_.Render();

  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiConfirmation::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  btn_close_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

void UiConfirmation::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  /// back
  sprite_.UpdateTransform();

  /// children
  btn_close_.UpdateTransform();
  btn_accept_.UpdateTransform();
  btn_decline_.UpdateTransform();
  cross_.UpdateTransform();
}

UiPopUpBase::UiPopUpBase(
    UiDynamicSprite&& sprite,
    float size_scale,
    UiSharedResources& ui_shared_resources,
    LocalTransform start_transform,
    LocalTransform end_transform)
    : UiBase(sprite.GetId(), {}),
      sprite_(std::move(sprite)),
      start_transform_(start_transform),
      end_transform_(end_transform),
      ui_shared_resources_(ui_shared_resources),
      size_scale_(size_scale),
      speed_(4.0f) {
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
      ui_shared_resources_(other.ui_shared_resources_) {}

bool UiPopUpBase::RenderBack(bool show) {
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  if (show) {
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
  sprite_.Render();
  if (progress_ == 0.0f) {
    return false;
  } else {
    return true;
  }
}

void UiPopUpBase::RenderPickingBack() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_.SetParentTransform(cur_transform_);
  sprite_.RenderPicking();
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
    UiDynamicSprite&& sprite_layer,
    UiSliderH&& scale,
    UiSliderH&& fetch,
    UiSliderH&& spread_blend,
    UiSliderH&& swell,
    UiSliderH&& peak_enhancement,
    UiSliderH&& short_waves_fade,
    UiSliderH&& lambda)
    : Base(std::move(sprite), size_scale, ui_shared_resources,
           start_transform, end_transform),
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
      static_cast<UiTransformDbg*>(this);
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
      = static_cast<UiTransformDbg*>(this);
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

void UiWaterLayerConfig::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
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
