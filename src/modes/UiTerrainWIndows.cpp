#include "UiTerrainWIndows.h"

#include "../core/TileRenderer.h"

UiEditTerrainNoise::UiEditTerrainNoise(
    UiSharedResources& ui_shared_resources,
    UiDynamicSprite&& config,
    UiTextLabelId&& text_name,
    UiToggle&& toggle_invert,
    UiToggle&& toggle_tiling,
    UiSliderH2&& slider_strength,
    UiDynamicSprite&& hmap)
    : config_(std::move(config)),
      text_name_(std::move(text_name)),
      toggle_invert_(std::move(toggle_invert)),
      toggle_tiling_(std::move(toggle_tiling)),
      slider_strength_(std::move(slider_strength)),
      ui_shared_resources_(ui_shared_resources),
      hmap_(std::move(hmap)) {}

void UiEditTerrainNoise::Render(const Texture32F& hmap_ref) {
  ui_shared_resources_.hmap_shader_.Bind();
  hmap_ref.Bind();
  hmap_.Render();
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  config_.Render();
  toggle_invert_.Render();
  toggle_tiling_.Render();
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  slider_strength_.Render(mouse_pos);
  // only after (due to internal shader & texture modification)
  text_name_.Render();
}

void UiEditTerrainNoise::RenderPicking() {
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  config_.RenderPicking();
  toggle_invert_.RenderPicking();
  toggle_tiling_.RenderPicking();
  slider_strength_.RenderPicking();
  hmap_.RenderPicking();
  // only after (due to internal shader & texture modification)
  if (debug::gCtrlMode) {
    text_name_.RenderPicking();
  }
}

