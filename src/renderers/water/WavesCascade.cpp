#include "WavesCascade.h"

#include "../../common/OpenGlUtility.h"
#include "../../common/ShadersBinding.h"

WavesCascade::WavesCascade(int size)
    : size_(size),
      init_spectrum_tex_(size_, size_, GL_RGBA32F, GL_LINEAR, GL_REPEAT),
      precomputed_data_tex_(size_, size_, GL_RGBA32F, GL_LINEAR, GL_REPEAT),
      displacement_tex_(size_, size_, GL_RGBA32F, GL_LINEAR, GL_REPEAT),
      derivatives_tex_(size_, size_, GL_RGBA32F, GL_LINEAR, GL_REPEAT),
      turbulence_tex_(size_, size_, GL_RGBA32F, GL_LINEAR, GL_REPEAT),
      buffer_tex_(size_, size_, GL_RG32F, GL_LINEAR, GL_REPEAT),
      dxdz_tex_(size_, size_, GL_RG32F, GL_LINEAR, GL_REPEAT),
      dydxz_tex_(size_, size_, GL_RG32F, GL_LINEAR, GL_REPEAT),
      dyxdyz_tex_(size_, size_, GL_RG32F, GL_LINEAR, GL_REPEAT),
      dxxdzz_tex_(size_, size_, GL_RG32F, GL_LINEAR, GL_REPEAT) {}

void WavesCascade::CalculateInitials(
    float length_scale, float cutoff_low, float cutoff_high) {
  using namespace utility;
  glUniform1f(shader::kOceanSpectrumLengthScale, length_scale);
  glUniform1f(shader::kOceanSpectrumCuttoffLow, cutoff_low);
  glUniform1f(shader::kOceanSpectrumCuttoffHigh, cutoff_high);

  BindImageTexture(shader::kOceanSpectrumBuffer,
                   buffer_tex_, GL_READ_WRITE);
  BindImageTexture(shader::kOceanSpectrumPrecomputedData,
                   precomputed_data_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanSpectrumData,
                   init_spectrum_tex_, GL_WRITE_ONLY);

  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanSpectrumBuffer,
                     buffer_tex_, GL_READ_WRITE);
  UnBindImageTexture(shader::kOceanSpectrumPrecomputedData,
                     precomputed_data_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanSpectrumData,
                     init_spectrum_tex_, GL_WRITE_ONLY);
#endif
}

void WavesCascade::PackIfftData() {
  using namespace utility;
  BindImageTexture(shader::kOceanDxDz, dxdz_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanDyDxz, dydxz_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanDyxyDz, dyxdyz_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanDxxDzz, dxxdzz_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanSpectrumPrecomputedData,
                   precomputed_data_tex_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanSpectrumData,
                   init_spectrum_tex_, GL_READ_ONLY);

  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanDxDz, dxdz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanDyDxz, dydxz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanDyxyDz, dyxdyz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanDxxDzz, dxxdzz_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanSpectrumPrecomputedData,
                     precomputed_data_tex_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanSpectrumData,
                     init_spectrum_tex_, GL_READ_ONLY);
#endif
}

void WavesCascade::ProcessIfft(const Ifft& ifft) {
  ifft.Compute(dxdz_tex_, buffer_tex_);
  ifft.Compute(dydxz_tex_, buffer_tex_);
  ifft.Compute(dyxdyz_tex_, buffer_tex_);
  ifft.Compute(dxxdzz_tex_, buffer_tex_);
}

void WavesCascade::UnPackIfftData() {
  using namespace utility;
  glUniform1f(shader::kOceanLambda, lambda_);

  BindImageTexture(shader::kOceanDxDz, dxdz_tex_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanDyDxz, dydxz_tex_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanDyxyDz, dyxdyz_tex_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanDxxDzz, dxxdzz_tex_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanDisplacement,
                   displacement_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanDerivatives,
                   derivatives_tex_, GL_WRITE_ONLY);
  BindImageTexture(shader::kOceanTurbulence,
                   turbulence_tex_, GL_READ_WRITE);

  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanDxDz, dxdz_tex_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanDyDxz, dydxz_tex_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanDyxyDz, dyxdyz_tex_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanDxxDzz, dxxdzz_tex_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanDisplacement,
                     displacement_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanDerivatives,
                     derivatives_tex_, GL_WRITE_ONLY);
  UnBindImageTexture(shader::kOceanTurbulence,
                     turbulence_tex_, GL_READ_WRITE);
#endif
}

void WavesCascade::SetLambda(float lambda) {
  lambda_ = std::clamp(lambda, 0.0f, 1.0f);
}
