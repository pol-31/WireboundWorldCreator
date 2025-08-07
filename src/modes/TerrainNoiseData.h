#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_

#include "../common/Texture.h"

// param "negative" int, not bool (OpenGL provides glUniform1i only)

struct NoiseDataBase {
  float strength = 0.0f;
  bool do_tiling = true;
  bool do_invert = true;
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
  int negative;
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
  int negative;
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
  int negative;
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
