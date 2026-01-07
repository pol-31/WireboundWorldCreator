#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OCEAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OCEAN_H_

#include <numbers>

#include "../common/GaussianNoise.h"
#include "../core/Environment.h"
#include "../io/Window.h"
#include "../modes/UiOceanConfig.h"
#include "Ifft.h"

class Ocean {
 public:
  Ocean(const OceanTraits& traits);

  ~Ocean() { DeInit(); }

  void BindRenderData();

  void Update();

 private:
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

  static SpectrumSettings GenSpectrumSettings(const OceanLayerTraits& traits);

  static float JonswapAlpha(float fetch, float windSpeed);

  static float JonswapPeakFrequency(float fetch, float windSpeed);

  void Init();

  void DeInit();

  void CalculateInitials(int cascade_id, float length_scale, float cutoff_low,
                         float cutoff_high);
  void PackIfftData(int cascade_id);
  void UnPackIfftData(int cascade_id, float lambda);

  void UpdateCascade(int cascade_id, float lambda);

  void InitTexArray(GLuint* id);

  /// using ocean_traits_ubo_id_
  void OceanTraitsToGpu() const;

  int size_ = 64;

  OceanTraits traits_;

  glm::vec3 layers_scales_ = glm::vec3(1.0f);  /// near, mid, far

  Shader init_spectrum_shader_;
  Shader time_spectrum_shader_;
  Shader textures_merger_shader_;

  Texture32F noise_tex_;
  Ifft fft_;

  GLuint ocean_traits_ubo_id_{0};

  GLuint tex_init_spectrum_ = 0;
  GLuint tex_precomputed_data_ = 0;
  GLuint tex_displacement_ = 0;
  GLuint tex_derivatives_ = 0;
  GLuint tex_turbulence_ = 0;

  /// buffers
  Texture32F buffer_tex_;
  Texture32F dxdz_tex_;
  Texture32F dydxz_tex_;
  Texture32F dyxdyz_tex_;
  Texture32F dxxdzz_tex_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_OCEAN_H_
