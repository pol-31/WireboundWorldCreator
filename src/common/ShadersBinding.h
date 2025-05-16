#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADERSBINDING_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADERSBINDING_H_

/// here all locations (binding points) of shaders uniform variables
namespace shader {

inline constexpr int kTerrainHeightMap = 0;
inline constexpr int kTerrainOcclusion = 1;

inline constexpr int kHeightMapPickingHeightMap = 0;
inline constexpr int kHeightMapPickingIdOffset = 1;

inline constexpr int kTextScale = 0;
inline constexpr int kTextTranslate = 1;
inline constexpr int kTextTexture = 2;


inline constexpr int kSpriteTexture = 0;
inline constexpr int kSpriteTransparency = 1;
// technically can't be in parallel with Transparency
inline constexpr int kSpriteId = 1;
inline constexpr int kSpriteBrightness = 2;
inline constexpr int kSpriteTransform = 3;

// e.g. for time-based with mask
inline constexpr int kSpriteProgress = 4;
inline constexpr int kSpriteMask = 6;

inline constexpr float kSpriteResolution = 5;


inline constexpr int kPlacementHeightMap = 0;
inline constexpr int kPlacementTexture = 1;

inline constexpr int kPoissonAreaSize = 0;

inline constexpr int kWaterHeightMap = 0;

inline constexpr int kGraphHeightMap = 0;
inline constexpr int kGraphColor = 1;

inline constexpr int kPlacementPointA = 0;
inline constexpr int kPlacementPointB = 1;
inline constexpr int kPlacementRadius = 2;
inline constexpr int kPlacementFalloff = 3;
inline constexpr int kPlacementColor = 4;

inline constexpr int kCubemapTexture = 0;

inline constexpr int kDrawPlacementHeightMap = 0;
inline constexpr int kDrawPlacementDrawLayer = 1;

inline constexpr int kGrassCurrentTime = 0;
inline constexpr int kGrassDeltaTime = 1;
inline constexpr int kGrassWindMagnitude = 2;
inline constexpr int kGrassWindWaveLength = 3;
inline constexpr int kGrassWindWavePeriod = 4;

/// uniform image2D (binding points)
inline constexpr int kOceanPrecomputeData = 0;
inline constexpr int kOceanIfftBuffer0 = 1;
inline constexpr int kOceanIfftBuffer1 = 2;
// skipped 3
inline constexpr int kOceanDxDz = 4;
inline constexpr int kOceanDyDxz = 5;
inline constexpr int kOceanDyxyDz = 6;
inline constexpr int kOceanDxxDzz = 7;
inline constexpr int kOceanDisplacement = 8;
inline constexpr int kOceanDerivatives = 9;
inline constexpr int kOceanTurbulence = 10;
inline constexpr int kOceanSpectrumBuffer = 11;
inline constexpr int kOceanSpectrumPrecomputedData = 12;
inline constexpr int kOceanSpectrumNoise = 13;
inline constexpr int kOceanSpectrumData = 14;

/// UBOs
inline constexpr int kUboCameraBind = 0;
inline constexpr int kUboMapScaleBind = 1;
inline constexpr int kUboEnvironmentBind = 2;
inline constexpr int kUboOceanSpectrumsBind = 3;

/// ordinary uniforms (location)
inline constexpr int kOceanSize = 0;
inline constexpr int kOceanTime = 1;
inline constexpr int kOceanDeltaTime = 2;
inline constexpr int kOceanIfftStep = 3;
inline constexpr int kOceanIfftPingpong = 4;
inline constexpr int kOceanLambda = 5;
inline constexpr int kOceanSpectrumLengthScale = 6;
inline constexpr int kOceanSpectrumCuttoffHigh = 7;
inline constexpr int kOceanSpectrumCuttoffLow = 8;

// TODO: can't be collision (?)
inline constexpr int kOceanSpectrumCascadeId = 9;
inline constexpr int kOceanIfftHorizontal = 9;

} // namespace shader

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADERSBINDING_H_