void UiEditTerrainNoise::SetParentId(int id) {
  gUiComponents[config_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
  gUiComponents[toggle_invert_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
  gUiComponents[toggle_tiling_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
  gUiComponents[slider_strength_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
  gUiComponents[text_name_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
  gUiComponents[hmap_.GetId() - details::kIdOffsetUi].parent_id_
      = id;
}

void UiEditTerrainNoise::UpdateTransform() {
  config_.UpdateTransform();
  toggle_invert_.UpdateTransform();
  toggle_tiling_.UpdateTransform();
  slider_strength_.UpdateTransform();
  text_name_.UpdateTransform();
  hmap_.UpdateTransform();
}

void UiEditTerrainNoise::SetConfig(NoiseDataBase* terrain_data) {
  toggle_invert_.Set(terrain_data->do_invert);
  toggle_tiling_.Set(terrain_data->do_tiling);
  slider_strength_.SetValue(terrain_data->strength);
}

UiEditTerrain::UiEditTerrain(
    Tile& cur_tile,
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    UiDynamicSprite&& accept,
    UiDynamicSprite&& name,
    UiDynamicSprite&& name_back,
    UiSlider2D&& color_palette,
    UiSliderH2&& color_brightness,
    UiDynamicSprite&& color_indicator,
    UiDynamicSprite&& random_generate,
    const std::vector<TerrainInstanceData>& instances,
    const int& instances_size)
    : Base(std::move(sprite), size_scale, std::move(pin),
           ui_shared_resources, window_queue),
      accept_(std::move(accept)),
      name_(std::move(name)),
      name_back_(std::move(name_back)),
      color_palette_(std::move(color_palette)),
      color_brightness_(std::move(color_brightness)),
      color_indicator_(std::move(color_indicator)),
      hmap_heights_(cur_tile.terrain_heights_),
      tex_hmap_(cur_tile.map_terrain_height),
      random_generate_(std::move(random_generate)),

      text_noise_invert_({
          text_renderer,0.05f, glm::vec2{0.0f},
          {data::VboIdMain::kTerrainEditLabelInvert, data::TextId::kNotYet},
          data::TextId::com}),
      text_noise_tiling_(
          {text_renderer, 0.05f, glm::vec2{0.0f},
           {data::VboIdMain::kTerrainEditLabelTiling, data::TextId::kNotYet},
           data::TextId::com}),
      text_noise_strength_(
          {text_renderer, 0.05f, glm::vec2{0.0f},
           {data::VboIdMain::kTerrainEditLabelStrength, data::TextId::kNotYet},
           data::TextId::com}),

      shader_merge_("../shaders/noise_shaders/MergeNoises.comp"),
      shader_sum_("../shaders/noise_shaders/SumNoises.comp"),

      noise1_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise1Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise1Name, data::TextId::kNotYet},
               data::TextId::kPerlin},
              {{data::VboIdMain::kTerrainEditNoise1InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise1TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise1StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise1StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise1Hmap, data::TextId::kNotYet}),
      noise2_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise2Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise2Name, data::TextId::kNotYet},
               data::TextId::kCellular},
              {{data::VboIdMain::kTerrainEditNoise2InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise2TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise2StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise2StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise2Hmap, data::TextId::kNotYet}),
      noise3_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise3Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise3Name, data::TextId::kNotYet},
               data::TextId::kMetaballs},
              {{data::VboIdMain::kTerrainEditNoise3InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise3TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise3StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise3StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise3Hmap, data::TextId::kNotYet}),
      noise4_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise4Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise4Name, data::TextId::kNotYet},
               data::TextId::kFbmGrid},
              {{data::VboIdMain::kTerrainEditNoise4InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise4TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise4StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise4StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise4Hmap, data::TextId::kNotYet}),
      noise5_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise5Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise5Name, data::TextId::kNotYet},
               data::TextId::kFbmMulti},
              {{data::VboIdMain::kTerrainEditNoise5InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise5TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise5StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise5StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise5Hmap, data::TextId::kNotYet}),
      noise6_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise6Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise6Name, data::TextId::kNotYet},
               data::TextId::kFbmdPerlin},
              {{data::VboIdMain::kTerrainEditNoise6InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise6TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise6StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise6StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise6Hmap, data::TextId::kNotYet}),
      noise7_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise7Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise7Name, data::TextId::kNotYet},
               data::TextId::kFbmWarp},
              {{data::VboIdMain::kTerrainEditNoise7InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise7TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise7StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise7StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise7Hmap, data::TextId::kNotYet}),
      noise8_(ui_shared_resources,
              {data::VboIdMain::kTerrainEditNoise8Config, data::TextId::kNotYet},
              {text_renderer, 0.025f, glm::vec2{0.0f},
               {data::VboIdMain::kTerrainEditNoise8Name, data::TextId::kNotYet},
               data::TextId::kFbmPerlinWarp},
              {{data::VboIdMain::kTerrainEditNoise8InvertOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8InvertOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8InvertOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8InvertOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise8TilingOff, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8TilingOn1, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8TilingOn2, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8TilingOn3, data::TextId::kNotYet}},
              {{data::VboIdMain::kTerrainEditNoise8StrengthArea, data::TextId::kNotYet},
               {data::VboIdMain::kTerrainEditNoise8StrengthIcon, data::TextId::kNotYet}},
              {data::VboIdMain::kTerrainEditNoise8Hmap, data::TextId::kNotYet}),

      noise_perlin_(ui_shared_resources, window_queue, text_renderer),
      noise_cellular_(ui_shared_resources, window_queue, text_renderer),
      noise_metaballs_(ui_shared_resources, window_queue, text_renderer),
      noise_fbm_grid_(ui_shared_resources, window_queue, text_renderer),
      noise_fbm_multi_(ui_shared_resources, window_queue, text_renderer),
      noise_fbmd_perlin_(ui_shared_resources, window_queue, text_renderer),
      noise_fbm_warp_(ui_shared_resources, window_queue, text_renderer),
      noise_fmb_perlin_warp_(ui_shared_resources, window_queue, text_renderer),

      ui_event_handler_({
          &pin_,
          &noise1_.config_, &noise1_.toggle_invert_, &noise1_.toggle_tiling_, &noise1_.slider_strength_,
          &noise2_.config_, &noise2_.toggle_invert_, &noise2_.toggle_tiling_, &noise2_.slider_strength_,
          &noise3_.config_, &noise3_.toggle_invert_, &noise3_.toggle_tiling_, &noise3_.slider_strength_,
          &noise4_.config_, &noise4_.toggle_invert_, &noise4_.toggle_tiling_, &noise4_.slider_strength_,
          &noise5_.config_, &noise5_.toggle_invert_, &noise5_.toggle_tiling_, &noise5_.slider_strength_,
          &noise6_.config_, &noise6_.toggle_invert_, &noise6_.toggle_tiling_, &noise6_.slider_strength_,
          &noise7_.config_, &noise7_.toggle_invert_, &noise7_.toggle_tiling_, &noise7_.slider_strength_,
          &noise8_.config_, &noise8_.toggle_invert_, &noise8_.toggle_tiling_, &noise8_.slider_strength_,
          &accept_, &name_back_, &color_palette_,
          &color_brightness_, &random_generate_
      }),
      ui_shared_resources_(ui_shared_resources),
      instances_(instances),
      instances_size_(instances_size) {
  Init();
  gUiComponents[pin_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[accept_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[name_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[name_back_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[color_palette_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[color_brightness_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[color_indicator_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[random_generate_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[text_noise_invert_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[text_noise_tiling_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[text_noise_strength_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  noise1_.SetParentId(sprite_.GetId());
  noise2_.SetParentId(sprite_.GetId());
  noise3_.SetParentId(sprite_.GetId());
  noise4_.SetParentId(sprite_.GetId());
  noise5_.SetParentId(sprite_.GetId());
  noise6_.SetParentId(sprite_.GetId());
  noise7_.SetParentId(sprite_.GetId());
  noise8_.SetParentId(sprite_.GetId());
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
  speed_ = 2.0f;
}

UiEditTerrain::UiEditTerrain(UiEditTerrain&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      name_(std::move(other.name_)),
      name_back_(std::move(other.name_back_)),
      color_palette_(std::move(other.color_palette_)),
      color_brightness_(std::move(other.color_brightness_)),
      color_indicator_(std::move(other.color_indicator_)),
      tex_hmap_(other.tex_hmap_),
      hmap_heights_(other.hmap_heights_),
      random_generate_(std::move(other.random_generate_)),

      text_noise_invert_(std::move(other.text_noise_invert_)),
      text_noise_tiling_(std::move(other.text_noise_tiling_)),
      text_noise_strength_(std::move(other.text_noise_strength_)),

      shader_merge_(std::move(other.shader_merge_)),
      shader_sum_(std::move(other.shader_sum_)),

      noise1_(std::move(other.noise1_)),
      noise2_(std::move(other.noise2_)),
      noise3_(std::move(other.noise3_)),
      noise4_(std::move(other.noise4_)),
      noise5_(std::move(other.noise5_)),
      noise6_(std::move(other.noise6_)),
      noise7_(std::move(other.noise7_)),
      noise8_(std::move(other.noise8_)),

      noise_perlin_(std::move(other.noise_perlin_)),
      noise_cellular_(std::move(other.noise_cellular_)),
      noise_metaballs_(std::move(other.noise_metaballs_)),
      noise_fbm_grid_(std::move(other.noise_fbm_grid_)),
      noise_fbm_multi_(std::move(other.noise_fbm_multi_)),
      noise_fbmd_perlin_(std::move(other.noise_fbmd_perlin_)),
      noise_fbm_warp_(std::move(other.noise_fbm_warp_)),
      noise_fmb_perlin_warp_(std::move(other.noise_fmb_perlin_warp_)),
      ui_event_handler_({
          &pin_,
          &noise1_.config_, &noise1_.toggle_invert_, &noise1_.toggle_tiling_, &noise1_.slider_strength_,
          &noise2_.config_, &noise2_.toggle_invert_, &noise2_.toggle_tiling_, &noise2_.slider_strength_,
          &noise3_.config_, &noise3_.toggle_invert_, &noise3_.toggle_tiling_, &noise3_.slider_strength_,
          &noise4_.config_, &noise4_.toggle_invert_, &noise4_.toggle_tiling_, &noise4_.slider_strength_,
          &noise5_.config_, &noise5_.toggle_invert_, &noise5_.toggle_tiling_, &noise5_.slider_strength_,
          &noise6_.config_, &noise6_.toggle_invert_, &noise6_.toggle_tiling_, &noise6_.slider_strength_,
          &noise7_.config_, &noise7_.toggle_invert_, &noise7_.toggle_tiling_, &noise7_.slider_strength_,
          &noise8_.config_, &noise8_.toggle_invert_, &noise8_.toggle_tiling_, &noise8_.slider_strength_,
          &accept_, &name_back_, &color_palette_,
          &color_brightness_, &random_generate_
      }),
      ui_shared_resources_(other.ui_shared_resources_),
      instances_(other.instances_),
      instances_size_(other.instances_size_) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

data::TextId UiEditTerrain::Hover(int id) {
  return ui_event_handler_.Hover(id);
}

void UiEditTerrain::UpdateHmap() {
  Texture32F hmap(details::gTerrainSize, GL_R32F);
  shader_sum_.Bind();
  for (int i = 0; i < instances_size_; ++i) {
    glBindImageTexture(
        0, hmap.GetId(), 0,
        GL_FALSE, 0, GL_READ_WRITE, hmap.GetFormat());
    glBindImageTexture(
        1, instances_[i].data.hmap.GetId(), 0,
        GL_FALSE, 0, GL_READ_ONLY, instances_[i].data.hmap.GetFormat());

    int size = details::gTerrainSize;
    glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }
  ui_shared_resources_.global_glfw_callback_data_.tile_renderer
      ->cur_tile_.map_terrain_height = std::move(hmap);
}

bool UiEditTerrain::Press(int id) {
  if (id == sprite_.GetId()) {
    return true;
  }
  if (id == pin_.GetId()) {
    pin_.Press();
    return true;
  }
  if (id == accept_.GetId()) {
    std::cout << "GENERATE TERRAIN" << std::endl;
    // can't be nullptr (not possible to get there -
    // - btn_settings is on slot_back)
    *terrain_data_ = Generate();
    UpdateHmap();
    return true;
  } else if (id == noise1_.config_.GetId()) {
    noise_perlin_.Show();
    return true;
  } else if (id == noise2_.config_.GetId()) {
    noise_cellular_.Show();
    return true;
  } else if (id == noise3_.config_.GetId()) {
    noise_metaballs_.Show();
    return true;
  } else if (id == noise4_.config_.GetId()) {
    noise_fbm_grid_.Show();
    return true;
  } else if (id == noise5_.config_.GetId()) {
    noise_fbm_multi_.Show();
    return true;
  } else if (id == noise6_.config_.GetId()) {
    noise_fbmd_perlin_.Show();
    return true;
  } else if (id == noise7_.config_.GetId()) {
    noise_fbm_warp_.Show();
    return true;
  } else if (id == noise8_.config_.GetId()) {
    noise_fmb_perlin_warp_.Show();
    return true;
  }
  return ui_event_handler_.Press(id);
}

void UiEditTerrain::Release() {
  ui_event_handler_.Release();
}

bool UiEditTerrain::Scroll(GLuint id, float yoffset) {
  return color_palette_.Scroll(id, yoffset) ||
         color_brightness_.Scroll(id, yoffset);
}

glm::vec4 HSBtoRGB(float h, float s, float b) {
  float r = 0, g = 0, bl = 0;

  if (s == 0.0f) {
    // Achromatic (gray)
    r = g = bl = b;
  } else {
    h = fmod(h, 1.0f) * 6.0f; // [0,6)
    int i = (int)floor(h);
    float f = h - i;
    float p = b * (1.0f - s);
    float q = b * (1.0f - s * f);
    float t = b * (1.0f - s * (1.0f - f));

    switch (i) {
      case 0: r = b; g = t; bl = p; break;
      case 1: r = q; g = b; bl = p; break;
      case 2: r = p; g = b; bl = t; break;
      case 3: r = p; g = q; bl = b; break;
      case 4: r = t; g = p; bl = b; break;
      case 5: r = b; g = p; bl = q; break;
    }
  }

  return {r, g, bl, 1.0f};
}

bool UiEditTerrain::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }

  ui_shared_resources_.static_sprite_shader_.Bind();
  accept_.Render();
  name_back_.Render();
  color_palette_.Render(mouse_pos);
  color_brightness_.Render(mouse_pos);


  glm::vec4 color = HSBtoRGB(
      color_palette_.GetProgressX(), color_palette_.GetProgressY(),
      color_brightness_.GetProgress());
  glUniform4fv(7, 1, glm::value_ptr(color));
  color_indicator_.Render();
  color = glm::vec4{1.0f};

  glUniform4fv(7, 1, glm::value_ptr(color));
  ui_shared_resources_.tex_ui_.Bind();
  random_generate_.Render();

  color_palette_.RenderIcon();
  color_brightness_.RenderIcon();

  //TODO: need pad
  if (terrain_data_) {
    noise1_.Render(terrain_data_->perlin.hmap);
    noise2_.Render(terrain_data_->cellular.hmap);
    noise3_.Render(terrain_data_->metaballs.hmap);
    noise4_.Render(terrain_data_->fbm_grid.hmap);
    noise5_.Render(terrain_data_->fbm_multi.hmap);
    noise6_.Render(terrain_data_->fbmd_perlin.hmap);
    noise7_.Render(terrain_data_->fbm_warp.hmap);
    noise8_.Render(terrain_data_->fbm_perlin_warp.hmap);
  }

  ui_shared_resources_.dynamic_sprite_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderText(name_, "name", 0.1f, glm::vec2{0.0f});
  text_noise_invert_.Render();
  text_noise_tiling_.Render();
  text_noise_strength_.Render();
  return false;
}

void UiEditTerrain::SetTerrainData(NoiseTerrainData* terrain_data) {
  terrain_data_ = terrain_data;
  noise_perlin_.SetConfig(terrain_data_->perlin);
  noise1_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->perlin));
  noise_cellular_.SetConfig(terrain_data_->cellular);
  noise2_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->cellular));
  noise_metaballs_.SetConfig(terrain_data_->metaballs);
  noise3_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->metaballs));
  noise_fbm_grid_.SetConfig(terrain_data_->fbm_grid);
  noise4_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->fbm_grid));
  noise_fbm_multi_.SetConfig(terrain_data_->fbm_multi);
  noise5_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->fbm_multi));
  noise_fbmd_perlin_.SetConfig(terrain_data_->fbmd_perlin);
  noise6_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->fbmd_perlin));
  noise_fbm_warp_.SetConfig(terrain_data_->fbm_warp);
  noise7_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->fbm_warp));
  noise_fmb_perlin_warp_.SetConfig(terrain_data_->fbm_perlin_warp);
  noise8_.SetConfig(static_cast<NoiseDataBase*>(&terrain_data_->fbm_perlin_warp));
}

