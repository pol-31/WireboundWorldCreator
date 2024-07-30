#ifndef WIREBOUNDWORLDCREATOR_SRC_GLWRAPPERS_H_
#define WIREBOUNDWORLDCREATOR_SRC_GLWRAPPERS_H_

#include <algorithm>
#include <array>

#include <glad/glad.h>

#include "Details.h"

namespace {

class GlWrapperBase {
 public:
  constexpr GlWrapperBase() = default;

  constexpr GlWrapperBase(float left, float right,
                          float top, float bottom) {
    data_[0] = right;
    data_[1] = bottom;
    data_[2] = right;
    data_[3] = top;
    data_[4] = left;
    data_[5] = bottom;
    data_[6] = left;
    data_[7] = top;
  }

  [[nodiscard]] float Width() const {
    return data_[0] - data_[6];
  }
  [[nodiscard]] float Height() const {
    return data_[7] - data_[1];
  }

  [[nodiscard]] float Left() const {
    return data_[6];
  }
  [[nodiscard]] float Right() const {
    return data_[0];
  }
  [[nodiscard]] float Top() const {
    return data_[7];
  }
  [[nodiscard]] float Bottom() const {
    return data_[1];
  }

 protected:
  /// GL_TRIANGLE_STRIP:
  /// right-bottom -> right-top -> left-bottom -> left-top
  std::array<float, 8> data_{};
};

}

class TexCoords : public GlWrapperBase {
 public:
  constexpr TexCoords() = default;

  using GlWrapperBase::GlWrapperBase;

  void Bind() const {
    glBufferSubData(GL_ARRAY_BUFFER, details::kTexPositionSize,
                    details::kTexCoordsSize, data_.data());
  }

  /// for debug
  void BindAsPosition() const {
    auto data_copy = data_;
    // execution policy not implemented in gcc...
    std::transform(data_copy.begin(), data_copy.end(), data_copy.begin(),
                   [](float x) {
                     return 2.0f * x - 1.0f;
                   });
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    details::kTexCoordsSize, data_copy.data());
  }
};

class TexPosition : public GlWrapperBase {
 public:
  constexpr TexPosition() = default;
  void Bind() const {
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    details::kTexPositionSize, data_.data());
  }
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_GLWRAPPERS_H_
