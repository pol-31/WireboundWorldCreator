#ifndef WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_

#include <glad/glad.h>

#include <set>

#include "../io/Window.h"
#include "Texture.h"

class PickingFramebuffer {
 public:
  PickingFramebuffer();

  ~PickingFramebuffer() { DeInit(); }

  // in case of gWindow resize
  void UpdateResolution();

  [[nodiscard]] GLuint GetIdByMousePos(glm::dvec2 pos) const;

  [[nodiscard]] std::set<GLuint> SelectSquare(glm::vec2 bot_left,
                                              glm::vec2 top_right);

  void Bind() const;

  void BindTexture(int bind_id, GLenum access) const;

  [[nodiscard]] GLuint GetId() const noexcept { return fbo_; }

  [[nodiscard]] GLuint GetTexId() const noexcept { return texture_.GetId(); }

  [[nodiscard]] const TextureUi& GetTex() const noexcept { return texture_; }

  void StoreTexture() const;

 private:
  void Init();

  void DeInit();

  GLuint fbo_{0};
  TextureUi texture_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
