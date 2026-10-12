#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_

#include <glad/glad.h>

#include "../render/Shader.h"
#include "../render/Texture.h"

struct CubemapRenderData {
  GLuint vao;
  GLuint texture_id;
};

class SceneZone;

class Cubemap {
 public:
  Cubemap() {
    Init();
  }

  ~Cubemap() {
    DeInit();
  }

  CubemapRenderData GetRenderData() const noexcept;

  void LoadZone(const SceneZone* zone);

  void LoadDefaultCubemap();

 private:
  void Init();

  void DeInit();

  Texture texture_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
