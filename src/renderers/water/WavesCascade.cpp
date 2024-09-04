#include "WavesCascade.h"

#include "../../common/ShadersBinding.h"

WavesCascade::WavesCascade(
    int size, const Shader& init_spectrum,
    const Shader& time_dependent_spectrum,
    const Shader& textures_merger, FastFourierTransform& fft,
    const Texture& gaussian_noise)
    : size_(size),
      init_spectrum_shader_(init_spectrum),
      time_dependent_spectrum_shader_(time_dependent_spectrum),
      textures_merger_shader_(textures_merger),
      fft_(fft),
      gaussian_noise_tex_(gaussian_noise),
      init_spectrum_tex_(size_, GL_RGBA32F),
      precomputed_data_tex_(size_, GL_RGBA32F),
      displacement_tex_(size_, GL_RGBA32F, GL_LINEAR, GL_CLAMP_TO_EDGE),
      derivatives_tex_(size_, GL_RGBA32F, GL_LINEAR, GL_CLAMP_TO_EDGE),
      turbulence_tex_(size_, GL_RGBA32F, GL_LINEAR, GL_CLAMP_TO_EDGE),
      buffer_tex_(size_, GL_RG32F),
      dxdz_tex_(size_, GL_RG32F),
      dydxz_tex_(size_, GL_RG32F),
      dyxdyz_tex_(size_, GL_RG32F),
      dxxdzz_tex_(size_, GL_RG32F) {
  Init();
}

void WavesCascade::BindTextures() {
  glActiveTexture(GL_TEXTURE0);
  displacement_tex_.Bind();
}

void WavesCascade::CalculateInitials(
    WavesSettings waves_settings, float length_scale,
    float cutoff_low, float cutoff_high) {
  init_spectrum_shader_.Bind();
  waves_settings.SetParametersToUbo(ubo_id_);
  glUniform1ui(shader::kOceanSize, size_);
  glUniform1f(shader::kOceanSpectrumLengthScale, length_scale);
  glUniform1f(shader::kOceanSpectrumCuttoffLow, cutoff_low);
  glUniform1f(shader::kOceanSpectrumCuttoffHigh, cutoff_high);
  glBindImageTexture(
      shader::kOceanSpectrumBuffer, buffer_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, buffer_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanSpectrumPrecomputedData, precomputed_data_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, precomputed_data_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanSpectrumNoise, gaussian_noise_tex_.GetId(),
      0, GL_FALSE, 0, GL_READ_ONLY, gaussian_noise_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanSpectrumData, init_spectrum_tex_.GetId(),
      0, GL_FALSE, 0, GL_READ_WRITE, init_spectrum_tex_.GetFormat());
  glDispatchCompute(size_ / 8, size_ / 8, 1);
}

void WavesCascade::CalculateWavesAtTime(float time, float delta_time) {
  // Calculating complex amplitudes
  time_dependent_spectrum_shader_.Bind();
  glUniform1f(shader::kOceanTime, time);
  glBindImageTexture(
      shader::kOceanDxDz, dxdz_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, dxdz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDyDxz, dydxz_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, dydxz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDyxyDz, dyxdyz_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, dyxdyz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDxxDzz, dxxdzz_tex_.GetId(),
      0, GL_FALSE, 0, GL_WRITE_ONLY, dxxdzz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanSpectrumData, init_spectrum_tex_.GetId(),
      0, GL_FALSE, 0, GL_READ_ONLY, init_spectrum_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanSpectrumPrecomputedData, precomputed_data_tex_.GetId(),
      0, GL_FALSE, 0, GL_READ_ONLY, precomputed_data_tex_.GetFormat());
  glDispatchCompute(size_ / 8, size_ / 8, 1);

  // Calculating IFFTs of complex amplitudes
  fft_.IFFT2D(dxdz_tex_, buffer_tex_);
  fft_.IFFT2D(dydxz_tex_, buffer_tex_);
  fft_.IFFT2D(dyxdyz_tex_, buffer_tex_);
  fft_.IFFT2D(dxxdzz_tex_, buffer_tex_);

  // Filling displacement and normals textures
  textures_merger_shader_.Bind();
  glUniform1f(shader::kOceanDeltaTime, delta_time);
  glUniform1f(shader::kOceanLambda, lambda_);
  glBindImageTexture(
      shader::kOceanDxDz, dxdz_tex_.GetId(), 0, GL_FALSE, 0,
      GL_READ_ONLY, dxdz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDyDxz, dydxz_tex_.GetId(), 0, GL_FALSE, 0,
      GL_READ_ONLY, dydxz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDyxyDz, dyxdyz_tex_.GetId(), 0, GL_FALSE, 0,
      GL_READ_ONLY, dyxdyz_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDxxDzz, dxxdzz_tex_.GetId(), 0, GL_FALSE, 0,
      GL_READ_ONLY, dxxdzz_tex_.GetFormat());
  glBindImageTexture(
      8, displacement_tex_.GetId(), 0, GL_FALSE, 0,
      GL_WRITE_ONLY, displacement_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanDerivatives, derivatives_tex_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, derivatives_tex_.GetFormat());
  glBindImageTexture(
      shader::kOceanTurbulence, turbulence_tex_.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, turbulence_tex_.GetFormat());
  glDispatchCompute(size_ / 8, size_ / 8, 1);

//  if ((static_cast<int>(time) % 5) == 0) {
//    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//    std::string name{};
//    name += std::to_string(static_cast<int>(time));
//    name += ".png";
//    displacement_tex_.Store(name, 4, GL_FLOAT, 0);
//    std::cout << time << std::endl;
//  }
}

void WavesCascade::Init() {
  glGenBuffers(1, &ubo_id_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(SpectrumSettings),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
