#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_IFFT_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_IFFT_H_

#include "../../common/Paths.h"
#include "../../common/Shader.h"
#include "../../common/Texture.h"

class Ifft {
 public:
  Ifft(const Paths& paths, int size);
  ~Ifft();

  void SetSize(int size);
  void Compute(Texture& input, Texture& buffer) const;

 private:
  void Init();
  void DeInit();

  void UpdateIndicesSsbo();

  //TODO: move to a separate utility header
  std::uint32_t Reverse(uint32_t x);
  std::vector<int> GetBitReversedArray(int size);

  int size_{0};
  GLuint ssbo_indices_{0};
  Shader ifft_shader_;
  Shader permute_shader_;
  Shader precompute_shader_;
  Texture precomputed_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_IFFT_H_
