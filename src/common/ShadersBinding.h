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
inline constexpr int kSpriteBrightness = 2;
inline constexpr int kSpriteColor = 3;
inline constexpr int kSpriteHoveFactor = 4;

inline constexpr int kSpritePickingTexture = 0;
inline constexpr int kSpritePickingId = 1;

inline constexpr int kDynamicSpriteScale = 0;
inline constexpr int kDynamicSpriteTranslate = 1;
inline constexpr int kDynamicSpriteTexture = 2;

inline constexpr int kPlacementHeightMap = 0;
inline constexpr int kPlacementTexture = 1;

inline constexpr int kPoissonAreaSize = 0;

inline constexpr int kWaterHeightMap = 0;

inline constexpr int kGraphHeightMap = 0;
inline constexpr int kGraphColor = 1;

inline constexpr int kPlacementPoint = 0;
inline constexpr int kPlacementRadius = 1;
inline constexpr int kPlacementFalloff = 2;
inline constexpr int kPlacementColor = 3;

inline constexpr int kCubemapTexture = 0;

inline constexpr int kGrassCurrentTime = 0;
inline constexpr int kGrassDeltaTime = 1;
inline constexpr int kGrassWindMagnitude = 2;
inline constexpr int kGrassWindWaveLength = 3;
inline constexpr int kGrassWindWavePeriod = 4;

const int LOCAL_WORK_GROUPS_X = 8;
const int LOCAL_WORK_GROUPS_Y = 8;

// Ocean.h Kernel IDs:
int KERNEL_INITIAL_SPECTRUM;
int KERNEL_CONJUGATE_SPECTRUM;
int KERNEL_TIME_DEPENDENT_SPECTRUMS;
int KERNEL_RESULT_TEXTURES;

// Ocean.h Property IDs
readonly int SIZE_PROP = Shader.PropertyToID("Size");
readonly int LENGTH_SCALE_PROP = Shader.PropertyToID("LengthScale");
readonly int CUTOFF_HIGH_PROP = Shader.PropertyToID("CutoffHigh");
readonly int CUTOFF_LOW_PROP = Shader.PropertyToID("CutoffLow");

readonly int NOISE_PROP = Shader.PropertyToID("Noise");
readonly int H0_PROP = Shader.PropertyToID("H0");
readonly int H0K_PROP = Shader.PropertyToID("H0K");
readonly int PRECOMPUTED_DATA_PROP = Shader.PropertyToID("WavesData");
readonly int TIME_PROP = Shader.PropertyToID("Time");

readonly int Dx_Dz_PROP = Shader.PropertyToID("Dx_Dz");
readonly int Dy_Dxz_PROP = Shader.PropertyToID("Dy_Dxz");
readonly int Dyx_Dyz_PROP = Shader.PropertyToID("Dyx_Dyz");
readonly int Dxx_Dzz_PROP = Shader.PropertyToID("Dxx_Dzz");
readonly int LAMBDA_PROP = Shader.PropertyToID("Lambda");

readonly int DISPLACEMENT_PROP = Shader.PropertyToID("Displacement");
readonly int DERIVATIVES_PROP = Shader.PropertyToID("Derivatives");
readonly int TURBULENCE_PROP = Shader.PropertyToID("Turbulence");


// Fft.h Kernel IDs:
const int LOCAL_WORK_GROUPS_X = 8;
const int LOCAL_WORK_GROUPS_Y = 8;
readonly int KERNEL_PRECOMPUTE;
readonly int KERNEL_HORIZONTAL_STEP_FFT;
readonly int KERNEL_VERTICAL_STEP_FFT;
readonly int KERNEL_HORIZONTAL_STEP_IFFT;
readonly int KERNEL_VERTICAL_STEP_IFFT;
readonly int KERNEL_SCALE;
readonly int KERNEL_PERMUTE;

// Fft.h Property IDs:
readonly int PROP_ID_PRECOMPUTE_BUFFER = Shader.PropertyToID("PrecomputeBuffer");
readonly int PROP_ID_PRECOMPUTED_DATA = Shader.PropertyToID("PrecomputedData");
readonly int PROP_ID_BUFFER0 = Shader.PropertyToID("Buffer0");
readonly int PROP_ID_BUFFER1 = Shader.PropertyToID("Buffer1");
readonly int PROP_ID_SIZE = Shader.PropertyToID("Size");
readonly int PROP_ID_STEP = Shader.PropertyToID("Step");
readonly int PROP_ID_PINGPONG = Shader.PropertyToID("PingPong");

} // namespace shader

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADERSBINDING_H_
