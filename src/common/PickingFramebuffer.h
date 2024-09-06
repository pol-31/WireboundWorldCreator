#ifndef WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_

#include <stdexcept>

#include "glad/glad.h"

#include "Texture.h"
#include "Details.h"

class PickingFramebuffer {
 public:
  PickingFramebuffer()
      : texture_(details::kWindowWidth, GL_R32UI,
                 GL_LINEAR, GL_CLAMP_TO_EDGE, true) {
    Init();
  }
  ~PickingFramebuffer() {
    DeInit();
  }

  [[nodiscard]] GLuint GetIdByMousePos(glm::dvec2 pos) const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    GLuint id;
    glReadPixels(static_cast<int>(pos.x),
                 details::kWindowHeight - static_cast<int>(pos.y), 1, 1,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return id;
  }

  void Bind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    GLuint clearValue = std::numeric_limits<GLuint>::max();
    glClearBufferuiv(GL_COLOR, 0, &clearValue);
  }

 private:
  void Init() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_.GetId(), 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void DeInit() {
    glDeleteFramebuffers(1, &fbo_);
  }

  GLuint fbo_{0};
  Texture texture_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
