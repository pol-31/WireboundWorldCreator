#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"
#include "Paths.h"

class TextRenderer {
 public:
  explicit TextRenderer(const Paths& paths);

  /// to render simple text. We could require vbo offset, translate and scle,
  /// but then if we need to draw few texts, we bind the same things few times.
  /// By providing this functino we can bind then only once
  const Shader& Bind() const;

  /// used in dbg only
  static void UnBind() ;

  /// at one frame there can be rendered only one description (for mouse pos),
  /// and we animate it is some way
  void RenderDescription(int vbo_offset) const;

 private:
  void Init();

  float animation_progress_{0.0f}; // [0;1]
  int last_animated_vbo_offset{-1};

  GLuint vao_{0};
  GLuint vbo_{0};
  Texture texture_;
  Shader shader_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_TEXTRENDERER_H_