void UiEditTerrain::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  accept_.RenderPicking();
  name_back_.RenderPicking();
  color_palette_.RenderPicking();
  color_brightness_.RenderPicking();
  color_indicator_.RenderPicking();
  random_generate_.RenderPicking();

  noise1_.RenderPicking();
  noise2_.RenderPicking();
  noise3_.RenderPicking();
  noise4_.RenderPicking();
  noise5_.RenderPicking();
  noise6_.RenderPicking();
  noise7_.RenderPicking();
  noise8_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.text_renderer
      ->RenderTextPicking(name_, "name", 0.1f, glm::vec2{0.0f});
  text_noise_invert_.RenderPicking();
  text_noise_tiling_.RenderPicking();
  text_noise_strength_.RenderPicking();
}

void UiEditTerrain::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  std::cout << "UPDATE" << std::endl;
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  accept_.UpdateTransform();
  name_.UpdateTransform();
  name_back_.UpdateTransform();
  color_palette_.UpdateTransform();
  color_brightness_.UpdateTransform();
  color_indicator_.UpdateTransform();
  random_generate_.UpdateTransform();

  text_noise_invert_.UpdateTransform();
  text_noise_tiling_.UpdateTransform();
  text_noise_strength_.UpdateTransform();

  noise1_.UpdateTransform();
  noise2_.UpdateTransform();
  noise3_.UpdateTransform();
  noise4_.UpdateTransform();
  noise5_.UpdateTransform();
  noise6_.UpdateTransform();
  noise7_.UpdateTransform();
  noise8_.UpdateTransform();

