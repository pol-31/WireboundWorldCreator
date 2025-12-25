#include "WavesCascade.h"

#include "../../common/OpenGlUtility.h"
#include "../../common/ShadersBinding.h"

WavesCascade::WavesCascade(int size)
    : size_(size),
      init_spectrum_tex_(size_, GL_RGBA32F),
      precomputed_data_tex_(size_, GL_RGBA32F),
      displacement_tex_(size_, GL_RGBA32F),
      derivatives_tex_(size_, GL_RGBA32F),
      turbulence_tex_(size_, GL_RGBA32F),
      buffer_tex_(size_, GL_RG32F),
      dxdz_tex_(size_, GL_RG32F),
      dydxz_tex_(size_, GL_RG32F),
      dyxdyz_tex_(size_, GL_RG32F),
      dxxdzz_tex_(size_, GL_RG32F) {
  float zeros4[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearTexImage(init_spectrum_tex_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
  glClearTexImage(precomputed_data_tex_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
  glClearTexImage(displacement_tex_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
  glClearTexImage(derivatives_tex_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
  glClearTexImage(turbulence_tex_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
  float zeros2[2] = {0.0f, 0.0f};
  glClearTexImage(buffer_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dxdz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dydxz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dyxdyz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
  glClearTexImage(dxxdzz_tex_.GetId(), 0, GL_RG, GL_FLOAT, zeros2);
}

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
  // auto name = std::to_string(length_scale);
  // name += "_cascade.png";
  // buffer_tex_.Store(name, 2, GL_RG);
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
  // displacement_tex_.Store("displ.png", 4, GL_RGBA);
  // derivatives_tex_.Store("deriv.png", 4, GL_RGBA);
  // turbulence_tex_.Store("turb.png", 4, GL_RGBA);
}

void WavesCascade::SetLambda(float lambda) {
  lambda_ = std::clamp(lambda, 0.0f, 1.0f);
}
