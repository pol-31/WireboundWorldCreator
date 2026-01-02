#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_

#include "Texture.h"

class GaussianNoise {
 public:
  GaussianNoise() = default;

  static Texture32F Generate(int size);

 private:
  static float NormalRandom();

  static float StableRandom(int seed);
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_GAUSSIANNOISE_H_
