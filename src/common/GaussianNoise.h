#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_

#include <random>
#include <cassert>

#include <glad/glad.h>

#include "Paths.h"
#include "Shader.h"
#include "Texture.h"

class GaussianNoise {
 public:
  explicit GaussianNoise(const Paths& paths)
      : paths_(paths)/*, shader_(paths.gaussian_noise)*/ {}

  static Texture Generate(int size) {
    //TODO: serialize and load if exist by paths_ (filesystem::exists())
    Texture texture(size, GL_RG32F, GL_NEAREST);
    assert(sizeof(float) == 4); // just in case)
    std::vector<GLfloat> data(size * size * 2);
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        data[(i * size + j) * 2] = NormalRandom();
        data[(i * size + j) * 2 + 1] = NormalRandom();
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size,
                 size, 0, GL_RG, GL_FLOAT, data.data());
    return texture;
  }

 private:
  static float NormalRandom() {
    //TODO: replace by some noise textures and gaussian_noise shader
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    float u = dis(gen);
    float v = dis(gen);
    // Box-Muller-Method
    return std::cos(2 * std::numbers::pi_v<float> * u) * std::sqrt(-2 * std::log(v));
  }

  Shader shader_;
  const Paths& paths_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_
