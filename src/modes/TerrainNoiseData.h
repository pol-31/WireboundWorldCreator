#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TERRAINNOISEDATA_H_

#include "../common/Texture.h"

/// can't make NoiseDataBase as an actual base, have to keep Derived as a POD,
/// what isn't possible with type Texture32F

struct NoiseDataBase {
  float strength = 0.0f;
  bool do_invert = false;
  bool do_tiling = false;
};

/// it seems all of this for debug only - we use cast to float* and memcpy

struct NoisePerlinData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float seed;
};

struct NoiseCellularData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float jitter;
  float seed;
};

struct NoiseMetaballsData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float jitter;
  float seed;
};

struct NoiseFbmGridData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float octaves;
  float shift;
  float gain;
  float lacunarity;
  float warp_strength;
  float octave_factor;
  float seed;
};

struct NoiseFbmMultiData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float lacunarity;
  float octaves;
  float seed;
};

struct NoiseFbmdPerlinData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float octaves;
  float gain;
  float lacunarity;
  float slopeness;
  float octave_factor;
  float seed;
};

struct NoiseFbmWarpData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float octaves;
  float gain;
  float lacunarity;
  float slopeness;
  float octave_factor;
  float seed;
  float q;
  float r;
};

struct NoiseFbmPerlinWarpData : public NoiseDataBase {
  float scale_x;
  float scale_y;
  float octaves;
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
