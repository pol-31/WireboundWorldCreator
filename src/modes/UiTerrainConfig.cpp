#include "UiTerrainConfig.h"

Texture32F GenAndSave(data::TextId tex_id) {
  int size = details::gTerrainSize;
  Texture32F height_map(size, GL_R32F);
  glBindImageTexture(0, height_map.GetId(), 0, GL_FALSE, 0,
                     GL_WRITE_ONLY, height_map.GetFormat());
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  //  auto tex_name = std::to_string(static_cast<int>(tex_id));
  //  tex_name += ".png";
  //  height_map.Store(tex_name, 1, GL_RED);
  return height_map;
}