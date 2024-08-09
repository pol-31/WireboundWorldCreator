#ifndef WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_

#include <stdexcept>

#include "glad/glad.h"

#include "Texture.h"
#include "Details.h"

class PickingFramebuffer {
 public:
  PickingFramebuffer() {
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

 private:
  void Init() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, details::kWindowWidth,
                 details::kWindowHeight, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // TODO: need dynamically adjust at size changes
    tex_ = Texture(tex_id, details::kWindowWidth,
                   details::kWindowHeight, GL_R32UI);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tex_.GetId(), 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("framebuffer is not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void DeInit() {
    glDeleteFramebuffers(1, &fbo_);
  }

  GLuint fbo_;
  Texture tex_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
