#ifndef WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_
#define WIREBOUNDWORLDCREATOR_SRC_PICKINGFRAMEBFFER_H_

#include <stdexcept>
#include <set>

#include <glad/glad.h>

#include "Texture.h"
#include "Details.h"
#include "../io/Window.h"

class PickingFramebuffer {
 public:
  PickingFramebuffer()
      : texture_(gWindowWidth, gWindowHeight, GL_R32UI,
                 GL_LINEAR, GL_CLAMP_TO_EDGE, true) {
    Init();
  }
  ~PickingFramebuffer() {
    DeInit();
  }

  //in case of gWindow resize
  void UpdateResolution() {
    DeInit();
    texture_ = Texture(gWindowWidth, gWindowHeight, GL_R32UI,
                       GL_LINEAR, GL_CLAMP_TO_EDGE, true);
    Init();
  }

  [[nodiscard]] GLuint GetIdByMousePos(glm::dvec2 pos) const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    GLuint id;
    glReadPixels(static_cast<int>(pos.x),
                 gWindowHeight - static_cast<int>(pos.y), 1, 1,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return id;
  }

  [[nodiscard]] std::set<GLuint> SelectSquare(
      glm::vec2 bot_left, glm::vec2 top_right) {
    int width = std::abs(top_right.x - bot_left.x);
    int height = std::abs(top_right.y - bot_left.y);
    std::vector<GLuint> vertices(width * height);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(static_cast<int>(bot_left.x),
                 gWindowHeight - static_cast<int>(bot_left.y), width, height,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, vertices.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::set<GLuint> vertices_set{vertices.begin(), vertices.end()};
    return vertices_set;
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