//  noise_perlin_.UpdateTransform();
//  noise_cellular_.UpdateTransform();
//  noise_metaballs.UpdateTransform();
//  noise_fbm_grid.UpdateTransform();
//  noise_fbm_multi_.UpdateTransform();
//  noise_fbmd_perlin_.UpdateTransform();
//  noise_fbm_warp_.UpdateTransform();
//  noise_fmb_perlin_warp.UpdateTransform();
}

NoiseTerrainData UiEditTerrain::Generate() {
  glm::vec2 resolution = glm::vec2{details::gTerrainSize};
  NoiseTerrainData terrain_data;
  terrain_data.hmap = Texture32F(resolution.x, GL_R32F);

  terrain_data.hmap.Store("hmap_start.png", 1, GL_RED);


  terrain_data.perlin = noise_perlin_.Generate(
      resolution, "hmap_perlin.png");
  MergeLayers(terrain_data.hmap, terrain_data.perlin.hmap,
              static_cast<NoiseDataBase*>(&terrain_data.perlin), noise1_);
  terrain_data.hmap.Store("hmap_end_1.png", 1, GL_RED);

  terrain_data.cellular = noise_cellular_.Generate(
      resolution, "hmap_cellular.png");
  MergeLayers(terrain_data.hmap, terrain_data.cellular.hmap,
              static_cast<NoiseDataBase*>(&terrain_data.cellular), noise2_);
  terrain_data.hmap.Store("hmap_end_2.png", 1, GL_RED);

  terrain_data.metaballs = noise_metaballs_.Generate(
      resolution, "hmap_metaballs.png");
  MergeLayers(terrain_data.hmap, terrain_data.metaballs.hmap,
              static_cast<NoiseDataBase*>(&terrain_data.metaballs), noise3_);
  terrain_data.hmap.Store("hmap_end_3.png", 1, GL_RED);

  terrain_data.fbm_grid = noise_fbm_grid_.Generate(
      resolution, "hmap_fbm_grid.png");
//  MergeLayers(terrain_data.hmap, terrain_data.fbm_grid.hmap,
//              static_cast<NoiseDataBase*>(&terrain_data.fbm_grid), noise4_);
  terrain_data.hmap.Store("hmap_end_4.png", 1, GL_RED);

  terrain_data.fbm_multi = noise_fbm_multi_.Generate(
      resolution, "hmap_fbm_multi.png");
//  MergeLayers(terrain_data.hmap, terrain_data.fbm_multi.hmap,
//              static_cast<NoiseDataBase*>(&terrain_data.fbm_multi), noise5_);
  terrain_data.hmap.Store("hmap_end_5.png", 1, GL_RED);

  terrain_data.fbmd_perlin = noise_fbmd_perlin_.Generate(
      resolution, "hmap_fbm_perlin.png");
//  MergeLayers(terrain_data.hmap, terrain_data.fbmd_perlin.hmap,
//              static_cast<NoiseDataBase*>(&terrain_data.fbmd_perlin), noise6_);
  terrain_data.hmap.Store("hmap_end_6.png", 1, GL_RED);

  terrain_data.fbm_warp = noise_fbm_warp_.Generate(
      resolution, "hmap_fbm_warp.png");
//  MergeLayers(terrain_data.hmap, terrain_data.fbm_warp.hmap,
//              static_cast<NoiseDataBase*>(&terrain_data.fbm_warp), noise7_);
  terrain_data.hmap.Store("hmap_end_7.png", 1, GL_RED);

  terrain_data.fbm_perlin_warp = noise_fmb_perlin_warp_.Generate(
      resolution, "hmap_fbm_perlin_warp.png");
//  MergeLayers(terrain_data.hmap, terrain_data.fbm_perlin_warp.hmap,
//              static_cast<NoiseDataBase*>(&terrain_data.fbm_perlin_warp), noise8_);
  terrain_data.hmap.Store("hmap_end_8.png", 1, GL_RED);

  terrain_data.hmap.Store("hmap_generated.png", 1, GL_RED);
  std::cout << "Generation :: DONE" << std::endl;
  glCopyImageSubData(
      terrain_data.hmap.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
      tex_hmap_.GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
      details::gTerrainSize, details::gTerrainSize, 1);
  return terrain_data;
}

