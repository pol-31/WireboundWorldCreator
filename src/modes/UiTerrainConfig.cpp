#include "UiTerrainConfig.h"

Texture32F GenAndSave(data::TextId text_id) {
  int size = details::gTerrainSize;
  Texture32F height_map(Texture::Type::TerrainR32F);
  height_map.BindImage(0, GL_WRITE_ONLY);
  glDispatchCompute((size + 15) / 16, (size + 15) / 16, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  //  auto tex_name = std::to_string(static_cast<int>(tex_id));
  //  tex_name += ".png";
  //  height_map.StoreImage(tex_name, 1);
  return height_map;
}