#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WATERBIOME_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WATERBIOME_H_

/// oceanographic spectra
struct OceanLayerTraits {
  float scale{1.0f};
  float fetch{1.0f}; // TODO: does it related to terrain?
  float spreadBlend{0.5f};
  float swell{0.5f};
  float peakEnhancement{0.5f};
  float shortWavesFade{0.5f};
  float lambda{1.0f};
  //1 float padding?
};

struct OceanTraits {
  OceanLayerTraits near;
  OceanLayerTraits mid;
  OceanLayerTraits far;
};

//TODO: after load from tile, we should validate it (e.g. clamp())
//TODO: remove windSpeed, windDirection, because it not related to traits,
//  but to surrounding nature;
//TODO: add lambda

/// computational fluid dynamic
struct CfdWaterTraits {};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDERERS_WATER_WATERBIOME_H_
