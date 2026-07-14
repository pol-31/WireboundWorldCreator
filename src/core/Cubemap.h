#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_

#include <glad/glad.h>

#include "../render/Shader.h"
#include "../render/Texture.h"

struct CubemapRenderData {
  GLuint vao;
  GLuint texture_id;
};

class Cubemap {
 public:
  Cubemap();

  CubemapRenderData GetRenderData() const noexcept;

  GLint FormatStbImageToOpenGL(GLint channels) noexcept;

 private:
  void Init();

  void InitBuffers();

  Texture texture_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
