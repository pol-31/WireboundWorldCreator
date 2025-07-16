#include "UiTerrainConfig.h"

TerrainNoisePerlin::TerrainNoisePerlin(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoisePerlinScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_seed_(
          {data::VboIdMain::kTerrainNoisePerlinSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoisePerlinSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoisePerlinSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  sliders_ = {&slider_scale_, &slider_seed_};
  texts_ = {&text_scale_, &text_seed_};
  ui_event_handler_ = {&slider_scale_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseCellular::TerrainNoiseCellular(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseCellularScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseCellularScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_(
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
  Base::sliders_ = {&slider_scale_, &slider_jitter_, &slider_seed_};
  Base::texts_ = {&text_scale_, &text_jitter_, &text_seed_};
  Base::ui_event_handler_ = {&slider_scale_, &slider_jitter_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseMetaballs::TerrainNoiseMetaballs(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseMetaballsScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsScaleText, data::TextId::kNotYet}, data::TextId::kScale),
      slider_jitter_(
          {data::VboIdMain::kTerrainNoiseMetaballsJitterArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterIcon, data::TextId::kNotYet}
          ),
      text_jitter_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsJitterText, data::TextId::kNotYet}, data::TextId::kJitter),
      slider_phase_(
          {data::VboIdMain::kTerrainNoiseMetaballsPhaseArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsPhaseIcon, data::TextId::kNotYet}
          ),
      text_phase_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsPhaseText, data::TextId::kNotYet}, data::TextId::kPhase),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseMetaballsSeedArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedIcon, data::TextId::kNotYet}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseMetaballsSeedText, data::TextId::kNotYet}, data::TextId::kSeed) {
  Base::sliders_ = {&slider_scale_, &slider_jitter_,
                    &slider_phase_, &slider_seed_};
  Base::texts_ = {&text_scale_, &text_jitter_,
                  &text_phase_, &text_seed_};
  Base::ui_event_handler_ = {&slider_scale_, &slider_jitter_,
                             &slider_phase_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseFbmGrid::TerrainNoiseFbmGrid(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseFbmGridScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_(
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
      slider_time_shift_(
          {data::VboIdMain::kTerrainNoiseFbmGridTimeShiftArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridTimeShiftIcon, data::TextId::kNotYet}
          ),
      text_time_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridTimeShiftText, data::TextId::kNotYet}, data::TextId::kShift),
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
      slider_translate_(
          {data::VboIdMain::kTerrainNoiseFbmGridTranslateArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmGridTranslateIcon, data::TextId::kNotYet}
          ),
      text_translate_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmGridTranslateText, data::TextId::kNotYet}, data::TextId::kTranslate),
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
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_translate_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  Base::texts_ = {
      &text_scale_, &text_octaves_, &text_shift_, &text_time_shift_,
      &text_gain_, &text_lacunarity_, &text_translate_,
      &text_warp_strength_, &text_octave_factor_, &text_seed_};
  Base::ui_event_handler_ = {
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_translate_,
      &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseFbmMulti::TerrainNoiseFbmMulti(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleArea, data::TextId::kNotYet},
          {data::VboIdMain::kTerrainNoiseFbmMultiScaleIcon, data::TextId::kNotYet}
          ),
      text_scale_(
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
      slider_phases_(
          {data::VboIdMain::kTerrainNoiseFbmMultiPhaseArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiPhaseIcon, data::TextId::kNone}
          ),
      //TODO; phases?
      text_phases_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiPhaseText, data::TextId::kNone}, data::TextId::kPhases),
      slider_seed_(
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon, data::TextId::kNone}
          ),
      text_seed_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmMultiSeedText, data::TextId::kNone}, data::TextId::kSeed) {
  Base::sliders_ = {
      &slider_scale_, &slider_lacunarity_, &slider_octaves_,
      &slider_phases_, &slider_seed_};
  Base::texts_ = {
      &text_scale_, &text_lacunarity_, &text_octaves_,
      &text_phases_, &text_seed_};
  Base::ui_event_handler_ = {
      &slider_scale_, &slider_lacunarity_, &slider_octaves_,
      &slider_phases_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseFbmdPerlin::TerrainNoiseFbmdPerlin(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleIcon, data::TextId::kNone}
          ),
      text_scale_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinShiftArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinShiftIcon, data::TextId::kNone}
          ),
      text_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinShiftText, data::TextId::kNone}, data::TextId::kShift),
      slider_transform_(
          {data::VboIdMain::kTerrainNoiseFbmdPerlinTransformArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinTransformIcon, data::TextId::kNone}
          ),
      text_transform_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmdPerlinTransformText, data::TextId::kNone}, data::TextId::kTransform),
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
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_transform_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_, &text_octaves_, &text_shift_, &text_transform_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_transform_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_};
  SetParent(parent_id);
}

TerrainNoiseFbmWarp::TerrainNoiseFbmWarp(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleIcon, data::TextId::kNone}
          ),
      text_scale_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_factors_(
          {data::VboIdMain::kTerrainNoiseFbmWarpFactorsArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpFactorsIcon, data::TextId::kNone}
          ),
      text_factors_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpFactorsText, data::TextId::kNone}, data::TextId::kFactors),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmWarpShiftArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpShiftIcon, data::TextId::kNone}
          ),
      text_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpShiftText, data::TextId::kNone}, data::TextId::kShift),
      slider_time_shift_(
          {data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftIcon, data::TextId::kNone}
          ),
      text_time_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftText, data::TextId::kNone}, data::TextId::kTimeShift),
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
      &slider_scale_, &slider_factors_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_, &text_factors_, &text_octaves_, &text_shift_, &text_time_shift_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  SetParent(parent_id);
}

TerrainNoiseFbmPerlinWarp::TerrainNoiseFbmPerlinWarp(TextRenderer& text_renderer, GLuint parent_id)
    : slider_scale_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleIcon, data::TextId::kNone}
          ),
      text_scale_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleText, data::TextId::kNone}, data::TextId::kScale),
      slider_factors_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsIcon, data::TextId::kNone}
          ),
      text_factors_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsText, data::TextId::kNone}, data::TextId::kFactors),
      slider_octaves_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesIcon, data::TextId::kNone}
          ),
      text_octaves_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesText, data::TextId::kNone}, data::TextId::kOctaves),
      slider_shift_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftIcon, data::TextId::kNone}
          ),
      text_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftText, data::TextId::kNone}, data::TextId::kShift),
      slider_time_shift_(
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftArea, data::TextId::kNone},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftIcon, data::TextId::kNone}
          ),
      text_time_shift_(
          text_renderer, 0.1f, glm::vec2{1.0f},
          {data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftText, data::TextId::kNone}, data::TextId::kTimeShift),
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
      &slider_scale_, &slider_factors_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  Base::toggles_ = {&toggle_negative_};
  Base::texts_ = {
      &text_scale_, &text_factors_, &text_octaves_, &text_shift_, &text_time_shift_,
      &text_gain_, &text_lacunarity_, &text_slopeness_,
      &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
  Base::ui_event_handler_ = {
      &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
      &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
      &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
  SetParent(parent_id);
}
