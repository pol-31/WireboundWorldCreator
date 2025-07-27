#include "UiTerrainConfig.h"

Texture GenAndSave(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  // YOU MUST: bind shader, set uniforms
  // YOU MUST: bind shader, set uniforms
  // YOU MUST: bind shader, set uniforms
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glReadPixels(0, 0, resolution.x, resolution.y, GL_RED, GL_UNSIGNED_BYTE,
               buffer.data());

  Texture height_map(resolution.x, resolution.y, GL_R8, GL_LINEAR,
                     GL_CLAMP_TO_EDGE);
  height_map.Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, resolution.x, resolution.y,
               0, GL_RED, GL_UNSIGNED_BYTE, buffer.data());
  height_map.Store(tex_name, 1, GL_RED, GL_UNSIGNED_BYTE);
  return std::move(height_map);
}

TerrainNoisePerlin::TerrainNoisePerlin(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoisePerlinScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_seed_(
          {data::VboIdMain::kTerrainNoisePerlinSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  sliders_ = {&slider_scale_x_, &slider_seed_};
  texts_ = {&text_scale_x_, &text_seed_};
  ui_event_handler_ = {&slider_scale_x_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/Perlin.frag");
  SetParent(parent_id);
}

Texture TerrainNoisePerlin::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1f(3, slider_seed_.GetProgress());
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseCellular::TerrainNoiseCellular(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseCellularScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseCellularJitterArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularJitterIcon, data::TextId::kNotYet}
          ),
      text_jitter_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularJitterText, data::TextId::kNotYet}, data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseCellularSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseCellularSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&slider_scale_x_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/Cellular.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseCellular::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1f(3, slider_jitter_.GetProgress());
  glUniform1f(4, slider_seed_.GetProgress());
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseMetaballs::TerrainNoiseMetaballs(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseMetaballsJitterArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterIcon, data::TextId::kNotYet}
          ),
      text_jitter_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterText, data::TextId::kNotYet}, data::TextId::kJitter),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseMetaballsSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_x_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_x_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&slider_scale_x_, &slider_jitter_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/Metaballs.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseMetaballs::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1f(3, slider_jitter_.GetProgress());
  glUniform1f(5, slider_seed_.GetProgress());
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesIcon, data::TextId::kNotYet}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridOctavesText, data::TextId::kNotYet}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmGridShiftArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridShiftIcon, data::TextId::kNotYet}
          ),
      text_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridShiftText, data::TextId::kNotYet}, data::TextId::kShift),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmGridGainArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridGainIcon, data::TextId::kNotYet}
          ),
      text_gain_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridGainText, data::TextId::kNotYet}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityIcon, data::TextId::kNotYet}
          ),
      text_lacunarity_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridLacunarityText, data::TextId::kNotYet}, data::TextId::kLacunarity),
      slider_warp_strength_(
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthIcon, data::TextId::kNotYet}
          ),
      text_warp_strength_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthText, data::TextId::kNotYet}, data::TextId::kWarpStrength),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorIcon, data::TextId::kNotYet}
          ),
      text_octave_factor_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorText, data::TextId::kNotYet}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmGridSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_octaves_, &slider_shift_,
      &slider_gain_, &slider_lacunarity_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  Base::texts_ = {
      &text_scale_x_, &text_octaves_, &text_shift_,
      &text_gain_, &text_lacunarity_,
      &text_warp_strength_, &text_octave_factor_, &text_seed_};
  Base::ui_event_handler_ = {
      &slider_scale_x_, &slider_octaves_, &slider_shift_,
      &slider_gain_, &slider_lacunarity_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/FbmGrid.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseFbmGrid::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1i(3, static_cast<int>(slider_octaves_.GetProgress()));
  glUniform1f(4, slider_shift_.GetProgress());
  glUniform1f(6, slider_gain_.GetProgress());
  glUniform1f(7, slider_lacunarity_.GetProgress());
  glUniform1f(9, slider_warp_strength_.GetProgress());
  glUniform1f(10, slider_octave_factor_.GetProgress());
  glUniform1f(11, slider_seed_.GetProgress());
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedText, data::TextId::kNone}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  Base::texts_ = {
      &text_scale_x_, &text_lacunarity_, &text_octaves_, &text_seed_};
  Base::ui_event_handler_ = {
      &slider_scale_x_, &slider_lacunarity_, &slider_octaves_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/FbmMulti.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseFbmMulti::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1f(3, slider_lacunarity_.GetProgress());
  glUniform1i(4, static_cast<int>(slider_octaves_.GetProgress()));
  glUniform1f(6, slider_seed_.GetProgress());
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedText, data::TextId::kNone}, data::TextId::kSeed),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/FbmdPerlin.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseFbmdPerlin::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1i(3, static_cast<int>(slider_octaves_.GetProgress()));
  glUniform1f(6, slider_gain_.GetProgress());
  glUniform1f(7, slider_lacunarity_.GetProgress());
  glUniform1f(8, slider_slopeness_.GetProgress());
  glUniform1f(9, slider_octave_factor_.GetProgress());
  glUniform1f(11, slider_seed_.GetProgress());
  glUniform1i(10, static_cast<int>(!toggle_negative_.IsOff()));
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmWarpGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpSeedText, data::TextId::kNone}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmWarpQArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpQIcon, data::TextId::kNone}
          ),
      text_q_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpQText, data::TextId::kNone}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmWarpRArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpRIcon, data::TextId::kNone}
          ),
      text_r_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpRText, data::TextId::kNone}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/FbmWarp.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseFbmWarp::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1i(4, static_cast<int>(slider_octaves_.GetProgress()));
  glUniform1f(7, slider_gain_.GetProgress());
  glUniform1f(8, slider_lacunarity_.GetProgress());
  glUniform1f(9, slider_slopeness_.GetProgress());
  glUniform1f(10, slider_octave_factor_.GetProgress());
  glUniform1f(12, slider_seed_.GetProgress());
  glUniform1f(13, slider_q_.GetProgress());
  glUniform1f(14, slider_r_.GetProgress());
  glUniform1i(11, static_cast<int>(!toggle_negative_.IsOff()));
  return GenAndSave(resolution, buffer, tex_name);
}

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_x_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleIcon, data::TextId::kNone}
          ),
      text_scale_x_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_gain_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainIcon, data::TextId::kNone}
          ),
      text_gain_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainText, data::TextId::kNone}, data::TextId::kGain),
      slider_lacunarity_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityIcon, data::TextId::kNone}
          ),
      text_lacunarity_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityText, data::TextId::kNone}, data::TextId::kLacunarity),
      slider_slopeness_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessIcon, data::TextId::kNone}
          ),
      text_slopeness_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessText, data::TextId::kNone}, data::TextId::kSlopeness),
      slider_octave_factor_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorIcon, data::TextId::kNone}
          ),
      text_octave_factor_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorText, data::TextId::kNone}, data::TextId::kOctaveFactor),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedText, data::TextId::kNone}, data::TextId::kSeed),
      slider_q_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQIcon, data::TextId::kNone}
          ),
      text_q_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQText, data::TextId::kNone}, data::TextId::kQ),
      slider_r_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon, data::TextId::kNone}
          ),
      text_r_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRText, data::TextId::kNone}, data::TextId::kR),
      toggle_negative_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOff, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn1, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn2, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn3, data::TextId::kNone}
          ),
      text_negative_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeText, data::TextId::kNone}, data::TextId::kNegative) {
  Base::sliders_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_x_, &text_octaves_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_x_, &slider_octaves_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  shader_ = Shader("../shaders/noise_shaders/TerrainNoise.vert",
                   "../shaders/noise_shaders/FbmPerlinWarp.frag");
  SetParent(parent_id);
}

Texture TerrainNoiseFbmPerlinWarp::Generate(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name) {
  shader_.Bind();
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, slider_scale_x_.GetProgress());
  glUniform1f(2, slider_scale_x_.GetProgress());
  glUniform1i(4, static_cast<int>(slider_octaves_.GetProgress()));
  glUniform1f(7, slider_gain_.GetProgress());
  glUniform1f(8, slider_lacunarity_.GetProgress());
  glUniform1f(9, slider_slopeness_.GetProgress());
  glUniform1f(10, slider_octave_factor_.GetProgress());
  glUniform1f(12, slider_seed_.GetProgress());
  glUniform1f(13, slider_q_.GetProgress());
  glUniform1f(14, slider_r_.GetProgress());
  glUniform1i(11, static_cast<int>(!toggle_negative_.IsOff()));
  return GenAndSave(resolution, buffer, tex_name);
}
