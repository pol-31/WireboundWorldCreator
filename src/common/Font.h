#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_FONT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_FONT_H_

#include <array>
#include <cinttypes>
#include <string_view>

namespace font {

inline constexpr int gTotalChar = 96;
inline constexpr int gRowNum = 12;
inline constexpr int gColumnNum = 8;

/// widths packed by 4 bits; how to convert:
/// float comp_width;
/// if (char_idx & 1) {
/// 	comp_width = static_cast<float>((widths[idx >> 1]) >> 4);
/// } else {
/// 	comp_width = static_cast<float>((widths[idx >> 1]) & 0x0F);
/// }
/// return static_cast<int>(comp_width * factor) + min;


// 32 is [space]
inline constexpr int gSize = 1024;
inline constexpr std::array<int, 95> gSymbols = {
    32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87,
    88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103,
    104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126,
};
inline constexpr int gWidthMin = 18;
inline constexpr int gHeight = 78;
inline constexpr int gFullHeight = 85;
//inline constexpr int gFullHeight = 78;
inline constexpr float gWidthFactor = 2.93333;
inline constexpr std::array<std::uint8_t, 48> gWidths = {
    0, 149, 184, 26, 68, 104, 97, 96,
    25, 136, 118, 88, 135, 16, 104, 120,
    135, 137, 89, 132, 8, 118, 212, 152,
    152, 136, 149, 216, 135, 54, 54, 103,
    98, 87, 86, 100, 7, 80, 240, 103,
    118, 84, 112, 245, 116, 52, 48, 6,
};
/*inline constexpr std::array<std::uint8_t, 48> gWidths = {
    18, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    113, 119, 71, 116, 55, 115, 247, 119,
    119, 119, 121, 117, 119, 23, 17, 17,
    33, 34, 34, 34, 2, 34, 32, 34,
    34, 34, 18, 34, 34, 18, 17, 33,
};*/

} // namespace font

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_FONT_H_
