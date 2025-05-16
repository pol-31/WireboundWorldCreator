#include "Ui.h"

#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "../modes/TerrainMode.h"
#include "../modes/WaterMode.h"
#include "../common/Colors.h"
#include "../common/ShadersBinding.h"

#include "../common/UiDebugger.h"

#ifdef NDEBUG
// see src/vbos/UiDataMain.h for explanation
float UiStaticSprite::GetLeftBorder() const {
  auto transform =
      debug::gUiTransforms[4 * (ui_data_.id - static_cast<int>(vbos::VboIdMain::kModeTerrain))];
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8] * transform.scale
         + transform.x_translate;
}

float UiStaticSprite::GetRightBorder() const {
  auto transform =
      debug::gUiTransforms[ui_data_.id - static_cast<int>(vbos::VboIdMain::kModeTerrain)];
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4] * transform.scale
         + transform.x_translate;
}

float UiStaticSprite::GetTopBorder() const {
  auto transform =
      debug::gUiTransforms[ui_data_.id - static_cast<int>(vbos::VboIdMain::kModeTerrain)];
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5] * transform.scale
         + transform.y_translate;
}

float UiStaticSprite::GetBottomBorder() const {
  auto transform =
      debug::gUiTransforms[ui_data_.id - static_cast<int>(vbos::VboIdMain::kModeTerrain)];
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1] * transform.scale
         + transform.y_translate;
}

float UiDynamicSprite::GetLeftBorder() const {
  std::terminate();
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x + width / 2;
}

float UiDynamicSprite::GetRightBorder() const {
  std::terminate();
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x - width / 2;
}

float UiDynamicSprite::GetTopBorder() const {
  std::terminate();
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y + height / 2;
}

float UiDynamicSprite::GetBottomBorder() const {
  std::terminate();
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y - height / 2;
}
#else

float UiDynamicSprite::GetLeftBorder() const {
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x + width / 2;
}

float UiDynamicSprite::GetRightBorder() const {
  float width = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4]
                - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
  return transform_[3].x - width / 2;
}

float UiDynamicSprite::GetTopBorder() const {
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y + height / 2;
}

float UiDynamicSprite::GetBottomBorder() const {
  float height = vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5]
                 - vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
  return transform_[3].y - height / 2;
}

// see src/vbos/UiDataMain.h for explanation
float UiStaticSprite::GetLeftBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 8];
}

float UiStaticSprite::GetRightBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4];
}

float UiStaticSprite::GetTopBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 5];
}

float UiStaticSprite::GetBottomBorder() const {
  return vbos::kUiVboDataMain[ui_data_.vbo_offset * 4 + 1];
}
#endif // NDEBUG

std::array<vbos::UiData, vbos::gVboIdSize> gUiComponents{};

