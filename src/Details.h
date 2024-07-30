#ifndef WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_
#define WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_

#include <array>
#include <string_view>

#include <glm/glm.hpp>

#include "GlWrappers.h"

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





inline constexpr glm::vec2 kMenuWrenchPos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kMenuWrenchTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};

inline constexpr glm::vec2 kMenuTerrainModePos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kMenuTerrainModeTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};

inline constexpr glm::vec2 kMenuWaterModePos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kMenuWaterModeTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};

inline constexpr glm::vec2 kMenuRoadsModePos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kMenuRoadsModeTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};

inline constexpr glm::vec2 kMenuFencesModePos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kMenuFencesModeTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};

inline constexpr glm::vec2 kMenuWireboundIconPos = {
    0.0f, 0.0f
};
inline constexpr TexCoords kWireboundIconTexCoords = {
    0.0f, 1.0f, 1.0f, 0.0f
};


enum class UiId;

inline constexpr std::size_t GetUiDataOffset(UiId id) {
  // for each Ui component 16 floats
  // (4 2d points with position and tex coords)
  return static_cast<std::size_t>(id) * 16;
}

/// related to kUiData
enum class UiId {
  //TODO: all ui components data
};

// We put all button data to the same vbo buffer;
// here all data for ui inteface. We need only 4
// veftices for GL_DRAW TRIANGLE in order:
// right bottom - right top - left bottom - left top.
// Here for each button we need position and tex coord.
// Because we need it only in 2d, x;y is enought. (u,v fo tex coords)
// Here data represented in such order:
// - btn1 vertex1: x, y, u, v
// - btn1 vertex2: x, y, u, v
// - btn1 vertex3: x, y, u, v
// - btn1 vertex4: x, y, u, v
// - btn2 buttons data...
// This way kUiData can use the same vbo and shader, but
// in case of dynamic position component (like slider) we
// need to use another shader (vbo with starting position is the same) 
inline constexpr std::array<float, 64 * 16> kUiData = {
  // --- --- Menu --- ---
  
  // menu icon "wrench" for mode switching
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Terrain Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Water Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Roads Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Fences Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Placement Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Objects Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Biomes Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Tiles Mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // menu icon "eye" for mode vision
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Terrain Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Water Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Roads Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Fences Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Placement Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Objects Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Biomes Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "Tiles Vision"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,

  // menu btn "render with Wirebound shaders" with Wirebound logo (left)
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- between these both there's the text "render with Wirebound shaders"

  // menu btn "render with Wirebound shaders" with Wirebound logo (right)
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // some components are shared across few Modes, but
  // we keep the same position and on different Modes the same
  // components has the same position. Here they are:
  
  // --- --- Ui components (shared) --- ---
  
  // btn "Add New"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // btn "Remove"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // btn "enable position mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // btn "enable smooth mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // btn "enable slope bottom mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // btn "enable slope top mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlider for "area radius" icon "min area"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlider for "area radius" icon "max area"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlider for "area radius" icon "slider area"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlider for "area radius" icon "slider" (only starting position)
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "previous slots"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "next slots"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "slot 1"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "slot 2"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "slot 3"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "slot 4"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // UiSlots "slot 5"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Terrain -specific --- ---
  
  // mode Terrain btn "enable smooth mode"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Water -specific --- ---
  
  // mode Water btn "bake waterfall"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Water btn "bake river"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Water btn "bake lake"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Roads -specific --- ---
  
  // mode Roads btn "bake Asphalt Road"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Roads btn "bake Gravel Road"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Roads btn "bake Cobblestone Road"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Fences -specific --- ---
  
  // mode Fences btn "bake Picked Fence"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Fences btn "bake Chain Link Fence"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Fences btn "bake Wooden Privacy Fence"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Placement -specific --- ---
  
  // mode Placement btn "Color White"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "Color Light Grey"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "Color Mid Grey"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "Color Dark Grey"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "Color Black"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "draw Trees"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "draw Bushes"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "draw Tall Grass"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // mode Placement btn "draw Undergrowth"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  // --- --- Mode Objects -specific --- ---
  
  //TODO: need something like table with pages (nav left/right)
  
  // --- --- Mode Biomes -specific --- ---
  
  // mode Biomes btn "biome #1"
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  -1.0f, 1.0f, 0.0f, 1.0f,
  
  //TODO: idk how many and what biomes do I need by now
  
  // --- --- Mode Tiles -specific --- ---
  
  //TODO: a set of all tiles... idk what, but
  //  here we can switch our current working tile to another
};

// we have starting position of slider in vbo, so
// final position would be starting - kSliderHeight
// (we start at lowest position). Used both for Radius and Falloff sliders
inline constexpr float kSliderHeight = 0.5f;

//Buffer config
//glBufferData(GL_ARRAY_BUFFER, kBtnData.size() * sizeof(float), kBtnData.data(), GL_STATIC_DRAW);
//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
//glEnableVertexAttribArray(0);
//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//glEnableVertexAttribArray(1);


//TODO: here (or not here) similar for text "render with Wirebound shaders"


//TODO: because we don't want store collision box for each button, we still can use
//  picking strategy... or... no, better: we know scaled tex_coords, so we can get
//  bounding box and initialization of tex_coords converting them from [0;1] to [-1;1] and
//  adding kMenu*ModePos


inline constexpr glm::vec2 kErrorMsgPos = {
    0.0f, 0.0f
};

namespace btn {

/// here tex coords. We don't provide scale, because icons were drawn already
/// with needed scale
/// all positions related to center, so you actually set its centre here;

// tex coords

inline constexpr TexCoords kBtnSliderMax = {
    0.0f, 1.0f, 1.0f, 0.0f
};
inline constexpr TexCoords kBtnSliderMin = {
    0.0f, 1.0f, 1.0f, 0.0f
};
inline constexpr TexCoords kBtnSliderObjLow = {
    0.0f, 1.0f, 1.0f, 0.0f
};
inline constexpr TexCoords kBtnSliderObjHigh = {
    0.0f, 1.0f, 1.0f, 0.0f
};
inline constexpr TexCoords kBtnSliderArea = {
    0.0f, 1.0f, 1.0f, 0.0f
};

// positions
// glm don't have "constexpr"
inline constexpr glm::vec2 kBtnSlotsTop = {
    0.0f, 0.0f
};
// you should add it to kBtnSlotsTop.y

inline constexpr float kBtnSlotsPrevOffset = 0.2f;
inline constexpr float kBtnSlots1Offset = 0.2f;
inline constexpr float kBtnSlots2Offset = 0.2f;
inline constexpr float kBtnSlots3Offset = 0.2f;
inline constexpr float kBtnSlots4Offset = 0.2f;
inline constexpr float kBtnSlots5Offset = 0.2f;
inline constexpr float kBtnSlotsNextOffset = 0.2f;

} // namespace btn

} // namespace details

#endif  // WIREBOUNDWORLDCREATOR_SRC__DETAILS_H_
