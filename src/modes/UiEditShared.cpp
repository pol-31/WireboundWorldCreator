#include "UiEditShared.h"

#include "../core/Ui.h"

UiEditConfigTerrain::UiEditConfigTerrain(UiSharedResources& ui_shared_resources,
                                         TextRenderer& text_renderer)
    : btn_config_(data::VboIdMain::kTerrainEditNoiseConfig),
      txt_name_(text_renderer, {data::VboIdMain::kTerrainEditNoiseName},
                data::TextId::kPerlin),
      tg_invert_({data::VboIdMain::kTerrainEditNoiseInvertOff},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn1},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn2},
                 {data::VboIdMain::kTerrainEditNoiseInvertOn3}),
      tg_tiling_({data::VboIdMain::kTerrainEditNoiseTilingOff},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn1},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn2},
                 {data::VboIdMain::kTerrainEditNoiseTilingOn3}),
      sl_strength_({data::VboIdMain::kTerrainEditNoiseStrengthArea},
                   {data::VboIdMain::kTerrainEditNoiseStrengthIcon}),
      ui_shared_resources_(ui_shared_resources),
      sp_hmap_(data::VboIdMain::kTerrainEditNoiseHmap) {}

void UiEditConfigTerrain::ResetTransform() {
  LocalTransform transform;
  sp_hmap_.SetParentTransform(transform);
  btn_config_.SetParentTransform(transform);
  tg_invert_.SetParentTransform(transform);
  tg_tiling_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
}
void UiEditConfigTerrain::Render(float& strength, bool do_invert,
                                 bool do_tiling, glm::vec2 translate,
                                 bool update_strength, data::TextId text_id) {
  LocalTransform transform;
  transform.translate = translate;
  sp_hmap_.SetParentTransform(transform);
  btn_config_.SetParentTransform(transform);
  tg_invert_.SetParentTransform(transform);
  tg_tiling_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);

  //  ui_shared_resources_.hmap_shader_.Bind();
  //  glBindTexture(GL_TEXTURE_2D, terrain_data->hmap_id);
  //  hmap_.Render();
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_config_.Render();
  tg_invert_.Set(do_invert);
  tg_tiling_.Set(do_tiling);
  tg_invert_.Render();
  tg_tiling_.Render();
  if (update_strength) {
    auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
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

void UiEditConfigTerrain::RenderPicking(glm::vec2 translate) {
  LocalTransform transform;
  transform.translate = translate;
  sp_hmap_.SetParentTransform(transform);
  btn_config_.SetParentTransform(transform);
  tg_invert_.SetParentTransform(transform);
  tg_tiling_.SetParentTransform(transform);
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  ui_shared_resources_.shader_sp_picking_.Bind();
  btn_config_.RenderPicking();
  tg_invert_.RenderPicking();
  tg_tiling_.RenderPicking();
  sl_strength_.RenderPicking();
  sp_hmap_.RenderPicking();
  // only after (due to internal shader & texture modification)
  if (debug::gUiAltMode) {
    txt_name_.RenderPicking();
  }
}

void UiEditConfigTerrain::AttachToHierarchy(UiHierarchy& hierarchy) {
  hierarchy.AddNested(&btn_config_, &tg_invert_, &txt_name_, &tg_tiling_,
                      &sl_strength_, &sp_hmap_);
}

int GetUiEditEntryId(const UiDynamicSprite* sprite, float size,
                     glm::vec2 mouse_pos, float start_height) {
  float entry_height = sprite->GetHeight() / (size + 5);
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

UiEditConfigOcean::UiEditConfigOcean(UiSharedResources& ui_shared_resources,
                                     TextRenderer& text_renderer)
    : btn_config_(data::VboIdMain::kWaterEditOceanConfig),
      txt_name_(text_renderer, {data::VboIdMain::kWaterEditOceanLayerName},
                data::TextId::kPerlin),
      tg_visible_({data::VboIdMain::kWaterEditOceanVisibleOff},
                  {data::VboIdMain::kWaterEditOceanVisibleOn1},
                  {data::VboIdMain::kWaterEditOceanVisibleOn2},
                  {data::VboIdMain::kWaterEditOceanVisibleOn3}),
      ui_shared_resources_(ui_shared_resources) {}

void UiEditConfigOcean::ResetTransform() {
  LocalTransform transform;
  btn_config_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  tg_visible_.SetParentTransform(transform);
}
void UiEditConfigOcean::Render(bool do_show, glm::vec2 translate,
                               data::TextId text_id) {
  LocalTransform transform;
  transform.translate = translate;
  btn_config_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  tg_visible_.SetParentTransform(transform);

  //  ui_shared_resources_.hmap_shader_.Bind();
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_config_.Render();
  tg_visible_.Set(do_show);
  tg_visible_.Render();
  txt_name_.SetText(text_id);
  txt_name_.Render();
}

void UiEditConfigOcean::RenderPicking(glm::vec2 translate) {
  LocalTransform transform;
  transform.translate = translate;
  btn_config_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
  tg_visible_.SetParentTransform(transform);
  ui_shared_resources_.shader_sp_picking_.Bind();
  btn_config_.RenderPicking();
  tg_visible_.RenderPicking();
  // only after (due to internal shader & texture modification)
  if (debug::gUiAltMode) {
    txt_name_.RenderPicking();
  }
}

void UiEditConfigOcean::AttachToHierarchy(UiHierarchy& hierarchy) {
  hierarchy.AddNested(&btn_config_, &txt_name_, &tg_visible_);
}

UiEditConfigSlTxt::UiEditConfigSlTxt(UiSharedResources& ui_shared_resources,
                                     TextRenderer& text_renderer,
                                     UiSliderH2&& sl_strength_,
                                     UiTextModeId&& txt_name)
    : sl_strength_(std::move(sl_strength_)),
      txt_name_(std::move(txt_name)),
      ui_shared_resources_(ui_shared_resources) {}

void UiEditConfigSlTxt::ResetTransform() {
  LocalTransform transform;
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);
}
void UiEditConfigSlTxt::Render(float& strength,  // viscosity / transparency
                               glm::vec2 translate, bool update_strength,
                               data::TextId text_id) {
  LocalTransform transform;
  transform.translate = translate;
  sl_strength_.SetParentTransform(transform);
  txt_name_.SetParentTransform(transform);

  //  ui_shared_resources_.hmap_shader_.Bind();
  ui_shared_resources_.shader_sp_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  if (update_strength) {
    auto mouse_pos = ui_shared_resources_.gltf_context_.cursor_pos_tex_norm_;
    sl_strength_.Render(mouse_pos);
    strength = sl_strength_.GetProgress();
  } else {
    sl_strength_.SetValue(strength);
    sl_strength_.Render();
  }
  txt_name_.SetText(text_id);
  txt_name_.Render();
}

void UiEditConfigSlTxt::RenderPicking(glm::vec2 translate) {
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
