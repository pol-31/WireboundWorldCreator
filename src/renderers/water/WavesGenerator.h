#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESGENERATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESGENERATOR_H_

#include <numbers>

#include "../../common/GaussianNoise.h"
#include "../../common/Paths.h"
#include "WavesCascade.h"
#include "WavesSettings.h"

#include "../../io/Window.h"

class WavesGenerator {
 public:
  explicit WavesGenerator(const Paths& paths);

  void BindTextures();

  void Update();

  float GetWaterHeight(/*Vector3 position*/);

 private:
  void InitialiseCascades();

  WavesCascade cascade_near_;
  WavesCascade cascade_mid_;
  WavesCascade cascade_far_;

  int size_{256};

  WavesSettings waves_settings_;
  float length_scale_near_ = 250;
  float length_scale_mid_ = 17;
  float length_scale_far_ = 5;

  Shader init_spectrum_shader_;
  Shader time_spectrum_shader_; // time-dependent
  Shader textures_merger_shader_;

  Texture noise_tex_;
  FastFourierTransform fft_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESGENERATOR_H_
