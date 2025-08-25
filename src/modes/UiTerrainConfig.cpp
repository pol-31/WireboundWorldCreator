#include "UiTerrainConfig.h"

//TODO: ubo?

Texture32F GenAndSave(std::string_view tex_name) {
  // YOU MUST: bind shader, set uniforms
  // YOU MUST: bind shader, set uniforms
  // YOU MUST: bind shader, set uniforms

  int size = details::gTerrainSize;
  Texture32F height_map(size, GL_R32F);
  glBindImageTexture(0, height_map.GetId(), 0, GL_FALSE, 0,
                     GL_WRITE_ONLY, height_map.GetFormat());
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  height_map.Store(tex_name, 1, GL_RED);
  return std::move(height_map);
}

TerrainNoisePerlin::TerrainNoisePerlin(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoisePerlinDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoisePerlinPinBack, []() {}},
            {data::VboIdMain::kTerrainNoisePerlinPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoisePerlinSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoisePerlinName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoisePerlinScaleXArea},
          {data::VboIdMain::kTerrainNoisePerlinScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoisePerlinScaleYArea},
          {data::VboIdMain::kTerrainNoisePerlinScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoisePerlinScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoisePerlinScaleYText},
          data::TextId::kScaleY),
      slider_seed_(
          {data::VboIdMain::kTerrainNoisePerlinSeedArea},
          {data::VboIdMain::kTerrainNoisePerlinSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoisePerlinSeedText}, data::TextId::kSeed) {
  sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_seed_};
  texts_ = {&text_scale_x_, &text_scale_y_, &text_seed_};
  ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_,
                       &slider_scale_y_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Perlin.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

TerrainNoisePerlin::TerrainNoisePerlin(TerrainNoisePerlin&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

NoisePerlinData TerrainNoisePerlin::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoisePerlinData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
  std::cout << "--- --- Perlin noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.seed
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1f(3, data.seed);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoisePerlin::SetConfig(const NoisePerlinData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_seed_.SetValue(config.seed);
}

TerrainNoiseCellular::TerrainNoiseCellular(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseCellularDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseCellularPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseCellularPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseCellularSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseCellularName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseCellularScaleXArea},
          {data::VboIdMain::kTerrainNoiseCellularScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseCellularScaleYArea},
          {data::VboIdMain::kTerrainNoiseCellularScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseCellularScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseCellularScaleYText},
          data::TextId::kScaleY),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseCellularJitterArea},
          {data::VboIdMain::kTerrainNoiseCellularJitterIcon},
          20.0f
          ),
      text_jitter_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseCellularJitterText}, data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseCellularSeedArea},
          {data::VboIdMain::kTerrainNoiseCellularSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseCellularSeedText}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_scale_y_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Cellular.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_jitter_, &text_jitter_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

TerrainNoiseCellular::TerrainNoiseCellular(TerrainNoiseCellular&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_jitter_(std::move(other.slider_jitter_)),
      text_jitter_(std::move(other.text_jitter_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_jitter_, &text_jitter_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

NoiseCellularData TerrainNoiseCellular::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseCellularData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.jitter = slider_jitter_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
  std::cout << "--- --- Cellular noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.jitter
            << ' ' << data.seed
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1f(3, data.jitter);
  glUniform1f(4, data.seed);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseCellular::SetConfig(const NoiseCellularData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_jitter_.SetValue(config.jitter);
  slider_seed_.SetValue(config.seed);
}

TerrainNoiseMetaballs::TerrainNoiseMetaballs(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseMetaballsDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseMetaballsPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseMetaballsPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseMetaballsSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseMetaballsName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXArea},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYArea},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYText},
          data::TextId::kScaleY),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseMetaballsJitterArea},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterIcon},
          20.0f
          ),
      text_jitter_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseMetaballsJitterText},
          data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseMetaballsSeedArea},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseMetaballsSeedText}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_scale_y_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Metaballs.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_jitter_, &text_jitter_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