void UiEditTerrain::MergeLayers(
    Texture32F& bottom_layer, Texture32F& top_layer,
    NoiseDataBase* noise_data, const UiEditTerrainNoise& noise) {
  noise_data->strength = noise.slider_strength_.GetProgress();
  noise_data->do_invert = !noise.toggle_invert_.IsOff();
  noise_data->do_tiling = !noise.toggle_tiling_.IsOff();
  std::cout << "merged with " << noise.slider_strength_.GetProgress()
            << ' ' << std::boolalpha << noise_data->do_invert
            << ' ' << noise_data->do_tiling << std::endl;
  shader_merge_.Bind();
  glUniform1i(0, static_cast<int>(noise_data->do_invert));
  //TODO: as well as transform_matrix:
//  glUniform1i(1, static_cast<int>(noise_data->do_tiling));
  glUniform1f(2, noise_data->strength);

  glBindImageTexture(
      0, bottom_layer.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, bottom_layer.GetFormat());
  glBindImageTexture(
      1, top_layer.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, top_layer.GetFormat());

  int size = details::gTerrainSize;
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void UiEditTerrain::Init() {
  hmap_heights_.resize(details::gTerrainSize * details::gTerrainSize);
  float vertices[] = {
      1.0f,  1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
  };
  unsigned int indices[] = {
      0, 1, 3,
      1, 2, 3
  };
  glGenVertexArrays(1, &vao_id_);
  glGenBuffers(1, &vbo_id_);
  glGenBuffers(1, &ebo_id_);

  glBindVertexArray(vao_id_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glGenTextures(1, &fbo_tex_id_);
  glBindTexture(GL_TEXTURE_2D, fbo_tex_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, details::gTerrainSize,
               details::gTerrainSize, 0, GL_RED, GL_FLOAT, nullptr);

  glGenFramebuffers(1, &fbo_id_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, fbo_tex_id_, 0);
}

void UiEditTerrain::DeInit() {
  glDeleteBuffers(1, &vbo_id_);
  glDeleteBuffers(1, &ebo_id_);
  glDeleteTextures(1, &fbo_tex_id_);
  glDeleteFramebuffers(1, &fbo_id_);
  glDeleteVertexArrays(1, &vao_id_);
}

UiTerrainBake::UiTerrainBake(
    Tile& cur_tile,
    UiDynamicSprite&& sprite,
    float size_scale,
    UiToggle2&& pin,
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer,
    UiDynamicSprite&& accept,
    UiTextLabelId&& erosion_label,
    UiDynamicSprite&& erosion_input,
    UiTextLabelId&& weathering_label,
    UiDynamicSprite&& weathering_input)
    : Base(std::move(sprite), size_scale, std::move(pin),
           ui_shared_resources, window_queue),
      accept_(std::move(accept)),
      erosion_label_(std::move(erosion_label)),
      erosion_input_(std::move(erosion_input)),
      weathering_label_(std::move(weathering_label)),
      weathering_input_(std::move(weathering_input)),
      sprite_hmap_(data::VboIdMain::kTerrainBakeHmap, data::TextId::kNotYet),
      sprite_nmap_(data::VboIdMain::kTerrainBakeNmap, data::TextId::kNotYet),
      sprite_slopemap_(data::VboIdMain::kTerrainBakeSlopemap, data::TextId::kNotYet),
      sprite_ao_(data::VboIdMain::kTerrainBakeAo, data::TextId::kNotYet),
      sprite_splatmap_(data::VboIdMain::kTerrainBakeSplatmap, data::TextId::kNotYet),
      sprite_erosion_thermal_(data::VboIdMain::kTerrainBakeErosionThermal, data::TextId::kNotYet),
      sprite_erosion_hydraulic_(data::VboIdMain::kTerrainBakeErosionHydraulic, data::TextId::kNotYet),
      sprite_water_accum_(data::VboIdMain::kTerrainBakeWaterAccum, data::TextId::kNotYet),
      sprite_water_flow_(data::VboIdMain::kTerrainBakeWaterFlow, data::TextId::kNotYet),
      ui_event_handler_({
          &pin_, &accept_, &erosion_input_, &weathering_input_
      }),
      ui_shared_resources_(ui_shared_resources),

      shader_gen_nmap_("../shaders/generate_shaders/NormalMap.comp"),
      shader_gen_slope_map_("../shaders/generate_shaders/SlopeMap.comp"),
      shader_gen_ao_map_("../shaders/generate_shaders/AoMap.comp"),
      shader_perturbate_("../shaders/generate_shaders/Perturbate.comp"),

      hmap_heights_(cur_tile.terrain_heights_),
      tex_hmap_(cur_tile.map_terrain_height),

      tex_nmap_(cur_tile.map_terrain_normal),
      tex_slope_map_(cur_tile.map_terrain_slope),
      tex_ao_map_(cur_tile.map_terrain_ao),
      tex_splat_map_(cur_tile.map_terrain_splat),

      tex_erosion_thermal_map_(cur_tile.map_terrain_erosion_thermal),
      tex_erosion_hydraulic_map_(cur_tile.map_terrain_erosion_hydraulic),
      tex_water_accum_(cur_tile.map_water_accum),
      tex_water_flow_(cur_tile.map_water_flow) {
  gUiComponents[accept_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[erosion_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[erosion_input_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[weathering_label_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[weathering_input_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[pin_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_hmap_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_nmap_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_slopemap_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_ao_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_splatmap_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_erosion_thermal_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_erosion_hydraulic_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_water_accum_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[sprite_water_flow_.GetId() - details::kIdOffsetUi].parent_id_
      = sprite_.GetId();
  gUiComponents[GetId() - details::kIdOffsetUi].ui =
      static_cast<UiBase*>(this);
  UiWindowBase::UpdateTransform();
  speed_ = 2.0f;
}

UiTerrainBake::UiTerrainBake(UiTerrainBake&& other) noexcept
    : Base(std::move(other)),
      accept_(std::move(other.accept_)),
      erosion_label_(std::move(other.erosion_label_)),
      erosion_input_(std::move(other.erosion_input_)),
      weathering_label_(std::move(other.weathering_label_)),
      weathering_input_(std::move(other.weathering_input_)),
      sprite_hmap_(std::move(other.sprite_hmap_)),
      sprite_nmap_(std::move(other.sprite_nmap_)),
      sprite_slopemap_(std::move(other.sprite_slopemap_)),
      sprite_ao_(std::move(other.sprite_ao_)),
      sprite_splatmap_(std::move(other.sprite_splatmap_)),
      sprite_erosion_thermal_(std::move(other.sprite_erosion_thermal_)),
      sprite_erosion_hydraulic_(std::move(other.sprite_erosion_hydraulic_)),
      sprite_water_accum_(std::move(other.sprite_water_accum_)),
      sprite_water_flow_(std::move(other.sprite_water_flow_)),
      ui_event_handler_({
          &pin_, &accept_, &erosion_input_, &weathering_input_
      }),
      ui_shared_resources_(other.ui_shared_resources_),

      shader_gen_nmap_(std::move(other.shader_gen_nmap_)),
      shader_gen_slope_map_(std::move(other.shader_gen_slope_map_)),
      shader_gen_ao_map_(std::move(other.shader_gen_ao_map_)),
      shader_perturbate_(std::move(other.shader_perturbate_)),

      hmap_heights_(other.hmap_heights_),

      tex_hmap_(other.tex_hmap_),
      tex_nmap_(other.tex_hmap_),
      tex_slope_map_(other.tex_hmap_),
      tex_ao_map_(other.tex_ao_map_),
      tex_splat_map_(other.tex_splat_map_),

      tex_erosion_thermal_map_(other.tex_erosion_thermal_map_),
      tex_erosion_hydraulic_map_(other.tex_erosion_hydraulic_map_),

      tex_water_accum_(other.tex_water_accum_),
      tex_water_flow_(other.tex_water_flow_) {
  gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
      = static_cast<UiBase*>(this);
}

data::TextId UiTerrainBake::Hover(int id) {
  return ui_event_handler_.Hover(id);
}

bool UiTerrainBake::Press(int id) {
  return ui_event_handler_.Press(id);
}

void UiTerrainBake::Release() {
  ui_event_handler_.Release();
}

bool UiTerrainBake::Scroll(GLuint id, float yoffset) {
  return false;
}

bool UiTerrainBake::Render() {
  bool stop_show = Base::RenderBack(true);
  if (!Base::BackIsReady()) {
    return stop_show;
  }
  ui_shared_resources_.hmap_shader_.Bind();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_height.Bind();
  sprite_hmap_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_normal.Bind();
  sprite_nmap_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_slope.Bind();
  sprite_slopemap_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_ao.Bind();
  sprite_ao_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_splat.Bind();
  sprite_splatmap_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_erosion_thermal.Bind();
  sprite_erosion_thermal_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_terrain_erosion_hydraulic.Bind();
  sprite_erosion_hydraulic_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_water_accum.Bind();
  sprite_water_accum_.Render();

  ui_shared_resources_.global_glfw_callback_data_.tile_renderer->cur_tile_.map_water_flow.Bind();
  sprite_water_flow_.Render();


  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  accept_.Render();
  erosion_input_.Render();
  weathering_input_.Render();

  erosion_label_.Render();
  weathering_label_.Render();
  return false;
}

void UiTerrainBake::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.static_sprite_picking_shader_.Bind();

  accept_.RenderPicking();

  sprite_hmap_.RenderPicking();
  sprite_nmap_.RenderPicking();
  sprite_slopemap_.RenderPicking();
  sprite_ao_.RenderPicking();
  sprite_splatmap_.RenderPicking();
  sprite_erosion_thermal_.RenderPicking();
  sprite_erosion_hydraulic_.RenderPicking();
  sprite_water_accum_.RenderPicking();
  sprite_water_flow_.RenderPicking();

  erosion_input_.RenderPicking();
  weathering_input_.RenderPicking();

  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  erosion_label_.RenderPicking();
  weathering_label_.RenderPicking();
}

void UiTerrainBake::UpdateTransform(
    float x_translate, float y_translate, float scale) {
  sprite_.UpdateTransform();
  pin_.UpdateTransform();

  accept_.UpdateTransform();

  sprite_hmap_.UpdateTransform();
  sprite_nmap_.UpdateTransform();
  sprite_slopemap_.UpdateTransform();
  sprite_ao_.UpdateTransform();
  sprite_splatmap_.UpdateTransform();
  sprite_erosion_thermal_.UpdateTransform();
  sprite_erosion_hydraulic_.UpdateTransform();
  sprite_water_accum_.UpdateTransform();
  sprite_water_flow_.UpdateTransform();

  erosion_label_.UpdateTransform();
  erosion_input_.UpdateTransform();
  weathering_label_.UpdateTransform();
  weathering_input_.UpdateTransform();
}

void UiTerrainBake::ErodeWithFlow(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum,
    int cycles) {
  int size = details::gTerrainSize;
  glm::vec2 dim = glm::vec2(size);
  float dt = 1.2f;
  float density = 1.0f;
  float evapRate = 0.001f;
  float depositionRate = 0.1f;
  float minVol = 0.01f;
  float friction = 0.05f;

  for(int i = 0; i < cycles; i++) {
    // Spawn droplet at a random position weighted by water accumulation to spawn more where water is abundant
    glm::vec2 newpos;
    {
      // Example: choose position randomly, but biased by water_accum
      // For simplicity, just random now
      newpos = glm::vec2(rand() % (int)dim.x, rand() % (int)dim.y);
    }

    Particle drop(newpos);
    drop.volume = water_accum[(int)newpos.y][(int)newpos.x];  // Use water accumulation as initial volume

    while(drop.volume > minVol) {
      glm::ivec2 ipos{static_cast<int>(drop.pos.x), static_cast<int>(drop.pos.y)};
      if (ipos.x < 0 || ipos.x >= (int)dim.x || ipos.y < 0 || ipos.y >= (int)dim.y) {
        break;
      }

      // Instead of updating speed by surface normal, move drop along flow_dir vector at current position
      glm::vec2 dir = flow_dir[ipos.y][ipos.x];  // assuming [row][col] access
      // Apply velocity along flow direction scaled by dt and drop volume and maybe some factor
      drop.speed.x = dir.x * dt * drop.volume;
      drop.speed.y = dir.y * dt * drop.volume;

      drop.pos.x += drop.speed.x;
      drop.pos.y += drop.speed.y;

      // Compute sediment capacity difference as before
      int x1 = ipos.x;
      int y1 = ipos.y;
      int x2 = (int)drop.pos.x;
      int y2 = (int)drop.pos.y;

      if (x2 < 0 || x2 >= (int)dim.x || y2 < 0 || y2 >= (int)dim.y) {
        break;
      }

      float heightDiff = hmap_heights_[y1 * size + x1]
                         - hmap_heights_[y2 * size + x2];
      float maxSediment = drop.volume * glm::length(drop.speed) * std::max(0.0f, heightDiff);

      float sdiff = maxSediment - drop.sediment;

      // Deposit or erode sediment
      drop.sediment += dt * depositionRate * sdiff;
      hmap_heights_[y1 * size + x1] -= dt * drop.volume * depositionRate * sdiff;
      hmap_heights_[y1 * size + x1] = std::max(0.0f, hmap_heights_[y1 * size + x1]);

      // Evaporate
      drop.volume *= (1.0f - dt * evapRate);
    }
  }
}


glm::vec3 UiTerrainBake::SurfaceNormal(int i, int j) {
  //"Physical" Height scaling of the map
  int size = details::gTerrainSize;
  double scale = 16.0f * 16.0f;
  /*
    Note: Surface normal is computed in this way, because the square-grid
    surface is meshed using triangles. To avoid spatial artifacts,
    you need to weight properly with all neighbors.
  */
  i = std::clamp(i, 1, size - 2);
  j = std::clamp(j, 1, size - 2);
  glm::vec3 n = glm::normalize(glm::vec3(scale*(hmap_heights_[i * size + j]-hmap_heights_[(i+1) * size + j]), 1.0, 0.0)) * glm::vec3(0.15);  //Positive X
  n += glm::normalize(glm::vec3(scale*(hmap_heights_[(i-1) * size + j]-hmap_heights_[i * size + j]), 1.0, 0.0)) * glm::vec3(0.15);  //Negative X
  n += glm::normalize(glm::vec3(0.0, 1.0, scale*(hmap_heights_[i * size + j]-hmap_heights_[i * size + j+1]))) * glm::vec3(0.15);    //Positive Y
  n += glm::normalize(glm::vec3(0.0, 1.0, scale*(hmap_heights_[i * size + j-1]-hmap_heights_[i * size + j]))) * glm::vec3(0.15);  //Negative Y

  //Diagonals! (This removes the last spatial artifacts)
  n += glm::normalize(
           glm::vec3(scale*(hmap_heights_[i * size + j]-hmap_heights_[(i+1) * size + j+1])/sqrt(2), sqrt(2),scale*(hmap_heights_[i * size + j]-hmap_heights_[(i + 1) * size + j+1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(hmap_heights_[i * size + j]-hmap_heights_[(i+1) * size + j-1])/sqrt(2), sqrt(2), scale*(hmap_heights_[i * size + j]-hmap_heights_[(i + 1) * size + j-1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(hmap_heights_[i * size + j]-hmap_heights_[(i-1) * size + j+1])/sqrt(2), sqrt(2), scale*(hmap_heights_[i * size + j]-hmap_heights_[(i - 1) * size + j+1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y
  n += glm::normalize(glm::vec3(scale*(hmap_heights_[i * size + j]-hmap_heights_[(i-1) * size + j-1])/sqrt(2), sqrt(2), scale*(hmap_heights_[i * size + j]-hmap_heights_[(i - 1) * size + j-1])/sqrt(2))) * glm::vec3(0.1);    //Positive Y

  return n;
}

void UiTerrainBake::ErodeWeathering(int cycles) {
  //Size of the heightmap array
  int size = details::gTerrainSize;
  glm::vec2 dim = glm::vec2(size);
  //Erosion Steps
  //  int remaining = 200000;
  //  int erosionstep = 1000;

  //Particle Properties
  float dt = 1.2;
  float density = 1.0;  //This gives varying amounts of inertia and stuff...
  float evapRate = 0.001;
  //  float depositionRate = 1.0f;
  float depositionRate = 0.1;

  float minVol = 0.01;
  float friction = 0.05;

  for(int i = 0; i < cycles; i++){

    //Spawn New Particle
    glm::vec2 newpos = glm::vec2(rand()%(int)dim.x, rand()%(int)dim.y);
    Particle drop(newpos);
    std::cout << "cycle " << i << std::endl;

    //As long as the droplet exists...
    while(drop.volume > minVol){
      glm::ivec2 ipos{static_cast<int>(drop.pos.x),
                      static_cast<int>(drop.pos.y)};
      //Floored Droplet Initial Position
      //Surface Normal at Position
      glm::vec3 n = SurfaceNormal(ipos.x, ipos.y);

      //Accelerate particle using newtonian mechanics using the surface normal.
      //F = ma, so a = F/m
      drop.speed.x += n.x * dt/(drop.volume*density);
      drop.speed.y += n.z * dt/(drop.volume*density);
      drop.pos.x   += drop.speed.x;
      drop.pos.y   += drop.speed.y;
      drop.speed.x *= (1.0-dt*friction);       //Friction Factor
      drop.speed.y *= (1.0-dt*friction);       //Friction Factor

      if (!(drop.pos.x >= 0.0f && drop.pos.y >= 0.0f &&
            drop.pos.x < dim.x && drop.pos.y < dim.y))
        break;

      //Compute sediment capacity difference
      float maxsediment =
          drop.volume * glm::length(drop.speed)
          * (hmap_heights_[(ipos.x) * size + ipos.y]
             - hmap_heights_[((int)drop.pos.x) * size + (int)drop.pos.y]);
      if(maxsediment < 0.0) maxsediment = 0.0;
      float sdiff = maxsediment - drop.sediment;

      //Act on the Heightmap and Droplet!
      drop.sediment += dt*depositionRate*sdiff;
      hmap_heights_[(ipos.x) * size + ipos.y] -= dt*drop.volume*depositionRate*sdiff;
      hmap_heights_[(ipos.x) * size + ipos.y] =
          std::max(0.0f, hmap_heights_[(ipos.x) * size + ipos.y]);

      //Evaporate the Droplet (Note: Proportional to Volume!
      // Better: Use shape factor to make proportional to the area instead.)
      drop.volume *= (1.0-dt*evapRate);
    }
  }
}


void UiTerrainBake::ProcessErosion(
    const std::vector<std::vector<glm::vec2>>& flow_dir,
    const std::vector<std::vector<float>>& water_accum,
    int iterations) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  auto start_height_map_data = hmap_heights_;

  ErodeWithFlow(flow_dir, water_accum, iterations);

  // Convert float -> uint8 for saving
  std::vector<float> hmap_heights_updated(width * height);
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = -hmap_heights_[i * size + j] + start_height_map_data[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
//          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_hydraulic_map_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_erosion_hydraulic_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
               GL_FLOAT, hmap_heights_updated.data());
  tex_erosion_hydraulic_map_.Store("erosion_wear.png", 1, GL_RED);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = hmap_heights_[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
//          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
               GL_FLOAT, hmap_heights_updated.data());
  tex_hmap_.Store("height_map_erosed.png", 1, GL_RED);
}

void UiTerrainBake::ProcessThermalWeathering(
    int iterations, float talus) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  auto start_height_map_data = hmap_heights_;

  ErodeWeathering(iterations);
  //  RunThermalWeathering(height_map_data, 50, 0.2f);

  // Convert float -> uint8 for saving
  std::vector<float> hmap_heights_updated(width * height);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = hmap_heights_[i * size + j] - start_height_map_data[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
//          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }

  tex_erosion_thermal_map_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_erosion_thermal_map_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0,
               GL_RED, GL_FLOAT, hmap_heights_updated.data());
  tex_erosion_thermal_map_.Store("weathering_wear.png", 1, GL_RED);

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      float acc = hmap_heights_[i * size + j];
      hmap_heights_updated[i * width + j] = acc;
//          static_cast<uint8_t>(std::clamp(acc, 0.0f, 1.0f) * 255.0f);
    }
  }
  tex_hmap_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_hmap_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
               GL_FLOAT, hmap_heights_updated.data());
  tex_hmap_.Store("height_map_weathered.png", 1, GL_RED);
}

void UiTerrainBake::GenerateSplatmap() {}

void UiTerrainBake::GenerateNmap() {
  shader_gen_nmap_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
  glBindImageTexture(
      1, tex_nmap_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_nmap_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_nmap_.Store("normal_map.png", 2, GL_RG, 1);
}

void UiTerrainBake::GenerateSlope() {
  shader_gen_slope_map_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_slope_map_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_slope_map_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_slope_map_.Store("slope_map.png", 1, GL_RED);
}

void UiTerrainBake::GenerateAo() {
  shader_gen_ao_map_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_ao_map_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_ao_map_.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_ao_map_.Store("ao_map.png", 1, GL_RED);
}

void UiTerrainBake::Perturbate() {
  Texture32F tex_hmap_new(details::gTerrainSize, GL_R32F);
  shader_perturbate_.Bind();
  glBindImageTexture(
      0, tex_hmap_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, tex_hmap_.GetFormat());
  glBindImageTexture(
      1, tex_hmap_new.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, tex_hmap_new.GetFormat());
  glDispatchCompute(details::gTerrainSize / 8, details::gTerrainSize / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  tex_hmap_ = std::move(tex_hmap_new);
  tex_hmap_.Store("tex_hmap_perturbated.png", 1, GL_RED);
}

void UiTerrainBake::ComputeFlowMaps(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;

  flow_dir.assign(height, std::vector<glm::vec2>(width, {0.0f, 0.0f}));
  // Start with 1 unit of water
  flow_accum.assign(height, std::vector<float>(width, 1.0f));

  // Step 1: Flow direction
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      float h0 = hmap_heights_[y * size + x];
      float maxSlope = 0.0f;
      glm::vec2 best = {0.0f, 0.0f};

      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dx == 0 && dy == 0) continue;

          float h1 = hmap_heights_[(y + dy) * size + x + dx];
          float drop = h0 - h1;
          float dist = std::sqrt(dx * dx + dy * dy);
          float slope = drop / dist;

          if (slope > maxSlope) {
            maxSlope = slope;
            best = { (float)dx, (float)dy };
          }
        }
      }

      float len = std::sqrt(best.x * best.x + best.y * best.y);
      if (len > 0.0f)
        flow_dir[y][x] = { best.x / len, best.y / len };
    }
  }

  // Step 2: Flow accumulation (simple topological sort by descending height)
  struct Cell {
    int x, y;
    float height;
  };
  std::vector<Cell> cells;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      cells.push_back({x, y, hmap_heights_[y * size + x]});

  std::sort(cells.begin(), cells.end(), [](const Cell& a, const Cell& b) {
    return a.height > b.height;
  });

  for (auto& cell : cells) {
    int x = cell.x;
    int y = cell.y;
    int nx = x + std::round(flow_dir[y][x].x);
    int ny = y + std::round(flow_dir[y][x].y);
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      flow_accum[ny][nx] += 0.1f * flow_accum[y][x];
    }
  }
}

void UiTerrainBake::GenerateFlowMap(
    std::vector<std::vector<glm::vec2>>& flow_dir,
    std::vector<std::vector<float>>& flow_accum) {
  int size = details::gTerrainSize;
  int height = details::gTerrainSize;
  int width = details::gTerrainSize;
  ComputeFlowMaps(flow_dir, flow_accum);

  //  int width = flow_accum[0].size();
  //  int height = flow_accum.size();

  // flow_dir: RGB -> (dx, dy, 0)
  std::vector<GLuint> flow_dir_data(width * height * 2);
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      int idx = (y * width + x) * 2;
      flow_dir_data[idx + 0] = static_cast<GLuint>((flow_dir[y][x].x * 0.5f + 0.5f) * 255); // map [-1,1] to [0,1]
      flow_dir_data[idx + 1] = static_cast<GLuint>((flow_dir[y][x].y * 0.5f + 0.5f) * 255);
    }

  // flow_accum: single channel
  std::vector<float> flow_accum_data(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      flow_accum_data[y * width + x] = std::log(1.0f + flow_accum[y][x]);
    }
  }

  tex_water_flow_ = Texture(
      details::gTerrainSize, details::gTerrainSize, GL_RG8,
      GL_LINEAR, GL_CLAMP_TO_EDGE);
  tex_water_flow_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0,
               GL_RG, GL_UNSIGNED_BYTE, flow_dir_data.data());
  tex_water_flow_.Store("water_flow.png", 3, GL_RGB);

  tex_water_accum_ = Texture32F(details::gTerrainSize, GL_R32F);
  tex_water_accum_.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
               GL_FLOAT, flow_accum_data.data());
  tex_water_accum_.Store("water_accum.png", 1, GL_RED);
}

void UiTerrainBake::UpdateCpuData() {
  tex_hmap_.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, hmap_heights_.data());
}

void UiTerrainBake::Bake(
    int steps_thermal, int steps_weathering, float talus) {
  Perturbate();
  std::cout << "CH1" << std::endl;
  UpdateCpuData();

  std::vector<std::vector<glm::vec2>> flow_dir;
  std::vector<std::vector<float>> flow_accum;
  GenerateFlowMap(flow_dir, flow_accum);
  std::cout << "CH4" << std::endl;

  /// erosion & thermal weathering
    ProcessErosion(flow_dir, flow_accum, steps_thermal);
//  ProcessErosion(flow_dir, flow_accum, 1000);
  std::cout << "CH5" << std::endl;
  UpdateCpuData();

    ProcessThermalWeathering(steps_weathering, talus);
//  ProcessThermalWeathering(1000, 0.1f);
  std::cout << "CH6" << std::endl;
  UpdateCpuData();

  GenerateNmap();
  std::cout << "CH7" << std::endl;
  GenerateSlope();
  std::cout << "CH8" << std::endl;
  GenerateAo();
  std::cout << "CH9 (final)" << std::endl;

  tex_hmap_.Store("hmap_end.png", 1, GL_RED);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, gWindowWidth, gWindowHeight);
  glBindVertexArray(0);
  std::cout << "Erosion :: DONE" << std::endl;
}
