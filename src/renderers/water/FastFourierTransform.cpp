#include "FastFourierTransform.h"

#include <cmath>

#include <glad/glad.h>

#include "../../common/ShadersBinding.h"

FastFourierTransform::FastFourierTransform(const Paths& paths, int size)
    : size_(size),
      ifft_shader_(paths.shader_ocean_ifft),
      permute_shader_(paths.shader_ocean_permute),
      precompute_shader_(paths.shader_ocean_ifft_precomp),
      precomputed_data_(size_, GL_RGBA32F) {
  Init();
}

void FastFourierTransform::IFFT2D(Texture& input, Texture& buffer) {
  auto log_size = static_cast<int>(std::log2(size_));
  bool ping_pong = false;

  ifft_shader_.Bind();
  glBindImageTexture(
      shader::kOceanPrecomputeData, precomputed_data_.GetId(), 0,
      GL_FALSE, 0, GL_READ_ONLY, precomputed_data_.GetFormat());
  glBindImageTexture(
      shader::kOceanIfftBuffer0, input.GetId(), 0, GL_FALSE,
      0, GL_READ_WRITE, input.GetFormat());
  glBindImageTexture(
      shader::kOceanIfftBuffer1, buffer.GetId(), 0, GL_FALSE,
      0, GL_READ_WRITE, buffer.GetFormat());
  glUniform1i(shader::kOceanIfftHorizontal, true);

  for (int i = 0; i < log_size; i++) {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    ping_pong = !ping_pong;
    glUniform1ui(shader::kOceanIfftStep, static_cast<GLuint>(i));
    glUniform1i(shader::kOceanIfftPingpong, ping_pong);
    glDispatchCompute(size_ / 8, size_ / 8, 1);
  }
  glUniform1i(shader::kOceanIfftHorizontal, false);
  for (int i = 0; i < log_size; i++) {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    ping_pong = !ping_pong;
    glUniform1ui(shader::kOceanIfftStep, static_cast<GLuint>(i));
    glUniform1i(shader::kOceanIfftPingpong, ping_pong);
    glDispatchCompute(size_ / 8, size_ / 8, 1);
  }

  if (ping_pong) {
    std::swap(buffer, input); // safely moved
  }

  permute_shader_.Bind();
  glUniform1ui(shader::kOceanSize, size_);
  glBindImageTexture(
      shader::kOceanIfftBuffer0, input.GetId(), 0,
      GL_FALSE, 0, GL_READ_WRITE, input.GetFormat());
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glDispatchCompute(size_ / 8, size_ / 8, 1);
}

void FastFourierTransform::Init() {
  auto log_size = static_cast<int>(std::log2(size_));
  precompute_shader_.Bind();
  glUniform1ui(shader::kOceanSize, size_);
  glBindImageTexture(
      shader::kOceanPrecomputeData, precomputed_data_.GetId(), 0,
      GL_FALSE, 0, GL_WRITE_ONLY, precomputed_data_.GetFormat());
  glDispatchCompute(log_size, size_ / 2 / 8, 1);
}