TerrainNoiseMetaballs::TerrainNoiseMetaballs(TerrainNoiseMetaballs&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_jitter_(std::move(other.slider_jitter_)),
      text_jitter_(std::move(other.text_jitter_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_jitter_, &text_jitter_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

NoiseMetaballsData TerrainNoiseMetaballs::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseMetaballsData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.jitter = slider_jitter_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
  std::cout << "--- --- Metaballs noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.jitter
            << ' ' << data.seed
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1f(3, data.jitter);
  glUniform1f(5, data.seed);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseMetaballs::SetConfig(const NoiseMetaballsData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_jitter_.SetValue(config.jitter);
  slider_seed_.SetValue(config.seed);
}

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmGridDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmGridPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmGridPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmGridSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseFbmGridName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXArea},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYArea},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYText},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesArea},
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesIcon},
          20.0f
          ),
      text_octaves_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesText}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmGridShiftArea},
          {data::VboIdMain::kTerrainNoiseFbmGridShiftIcon},
          20.0f
          ),
      text_shift_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridShiftText}, data::TextId::kShift),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmGridGainArea},
          {data::VboIdMain::kTerrainNoiseFbmGridGainIcon},
          20.0f
          ),
      text_gain_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridGainText}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityArea},
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityIcon},
          20.0f
          ),
      text_lacunarity_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityText}, data::TextId::kLacunarity),
      slider_warp_strength_(
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthArea},
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthIcon},
          20.0f
          ),
      text_warp_strength_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthText}, data::TextId::kWarpStrength),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorArea},
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorIcon},
          20.0f
          ),
      text_octave_factor_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorText}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmGridSeedArea},
          {data::VboIdMain::kTerrainNoiseFbmGridSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmGridSeedText}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_octaves_, &slider_shift_,
      &slider_gain_, &slider_lacunarity_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_octaves_, &text_shift_,
      &text_gain_, &text_lacunarity_,
      &text_warp_strength_, &text_octave_factor_, &text_seed_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_octaves_, &slider_shift_,
      &slider_gain_, &slider_lacunarity_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/FbmGrid.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_shift_, &text_shift_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_warp_strength_, &text_warp_strength_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid(TerrainNoiseFbmGrid&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_octaves_(std::move(other.slider_octaves_)),
      text_octaves_(std::move(other.text_octaves_)),
      slider_shift_(std::move(other.slider_shift_)),
      text_shift_(std::move(other.text_shift_)),
      slider_gain_(std::move(other.slider_gain_)),
      text_gain_(std::move(other.text_gain_)),
      slider_lacunarity_(std::move(other.slider_lacunarity_)),
      text_lacunarity_(std::move(other.text_lacunarity_)),
      slider_warp_strength_(std::move(other.slider_warp_strength_)),
      text_warp_strength_(std::move(other.text_warp_strength_)),
      slider_octave_factor_(std::move(other.slider_octave_factor_)),
      text_octave_factor_(std::move(other.text_octave_factor_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_shift_, &text_shift_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_warp_strength_, &text_warp_strength_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

NoiseFbmGridData TerrainNoiseFbmGrid::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseFbmGridData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.octaves = static_cast<int>(slider_octaves_.GetProgress());
  data.shift = slider_shift_.GetProgress();
  data.gain = slider_gain_.GetProgress();
  data.lacunarity = slider_lacunarity_.GetProgress();
  data.warp_strength = slider_warp_strength_.GetProgress();
  data.octave_factor = slider_octave_factor_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
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
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1i(3, data.octaves);
  glUniform1f(4, data.shift);
  glUniform1f(6, data.gain);
  glUniform1f(7, data.lacunarity);
  glUniform1f(9, data.warp_strength);
  glUniform1f(10, data.octave_factor);
  glUniform1f(11, data.seed);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseFbmGrid::SetConfig(const NoiseFbmGridData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_octaves_.SetValue(static_cast<float>(config.octaves));
  slider_shift_.SetValue(config.shift);
  slider_gain_.SetValue(config.gain);
  slider_lacunarity_.SetValue(config.lacunarity);
  slider_warp_strength_.SetValue(config.warp_strength);
  slider_octave_factor_.SetValue(config.octave_factor);
  slider_seed_.SetValue(config.seed);
}

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmMultiDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmMultiPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmMultiPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmMultiSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseFbmMultiName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXArea},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYArea},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYText},
          data::TextId::kScaleY),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityArea},
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityIcon},
          20.0f
          ),
      text_lacunarity_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityText}, data::TextId::kLacunarity),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesArea},
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesIcon},
          20.0f
          ),
      text_octaves_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesText}, data::TextId::kOctaves),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedArea},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedText}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_lacunarity_, &text_octaves_, &text_seed_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/FbmMulti.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti(TerrainNoiseFbmMulti&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_lacunarity_(std::move(other.slider_lacunarity_)),
      text_lacunarity_(std::move(other.text_lacunarity_)),
      slider_octaves_(std::move(other.slider_octaves_)),
      text_octaves_(std::move(other.text_octaves_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
}

NoiseFbmMultiData TerrainNoiseFbmMulti::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseFbmMultiData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.lacunarity = slider_lacunarity_.GetProgress();
  data.octaves = static_cast<int>(slider_octaves_.GetProgress());
  data.seed = slider_seed_.GetProgress();
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
  std::cout << "--- --- FbmMulti noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.lacunarity
            << ' ' << data.octaves
            << ' ' << data.seed
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1f(3, data.lacunarity);
  glUniform1i(4, data.octaves);
  glUniform1f(6, data.seed);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseFbmMulti::SetConfig(const NoiseFbmMultiData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_lacunarity_.SetValue(config.lacunarity);
  slider_octaves_.SetValue(static_cast<float>(config.octaves));
  slider_seed_.SetValue(config.seed);
}

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmdPerlinDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmdPerlinPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmdPerlinSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseFbmdPerlinName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYText},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesIcon},
          20.0f
          ),
      text_octaves_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesText}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainIcon},
          20.0f
          ),
      text_gain_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainText}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityIcon},
          20.0f
          ),
      text_lacunarity_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityText}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessIcon},
          20.0f
          ),
      text_slopeness_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessText}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorIcon},
          20.0f
          ),
      text_octave_factor_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorText}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedArea},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedText}, data::TextId::kSeed),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOff},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn1},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn2},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn3}
          ),
      text_negative_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeText}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_negative_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/FbmdPerlin.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin(TerrainNoiseFbmdPerlin&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_octaves_(std::move(other.slider_octaves_)),
      text_octaves_(std::move(other.text_octaves_)),
      slider_gain_(std::move(other.slider_gain_)),
      text_gain_(std::move(other.text_gain_)),
      slider_lacunarity_(std::move(other.slider_lacunarity_)),
      text_lacunarity_(std::move(other.text_lacunarity_)),
      slider_slopeness_(std::move(other.slider_slopeness_)),
      text_slopeness_(std::move(other.text_slopeness_)),
      slider_octave_factor_(std::move(other.slider_octave_factor_)),
      text_octave_factor_(std::move(other.text_octave_factor_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)),
      toggle_negative_(std::move(other.toggle_negative_)),
      text_negative_(std::move(other.text_negative_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

NoiseFbmdPerlinData TerrainNoiseFbmdPerlin::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseFbmdPerlinData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.octaves = static_cast<int>(slider_octaves_.GetProgress());
  data.gain = slider_gain_.GetProgress();
  data.lacunarity = slider_lacunarity_.GetProgress();
  data.slopeness = slider_slopeness_.GetProgress();
  data.octave_factor = slider_octave_factor_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  data.negative = static_cast<int>(!toggle_negative_.IsOff());
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
  std::cout << "--- --- FbmdPerlin noise --- ---" << std::endl;
  std::cout << data.scale_x
            << ' ' << data.scale_y
            << ' ' << data.octaves
            << ' ' << data.gain
            << ' ' << data.lacunarity
            << ' ' << data.slopeness
            << ' ' << data.octave_factor
            << ' ' << data.seed
            << ' ' << data.negative
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1i(3, data.octaves);
  glUniform1f(6, data.gain);
  glUniform1f(7, data.lacunarity);
  glUniform1f(8, data.slopeness);
  glUniform1f(9, data.octave_factor);
  glUniform1f(11, data.seed);
  glUniform1i(10, data.negative);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseFbmdPerlin::SetConfig(const NoiseFbmdPerlinData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_octaves_.SetValue(static_cast<float>(config.octaves));
  slider_gain_.SetValue(config.gain);
  slider_lacunarity_.SetValue(config.lacunarity);
  slider_slopeness_.SetValue(config.slopeness);
  slider_octave_factor_.SetValue(config.octave_factor);
  slider_seed_.SetValue(config.seed);
  toggle_negative_.Set(static_cast<bool>(config.negative));
}

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmWarpDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmWarpPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmWarpPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmWarpSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseFbmWarpName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYText},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesIcon},
          20.0f
          ),
      text_octaves_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesText}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmWarpGainArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpGainIcon},
          20.0f
          ),
      text_gain_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpGainText}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityIcon},
          20.0f
          ),
      text_lacunarity_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityText}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessIcon}
          ),
      text_slopeness_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessText}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorIcon},
          20.0f
          ),
      text_octave_factor_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorText}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedText}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmWarpQArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpQIcon},
          20.0f
          ),
      text_q_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpQText}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmWarpRArea},
          {data::VboIdMain::kTerrainNoiseFbmWarpRIcon},
          20.0f
          ),
      text_r_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpRText}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOff},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn1},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn2},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn3}
          ),
      text_negative_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeText}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  shader_ = Shader("../shaders/noise_shaders/FbmWarp.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&slider_q_, &text_q_);
  hierarchy_.AddNested(&slider_r_, &text_r_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp(TerrainNoiseFbmWarp&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_octaves_(std::move(other.slider_octaves_)),
      text_octaves_(std::move(other.text_octaves_)),
      slider_gain_(std::move(other.slider_gain_)),
      text_gain_(std::move(other.text_gain_)),
      slider_lacunarity_(std::move(other.slider_lacunarity_)),
      text_lacunarity_(std::move(other.text_lacunarity_)),
      slider_slopeness_(std::move(other.slider_slopeness_)),
      text_slopeness_(std::move(other.text_slopeness_)),
      slider_octave_factor_(std::move(other.slider_octave_factor_)),
      text_octave_factor_(std::move(other.text_octave_factor_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)),
      slider_q_(std::move(other.slider_q_)),
      text_q_(std::move(other.text_q_)),
      slider_r_(std::move(other.slider_r_)),
      text_r_(std::move(other.text_r_)),
      toggle_negative_(std::move(other.toggle_negative_)),
      text_negative_(std::move(other.text_negative_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&slider_q_, &text_q_);
  hierarchy_.AddNested(&slider_r_, &text_r_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

NoiseFbmWarpData TerrainNoiseFbmWarp::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseFbmWarpData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.octaves = static_cast<int>(slider_octaves_.GetProgress());
  data.gain = slider_gain_.GetProgress();
  data.lacunarity = slider_lacunarity_.GetProgress();
  data.slopeness = slider_slopeness_.GetProgress();
  data.octave_factor = slider_octave_factor_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  data.q = slider_q_.GetProgress();
  data.r = slider_r_.GetProgress();
  data.negative = static_cast<int>(!toggle_negative_.IsOff());
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
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
            << ' ' << data.negative
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1i(4, data.octaves);
  glUniform1f(7, data.gain);
  glUniform1f(8, data.lacunarity);
  glUniform1f(9, data.slopeness);
  glUniform1f(10, data.octave_factor);
  glUniform1f(12, data.seed);
  glUniform1f(13, data.q);
  glUniform1f(14, data.r);
  glUniform1i(11, data.negative);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseFbmWarp::SetConfig(const NoiseFbmWarpData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_octaves_.SetValue(static_cast<float>(config.octaves));
  slider_gain_.SetValue(config.gain);
  slider_lacunarity_.SetValue(config.lacunarity);
  slider_slopeness_.SetValue(config.slopeness);
  slider_octave_factor_.SetValue(config.octave_factor);
  slider_seed_.SetValue(config.seed);
  slider_q_.SetValue(config.q);
  slider_r_.SetValue(config.r);
  toggle_negative_.Set(static_cast<bool>(config.negative));
}

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmPerlinWarpDesk},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmPerlinWarpPinBack, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpPinPoint}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSave},
           {text_renderer, 0.05f,
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpName},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXIcon},
          20.0f
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYIcon},
          20.0f
          ),
      text_scale_x_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXText},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYText},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesIcon},
          20.0f
          ),
      text_octaves_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesText}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainIcon},
          20.0f
          ),
      text_gain_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainText}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityIcon},
          20.0f
          ),
      text_lacunarity_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityText}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessIcon},
          20.0f
          ),
      text_slopeness_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessText}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorIcon},
          20.0f
          ),
      text_octave_factor_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorText}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedIcon},
          20.0f
          ),
      text_seed_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedText}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQIcon},
          20.0f
          ),
      text_q_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQText}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRArea},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon},
          20.0f
          ),
      text_r_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRText}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOff},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn1},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn2},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn3}
          ),
      text_negative_(
          text_renderer, 0.025f,
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeText}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  shader_ = Shader("../shaders/noise_shaders/FbmPerlinWarp.comp");
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&slider_q_, &text_q_);
  hierarchy_.AddNested(&slider_r_, &text_r_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp(TerrainNoiseFbmPerlinWarp&& other)
    : Base(std::move(other)),
      slider_scale_x_(std::move(other.slider_scale_x_)),
      text_scale_x_(std::move(other.text_scale_x_)),
      slider_scale_y_(std::move(other.slider_scale_y_)),
      text_scale_y_(std::move(other.text_scale_y_)),
      slider_octaves_(std::move(other.slider_octaves_)),
      text_octaves_(std::move(other.text_octaves_)),
      slider_gain_(std::move(other.slider_gain_)),
      text_gain_(std::move(other.text_gain_)),
      slider_lacunarity_(std::move(other.slider_lacunarity_)),
      text_lacunarity_(std::move(other.text_lacunarity_)),
      slider_slopeness_(std::move(other.slider_slopeness_)),
      text_slopeness_(std::move(other.text_slopeness_)),
      slider_octave_factor_(std::move(other.slider_octave_factor_)),
      text_octave_factor_(std::move(other.text_octave_factor_)),
      slider_seed_(std::move(other.slider_seed_)),
      text_seed_(std::move(other.text_seed_)),
      slider_q_(std::move(other.slider_q_)),
      text_q_(std::move(other.text_q_)),
      slider_r_(std::move(other.slider_r_)),
      text_r_(std::move(other.text_r_)),
      toggle_negative_(std::move(other.toggle_negative_)),
      text_negative_(std::move(other.text_negative_)) {
  hierarchy_ = UiHierarchy(&sprite_, &pin_, &name_, &btn_save_);
  hierarchy_.AddNested(&slider_scale_x_, &text_scale_x_);
  hierarchy_.AddNested(&slider_scale_y_, &text_scale_y_);
  hierarchy_.AddNested(&slider_octaves_, &text_octaves_);
  hierarchy_.AddNested(&slider_gain_, &text_gain_);
  hierarchy_.AddNested(&slider_lacunarity_, &text_lacunarity_);
  hierarchy_.AddNested(&slider_slopeness_, &text_slopeness_);
  hierarchy_.AddNested(&slider_octave_factor_, &text_octave_factor_);
  hierarchy_.AddNested(&slider_seed_, &text_seed_);
  hierarchy_.AddNested(&slider_q_, &text_q_);
  hierarchy_.AddNested(&slider_r_, &text_r_);
  hierarchy_.AddNested(&toggle_negative_, &text_negative_);
}

NoiseFbmPerlinWarpData TerrainNoiseFbmPerlinWarp::Generate(
    glm::vec2 resolution, std::string_view tex_name) {
  NoiseFbmPerlinWarpData data{};
  data.scale_x = slider_scale_x_.GetProgress();
  data.scale_y = slider_scale_y_.GetProgress();
  data.octaves = static_cast<int>(slider_octaves_.GetProgress());
  data.gain = slider_gain_.GetProgress();
  data.lacunarity = slider_lacunarity_.GetProgress();
  data.slopeness = slider_slopeness_.GetProgress();
  data.octave_factor = slider_octave_factor_.GetProgress();
  data.seed = slider_seed_.GetProgress();
  data.q = slider_q_.GetProgress();
  data.r = slider_r_.GetProgress();
  data.negative = static_cast<int>(!toggle_negative_.IsOff());
  shader_.Bind();
  //glUniform2fv(0, 1, glm::value_ptr(resolution));
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
            << ' ' << data.negative
            << std::endl;
  glUniform1f(1, data.scale_x);
  glUniform1f(2, data.scale_y);
  glUniform1i(4, data.octaves);
  glUniform1f(7, data.gain);
  glUniform1f(8, data.lacunarity);
  glUniform1f(9, data.slopeness);
  glUniform1f(10, data.octave_factor);
  glUniform1f(12, data.seed);
  glUniform1f(13, data.q);
  glUniform1f(14, data.r);
  glUniform1i(11, data.negative);
  data.hmap = GenAndSave(tex_name);
  return data;
}

void TerrainNoiseFbmPerlinWarp::SetConfig(const NoiseFbmPerlinWarpData& config) {
  slider_scale_x_.SetValue(config.scale_x);
  slider_scale_y_.SetValue(config.scale_y);
  slider_octaves_.SetValue(static_cast<float>(config.octaves));
  slider_gain_.SetValue(config.gain);
  slider_lacunarity_.SetValue(config.lacunarity);
  slider_slopeness_.SetValue(config.slopeness);
  slider_octave_factor_.SetValue(config.octave_factor);
  slider_seed_.SetValue(config.seed);
  slider_q_.SetValue(config.q);
  slider_r_.SetValue(config.r);
  toggle_negative_.Set(static_cast<bool>(config.negative));
}
