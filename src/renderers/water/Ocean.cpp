#include "Ocean.h"

#include <algorithm>

#include "../../common/OpenGlUtility.h"
#include "../../common/ShadersBinding.h"
#include "../../common/Math.h"

//TODO: check size_ is a power of 2
Ocean::Ocean(const Paths& paths, const Environment& environment,
             const OceanTraits& traits)
    : init_spectrum_shader_(paths.shader_ocean_init_spectrum),
      time_spectrum_shader_(paths.shader_ocean_time_spectrum),
      textures_merger_shader_(paths.shader_ocean_tex_merger),
      size_(64),
      fft_(paths, size_),
      noise_tex_(GaussianNoise{paths}.Generate(size_)),
      cascade_near_(size_),
      cascade_mid_(size_),
      cascade_far_(size_),
//      length_scale_near_(1),
//      length_scale_mid_(20),
//      length_scale_far_(100),
      environment_(environment) {
  Init(traits);
}

Ocean::~Ocean() {
  DeInit();
}

void Ocean::DeInit() {
  glDeleteBuffers(1, &ocean_traits_ubo_id_);
}

void Ocean::BindRenderData() {
  glActiveTexture(GL_TEXTURE0);
  cascade_near_.BindDisplacementTex();
  glActiveTexture(GL_TEXTURE1);
  cascade_mid_.BindDisplacementTex();
  glActiveTexture(GL_TEXTURE2);
  cascade_far_.BindDisplacementTex();
  glActiveTexture(GL_TEXTURE3);
  cascade_near_.BindDerivativesTex();
  glActiveTexture(GL_TEXTURE4);
  cascade_mid_.BindDerivativesTex();
  glActiveTexture(GL_TEXTURE5);
  cascade_far_.BindDerivativesTex();
  glActiveTexture(GL_TEXTURE6);
  cascade_near_.BindTurbulenceTex();
  glActiveTexture(GL_TEXTURE7);
  cascade_mid_.BindTurbulenceTex();
  glActiveTexture(GL_TEXTURE8);
  cascade_far_.BindTurbulenceTex();
  glUniform1f(9, length_scale_near_);
  glUniform1f(10, length_scale_mid_);
  glUniform1f(11, length_scale_far_);
}

void Ocean::Update() {
  // calculating complex amplitudes
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  time_spectrum_shader_.Bind();
  cascade_near_.PackIfftData();
  cascade_mid_.PackIfftData();
  cascade_far_.PackIfftData();

  cascade_near_.ProcessIfft(fft_);
  cascade_mid_.ProcessIfft(fft_);
  cascade_far_.ProcessIfft(fft_);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  textures_merger_shader_.Bind();
  cascade_mid_.UnPackIfftData();
  cascade_far_.UnPackIfftData();
  cascade_near_.UnPackIfftData();
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Ocean::UpdateSpectrum(const OceanTraits& traits) {
  traits_ = traits;
  cascade_near_.SetLambda(traits.near.lambda);
  cascade_mid_.SetLambda(traits.mid.lambda);
  cascade_far_.SetLambda(traits.far.lambda);
  /// no blending with surrounding map tiles by now (or never?)
  OceanTraitsToGpu();
}

void Ocean::Init(const OceanTraits& traits) {
  utility::InitUbo(ocean_traits_ubo_id_, 3 * sizeof(SpectrumSettings),
                   GL_DYNAMIC_DRAW, shader::kUboOceanSpectrumsBind);
  UpdateSpectrum(traits);

  length_scale_near_ = traits_.near.scale;
  length_scale_mid_ = traits_.mid.scale;
  length_scale_far_ = traits_.far.scale;


  float boundary_min = 0.0001f;
  auto pi = std::numbers::pi_v<float>;
  float boundary_near = 2 * pi / length_scale_mid_ * 6.0f;
  float boundary_far = 2 * pi / length_scale_far_ * 6.0f;
  float boundary_max = 10.0f;
  // noise_tex_.Store("noise_tex_.png", 2, GL_RG);
  init_spectrum_shader_.Bind();
  glUniform1ui(shader::kOceanSize, size_);
  utility::BindImageTexture(shader::kOceanSpectrumNoise,
                            noise_tex_, GL_READ_ONLY);

  glUniform1ui(shader::kOceanSpectrumCascadeId, 0);
  cascade_near_.CalculateInitials(
      length_scale_near_, boundary_min, boundary_max);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glUniform1ui(shader::kOceanSpectrumCascadeId, 1);
  cascade_mid_.CalculateInitials(
      length_scale_mid_, boundary_min, boundary_max);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glUniform1ui(shader::kOceanSpectrumCascadeId, 2);
  cascade_far_.CalculateInitials(
      length_scale_far_, boundary_min, boundary_max);

#ifndef NDEBUG
  utility::UnBindImageTexture(shader::kOceanSpectrumNoise,
                              noise_tex_, GL_READ_ONLY);
#endif
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  Update();
}

float Ocean::GetWaterHeight(/*Vector3 position*/) {
  //    Color c = physicsReadback.GetPixelBilinear(
  //        position.x / lengthScale0, position.z / lengthScale0);
  return 0.0f;
}

void Ocean::OceanTraitsToGpu() const {
  const Wind& wind = environment_.GetWind();
  std::array<SpectrumSettings, 3> settings = {
      GenSpectrumSettings(traits_.near, wind),
      GenSpectrumSettings(traits_.mid, wind),
      GenSpectrumSettings(traits_.far, wind)
  };
  utility::UpdateUbo(ocean_traits_ubo_id_, 0,
                     3 * sizeof(SpectrumSettings), settings.data());
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

Ocean::SpectrumSettings Ocean::GenSpectrumSettings(
    const OceanLayerTraits& traits, const Wind& wind) {
  float wind_speed = wind.GetSpeed();
  SpectrumSettings settings{};
  settings.scale = std::max(std::numeric_limits<float>::min(), traits.scale);
  settings.angle = wind.GetDirection() / 180 * std::numbers::pi_v<float>;
  settings.spreadBlend = std::clamp(traits.spreadBlend, 0.0f, 1.0f);
  settings.swell = std::max(0.0f, traits.swell);
  settings.alpha = math::JonswapAlpha(traits.fetch, wind_speed);
  settings.peakOmega = math::JonswapPeakFrequency(traits.fetch, wind_speed);
  settings.gamma = traits.peakEnhancement;
  settings.shortWavesFade = traits.shortWavesFade;
  return settings;
}
