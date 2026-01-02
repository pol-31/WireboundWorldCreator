#include "Ocean.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include "../common/OpenGlUtility.h"

Ocean::Ocean(const OceanTraits& traits)
    : init_spectrum_shader_("../shaders/ocean/InitialSpectrum.comp"),
      time_spectrum_shader_("../shaders/ocean/TimeDependentSpectrum.comp"),
      textures_merger_shader_("../shaders/ocean/WavesTexturesMerger.comp"),
      buffer_tex_(size_, GL_RG32F),
      dxdz_tex_(size_, GL_RG32F),
      dydxz_tex_(size_, GL_RG32F),
      dyxdyz_tex_(size_, GL_RG32F),
      dxxdzz_tex_(size_, GL_RG32F),
      size_(64),
      fft_(size_),
      noise_tex_(GaussianNoise{}.Generate(size_)),
      //      length_scale_near_(1),
      //      length_scale_mid_(20),
      //      length_scale_far_(100),
      traits_(traits) {
  Init();
}

void Ocean::DeInit() {
  glDeleteBuffers(1, &ocean_traits_ubo_id_);
  GLuint textures[] = {tex_init_spectrum_, tex_precomputed_data_,
                       tex_displacement_, tex_derivatives_, tex_turbulence_};
  glDeleteTextures(5, textures);
}

void Ocean::BindRenderData() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex_displacement_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex_derivatives_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex_turbulence_);
  // glUniform3fv(3, 1, glm::value_ptr(layers_scales_));
}

void Ocean::Update() {
  UpdateCascade(0, traits_.near.lambda);
  UpdateCascade(1, traits_.mid.lambda);
  UpdateCascade(2, traits_.far.lambda);
}

