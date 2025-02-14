#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_MATH_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_MATH_H_

#include <cmath>

namespace math {

inline constexpr float g = 9.81f;

inline float JonswapAlpha(float fetch, float windSpeed) {
  return 0.076f * std::pow(math::g * fetch / windSpeed / windSpeed, -0.22f);
}
inline float JonswapPeakFrequency(float fetch, float windSpeed) {
  return 22 * std::pow(windSpeed * fetch / math::g / math::g, -0.33f);
}

} // namespace math

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_MATH_H_
