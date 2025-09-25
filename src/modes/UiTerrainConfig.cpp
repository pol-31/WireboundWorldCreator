#include "UiTerrainConfig.h"

/// UBOs? don't think so - we'd need extra 8 ubo ids;
/// not much overhead, but a little much for code & logic

Texture32F GenAndSave(std::string_view tex_name) {
  int size = details::gTerrainSize;
  Texture32F height_map(size, GL_R32F);
  glBindImageTexture(0, height_map.GetId(), 0, GL_FALSE, 0,
                     GL_WRITE_ONLY, height_map.GetFormat());
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  height_map.Store(tex_name, 1, GL_RED);
  return std::move(height_map);
}

TerrainNoisePerlin::TerrainNoisePerlin() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/Perlin.comp");
}

NoisePerlinData TerrainNoisePerlin::Generate() {
  NoisePerlinData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.seed = value_[2];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- Perlin noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_perlin.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoisePerlin::SetConfig(const NoisePerlinData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoisePerlin::GetTextId() const noexcept {
  return data::TextId::kPerlin;
}

TerrainNoiseCellular::TerrainNoiseCellular() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kJitter, data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/Cellular.comp");
}

NoiseCellularData TerrainNoiseCellular::Generate() {
  NoiseCellularData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.jitter = value_[2];
  data.seed = value_[3];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- Cellular noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.jitter
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_cellular.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseCellular::SetConfig(const NoiseCellularData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = config.jitter;
  value_[3] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseCellular::GetTextId() const noexcept {
  return data::TextId::kCellular;
}

TerrainNoiseMetaballs::TerrainNoiseMetaballs() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kJitter, data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/Metaballs.comp");
}

NoiseMetaballsData TerrainNoiseMetaballs::Generate() {
  NoiseMetaballsData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.jitter = value_[2];
  data.seed = value_[3];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- Metaballs noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.jitter
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_metaballs.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseMetaballs::SetConfig(const NoiseMetaballsData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = config.jitter;
  value_[3] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseMetaballs::GetTextId() const noexcept {
  return data::TextId::kMetaballs;
}

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kOctaves, data::TextId::kShift,
              data::TextId::kGain, data::TextId::kLacunarity,
              data::TextId::kWarpStrength, data::TextId::kOctaveFactor,
              data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/FbmGrid.comp");
}

NoiseFbmGridData TerrainNoiseFbmGrid::Generate() {
  NoiseFbmGridData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.octaves = static_cast<int>(value_[2]);
  data.shift = value_[3];
  data.gain = value_[4];
  data.lacunarity = value_[5];
  data.warp_strength = value_[6];
  data.octave_factor = value_[7];
  data.seed = value_[8];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- FbmGrid noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.octaves
            << ' ' << data.shift
            << ' ' << data.gain
            << ' ' << data.lacunarity
            << ' ' << data.warp_strength
            << ' ' << data.octave_factor
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_fbm_grid.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseFbmGrid::SetConfig(const NoiseFbmGridData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = static_cast<float>(config.octaves);
  value_[3] = config.shift;
  value_[4] = config.gain;
  value_[5] = config.lacunarity;
  value_[6] = config.warp_strength;
  value_[7] = config.octave_factor;
  value_[8] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseFbmGrid::GetTextId() const noexcept {
  return data::TextId::kFbmGrid;
}

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kLacunarity, data::TextId::kOctaves,
              data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/FbmMulti.comp");
}

NoiseFbmMultiData TerrainNoiseFbmMulti::Generate() {
  NoiseFbmMultiData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.lacunarity = value_[2];
  data.octaves = static_cast<int>(value_[3]);
  data.seed = value_[4];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- FbmMulti noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.lacunarity
            << ' ' << data.octaves
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_fbm_multi.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseFbmMulti::SetConfig(const NoiseFbmMultiData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = config.lacunarity;
  value_[3] = static_cast<float>(config.octaves);
  value_[4] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseFbmMulti::GetTextId() const noexcept {
  return data::TextId::kFbmMulti;
}

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kOctaves, data::TextId::kGain,
              data::TextId::kLacunarity, data::TextId::kSlopeness,
              data::TextId::kOctaveFactor, data::TextId::kSeed};
  shader_ = Shader("../shaders/noise_shaders/FbmdPerlin.comp");
}

NoiseFbmdPerlinData TerrainNoiseFbmdPerlin::Generate() {
  NoiseFbmdPerlinData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.octaves = static_cast<int>(value_[2]);
  data.gain = value_[3];
  data.lacunarity = value_[4];
  data.slopeness = value_[5];
  data.octave_factor = value_[6];
  data.seed = value_[7];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- FbmdPerlin noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.octaves
            << ' ' << data.gain
            << ' ' << data.lacunarity
            << ' ' << data.slopeness
            << ' ' << data.octave_factor
            << ' ' << data.seed
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_fbmd_perlin.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseFbmdPerlin::SetConfig(const NoiseFbmdPerlinData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = static_cast<float>(config.octaves);
  value_[3] = config.gain;
  value_[4] = config.lacunarity;
  value_[5] = config.slopeness;
  value_[6] = config.octave_factor;
  value_[7] = config.seed;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseFbmdPerlin::GetTextId() const noexcept {
  return data::TextId::kFbmdPerlin;
}

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f, 20.0f,
            20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kOctaves, data::TextId::kGain,
              data::TextId::kLacunarity, data::TextId::kSlopeness,
              data::TextId::kOctaveFactor, data::TextId::kSeed,
              data::TextId::kQ, data::TextId::kR};
  shader_ = Shader("../shaders/noise_shaders/FbmWarp.comp");
}

NoiseFbmWarpData TerrainNoiseFbmWarp::Generate() {
  NoiseFbmWarpData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.octaves = static_cast<int>(value_[2]);
  data.gain = value_[3];
  data.lacunarity = value_[4];
  data.slopeness = value_[5];
  data.octave_factor = value_[6];
  data.seed = value_[7];
  data.q = value_[8];
  data.r = value_[9];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- FbmWarp noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.octaves
            << ' ' << data.gain
            << ' ' << data.lacunarity
            << ' ' << data.slopeness
            << ' ' << data.octave_factor
            << ' ' << data.seed
            << ' ' << data.q
            << ' ' << data.r
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_fbm_warp.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseFbmWarp::SetConfig(const NoiseFbmWarpData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = static_cast<float>(config.octaves);
  value_[3] = config.gain;
  value_[4] = config.lacunarity;
  value_[5] = config.slopeness;
  value_[6] = config.octave_factor;
  value_[7] = config.seed;
  value_[8] = config.q;
  value_[9] = config.r;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseFbmWarp::GetTextId() const noexcept {
  return data::TextId::kFbmWarp;
}

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp() {
  value_ = {0.0f};
  scale_ = {20.0f, 20.0f, 20.0f, 20.0f, 20.0f,
            20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
  text_id_ = {data::TextId::kScaleX, data::TextId::kScaleY,
              data::TextId::kOctaves, data::TextId::kGain,
              data::TextId::kLacunarity, data::TextId::kSlopeness,
              data::TextId::kOctaveFactor, data::TextId::kSeed,
              data::TextId::kQ, data::TextId::kR};
  shader_ = Shader("../shaders/noise_shaders/FbmPerlinWarp.comp");
}

NoiseFbmPerlinWarpData TerrainNoiseFbmPerlinWarp::Generate() {
  NoiseFbmPerlinWarpData data{};
  data.scale_x = value_[0];
  data.scale_y = value_[1];
  data.octaves = static_cast<int>(value_[2]);
  data.gain = value_[3];
  data.lacunarity = value_[4];
  data.slopeness = value_[5];
  data.octave_factor = value_[6];
  data.seed = value_[7];
  data.q = value_[8];
  data.r = value_[9];
  data.strength = base_data_.strength;
  data.do_invert = base_data_.do_invert;
  data.do_tiling = base_data_.do_tiling;
  shader_.Bind();
  std::cout << "--- --- FbmPerlinWarp noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.octaves
            << ' ' << data.gain
            << ' ' << data.lacunarity
            << ' ' << data.slopeness
            << ' ' << data.octave_factor
            << ' ' << data.seed
            << ' ' << data.q
            << ' ' << data.r
            << std::endl;
  for (int i = 0; i < value_.size(); ++i) {
    glUniform1f(i, value_[i] * scale_[i]);
  }
  data.hmap = GenAndSave("hmap_fbm_perlin_warp.png");
  base_data_.hmap_id = data.hmap.GetId();
  return data;
}

void TerrainNoiseFbmPerlinWarp::SetConfig(const NoiseFbmPerlinWarpData& config) {
  value_[0] = config.scale_x;
  value_[1] = config.scale_y;
  value_[2] = static_cast<float>(config.octaves);
  value_[3] = config.gain;
  value_[4] = config.lacunarity;
  value_[5] = config.slopeness;
  value_[6] = config.octave_factor;
  value_[7] = config.seed;
  value_[8] = config.q;
  value_[9] = config.r;
  SetBaseData(config.do_invert, config.do_tiling,
              config.strength, config.hmap_id);
}

data::TextId TerrainNoiseFbmPerlinWarp::GetTextId() const noexcept {
  return data::TextId::kFbmPerlinWarp;
}

UiTerrainNoise::UiTerrainNoise(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : UiWindowAppear(
          (data::VboIdMain::kTerrainWindowNoiseDesk), 1.0f,
          {data::VboIdMain::kTerrainWindowNoisePinBack,
           data::VboIdMain::kTerrainWindowNoisePinPoint},
          ui_shared_resources, window_queue),
      name_(text_renderer, data::VboIdMain::kTerrainWindowNoiseName,
            data::TextId::kSeed),
      btn_save_(data::VboIdMain::kTerrainWindowNoiseSave),
      sl_data_({data::VboIdMain::kTerrainWindowNoiseSlider},
               {data::VboIdMain::kTerrainWindowNoiseHandler},
               6, 0.75f, 0.8f),
      config_slider_(data::VboIdMain::kTerrainWindowNoiseSliderArea,
                     data::VboIdMain::kTerrainWindowNoiseSliderIcon),
      config_text_(text_renderer, data::VboIdMain::kTerrainWindowNoiseSliderText,
                   data::TextId::kSeed) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_, &sl_data_);
  hierarchy_.AddNested(&config_slider_, &config_text_);
  sl_data_.SetSlotPtr(config_slider_.GetTrackPtr());
}

UiTerrainNoise::UiTerrainNoise(UiTerrainNoise&& other) noexcept
    : UiWindowAppear(std::move(other)),
      name_(std::move(other.name_)),
      btn_save_(std::move(other.btn_save_)),
      sl_data_(std::move(other.sl_data_)),
      config_slider_(std::move(other.config_slider_)),
      config_text_(std::move(other.config_text_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_, &sl_data_);
  hierarchy_.AddNested(&config_slider_, &config_text_);
}

void UiTerrainNoise::RenderSlotsSprites(glm::vec2 mouse_pos) {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  LocalTransform transform;
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    transform.translate = next_offset;
//    config_slider_.SetTranslate(next_offset);
    config_slider_.SetParentTransform(transform);
    if (i == sl_pressed_config_) {
      config_slider_.Render(mouse_pos);
      value_[i] = config_slider_.GetProgress();
    } else {
      config_slider_.SetValue(value_[i]);
      config_slider_.Render();
    }
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiTerrainNoise::RenderSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    auto graph_name = text_id_[i];
    config_text_.SetText(graph_name);
    config_text_.SetTranslate(next_offset);
    config_text_.Render();
    next_offset.y -= sl_data_.slot_height_;
  }
}

bool UiTerrainNoise::Render() {
  RenderBack(true);
  auto mouse_pos =
      ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
  ui_shared_resources_.dynamic_sprite_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_save_.Render();
  sl_data_.Render(mouse_pos);
  glEnable(GL_SCISSOR_TEST);
  sl_data_.SetUpScissors();
  RenderSlotsSprites(mouse_pos);
  RenderSlotsText();
  glDisable(GL_SCISSOR_TEST);
  name_.Render();
  return false;
}

void UiTerrainNoise::RenderPickingSlotsSprites() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  //TODO: config_slider_::handle probably already use SetTranslate
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(value_.size()));
  LocalTransform transform;
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    transform.translate = next_offset;
    config_slider_.SetParentTransform(transform);
    config_slider_.SetValue(value_[i]);
    config_slider_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiTerrainNoise::RenderPickingSlotsText() {
  glm::vec2 next_offset = sl_data_.start_slot_translate_;
  int slots_to_render = std::min(
      sl_data_.slots_num_ - 1, static_cast<int>(text_id_.size()));
  int start_id = sl_data_.cur_slots_offset_;
  int end_id = start_id + slots_to_render;
  for (int i = start_id; i < end_id; ++i) {
    auto graph_name = text_id_[i];
    config_text_.SetText(graph_name);
    config_text_.SetTranslate(next_offset);
    config_text_.RenderPicking();
    next_offset.y -= sl_data_.slot_height_;
  }
}

void UiTerrainNoise::RenderPicking() {
  RenderPickingBack();
  ui_shared_resources_.dynamic_sprite_picking_shader_.Bind();
  ui_shared_resources_.tex_ui_.Bind();
  btn_save_.RenderPicking();
  sl_data_.RenderPicking();
  glEnable(GL_SCISSOR_TEST);
  sl_data_.SetUpScissors();
  RenderPickingSlotsSprites();
  RenderPickingSlotsText();
  glDisable(GL_SCISSOR_TEST);
  name_.RenderPicking();
}

bool UiTerrainNoise::Scroll(GLuint id, float yoffset) {
  // any inside
  return false;
//  return slider_.Scroll(id, yoffset);
}

bool UiTerrainNoise::Press(int id) {
  if (id == pin_.GetId()) {
    pin_.Press();
  } else if (id == sl_data_.GetId()) {
    sl_data_.Press();
    return true;
  } else if (id == config_slider_.GetId()) {
    sl_pressed_config_ = sl_data_.GetSlotId(
        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_);
    std::cout << sl_pressed_config_ << std::endl;
    config_slider_.Press();
    return true;
  }
  return false;
}

void UiTerrainNoise::Release() {
  sl_data_.Release();
  sl_pressed_config_ = -1;
  config_slider_.Release();
}

void UiTerrainNoise::SetNoise(ITerrainNoise* noise) {
  value_ = noise->GetValueSpan();
  text_id_ = noise->GetTextIdSpan();
  name_.SetText(noise->GetTextId());
  sl_data_.SetEntryNum(static_cast<int>(value_.size()));
}