void UiDynamicSprite::Render() {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform_));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::Render() {
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::RenderPicking() const {
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

// --- --- --- ---
// --- --- --- ---
// --- --- --- ---

UiSliderV::UiSliderV(
    UiStaticSprite&& fill_sprite,
    UiStaticSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : UiCallable(fill_sprite.ui_data_, {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      length_(0.9f * (fill_sprite_.GetTopBorder()
              - fill_sprite_.GetBottomBorder())),
      centre_((fill_sprite_.GetTopBorder()
               + fill_sprite_.GetBottomBorder()) / 2.0f),
      scale_(scale) {
  auto transform =
      debug::gUiTransforms[
          4 * (fill_sprite_.ui_data_.id
               - static_cast<int>(vbos::VboIdMain::kMenuTerrain))
  ];
  UpdateTransform(transform.x_translate, transform.y_translate,
                  transform.scale);
  fill_sprite_.ui_data_.children_num = 2;
  fill_sprite_.ui_data_.ui = static_cast<UiTransformDbg*>(this);

//  icon_sprite_.ui_data_.parent_scale = 0.5f; // seems useless
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_scale = 0.9f;


  vbos::UiData ui_data = fill_sprite_.ui_data_;
  gUiComponents[ui_data.id - details::kIdOffsetUi] = ui_data;
}

void UiSliderV::RenderPicking() const {
  fill_sprite_.RenderPicking();
}

size_t UiSliderV::Hover(std::uint32_t id) {
  //TODO: set higher brightness?
  return fill_sprite_.Hover();
}

void UiSliderV::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(mouse_pos.y - centre_,
                            -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::mat3 transform{1.0f};
  float scale = debug::gUiTransforms[icon_sprite_.GetVboOffset()].scale
      * gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_scale;
  std::cout << "scale " << gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_scale << std::endl;
  transform = glm::translate(
      transform,
      glm::vec2(debug::gUiTransforms[icon_sprite_.GetVboOffset()].x_translate,
                debug::gUiTransforms[icon_sprite_.GetVboOffset()].y_translate + offset));
  transform = glm::scale(transform, glm::vec2(scale, scale));
  icon_sprite_.SetTransform(transform);
//  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_offset_y = related_pos;
//  std::cout << "ID: " << icon_sprite_.GetId() - details::kIdOffsetUi << std::endl;
//    std::cout << related_pos << ' ' << progress_ << ' ' << length_ << ' ' << centre_ << std::endl;
//  std::cout << (1.0f - progress_) << " and scaled "
//            << ((1.0f - progress_) * scale_) << std::endl;
}

void UiSliderV::UnHover() {
  //TODO: set lower brightness?
}

float UiSliderV::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderV::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  length_ = 0.9f * scale *
            (fill_sprite_.GetTopBorder()
             - fill_sprite_.GetBottomBorder());
//  std::cout << "length: " << length_ << ' ' << scale
//            << ' ' << x_translate << ' ' << y_translate << std::endl;
//  std::cout << "and " << fill_sprite_.GetTopBorder() << ' '
//            << fill_sprite_.GetBottomBorder() << " id "
//            << fill_sprite_.GetVboOffset() << std::endl;
  centre_ = y_translate;

  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({0.0f, related_pos});
}

void UiSliderV::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  back_sprite_.Render();

  //TODO: move up to *Mode (1 state change?)
  glEnable(GL_SCISSOR_TEST);
  // ---- ---- ----
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  float y_ndc = centre_ - length_ / 2;
  int y_px = int((y_ndc + 1.0f) * 0.5f * gWindowHeight);
  int height_px = int(progress_ * length_ * 0.5f * gWindowHeight);
  glScissor(0, y_px, 4000, height_px);
  // ---- ---- ----
  fill_sprite_.Render();
  glDisable(GL_SCISSOR_TEST);

  //TODO: progress-based
//  icon_sprite_.Render();

  UnHover();
}

void UiSliderV::RenderIcon() {
  icon_sprite_.Render();
}

// --- --- --- ---
// --- --- --- ---
// --- --- --- ---

UiSliderH::UiSliderH(
    UiStaticSprite&& fill_sprite,
    UiStaticSprite&& back_sprite,
    UiDynamicSprite&& handler_sprite,
    UiStaticSprite&& icon_sprite,
    float scale)
    : UiCallable(fill_sprite.ui_data_, {}),
      fill_sprite_(std::move(fill_sprite)),
      back_sprite_(std::move(back_sprite)),
      icon_sprite_(std::move(icon_sprite)),
      handler_sprite_(std::move(handler_sprite)),
      length_(0.6f * (fill_sprite_.GetRightBorder()
              - fill_sprite_.GetLeftBorder())),
      centre_((fill_sprite_.GetRightBorder()
               + fill_sprite_.GetLeftBorder()) / 2.0f),
      scale_(scale) {
  auto transform =
      debug::gUiTransforms[
          4 * (fill_sprite_.ui_data_.id
               - static_cast<int>(vbos::VboIdMain::kMenuTerrain))
  ];
  UpdateTransform(transform.x_translate, transform.y_translate,
                  transform.scale);
  fill_sprite_.ui_data_.children_num = 3;
  fill_sprite_.ui_data_.ui = static_cast<UiTransformDbg*>(this);

//  icon_sprite_.ui_data_.parent_scale = 0.5f; // seems useless
  gUiComponents[handler_sprite_.GetId() - details::kIdOffsetUi].parent_scale = 0.5f;
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_scale = 0.7f;
  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_offset_x = -0.1f;

  vbos::UiData ui_data = fill_sprite_.ui_data_;
  gUiComponents[ui_data.id - details::kIdOffsetUi] = ui_data;
}

void UiSliderH::RenderPicking() const {
  fill_sprite_.RenderPicking();
}

size_t UiSliderH::Hover(std::uint32_t id) {
  //TODO: set higher brightness?
  return fill_sprite_.Hover();
}

void UiSliderH::Set(glm::vec2 mouse_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(
      mouse_pos.x - centre_, -half_length_, +half_length_);
  progress_ = (offset + half_length_) / length_;
  glm::mat3 transform{1.0f};
  float scale = debug::gUiTransforms[handler_sprite_.GetVboOffset()].scale
      * gUiComponents[handler_sprite_.GetId() - details::kIdOffsetUi].parent_scale;
//  std::cout << "scale " << gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_scale << std::endl;
  transform = glm::translate(
      transform,
      glm::vec2(debug::gUiTransforms[handler_sprite_.GetVboOffset()].x_translate + offset,
                debug::gUiTransforms[handler_sprite_.GetVboOffset()].y_translate));
  transform = glm::scale(transform, glm::vec2(scale, scale));
  handler_sprite_.SetTransform(transform);
//  gUiComponents[icon_sprite_.GetId() - details::kIdOffsetUi].parent_offset_y = related_pos;
//  std::cout << "ID: " << icon_sprite_.GetId() - details::kIdOffsetUi << std::endl;
//    std::cout << related_pos << ' ' << progress_ << ' ' << length_ << ' ' << centre_ << std::endl;
//  std::cout << (1.0f - progress_) << " and scaled "
//            << ((1.0f - progress_) * scale_) << std::endl;
}

void UiSliderH::UnHover() {
  //TODO: set lower brightness?
}

float UiSliderH::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSliderH::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  length_ = 0.6f * scale *
            (fill_sprite_.GetRightBorder()
             - fill_sprite_.GetLeftBorder());
//  std::cout << "length: " << length_ << ' ' << scale
//            << ' ' << x_translate << ' ' << y_translate << std::endl;
//  std::cout << "and " << fill_sprite_.GetTopBorder() << ' '
//            << fill_sprite_.GetBottomBorder() << " id "
//            << fill_sprite_.GetVboOffset() << std::endl;
  centre_ = x_translate;

  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set({related_pos, 0.0f});
}

void UiSliderH::Render(glm::vec2 mouse_pos) {
  if (pressed_) {
    Set(mouse_pos);
  }
  back_sprite_.Render();

  //TODO: move up to *Mode (1 state change?)
  glEnable(GL_SCISSOR_TEST);
  // ---- ---- ----
  // NDC to pixels: ((ndc + 1.0) / 2.0) * dimension
  float x_ndc = centre_ - length_ / 2;
  int x_px = int((x_ndc + 1.0f) * 0.5f * gWindowWidth);
  int width_px = int(progress_ * length_ * 0.5f * gWindowWidth);
  glScissor(x_px, 0, width_px, 4000);
  // ---- ---- ----
  fill_sprite_.Render();
  glDisable(GL_SCISSOR_TEST);

  icon_sprite_.Render();
  UnHover();
}

void UiSliderH::RenderIcon() {
  handler_sprite_.Render();
}

UiOceanCascadeConfig::UiOceanCascadeConfig(
    UiStaticSprite&& scale_fill, UiStaticSprite&& scale_back,
    UiDynamicSprite&& scale_handler, UiStaticSprite&& scale_icon,
    float scale_scale,
    UiStaticSprite&& fetch_fill, UiStaticSprite&& fetch_back,
    UiDynamicSprite&& fetch_handler, UiStaticSprite&& fetch_icon,
    float fetch_scale,
    UiStaticSprite&& spread_blend_fill, UiStaticSprite&& spread_blend_back,
    UiDynamicSprite&& spread_blend_handler, UiStaticSprite&& spread_blend_icon,
    float spread_blend_scale,
    UiStaticSprite&& swell_fill, UiStaticSprite&& swell_back,
    UiDynamicSprite&& swell_handler, UiStaticSprite&& swell_icon,
    float swell_scale,
    UiStaticSprite&& peaks_fill, UiStaticSprite&& peaks_back,
    UiDynamicSprite&& peaks_handler, UiStaticSprite&& peaks_icon,
    float peaks_scale,
    UiStaticSprite&& fade_fill, UiStaticSprite&& fade_back,
    UiDynamicSprite&& fade_handler, UiStaticSprite&& fade_icon,
    float fade_scale,
    UiStaticSprite&& lambda_fill, UiStaticSprite&& lambda_back,
    UiDynamicSprite&& lambda_handler, UiStaticSprite&& lambda_icon,
    float lambda_scale)
    : scale_(std::move(scale_fill), std::move(scale_back),
             std::move(scale_handler), std::move(scale_icon), scale_scale),
      fetch_(std::move(fetch_fill), std::move(fetch_back),
             std::move(fetch_handler), std::move(fetch_icon), fetch_scale),
      spread_blend_(std::move(spread_blend_fill), std::move(spread_blend_back),
                    std::move(spread_blend_handler), std::move(spread_blend_icon), spread_blend_scale),
      swell_(std::move(swell_fill), std::move(swell_back),
             std::move(swell_handler), std::move(swell_icon), swell_scale),
      peak_enhancement_(std::move(peaks_fill), std::move(peaks_back),
                        std::move(peaks_handler), std::move(peaks_icon), peaks_scale),
      short_waves_fade_(std::move(fade_fill), std::move(fade_back),
                        std::move(fade_handler), std::move(fade_icon), fade_scale),
      lambda_(std::move(lambda_fill), std::move(lambda_back),
              std::move(lambda_handler), std::move(lambda_icon), lambda_scale) {}

//  void Update(); // update ubo not here

OceanLayerTraits UiOceanCascadeConfig::GetOceanLayerTraits() const {
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

bool UiOceanCascadeConfig::Modified() {
  if (modified_) {
    modified_ = false;
    return true;
  }
  return false;
}

bool UiOceanCascadeConfig::CheckId(int id) {
  if (id < scale_.GetTrackId() || id > lambda_.GetTrackId()) {
    return false;
  } else {
    return true;
  }
}

bool UiOceanCascadeConfig::Press(int id) {
  if (id < scale_.GetTrackId() || id > lambda_.GetTrackId()) {
    return false;
  }
  //TODO: rewrite with switch statement (need constexpr)
  //TODO: aabb
  if (id == scale_.GetTrackId()) {
    scale_.Press();
  } else if (id == fetch_.GetTrackId()) {
    fetch_.Press();
  } else if (id == spread_blend_.GetTrackId()) {
    spread_blend_.Press();
  } else if (id == swell_.GetTrackId()) {
    swell_.Press();
  } else if (id == peak_enhancement_.GetTrackId()) {
    peak_enhancement_.Press();
  } else if (id == short_waves_fade_.GetTrackId()) {
    short_waves_fade_.Press();
  } else if (id == lambda_.GetTrackId()) {
    lambda_.Press();
  }
  modified_ = true;
  return true;
}

size_t UiOceanCascadeConfig::Hover(int id) {
  if (id < scale_.GetTrackId() || id > lambda_.GetTrackId()) {
    return false;
  }
  //TODO: rewrite with switch statement (need constexpr)
  //TODO: aabb
  if (id == scale_.GetTrackId()) {
    return scale_.Hover(id);
  } else if (id == fetch_.GetTrackId()) {
    return fetch_.Hover(id);
  } else if (id == spread_blend_.GetTrackId()) {
    return spread_blend_.Hover(id);
  } else if (id == swell_.GetTrackId()) {
    return swell_.Hover(id);
  } else if (id == peak_enhancement_.GetTrackId()) {
    return peak_enhancement_.Hover(id);
  } else if (id == short_waves_fade_.GetTrackId()) {
    return short_waves_fade_.Hover(id);
  } else /*if (id == lambda_.GetTrackId())*/ {
    return lambda_.Hover(id);
  }
}

void UiOceanCascadeConfig::Release() {
  scale_.Release();
  fetch_.Release();
  spread_blend_.Release();
  swell_.Release();
  peak_enhancement_.Release();
  short_waves_fade_.Release();
  lambda_.Release();
}

void UiOceanCascadeConfig::Render(glm::vec2 mouse_pos) {
  scale_.Render(mouse_pos);
  fetch_.Render(mouse_pos);
  spread_blend_.Render(mouse_pos);
  swell_.Render(mouse_pos);
  peak_enhancement_.Render(mouse_pos);
  short_waves_fade_.Render(mouse_pos);
  lambda_.Render(mouse_pos);
}

void UiOceanCascadeConfig::RenderIcons() {
  scale_.RenderIcon();
  fetch_.RenderIcon();
  spread_blend_.RenderIcon();
  swell_.RenderIcon();
  peak_enhancement_.RenderIcon();
  short_waves_fade_.RenderIcon();
  lambda_.RenderIcon();
}

void UiOceanCascadeConfig::RenderPicking() const {
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();
}

UiPopUpBase::UiPopUpBase(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    glm::vec2 start_translate,
    glm::vec2 end_translate,
    glm::vec2 start_scale,
    glm::vec2 end_scale,
    float start_angle,
    float end_angle)
    : sprite_(sprite),
      start_translate_(start_translate),
      end_translate_(end_translate),
      start_scale_(start_scale),
      end_scale_(end_scale),
      start_angle_(start_angle),
      end_angle_(end_angle),
      shared_resources_(shared_resources),
      speed_(4.0f) {}

bool UiPopUpBase::RenderBack(bool show) {
  shared_resources_.dynamic_sprite_shader_.Bind();
  if (show) {
    progress_ += speed_ * gDeltaTime;
    if (progress_ >= 1.0f) {
      back_ready_ = true;
    }
  } else {
    progress_ -= speed_ * gDeltaTime;
    if (progress_ <= 0.0f) {
      return false;
    }
  }
  progress_ = std::clamp(progress_, 0.0f, 1.0f);
  CubicInterpolation();
  glUniformMatrix3fv(shader::kSpriteTransform, 1, false,
                     glm::value_ptr(cur_transform_));
  sprite_.Render();
  return true;
}

void UiPopUpBase::RenderPickingBack() {
  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  glUniformMatrix3fv(shader::kSpriteTransform, 1, false,
                     glm::value_ptr(cur_transform_));
  sprite_.RenderPicking();
}

void UiPopUpBase::CubicInterpolation() {
  cur_transform_ = glm::mat3(1.0f);
  glm::vec2 translate =
      start_translate_ + progress_ * (end_translate_ - start_translate_);
  glm::vec2 scale =
      start_scale_ + progress_ * (end_scale_ - start_scale_);
  float angle =
      start_angle_ + progress_ * (end_angle_ - start_angle_);
  cur_transform_ = glm::scale(cur_transform_, scale);
  cur_transform_ = glm::rotate(cur_transform_, glm::radians(angle));
  cur_transform_ = glm::translate(cur_transform_, translate);
}

UiWaterLayerConfig::UiWaterLayerConfig(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    glm::vec2 start_translate,
    glm::vec2 end_translate,
    glm::vec2 start_scale,
    glm::vec2 end_scale,
    float start_angle,
    float end_angle,
    UiStaticSprite&& sprite_layer,
    UiSliderH&& scale,
    UiSliderH&& fetch,
    UiSliderH&& spread_blend,
    UiSliderH&& swell,
    UiSliderH&& peak_enhancement,
    UiSliderH&& short_waves_fade,
    UiSliderH&& lambda)
    : Base(std::move(sprite), shared_resources, start_translate,
           end_translate, start_scale, end_scale, start_angle, end_angle),
      sprite_layer_(std::move(sprite_layer)),
      scale_(std::move(scale)),
      fetch_(std::move(fetch)),
      spread_blend_(std::move(spread_blend)),
      swell_(std::move(swell)),
      peak_enhancement_(std::move(peak_enhancement)),
      short_waves_fade_(std::move(short_waves_fade)),
      lambda_(std::move(lambda)),
      ui_event_handler_({
          &sprite_layer_, &scale_, &fetch_, &spread_blend_, &swell_,
          &peak_enhancement_, &short_waves_fade_, &lambda_
      }),
      shared_resources_(shared_resources) {}

// returns "stop render"
bool UiWaterLayerConfig::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.dynamic_sprite_shader_.Bind();
  /// transform matrix already bind by Base class
  auto mouse_pos =
      shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  sprite_layer_.Render();
  scale_.Render(mouse_pos);
  fetch_.Render(mouse_pos);
  spread_blend_.Render(mouse_pos);
  swell_.Render(mouse_pos);
  peak_enhancement_.Render(mouse_pos);
  short_waves_fade_.Render(mouse_pos);
  lambda_.Render(mouse_pos);

  scale_.ApplyTransform(Base::cur_transform_);
  fetch_.ApplyTransform(Base::cur_transform_);
  spread_blend_.ApplyTransform(Base::cur_transform_);
  swell_.ApplyTransform(Base::cur_transform_);
  peak_enhancement_.ApplyTransform(Base::cur_transform_);
  short_waves_fade_.ApplyTransform(Base::cur_transform_);
  lambda_.ApplyTransform(Base::cur_transform_);

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
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  sprite_layer_.RenderPicking();
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();
}

UiToggle::UiToggle(
    UiStaticSprite&& off, UiStaticSprite&& on1,
    UiStaticSprite&& on2, UiStaticSprite&& on3)
    : UiCallable(off.ui_data_, {}),
      off_(std::move(off)),
      on1_(std::move(on1)),
      on2_(std::move(on2)),
      on3_(std::move(on3)),
      state_(&off_),
      speed_(0.5f) {
  off_.ui_data_.children_num = 3;
  vbos::UiData ui_data = off_.ui_data_;
  gUiComponents[ui_data.id - details::kIdOffsetUi] = ui_data;
}

void UiToggle::Render() {
  if (turned_off_) {
    state_ = &off_;
  } else {
    progress_ += speed_ * gDeltaTime;
    // taking fractal part (looping)
    progress_ = progress_ - static_cast<float>(static_cast<int>(progress_));
    if (progress_ < 0.33f) {
      state_ = &on1_;
    } else if (progress_ < 0.67f) {
      state_ = &on2_;
    } else {
      state_ = &on3_;
    }
  }
  state_->Render();
}

void UiToggle::RenderPicking() const {
  off_.RenderPicking();
}

void UiToggle::Press() {
  turned_off_ = !turned_off_;
  progress_ = 0.0f;
}

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
    : sprites_{
          {std::move(sprite0), std::move(sprite10), std::move(sprite20),
           std::move(sprite30), std::move(sprite40), std::move(sprite50),
           std::move(sprite60), std::move(sprite70), std::move(sprite80),
           std::move(sprite90), std::move(sprite100)}} {
  sprites_[0].ui_data_.children_num = 10;
  vbos::UiData ui_data = sprites_[0].ui_data_;
  gUiComponents[ui_data.id - details::kIdOffsetUi] = ui_data;
}

void UiLoading::Render(float progress) {
  int idx = static_cast<int>(std::round(progress * 10.0f));
  idx = std::clamp(idx, 0, 10);
  sprites_[idx].Render();
}

void UiLoading::RenderPicking() const {
  sprites_[0].RenderPicking();
}

UiWindowBase::UiWindowBase(
    UiStaticSprite&& sprite, SharedResources& shared_resources)
    : sprite_(std::move(sprite)),
      shared_resources_(shared_resources),
      speed_(4.0f) {}

/// back_ready_==false when appearing or disappearing animation
/// returs false when disappearing fading is over
bool UiWindowBase::RenderBack(bool show) {
  shared_resources_.mask_sprite_shader_.Bind();
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
  shared_resources_.tex_ui_mask_.Bind();
  glUniform1f(shader::kSpriteProgress, progress_);
  sprite_.Render();
  glActiveTexture(GL_TEXTURE0); // go back to default
  return true;
}

void UiWindowBase::RenderPickingBack() {
  shared_resources_.static_sprite_picking_shader_.Bind();
  sprite_.RenderPicking();
}

UiSettings::UiSettings(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,
    UiStaticSprite&& resolution,
    UiStaticSprite&& music,
    UiStaticSprite&& sound,
    UiStaticSprite&& sensitivity,
    UiStaticSprite&& keyboard,
    UiToggle&& toggle_music,
    UiToggle&& toggle_sound,
    UiToggle&& toggle_3,
    UiToggle&& toggle_4,
    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
      resolution_(std::move(resolution)),
      music_(std::move(music)),
      sound_(std::move(sound)),
      sensitivity_(std::move(sensitivity)),
      keyboard_(std::move(keyboard)),
      toggle_music_(std::move(toggle_music)),
      toggle_sound_(std::move(toggle_sound)),
      toggle_3_(std::move(toggle_3)),
      toggle_4_(std::move(toggle_4)),
      ui_event_handler_({
          &resolution_, &music_, &sound_, &sensitivity_, &keyboard_,
          &toggle_music_, &toggle_sound_, &toggle_3_, &toggle_4_
      }),
      cross_(std::move(cross)),
      shared_resources_(shared_resources) {}

// returns "stop render"
bool UiSettings::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

  resolution_.Render();
  music_.Render();
  sound_.Render();
  sensitivity_.Render();
  keyboard_.Render();

  toggle_music_.Render();
  toggle_sound_.Render();
  toggle_3_.Render();
  toggle_4_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiSettings::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

  resolution_.RenderPicking();
  music_.RenderPicking();
  sound_.RenderPicking();
  sensitivity_.RenderPicking();
  keyboard_.RenderPicking();

  toggle_music_.RenderPicking();
  toggle_sound_.RenderPicking();
  toggle_3_.RenderPicking();
  toggle_4_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

UiTabMenu::UiTabMenu(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,

    UiStaticSprite&& btn_mode_terrain,
    UiStaticSprite&& btn_mode_water,
    UiStaticSprite&& btn_mode_roads,
    UiStaticSprite&& btn_mode_fences,
    UiStaticSprite&& btn_mode_placement,
    UiStaticSprite&& btn_mode_objects,
    UiStaticSprite&& btn_mode_biomes,
    UiStaticSprite&& btn_mode_tiles,

    UiStaticSprite&& btn_vision_terrain,
    UiStaticSprite&& btn_vision_water,
    UiStaticSprite&& btn_vision_roads,
    UiStaticSprite&& btn_vision_fences,
    UiStaticSprite&& btn_vision_placement,
    UiStaticSprite&& btn_vision_objects,
    UiStaticSprite&& btn_vision_biomes,
    UiStaticSprite&& btn_vision_tiles,

    UiStaticSprite&& btn_settings,
    UiStaticSprite&& btn_shader_wirebound,

    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
      btn_mode_terrain_(std::move(btn_mode_terrain)),
      btn_mode_water_(std::move(btn_mode_water)),
      btn_mode_roads_(std::move(btn_mode_roads)),
      btn_mode_fences_(std::move(btn_mode_fences)),
      btn_mode_placement_(std::move(btn_mode_placement)),
      btn_mode_objects_(std::move(btn_mode_objects)),
      btn_mode_biomes_(std::move(btn_mode_biomes)),
      btn_mode_tiles_(std::move(btn_mode_tiles)),

      btn_vision_terrain_(std::move(btn_vision_terrain)),
      btn_vision_water_(std::move(btn_vision_water)),
      btn_vision_roads_(std::move(btn_vision_roads)),
      btn_vision_fences_(std::move(btn_vision_fences)),
      btn_vision_placement_(std::move(btn_vision_placement)),
      btn_vision_objects_(std::move(btn_vision_objects)),
      btn_vision_biomes_(std::move(btn_vision_biomes)),
      btn_vision_tiles_(std::move(btn_vision_tiles)),

      btn_settings_(std::move(btn_settings)),
      btn_shader_wirebound_(std::move(btn_shader_wirebound)),
      ui_event_handler_({
        &btn_mode_terrain_,
        &btn_mode_water_,
        &btn_mode_roads_,
        &btn_mode_fences_,
        &btn_mode_placement_,
        &btn_mode_objects_,
        &btn_mode_biomes_,
        &btn_mode_tiles_,

        &btn_vision_terrain_,
        &btn_vision_water_,
        &btn_vision_roads_,
        &btn_vision_fences_,
        &btn_vision_placement_,
        &btn_vision_objects_,
        &btn_vision_biomes_,
        &btn_vision_tiles_,

        &btn_settings_,
        &btn_shader_wirebound_
      }),
      cross_(std::move(cross)),
      shared_resources_(shared_resources) {}

// returns "stop render"
bool UiTabMenu::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

  btn_mode_terrain_.Render();
  btn_mode_water_.Render();
  btn_mode_roads_.Render();
  btn_mode_fences_.Render();
  btn_mode_placement_.Render();
  btn_mode_objects_.Render();
  btn_mode_biomes_.Render();
  btn_mode_tiles_.Render();

  btn_vision_terrain_.Render();
  btn_vision_water_.Render();
  btn_vision_roads_.Render();
  btn_vision_fences_.Render();
  btn_vision_placement_.Render();
  btn_vision_objects_.Render();
  btn_vision_biomes_.Render();
  btn_vision_tiles_.Render();

  btn_settings_.Render();
  btn_shader_wirebound_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiTabMenu::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_mode_terrain_.RenderPicking();
  btn_mode_water_.RenderPicking();
  btn_mode_roads_.RenderPicking();
  btn_mode_fences_.RenderPicking();
  btn_mode_placement_.RenderPicking();
  btn_mode_objects_.RenderPicking();
  btn_mode_biomes_.RenderPicking();
  btn_mode_tiles_.RenderPicking();

  btn_vision_terrain_.RenderPicking();
  btn_vision_water_.RenderPicking();
  btn_vision_roads_.RenderPicking();
  btn_vision_fences_.RenderPicking();
  btn_vision_placement_.RenderPicking();
  btn_vision_objects_.RenderPicking();
  btn_vision_biomes_.RenderPicking();
  btn_vision_tiles_.RenderPicking();

  btn_settings_.RenderPicking();
  btn_shader_wirebound_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

UiConfirmation::UiConfirmation(
    UiStaticSprite&& sprite,
    SharedResources& shared_resources,

    UiStaticSprite&& btn_close,
    UiStaticSprite&& btn_accept,
    UiStaticSprite&& btn_decline,

    UiDynamicSprite&& cross)
    : Base(std::move(sprite), shared_resources),
      btn_close_(std::move(btn_close)),
      btn_accept_(std::move(btn_accept)),
      btn_decline_(std::move(btn_decline)),
      ui_event_handler_({
          &btn_close_,
          &btn_accept_,
          &btn_decline_,
      }),
      cross_(std::move(cross)),
      shared_resources_(shared_resources) {}

// returns "stop render"
bool UiConfirmation::Render(bool show) {
  bool stop_show = Base::RenderBack(show);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  shared_resources_.static_sprite_shader_.Bind();

  btn_close_.Render();
  btn_accept_.Render();
  btn_decline_.Render();

  shared_resources_.dynamic_sprite_shader_.Bind();
  cross_.Render();

  return stop_show;
}

void UiConfirmation::RenderPicking() {
  Base::RenderPickingBack();
  if (!Base::BackIsReady()) {
    return;
  }
  shared_resources_.static_sprite_picking_shader_.Bind();

  btn_close_.RenderPicking();
  btn_accept_.RenderPicking();
  btn_decline_.RenderPicking();

  shared_resources_.dynamic_sprite_picking_shader_.Bind();
  cross_.RenderPicking();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//TODO: what about 5 slots with different sprites and positions (not aligned)
// THEN to render selected I simply change its vbo offset to 6th_sprite_selected
// to define selected - we set __selected_slot_sprite__;
// to define non-existent we simply don't move to it by btn_next/prev

//TODO: we need to render text (id number) as well

UiSlots::UiSlots(const std::size_t& total_size, UiStaticSprite&& btn_next,
                 UiStaticSprite&& btn_prev, UiStaticSprite&& slot1,
                 UiStaticSprite&& slot2, UiStaticSprite&& slot3,
                 UiStaticSprite&& slot4, UiStaticSprite&& slot5,
                 int& edit_mode_selected_sample_id,
                 const TextRenderer& text_renderer)
    : total_size_(total_size),
      slots_({slot1, slot2, slot3, slot4, slot5, btn_next, btn_prev}),
      edit_mode_selected_sample_id_(edit_mode_selected_sample_id),
      text_renderer_(text_renderer) {
  InitTextIdsPositions();
}

void UiSlots::InitTextIdsPositions() {
  for (int i = 0; i < text_ids_positions_.size(); ++i) {
    text_ids_positions_[i] = glm::vec2{
        (slots_[i].GetLeftBorder() + slots_[i].GetRightBorder()) / 2,
        (slots_[i].GetTopBorder() + slots_[i].GetBottomBorder()) / 2
    };
  }
}

void UiSlots::Render() {
  for (auto& slot : slots_) {
    slot.Render();
  }
}

void UiSlots::RenderPicking() const {
  for (const auto& slot : slots_) {
    slot.RenderPicking();
  }
}

void UiSlots::RenderIds() const {
  text_renderer_.Bind();
  for (int i = 0; i < text_ids_positions_.size(); ++i) {
    text_renderer_.RenderNumber(start_idx_ + i, text_ids_positions_[i]);
  }
}

bool UiSlots::Press(std::uint32_t id) {
  if (id >= slots_[0].GetId() && id <= slots_[slots_.size() - 1].GetId()) {
    // two last are btn_next, btn_prev
    if (id == slots_[slots_.size() - 2].GetId()) {
      if (start_idx_ + 5 < total_size_) {
        ++start_idx_;
      }
    } else if (id == slots_[slots_.size() - 1].GetId()) {
      if (start_idx_ > 0) {
        --start_idx_;
      }
    } else {
      edit_mode_selected_sample_id_ = start_idx_ + (id - slots_[0].GetId());
    }
    return true;
  } else {
    return false;
  }
}

size_t UiSlots::Hover(std::uint32_t id) {
  if (id >= slots_[0].GetId() && id <= slots_[slots_.size() - 1].GetId()) {
    return slots_[id - slots_[0].GetId()].Hover();
  }
  return {};
}

/*
UiBiomesList::UiBiomesList(
    int vbo_offset, int instances_num,
    UiButton&& btn_next, UiButton&& btn_prev)
    : vbo_offset_(vbo_offset),
      instances_num_(instances_num),
      points_data_(points_data),
      btn_next_(btn_next),
      btn_prev_(btn_prev),
      edit_mode_selected_sample_id_(edit_mode_selected_sample_id) {
  // vao has been already created at shared_resources initialization
}
*/

/*TODO:
 * instanced draw:
_pass_1: draw boxes (simply draw with the same sprites but different positions);
_pass_2: draw possible biomes (if less - simply don't draw - the same for picking)

_pass_picking: only _pass_2:
    */
/*
void UiBiomesList::Render() const {
  btn_next_.Render();
  btn_prev_.Render();
//  TODO: vao_.Bind(); here?
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, vbo_offset_, 4, instances_num_);
}

void UiBiomesList::RenderPicking() const {
  //
}

/// return true if there was a button with such id
bool UiBiomesList::Press(std::uint32_t id) {
  //
}

int UiBiomesList::Hover(std::uint32_t id) const {
  //
}
*/
