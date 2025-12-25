#include "Ifft.h"

#include <cmath>

#include <glad/glad.h>

#include "../../common/OpenGlUtility.h"
#include "../../common/ShadersBinding.h"

Ifft::Ifft(const Paths& paths, int size)
    : size_(size),
      ifft_shader_(paths.shader_ocean_ifft),
      permute_shader_(paths.shader_ocean_permute),
      precompute_shader_(paths.shader_ocean_ifft_precomp),
      precomputed_data_(static_cast<int>(std::log2(size_)),
                        size_, GL_RGBA32F) {
  Init();
  SetSize(size);
}

void Ifft::Init() {
  glGenBuffers(1, &ssbo_indices_);
  //TODO: 1 to header ShadersBindings
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_indices_);
  float zeros4[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  glClearTexImage(precomputed_data_.GetId(), 0, GL_RGBA, GL_FLOAT, zeros4);
}

void Ifft::DeInit() {
  glDeleteBuffers(1, &ssbo_indices_);
}

void Ifft::Compute(Texture32F& input, Texture32F& buffer) const {
  using namespace utility;
  auto log_size = static_cast<int>(std::log2(size_));
  bool ping_pong = false;

  ifft_shader_.Bind();
  BindImageTexture(shader::kOceanPrecomputeData,
                   precomputed_data_, GL_READ_ONLY);
  BindImageTexture(shader::kOceanIfftBuffer0, input, GL_READ_WRITE);
  BindImageTexture(shader::kOceanIfftBuffer1, buffer, GL_READ_WRITE);

  glUniform1i(shader::kOceanIfftHorizontal, true);
  for (int i = 0; i < log_size; ++i) {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    ping_pong = !ping_pong;
    glUniform1ui(shader::kOceanIfftStep, static_cast<GLuint>(i));
    glUniform1i(shader::kOceanIfftPingpong, ping_pong);
    glDispatchCompute(size_ / 8, size_ / 8, 1);
  }

  glUniform1i(shader::kOceanIfftHorizontal, false);
  for (int i = 0; i < log_size; ++i) {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    ping_pong = !ping_pong;
    glUniform1ui(shader::kOceanIfftStep, static_cast<GLuint>(i));
    glUniform1i(shader::kOceanIfftPingpong, ping_pong);
    glDispatchCompute(size_ / 8, size_ / 8, 1);
  }

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanPrecomputeData,
                     precomputed_data_, GL_READ_ONLY);
  UnBindImageTexture(shader::kOceanIfftBuffer0, input, GL_READ_WRITE);
  UnBindImageTexture(shader::kOceanIfftBuffer1, buffer, GL_READ_WRITE);
#endif

  if (ping_pong) {
    std::swap(buffer, input);
  }
  permute_shader_.Bind();
  glUniform1ui(shader::kOceanSize, size_);
  BindImageTexture(shader::kOceanIfftBuffer0, input, GL_READ_WRITE);
  glDispatchCompute(size_ / 8, size_ / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanIfftBuffer0, input, GL_READ_WRITE);
#endif
}

void Ifft::SetSize(int size) {
  using namespace utility;
  size_ = size;
  UpdateIndicesSsbo();
  precompute_shader_.Bind();
  glUniform1ui(shader::kOceanSize, size_);
  BindImageTexture(shader::kOceanPrecomputeData,
                   precomputed_data_, GL_WRITE_ONLY);

  auto log_size = static_cast<int>(std::log2(size_));
  glDispatchCompute(log_size, size_ / 2 / 8, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifndef NDEBUG
  UnBindImageTexture(shader::kOceanPrecomputeData,
                     precomputed_data_, GL_WRITE_ONLY);
#endif
}

std::uint32_t Ifft::Reverse(uint32_t x) {
  // idk is this correct; ty https://stackoverflow.com/questions/49943399/most-efficient-algorithm-for-bit-reversal-with-undefined-size-of-data
  x = ((x & 0x55555555) <<  1) | ((x & 0xAAAAAAAA) >>  1);
  x = ((x & 0x33333333) <<  2) | ((x & 0xCCCCCCCC) >>  2);
  x = ((x & 0x0F0F0F0F) <<  4) | ((x & 0xF0F0F0F0) >>  4);
  x = ((x & 0x00FF00FF) <<  8) | ((x & 0xFF00FF00) >>  8);
  x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16);
  return x;
}

std::vector<int> Ifft::GetBitReversedArray(int size) {
  struct Value {
    uint32_t value{0};
    uint32_t id{0};
    Value() = default;
    explicit Value(uint32_t number_, uint32_t id_)
        : value(number_), id(id_) {}
  };

  std::vector<Value> start_array(size);
  for (int i = 0; i < size; ++i) {
    start_array[i] = Value(Reverse(i), i);
  }
  std::sort(start_array.begin(), start_array.end(),
            [](const Value& val1, const Value& val2) {
              return val1.value < val2.value;
            });
  //TODO: can we use <algorithm>?
  std::vector<int> bit_reversed_array(size);
  for (int i = 0; i < size; ++i) {
    bit_reversed_array[i] = static_cast<int>(start_array[i].id);
  }
  return bit_reversed_array;
}

void Ifft::UpdateIndicesSsbo() {
  auto bit_reversed = GetBitReversedArray(size_);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_indices_);
  glBufferData(GL_SHADER_STORAGE_BUFFER, size_ * sizeof(int),
               bit_reversed.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
