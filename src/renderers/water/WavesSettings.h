#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESSETTINGS_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESSETTINGS_H_

#include <algorithm>
#include <cmath>

#include "../../common/ShadersBinding.h"

struct SpectrumSettings {
  float scale;
  float angle;
  float spreadBlend;
  float swell;
  float alpha;
  float peakOmega;
  float gamma;
  float shortWavesFade;
};

struct DisplaySpectrumSettings {
  float scale{0.5f}; // 0-1
  float windSpeed{1.0f};
  float windDirection{1.0f};
  float fetch{1.0f};
  float spreadBlend{0.5f}; // 0-1
  float swell{0.5f}; // 0-1
  float peakEnhancement{0.5f};
  float shortWavesFade{0.5f};
};

class WavesSettings {
 public:
  WavesSettings() = default;

  void SetParametersToUbo(GLuint ubo_id) {
    FillSettingsStruct(local_, spectrums_[0]);
    FillSettingsStruct(swell_, spectrums_[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
                    spectrums_.size() * sizeof(SpectrumSettings),
                    spectrums_.data());
    glBindBufferBase(GL_UNIFORM_BUFFER, shader::kOceanSpectrums, ubo_id);
  }

 private:
  void FillSettingsStruct(DisplaySpectrumSettings display, SpectrumSettings& settings) const {
    settings.scale = display.scale;
    settings.angle = display.windDirection / 180 * std::numbers::pi_v<float>;
    settings.spreadBlend = display.spreadBlend;
    settings.swell = std::clamp(display.swell, 0.01f, 1.0f);
    settings.alpha = JonswapAlpha(g_, display.fetch, display.windSpeed);
    settings.peakOmega = JonswapPeakFrequency(g_, display.fetch, display.windSpeed);
    settings.gamma = display.peakEnhancement;
    settings.shortWavesFade = display.shortWavesFade;
  }

  static float JonswapAlpha(float g, float fetch, float windSpeed) {
    return 0.076f * std::pow(g * fetch / windSpeed / windSpeed, -0.22f);
  }

  static float JonswapPeakFrequency(float g, float fetch, float windSpeed) {
    return 22 * std::pow(windSpeed * fetch / g / g, -0.33f);
  }

  float g_{9.8f};

  //TODO: need to configure it
  DisplaySpectrumSettings local_;
  DisplaySpectrumSettings swell_;

  std::array<SpectrumSettings, 2> spectrums_{};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESSETTINGS_H_
