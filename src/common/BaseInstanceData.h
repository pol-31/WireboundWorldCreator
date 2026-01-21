#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_

#include <cmath>
#include <glm/glm.hpp>
#include <string>

inline glm::vec4 HSBtoRGB(float h, float s, float b) {
  float r = 0, g = 0, bl = 0;
  if (s == 0.0f) {
    // Achromatic (gray)
    r = g = bl = b;
  } else {
    h = fmod(h, 1.0f) * 6.0f;  // [0,6)
    int i = (int)floor(h);
    float f = h - i;
    float p = b * (1.0f - s);
    float q = b * (1.0f - s * f);
    float t = b * (1.0f - s * (1.0f - f));

    switch (i) {
      case 0:
        r = b;
        g = t;
        bl = p;
        break;
      case 1:
        r = q;
        g = b;
        bl = p;
        break;
      case 2:
        r = p;
        g = b;
        bl = t;
        break;
      case 3:
        r = p;
        g = q;
        bl = b;
        break;
      case 4:
        r = t;
        g = p;
        bl = b;
        break;
      case 5:
        r = b;
        g = p;
        bl = q;
        break;
    }
  }

  return {r, g, bl, 1.0f};
}

inline glm::vec4 HSBtoRGB(glm::vec3 hsb) {
  return HSBtoRGB(hsb.x, hsb.y, hsb.z);
}

struct BaseInstanceData {
  std::string name = {};
  glm::vec4 color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
  bool do_show = true;
};

inline glm::vec3 RGBtoHSB(float r, float g, float b) {
  float maxc = std::max(std::max(r, g), b);
  float minc = std::min(std::min(r, g), b);
  float delta = maxc - minc;

  float h = 0.0f;
  float s = 0.0f;
  float v = maxc;  // brightness

  if (maxc > 0.0f)
    s = delta / maxc;
  else
    return {0.0f, 0.0f, 0.0f};  // black

  if (delta == 0.0f) {
    h = 0.0f;  // undefined hue
  } else {
    if (maxc == r)
      h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
    else if (maxc == g)
      h = (b - r) / delta + 2.0f;
    else
      h = (r - g) / delta + 4.0f;

    h /= 6.0f;  // normalize to [0,1]
  }

  return {h, s, v};
}

inline glm::vec3 RGBtoHSB(glm::vec3 rgb) {
  return RGBtoHSB(rgb.x, rgb.y, rgb.z);
}

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_BASEINSTANCEDATA_H_
