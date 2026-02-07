#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_IFFT_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_IFFT_H_

#include "../render/Texture.h"
#include "../render/Shader.h"

class Ifft {
 public:
  explicit Ifft(int size);

  ~Ifft() { DeInit(); }

  void SetSize(int size);

  void Compute(Texture32F& input, Texture32F& buffer) const;

 private:
  void Init();

  void DeInit();

  void UpdateIndicesSsbo();

  std::uint32_t Reverse(uint32_t x);

  std::vector<int> GetBitReversedArray(int size);

  int size_{0};
  GLuint ssbo_indices_{0};
  Shader ifft_shader_;
  Shader permute_shader_;
  Shader precompute_shader_;
  Texture32F precomputed_data_;
};

#endif  //  WIREBOUNDWORLDCREATOR_SRC_RENDERERS_IFFT_H_
