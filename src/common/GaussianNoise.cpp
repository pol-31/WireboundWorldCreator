#include "GaussianNoise.h"

#include <glad/glad.h>

#include <cassert>
#include <random>

Texture32F GaussianNoise::Generate(int size) {
  Texture32F texture(size, size, GL_RG, GL_RG32F, GL_FLOAT);
  std::vector<GLfloat> data(size * size * 2);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      data[(i * size + j) * 2] = NormalRandom();
      data[(i * size + j) * 2 + 1] = NormalRandom();
      // data[(i * size + j) * 2] = StableRandom(1337);
      // data[(i * size + j) * 2 + 1] = StableRandom(248);
    }
  }
  texture.BindSampler(0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT,
               data.data());
  return texture;
}

float GaussianNoise::NormalRandom() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);
  float u = dis(gen);
  float v = dis(gen);
  // Box-Muller-Method
  return std::cos(2 * std::numbers::pi_v<float> * u) *
         std::sqrt(-2 * std::log(v));
}

float GaussianNoise::StableRandom(int seed) {
  static std::mt19937 gen(seed);
  static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  float u = dis(gen);
  float v = dis(gen);

  return std::cos(2.0f * std::numbers::pi_v<float> * u) *
         std::sqrt(-2.0f * std::log(v));
}