void Ocean::UpdateCascade(int cascade_id, float lambda) {
  float zeros2[2] = {0.0f, 0.0f};
  glClearTexImage(buffer_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dxdz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dydxz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dyxdyz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dxxdzz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  time_spectrum_shader_.Bind();
  PackIfftData(cascade_id);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  fft_.Compute(dxdz_tex_, buffer_tex_);
  fft_.Compute(dydxz_tex_, buffer_tex_);
  fft_.Compute(dyxdyz_tex_, buffer_tex_);
  fft_.Compute(dxxdzz_tex_, buffer_tex_);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  textures_merger_shader_.Bind();
  UnPackIfftData(cascade_id, lambda);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Ocean::Init() {
  InitTexArray(&tex_init_spectrum_);
  InitTexArray(&tex_precomputed_data_);
  InitTexArray(&tex_displacement_);
  InitTexArray(&tex_derivatives_);
  InitTexArray(&tex_turbulence_);
  utility::InitUbo(ocean_traits_ubo_id_, 3 * sizeof(SpectrumSettings),
                   GL_DYNAMIC_DRAW, 3);
  OceanTraitsToGpu();

  layers_scales_ =
      glm::vec3{traits_.near.scale, traits_.mid.scale, traits_.far.scale};

  float boundary_min = 0.0001f;
  auto pi = std::numbers::pi_v<float>;
  float boundary_near = 2 * pi / traits_.mid.scale * 6.0f;
  float boundary_far = 2 * pi / traits_.far.scale * 6.0f;
  float boundary_max = 10.0f;
  init_spectrum_shader_.Bind();
  glUniform1ui(0, size_);
  utility::BindImageTexture(13, noise_tex_, GL_READ_ONLY);

  glUniform1ui(9, 0);
  CalculateInitials(0, traits_.near.scale, boundary_min, boundary_max);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glUniform1ui(9, 1);
  CalculateInitials(1, traits_.mid.scale, boundary_min, boundary_max);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glUniform1ui(9, 2);
  CalculateInitials(2, traits_.far.scale, boundary_min, boundary_max);

#ifndef NDEBUG
  utility::UnBindImageTexture(13, noise_tex_, GL_READ_ONLY);
#endif
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  Update();
}

void Ocean::InitTexArray(GLuint* id) {
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D_ARRAY, *id);
  int layers = 3;
  int mipLevels = 1 + floor(log2(size_));
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, GL_RGBA32F, size_, size_,
                 layers);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  float zeros[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearTexImage(*id, 0, GL_RGBA, GL_FLOAT, zeros);
  glClearTexImage(*id, 1, GL_RGBA, GL_FLOAT, zeros);
  glClearTexImage(*id, 2, GL_RGBA, GL_FLOAT, zeros);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Ocean::OceanTraitsToGpu() const {
  std::array<SpectrumSettings, 3> settings = {GenSpectrumSettings(traits_.near),
                                              GenSpectrumSettings(traits_.mid),
                                              GenSpectrumSettings(traits_.far)};
  utility::UpdateUbo(ocean_traits_ubo_id_, 0, 3 * sizeof(SpectrumSettings),
                     settings.data());
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

Ocean::SpectrumSettings Ocean::GenSpectrumSettings(
    const OceanLayerTraits& traits) {
  float wind_direction = 0.0f;
  SpectrumSettings settings{};
  settings.scale = std::max(std::numeric_limits<float>::min(), traits.scale);
  settings.angle = wind_direction / 180 * std::numbers::pi_v<float>;
  settings.spreadBlend = std::clamp(traits.spreadBlend, 0.0f, 1.0f);
  settings.swell = std::max(0.0f, traits.swell);
  settings.alpha = JonswapAlpha(traits.fetch, traits.wind);
  settings.peakOmega = JonswapPeakFrequency(traits.fetch, traits.wind);
  settings.gamma = traits.peakEnhancement;
  settings.shortWavesFade = traits.shortWavesFade;
  return settings;
}

float Ocean::JonswapAlpha(float fetch, float windSpeed) {
  const float g = 9.81f;
  return 0.076f * std::pow(g * fetch / windSpeed / windSpeed, -0.22f);
}

float Ocean::JonswapPeakFrequency(float fetch, float windSpeed) {
  const float g = 9.81f;
  return 22 * std::pow(windSpeed * fetch / g / g, -0.33f);
}

void Ocean::CalculateInitials(int cascade_id, float length_scale,
                              float cutoff_low, float cutoff_high) {
  using namespace utility;
  glUniform1f(6, length_scale);
  // glUniform1f(8, cutoff_low);
  // glUniform1f(7, cutoff_high);
  float zeros2[2] = {0.0f, 0.0f};
  glClearTexImage(buffer_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  BindImageTexture(11, buffer_tex_, GL_READ_WRITE);
  glBindImageTexture(12, tex_precomputed_data_, 0, false, cascade_id,
                     GL_READ_ONLY, GL_RGBA32F);
  glBindImageTexture(14, tex_init_spectrum_, 0, false, cascade_id, GL_READ_ONLY,
                     GL_RGBA32F);
  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#ifndef NDEBUG
  UnBindImageTexture(11, buffer_tex_, GL_READ_WRITE);
  glBindImageTexture(12, 0, 0, false, cascade_id, GL_READ_ONLY, GL_RGBA32F);
  glBindImageTexture(14, 0, 0, false, cascade_id, GL_READ_ONLY, GL_RGBA32F);
#endif
}

void Ocean::PackIfftData(int cascade_id) {
  using namespace utility;
  BindImageTexture(4, dxdz_tex_, GL_WRITE_ONLY);
  BindImageTexture(5, dydxz_tex_, GL_WRITE_ONLY);
  BindImageTexture(6, dyxdyz_tex_, GL_WRITE_ONLY);
  BindImageTexture(7, dxxdzz_tex_, GL_WRITE_ONLY);
  glBindImageTexture(12, tex_precomputed_data_, 0, false, cascade_id,
                     GL_READ_ONLY, GL_RGBA32F);
  glBindImageTexture(14, tex_init_spectrum_, 0, false, cascade_id, GL_READ_ONLY,
                     GL_RGBA32F);
  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
#ifndef NDEBUG
  UnBindImageTexture(4, dxdz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(5, dydxz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(6, dyxdyz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(7, dxxdzz_tex_, GL_WRITE_ONLY);
  glBindImageTexture(12, 0, 0, false, cascade_id, GL_READ_ONLY, GL_RGBA32F);
  glBindImageTexture(14, 0, 0, false, cascade_id, GL_READ_ONLY, GL_RGBA32F);
#endif
}

void Ocean::UnPackIfftData(int cascade_id, float lambda) {
  using namespace utility;
  glUniform1f(5, lambda);

  BindImageTexture(4, dxdz_tex_, GL_READ_ONLY);
  BindImageTexture(5, dydxz_tex_, GL_READ_ONLY);
  BindImageTexture(6, dyxdyz_tex_, GL_READ_ONLY);
  BindImageTexture(7, dxxdzz_tex_, GL_READ_ONLY);

  glBindImageTexture(8, tex_displacement_, 0, false, cascade_id, GL_WRITE_ONLY,
                     GL_RGBA32F);
  glBindImageTexture(9, tex_derivatives_, 0, false, cascade_id, GL_WRITE_ONLY,
                     GL_RGBA32F);
  glBindImageTexture(10, tex_turbulence_, 0, false, cascade_id, GL_READ_WRITE,
                     GL_RGBA32F);

  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(4, dxdz_tex_, GL_READ_ONLY);
  UnBindImageTexture(5, dydxz_tex_, GL_READ_ONLY);
  UnBindImageTexture(6, dyxdyz_tex_, GL_READ_ONLY);
  UnBindImageTexture(7, dxxdzz_tex_, GL_READ_ONLY);
  glBindImageTexture(8, 0, 0, false, cascade_id, GL_WRITE_ONLY, GL_RGBA32F);
  glBindImageTexture(9, 0, 0, false, cascade_id, GL_WRITE_ONLY, GL_RGBA32F);
  glBindImageTexture(10, 0, 0, false, cascade_id, GL_READ_WRITE, GL_RGBA32F);
#endif
}
