#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_FASTFOURIERTRANSFORM_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_FASTFOURIERTRANSFORM_H_

#include "../../common/Paths.h"
#include "../../common/Shader.h"
#include "../../common/Texture.h"

class FastFourierTransform {
 public:
  FastFourierTransform(const Paths& paths, int size);

  void IFFT2D(Texture& input, Texture& buffer);

 private:
  void Init();

  int size_;
  Shader ifft_shader_;
  Shader permute_shader_;
  Shader precompute_shader_;
  Texture precomputed_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_FASTFOURIERTRANSFORM_H_
