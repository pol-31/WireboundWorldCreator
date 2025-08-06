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
    : Base({data::VboIdMain::kTerrainNoisePerlinDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoisePerlinPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoisePerlinPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoisePerlinSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoisePerlinName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoisePerlinScaleXArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinScaleXIcon, data::TextId::kNotYet}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoisePerlinScaleYArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinScaleYIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinScaleXText, data::TextId::kNotYet},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinScaleYText, data::TextId::kNotYet},
          data::TextId::kScaleY),
      slider_seed_(
          {data::VboIdMain::kTerrainNoisePerlinSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_seed_};
  texts_ = {&text_scale_x_, &text_scale_y_, &text_seed_};
  ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_,
                       &slider_scale_y_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Perlin.comp");
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseCellular::TerrainNoiseCellular(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseCellularDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseCellularPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseCellularPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseCellularSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseCellularName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseCellularScaleXArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularScaleXIcon, data::TextId::kNotYet}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseCellularScaleYArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularScaleYIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularScaleXText, data::TextId::kNotYet},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularScaleYText, data::TextId::kNotYet},
          data::TextId::kScaleY),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseCellularJitterArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularJitterIcon, data::TextId::kNotYet}
          ),
      text_jitter_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularJitterText, data::TextId::kNotYet}, data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseCellularSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_scale_y_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Cellular.comp");
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseMetaballs::TerrainNoiseMetaballs(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseMetaballsDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseMetaballsPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseMetaballsPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseMetaballsSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseMetaballsName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXIcon, data::TextId::kNotYet}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleXText, data::TextId::kNotYet},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleYText, data::TextId::kNotYet},
          data::TextId::kScaleY),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseMetaballsJitterArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterIcon, data::TextId::kNotYet}
          ),
      text_jitter_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterText, data::TextId::kNotYet}, data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseMetaballsSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_scale_y_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/Metaballs.comp");
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmGridDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmGridPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmGridPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmGridSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseFbmGridName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXIcon, data::TextId::kNotYet}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleXText, data::TextId::kNotYet},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleYText, data::TextId::kNotYet},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesIcon, data::TextId::kNotYet}
          ),
      text_octaves_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesText, data::TextId::kNotYet}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmGridShiftArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridShiftIcon, data::TextId::kNotYet}
          ),
      text_shift_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridShiftText, data::TextId::kNotYet}, data::TextId::kShift),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmGridGainArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridGainIcon, data::TextId::kNotYet}
          ),
      text_gain_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridGainText, data::TextId::kNotYet}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityIcon, data::TextId::kNotYet}
          ),
      text_lacunarity_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityText, data::TextId::kNotYet}, data::TextId::kLacunarity),
      slider_warp_strength_(
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthIcon, data::TextId::kNotYet}
          ),
      text_warp_strength_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthText, data::TextId::kNotYet}, data::TextId::kWarpStrength),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorIcon, data::TextId::kNotYet}
          ),
      text_octave_factor_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorText, data::TextId::kNotYet}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmGridSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
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
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmMultiDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmMultiPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmMultiPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmMultiSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseFbmMultiName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXIcon, data::TextId::kNotYet}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleXText, data::TextId::kNotYet},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleYText, data::TextId::kNotYet},
          data::TextId::kScaleY),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedText, data::TextId::kNone}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_scale_y_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  Base::texts_ = {
      &text_scale_x_, &text_scale_y_, &text_lacunarity_, &text_octaves_, &text_seed_};
  Base::ui_event_handler_ = {
      &btn_save_, &pin_, &slider_scale_x_, &slider_scale_y_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/FbmMulti.comp");
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmdPerlinDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmdPerlinPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmdPerlinSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXIcon, data::TextId::kNone}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleXText, data::TextId::kNone},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleYText, data::TextId::kNone},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedText, data::TextId::kNone}, data::TextId::kSeed),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
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
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmWarpDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmWarpPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmWarpPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmWarpSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseFbmWarpName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXIcon, data::TextId::kNone}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleXText, data::TextId::kNone},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleYText, data::TextId::kNone},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmWarpGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedText, data::TextId::kNone}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmWarpQArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpQIcon, data::TextId::kNone}
          ),
      text_q_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpQText, data::TextId::kNone}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmWarpRArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpRIcon, data::TextId::kNone}
          ),
      text_r_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpRText, data::TextId::kNone}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
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
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp(
    UiSharedResources& ui_shared_resources,
    WindowQueue& window_queue,
    TextRenderer& text_renderer)
    : Base({data::VboIdMain::kTerrainNoiseFbmPerlinWarpDesk, data::TextId::kNotYet},
           1.0f,
           {{data::VboIdMain::kTerrainNoiseFbmPerlinWarpPinBack, data::TextId::kNotYet, []() {}},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpPinPoint, data::TextId::kNotYet}},
           ui_shared_resources, window_queue,
           {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSave, data::TextId::kNotYet},
           {text_renderer, 0.05f, glm::vec2{0.0f},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpName, data::TextId::kNotYet},
            data::TextId::kScale}),
      slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXIcon, data::TextId::kNone}
          ),
      slider_scale_y_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleXText, data::TextId::kNone},
          data::TextId::kScaleX),
      text_scale_y_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleYText, data::TextId::kNone},
          data::TextId::kScaleY),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedText, data::TextId::kNone}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQIcon, data::TextId::kNone}
          ),
      text_q_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQText, data::TextId::kNone}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon, data::TextId::kNone}
          ),
      text_r_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRText, data::TextId::kNone}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.025f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
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
  SetParent(GetId());
  UiWindowBase::UpdateTransform();
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
