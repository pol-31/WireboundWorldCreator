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

void UiDynamicSprite::Render() const {
  glUniformMatrix3fv(3, 1, false, glm::value_ptr(transform_));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiDynamicSprite::RenderPicking() const {
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::Render() const {
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

void UiStaticSprite::RenderPicking() const {
  glUniform1ui(shader::kSpriteId, static_cast<uint32_t>(ui_data_.id));
  glDrawArrays(GL_TRIANGLE_STRIP, ui_data_.vbo_offset, 4);
}

UiSlider::UiSlider(
    UiStaticSprite&& fill_sprite,
    UiStaticSprite&& back_sprite,
    UiDynamicSprite&& icon_sprite,
    float scale)
    : fill_sprite_(fill_sprite),
      back_sprite_(back_sprite),
      icon_sprite_(icon_sprite),
      length_(fill_sprite_.GetTopBorder()
              - fill_sprite_.GetBottomBorder()),
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

void UiSlider::RenderPicking() {
  fill_sprite_.RenderPicking();
}

size_t UiSlider::Hover(std::uint32_t id) const {
  //TODO: set higher brightness?
  return fill_sprite_.Hover();
}

void UiSlider::Set(float related_pos) {
  float half_length_ = length_ / 2.0f;
  float offset = glm::clamp(related_pos - centre_,
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

void UiSlider::UnHover() {
  //TODO: set lower brightness?
}

float UiSlider::GetProgress() const {
  //TODO: make some *magic* with sprite and "return progress_;"
  return (1.0f - progress_) * scale_;
}

void UiSlider::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  length_ = scale *
            (fill_sprite_.GetTopBorder()
             - fill_sprite_.GetBottomBorder());
//  std::cout << "length: " << length_ << ' ' << scale
//            << ' ' << x_translate << ' ' << y_translate << std::endl;
//  std::cout << "and " << fill_sprite_.GetTopBorder() << ' '
//            << fill_sprite_.GetBottomBorder() << " id "
//            << fill_sprite_.GetVboOffset() << std::endl;
  centre_ = y_translate;

  float related_pos = progress_ * length_ - length_ / 2.0f + centre_;
  Set(related_pos);
}

void UiSlider::Render(float related_pos) {
  if (pressed_) {
    Set(related_pos);
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

void UiSlider::RenderIcon() {
  icon_sprite_.Render();
}

UiOceanCascadeConfig::UiOceanCascadeConfig(
    UiStaticSprite&& scale_fill, UiStaticSprite&& scale_back,
    UiDynamicSprite&& scale_icon, float scale_scale,
    UiStaticSprite&& fetch_fill, UiStaticSprite&& fetch_back,
    UiDynamicSprite&& fetch_icon, float fetch_scale,
    UiStaticSprite&& spread_blend_fill, UiStaticSprite&& spread_blend_back,
    UiDynamicSprite&& spread_blend_icon, float spread_blend_scale,
    UiStaticSprite&& swell_fill, UiStaticSprite&& swell_back,
    UiDynamicSprite&& swell_icon, float swell_scale,
    UiStaticSprite&& peaks_fill, UiStaticSprite&& peaks_back,
    UiDynamicSprite&& peaks_icon, float peaks_scale,
    UiStaticSprite&& fade_fill, UiStaticSprite&& fade_back,
    UiDynamicSprite&& fade_icon, float fade_scale,
    UiStaticSprite&& lambda_fill, UiStaticSprite&& lambda_back,
    UiDynamicSprite&& lambda_icon, float lambda_scale)
    : scale_(std::move(scale_fill), std::move(scale_back),
             std::move(scale_icon), scale_scale),
      fetch_(std::move(fetch_fill), std::move(fetch_back),
             std::move(fetch_icon), fetch_scale),
      spread_blend_(std::move(spread_blend_fill), std::move(spread_blend_back),
                    std::move(spread_blend_icon), spread_blend_scale),
      swell_(std::move(swell_fill), std::move(swell_back),
             std::move(swell_icon), swell_scale),
      peak_enhancement_(std::move(peaks_fill), std::move(peaks_back),
                        std::move(peaks_icon), peaks_scale),
      short_waves_fade_(std::move(fade_fill), std::move(fade_back),
                        std::move(fade_icon), fade_scale),
      lambda_(std::move(lambda_fill), std::move(lambda_back),
              std::move(lambda_icon), lambda_scale) {}

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

void UiOceanCascadeConfig::Render(float related_pos) {
  scale_.Render(related_pos);
  fetch_.Render(related_pos);
  spread_blend_.Render(related_pos);
  swell_.Render(related_pos);
  peak_enhancement_.Render(related_pos);
  short_waves_fade_.Render(related_pos);
  lambda_.Render(related_pos);
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

void UiOceanCascadeConfig::RenderPicking() {
  scale_.RenderPicking();
  fetch_.RenderPicking();
  spread_blend_.RenderPicking();
  swell_.RenderPicking();
  peak_enhancement_.RenderPicking();
  short_waves_fade_.RenderPicking();
  lambda_.RenderPicking();
}

UiTabAnimation::UiTabAnimation(
    UiDynamicSprite&& sprite, glm::vec2 start_pos,
    glm::vec2 end_pos, float speed)
    : sprite_(sprite),
      start_pos_(start_pos),
      end_pos_(end_pos),
      speed_(speed) {
  End();
}

void UiTabAnimation::Render() {
  if (progress_ < 1.0f) {
    /// if active
    progress_ += gDeltaTime * speed_;
    glm::vec2 position = glm::mix(start_pos_, end_pos_, progress_);
    sprite_.SetTransform(glm::translate(glm::mat3{}, position));
    if (progress_ > 1.0f) {
      progress_ = 1.0f;
    }
  }
  sprite_.Render();
}

void UiTabAnimation::Start() {
  progress_ = 0.0f;
}

void UiTabAnimation::End() {
  sprite_.SetTransform(glm::translate(glm::mat3{}, start_pos_));
  progress_ = 1.0f;
}

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

void UiSlots::Render() const {
  for (const auto& slot : slots_) {
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

size_t UiSlots::Hover(std::uint32_t id) const {
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
