#ifndef WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_
#define WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_

#include <array>
#include <string_view>

#include <glm/glm.hpp>

#include "GlWrappers.h"

#include "vbo/UiDataMain.h"

namespace details {

inline constexpr int kTexPositionSize = sizeof(float) * 8;
inline constexpr int kTexCoordsSize = sizeof(float) * 8;

/// GL_TRIANGLE_STRIP
inline constexpr std::array<float, 8> kTexPositionDefault = {
    1.0f, -1.0f,
    1.0f, 1.0f,
    -1.0f, -1.0f,
    -1.0f, 1.0f
};
inline constexpr std::array<float, 8> kTexCoordsDefault = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

inline constexpr std::array<float, 8> kTexPositionCursor = {
    1.0f / 64.0f, -1.0f / 64.0f,
    1.0f / 64.0f, 1.0f / 64.0f,
    -1.0f / 64.0f, -1.0f / 64.0f,
    -1.0f / 64.0f, 1.0f / 64.0f
};

inline constexpr int kWindowWidth = 1024;
inline constexpr int kWindowHeight = 1024;

inline constexpr int kPrerenderedFramebufferSize = 1024;

inline constexpr int kBitmapTotalChar = 96; // TODO: static_cast
inline constexpr int kCharMaxResolution = 128;

/// column num should be power of two and kBitmapTotalChar in total
/// (alternative is 16 by 6);
/// texture size also power of two for GPU optimizations,
/// better mip-mapping, cache coherence, etc...
inline constexpr int kBitmapColumnNum = 8;
inline constexpr int kBitmapRowNum = 12;

/// small adjustment, used for rendering
/// useful for playing around with kBitmapRowNum & kBitmapColumnNum
inline consteval int GetRenderRowNum() {
  /// -1 just because it doesn't work without it... idk...
  return details::kBitmapRowNum + 1;
}

inline constexpr int kFontBufferSize = 1 << 25;

inline constexpr int kShaderErrorBufferSize = 1024;

inline constexpr int kAsciiFirstChar = 0x0020; // ' '
inline constexpr int kAsciiLastChar = 0x007E; // '~'

// TODO: adjust
inline constexpr float kMinSkylineLen = 0.005f;

inline constexpr int kUboCameraBind = 0;
inline constexpr int kUboMapScaleBind = 1;

inline constexpr float kHeightMapScale = 256.0f;

inline constexpr std::string_view kPathsFilePath = "../assets/Paths.txt";


// TODO; separate shader for "text" messages
// centre pos for message info text sprite
inline constexpr std::array<float, 2> kMsgInfoPos = {0.0f, -0.9f};
// centre pos for message error text sprite
inline constexpr std::array<float, 2> kMsgErrorPos = {0.0f, 0.0f};


// we have starting position of slider in vbo, so
// final position would be starting - kSliderHeight
// (we start at lowest position). Used both for Radius and Falloff sliders
// (both Radius and Falloff sliders have the same height - we picked Radius)
inline constexpr float kSliderHeight =
    kUiVboDataMain[GetUiDataMainOffset(
                       UiVboDataMainId::kUiSliderRadiusArea) + 3]
    - kUiVboDataMain[GetUiDataMainOffset(
                         UiVboDataMainId::kUiSliderRadiusSlider) + 1];



} // namespace details

#endif  // WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_
