#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_

#include "../common/Texture.h"

struct NoiseDataBase {
//  Texture32F hmap; can't have it here; anyway need to store it in
// NoisePerlinData (derived class) to serialize it; no benefits from it here
  GLuint hmap_id = 0; // idk.. soryy
  float strength = 0.0f;
  bool do_tiling = false;
  bool do_invert = false;
};

struct NoisePerlinData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  float seed;
};

struct NoiseCellularData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  float jitter;
  float seed;
};

struct NoiseMetaballsData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  float jitter;
  float seed;
};

struct NoiseFbmGridData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  int octaves;
  float shift;
  float gain;
  float lacunarity;
  float warp_strength;
  float octave_factor;
  float seed;
};

struct NoiseFbmMultiData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  float lacunarity;
  int octaves;
  float seed;
};

struct NoiseFbmdPerlinData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  int octaves;
  float gain;
  float lacunarity;
  float slopeness;
  float octave_factor;
  float seed;
};

struct NoiseFbmWarpData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  int octaves;
  float gain;
  float lacunarity;
  float slopeness;
  float octave_factor;
  float seed;
  float q;
  float r;
};

struct NoiseFbmPerlinWarpData : public NoiseDataBase {
  Texture32F hmap;
  float scale_x;
  float scale_y;
  int octaves;
  float gain;
  float lacunarity;
  float slopeness;
  float octave_factor;
  float seed;
  float q;
  float r;
};

struct NoiseTerrainData {
  Texture32F hmap;
  NoisePerlinData perlin;
  NoiseCellularData cellular;
  NoiseMetaballsData metaballs;
  NoiseFbmGridData fbm_grid;
  NoiseFbmMultiData fbm_multi;
  NoiseFbmdPerlinData fbmd_perlin;
  NoiseFbmWarpData fbm_warp;
  NoiseFbmPerlinWarpData fbm_perlin_warp;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_
