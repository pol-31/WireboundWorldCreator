#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_LOCALTRANSFORM_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_LOCALTRANSFORM_H_

#include <glm/glm.hpp>

// no rotation (for parsing/serializing - we don't store rotation,
// because sprites should initially be drawn with needed angle)
struct LocalTransformLinear {
  glm::vec2 translate{0.0f, 0.0f};
  float scale{1.0f};

  void Apply(LocalTransformLinear other) {
    translate += other.translate;
    scale *= other.scale;
  }

  [[nodiscard]] bool Empty() const noexcept {
    return translate == glm::vec2(0.0f) && scale == 1.0f;
  }
};

// for parent-child transform hierarchy (we should keep components separately)
struct LocalTransform {
  glm::vec2 translate{0.0f, 0.0f};
  // scale is first for convenient parsing (we don't parese roatea)
  float scale{1.0f};
  float rotate{0.0f};  // radians

  void Apply(LocalTransform other) {
    translate += other.translate;
    rotate += other.rotate;
    scale *= other.scale;
  }

  void Apply(LocalTransformLinear other) {
    translate += other.translate;
    scale *= other.scale;
  }

  [[nodiscard]] bool Empty() const noexcept {
    return translate == glm::vec2(0.0f) && rotate == 0.0f && scale == 1.0f;
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_LOCALTRANSFORM_H_
