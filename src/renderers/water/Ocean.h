#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_OCEAN_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_OCEAN_H_

#include <numbers>

#include "../../common/GaussianNoise.h"
#include "../../common/Paths.h"
#include "../../environment/Environment.h"
#include "WavesCascade.h"
#include "../../modes/UiWaterConfig.h"

#include "../../io/Window.h"

class Ocean {
 public:
  Ocean(const Paths& paths, const Environment& environment,
        const OceanTraits& traits);
  ~Ocean();

  void BindRenderData();

  void Update();
  void UpdateSpectrum(const OceanTraits& traits);

  float GetWaterHeight(/*Vector3 position*/);

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

  /// using ocean_traits_ubo_id_
  void OceanTraitsToGpu() const;
  static SpectrumSettings GenSpectrumSettings(
      const OceanLayerTraits& traits, const Wind& wind);

  void Init(const OceanTraits& traits);
  void DeInit();

  int size_{64};

  WavesCascade cascade_near_;
  WavesCascade cascade_mid_;
  WavesCascade cascade_far_;

  //TODO: replace by data from tile_
  OceanTraits traits_;

  float length_scale_near_;
  float length_scale_mid_;
  float length_scale_far_;

  Shader init_spectrum_shader_;
  Shader time_spectrum_shader_; // time-dependent
  Shader textures_merger_shader_;

  Texture32F noise_tex_;
  Ifft fft_;

  // for OceanTraitsToGpu()
  GLuint ocean_traits_ubo_id_{0};
  const Environment& environment_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_OCEAN_H_
