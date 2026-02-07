#include "PickingFramebuffer.h"

#include <stb_image_write.h>

#include <stdexcept>

#include "Details.h"
#include "OpenGlUtility.h"

PickingFramebuffer::PickingFramebuffer() {
  UpdateResolution();
}

void PickingFramebuffer::UpdateResolution() {
  DeInit();
  texture_ = TextureUi(gWindowWidth, gWindowHeight, GL_RED_INTEGER, GL_R32UI,
    GL_UNSIGNED_INT, GL_NEAREST, GL_CLAMP_TO_EDGE);
  Init();
}

GLuint PickingFramebuffer::GetIdByMousePos(glm::dvec2 pos) const {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  GLuint id;
  glReadPixels(static_cast<int>(pos.x), gWindowHeight - static_cast<int>(pos.y),
               1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return id;
}

std::set<GLuint> PickingFramebuffer::SelectSquare(glm::vec2 bot_left,
                                                  glm::vec2 top_right) {
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

void PickingFramebuffer::Bind() const {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
  GLuint clearValue = std::numeric_limits<GLuint>::max();
  glClearBufferuiv(GL_COLOR, 0, &clearValue);
}

void PickingFramebuffer::BindTexture(int bind_id, GLenum access) const {
  texture_.BindImage(bind_id, access);
}

void PickingFramebuffer::StoreTexture() const {
  int screen_size = gWindowWidth * gWindowHeight;
  std::vector<GLuint> data(screen_size);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0, 0, gWindowWidth, gWindowHeight, GL_RED_INTEGER,
               GL_UNSIGNED_INT, data.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  std::vector<unsigned char> data_fbo_bytes(screen_size);
  for (int i = 0; i < screen_size; ++i) {
    data_fbo_bytes[i] = static_cast<unsigned char>(data[i] & 0xFF);
  }
  stbi_write_png("FboPicking.png", gWindowWidth, gWindowHeight, 1,
                 data_fbo_bytes.data(), 0);
}

void PickingFramebuffer::Init() {
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_.GetId(), 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("framebuffer is not complete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingFramebuffer::DeInit() { glDeleteFramebuffers(1, &fbo_); }