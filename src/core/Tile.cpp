#include "Tile.h"

Tile::Tile(const TileInfo& tile_info) {
  // TODO; use placeholders (full black / full white texture)

  // position data already valid
  // (we've thrown at TileRenderer::Init()) in case of missing
  pos_x = tile_info.pos_x;
  pos_y = tile_info.pos_y;
  // map_terrain_height is necessary (if float GL_RED is ignored)
  /// --- --- --- no LOAD --- --- ---
//  map_terrain_height = Texture32F(tile_info.map_terrain_height, GL_R32F);
  //    map_erosion_wear = Texture(tile_info.map_erosion_wear, GL_RED);

  map_terrain_height = Texture32F(1024, GL_R32F);

  map_terrain_normal = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_slope = Texture(1024, 1024, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_ao = Texture(1024, 1024, GL_R8, GL_LINEAR, GL_CLAMP_TO_EDGE);
  map_terrain_splat = Texture(1024, 1024, GL_RGBA8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_terrain_erosion_thermal = Texture32F(1024, GL_R32F);
  map_terrain_erosion_hydraulic = Texture32F(1024, GL_R32F);
  map_water_accum = Texture32F(1024, GL_R32F);
  map_water_flow = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  map_water_height = Texture(1024, 1024, GL_RG8, GL_LINEAR, GL_CLAMP_TO_EDGE);

  // duplicating data both on CPU & GPU (see Tile.h for more info)
  terrain_heights_.resize(1024 * 1024);
  map_terrain_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                terrain_heights_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  water_heights_init_.resize(1024 * 1024);
  map_water_height.Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE,
                water_heights_init_.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  /// see explanation at header file (Tile.h)
  water_heights_ = water_heights_init_;
}
