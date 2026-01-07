#include "UiEditShared.h"

#include "../core/Ui.h"

int GetUiEditEntryId(float height, float size,
                     glm::vec2 mouse_pos, float start_height) {
  float entry_height = height / (size + 5);
  float cursor = start_height;
  // pretty heavy check on DBG, skip
  //  float top = sprite_.GetTopBorder();
  //  float bottom = sprite_.GetBottomBorder();
  //  if (mouse_pos.y > top || mouse_pos.y < bottom) {
  //    return -1;
  //  }
  for (int i = 0; i < size; ++i) {
    cursor -= entry_height;
    if (cursor < mouse_pos.y) {
      return i;
    }
  }
  return -1;
}

UiEditConfigSlCfg::UiEditConfigSlCfg(UiSharedResources& ui_shared_resources,
                                     TextRenderer& text_renderer)
    : btn_config_(data::VboIdMain::kTerrainEditNoiseConfig),
      txt_name_(text_renderer, {data::VboIdMain::kTerrainEditNoiseName},
                data::TextId::kPerlin),
      tg1_({data::VboIdMain::kTerrainEditNoiseInvertOff},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn1},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn2},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn3}),
      tg2_({data::VboIdMain::kTerrainEditNoiseTilingOff},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn1},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn2},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn3}),
      sl_strength_({data::VboIdMain::kTerrainEditNoiseStrengthArea},
                   {data::VboIdMain::kTerrainEditNoiseStrengthIcon}),
      ui_shared_resources_(ui_shared_resources) {}

void UiEditConfigSlCfg::Release() {
  sl_strength_.Release();
  pressed_strength_id_ = -1;
}

void UiEditConfigSlCfg::Render(float& strength, bool do_invert,
                                 bool do_tiling, glm::vec2 translate,
                                 bool update_strength, data::TextId text_id) {
  LocalTransform transform;
  transform.translate = translate;
  btn_config_.SetParentTransform(transform);
  tg1_.SetParentTransform(transform);
  tg2_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);

  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_config_.Render();
  tg1_.Set(do_invert);
  tg2_.Set(do_tiling);
  tg1_.Render();
  tg2_.Render();
  if (update_strength) {
    auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_;
    sl_strength_.Render(mouse_pos);
    strength = sl_strength_.GetProgress();
  } else {
    sl_strength_.SetValue(strength);
    sl_strength_.Render();
  }
  // only after (due to internal shader & texture modification)
  txt_name_.SetText(text_id);
  txt_name_.Render();
}

void UiEditConfigSlCfg::RenderPicking(
    float sprite_height, int num) {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_height / (num + 5);  // +pads
  for (int i = 0; i < num; ++i) {
    RenderPickingEntry(next_offset);
    next_offset.y -= entry_height;
  }
}

void UiEditConfigSlCfg::ResetTransform() {
  LocalTransform transform;
  btn_config_.SetParentTransform(transform);
  tg1_.SetParentTransform(transform);
  tg2_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
}

void UiEditConfigSlCfg::RenderPickingEntry(glm::vec2 translate) {
  LocalTransform transform;
  transform.translate = translate;
  btn_config_.SetParentTransform(transform);
  tg1_.SetParentTransform(transform);
  tg2_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  ui_shared_resources_.shader_sp_picking_.Bind();
  btn_config_.RenderPicking();
  tg1_.RenderPicking();
  tg2_.RenderPicking();
  sl_strength_.RenderPicking();
  // only after (due to internal shader & texture modification)
  if (debug::gUiAltMode) {
    txt_name_.RenderPicking();
  }
}

void UiEditConfigSlCfg::AttachToHierarchy(UiHierarchy& hierarchy) {
  hierarchy.AddNested(&btn_config_, &tg1_, &txt_name_, &tg2_, &sl_strength_);
}

UiEditConfigSlTxt::UiEditConfigSlTxt(UiSharedResources& ui_shared_resources,
                                     TextRenderer& text_renderer)
: sl_strength_({{data::VboIdMain::kBiomesEditNoiseStrengthArea},
                 {data::VboIdMain::kBiomesEditNoiseStrengthIcon}}),
txt_name_({text_renderer,
               {data::VboIdMain::kBiomesEditNoiseName},
               data::TextId::kPerlin}),
      ui_shared_resources_(ui_shared_resources) {}

bool UiEditConfigSlTxt::Press(int id, float height, int num) {
  ResetTransform();
  int pressed_line_id = GetUiEditEntryId(
      height, num, ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_,
      sl_strength_.GetTrackPtr()->GetTopBorder());
  if (id == sl_strength_.GetId()) {
    pressed_strength_id_ = pressed_line_id;
    sl_strength_.Press();
    return true;
  }
  return false;
}

void UiEditConfigSlTxt::Release() {
  sl_strength_.Release();
  pressed_strength_id_ = -1;
}

void UiEditConfigSlTxt::Render(
    float sprite_height, std::span<Trait> traits) {
  glm::vec2 offset{0.0f};
  float entry_height = sprite_height / (traits.size() + 5);  // +pads
  for (int i = 0; i < traits.size(); ++i) {
    RenderEntry(*traits[i].value, offset,
      pressed_strength_id_ == i, traits[i].label);
    offset.y -= entry_height;
  }
}

void UiEditConfigSlTxt::RenderPicking(
    float sprite_height, int num) {
  glm::vec2 next_offset = glm::vec2{0.0f};
  float entry_height = sprite_height / (num + 5);  // +pads
  for (int i = 0; i < num; ++i) {
    RenderPickingEntry(next_offset);
    next_offset.y -= entry_height;
  }
}

void UiEditConfigSlTxt::RenderEntry(
    float& strength, glm::vec2 translate,
    bool update_strength, data::TextId text_id) {
  LocalTransform transform;
  transform.translate = translate;
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);

  //  ui_shared_resources_.hmap_shader_.Bind();
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  if (update_strength) {
    auto mouse_pos = ui_shared_resources_.glfw_context_.cursor_pos_tex_norm_;
    sl_strength_.Render(mouse_pos);
    strength = sl_strength_.GetProgress();
  } else {
    sl_strength_.SetValue(strength);
    sl_strength_.Render();
  }
  txt_name_.SetText(text_id);
  txt_name_.Render();
}

void UiEditConfigSlTxt::RenderPickingEntry(glm::vec2 translate) {
  LocalTransform transform;
  transform.translate = translate;
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  ui_shared_resources_.shader_sp_picking_.Bind();
  sl_strength_.RenderPicking();
  // only after (due to internal shader & texture modification)
  if (debug::gUiAltMode) {
    txt_name_.RenderPicking();
  }
}

void UiEditConfigSlTxt::AttachToHierarchy(UiHierarchy& hierarchy) {
  hierarchy.AddNested(&sl_strength_, &txt_name_);
}

void UiEditConfigSlTxt::ResetTransform() {
  LocalTransform transform;
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
}
