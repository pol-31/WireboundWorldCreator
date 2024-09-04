#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_

#include "../../common/Shader.h"
#include "../../common/Texture.h"
#include "FastFourierTransform.h"
#include "WavesSettings.h"

class WavesCascade {
 public:
  WavesCascade(int size, const Shader& init_spectrum,
               const Shader& time_dependent_spectrum,
               const Shader& textures_merger, FastFourierTransform& fft,
               const Texture& gaussian_noise);

  void BindTextures();

  void CalculateInitials(WavesSettings waves_settings, float length_scale,
                         float cutoff_low, float cutoff_high);

  void CalculateWavesAtTime(float time, float delta_time);

 private:
  void Init();

  int size_;
  const Shader& init_spectrum_shader_;
  const Shader& time_dependent_spectrum_shader_;
  const Shader& textures_merger_shader_;
  FastFourierTransform& fft_;

  const Texture& gaussian_noise_tex_;

  Texture init_spectrum_tex_;
  Texture precomputed_data_tex_;

  Texture buffer_tex_;
  Texture dxdz_tex_;
  Texture dydxz_tex_;
  Texture dyxdyz_tex_;
  Texture dxxdzz_tex_;

  Texture displacement_tex_;
  Texture derivatives_tex_;
  Texture turbulence_tex_;

  GLuint ubo_id_{0};

  float lambda_{0.5f}; // 0-1
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WAVESCASCADE_H_
