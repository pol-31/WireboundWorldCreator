#include "Texture.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "Details.h"

void Texture::Init(std::string_view path) {
  // don't need it in runtime, but good for fast test check
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int channels;
  unsigned char* data = stbi_load(path.data(), &width_,
                                  &height_, &channels, 0);
  std::cout << path << ' ';
  if (data) {
    switch (channels) {
      case 1:
        format_ = GL_RED;
        std::cout << "red" << std::endl;
        break;
      case 2:
        format_ = GL_RG;
        std::cout << "rg" << std::endl;
        break;
      case 3:
        format_ = GL_RGB;
        std::cout << "rgb" << std::endl;
        break;
      default:
        std::cout << "rgba" << std::endl;
        format_ = GL_RGBA;
    }
    //TODO: always unsigned_byte?
    if ((format_ == GL_RGBA &&
        path.starts_with("../assets/map_")) ||
        (format_ == GL_RGB &&
         path.starts_with("../assets/poisson"))) { // TODO: refactor (wtf?)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0,
                   format_, GL_UNSIGNED_BYTE, data);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0,
                   format_, GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
//    throw std::runtime_error("Failed to load texture");
    std::cerr << "failed to load texture " << path << std::endl;
  }
}

void Texture::InitIdTexture() {
  // TODO: need dynamically adjust at size changes
  width_ = details::kWindowWidth;
  height_ = details::kWindowHeight;
  format_ = GL_R32UI;
    glGenTextures(1, &opengl_id_);
    glBindTexture(GL_TEXTURE_2D, opengl_id_);

    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_,
                 height_, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::InitHeightMap() {
  // we use only 1024x1024 tile size in Wirebound
  width_ = 1024;
  height_ = 1024;
  format_ = GL_RED;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  // default-init with 0 height

  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_,
               height_, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::InitPlacementMap() {
  width_ = 1024;
  height_ = 1024;
  format_ = GL_R8UI;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_,
               height_, 0, GL_RED_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Init(const std::array<std::string, 6>& cubemap_paths) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, opengl_id_);

  //TODO; we want uniform-format textures for cubemap and ui, so
  // need flip in in advance
  stbi_set_flip_vertically_on_load(false);

  int channels;
  for (int i = 0; i < cubemap_paths.size(); ++i) {
    unsigned char* data = stbi_load(
        cubemap_paths[i].data(), &width_, &height_, &channels, 0);
    if (data) { // TODO: channels?
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                   width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      format_ = GL_RGB; // TODO: only rgb?
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: "
                << cubemap_paths[i] << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  stbi_set_flip_vertically_on_load(true);  
}

Texture::Texture(Texture&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
}
Texture& Texture::operator=(Texture&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
  return *this;
}

void Texture::DeInit() {
  glDeleteTextures(1, &opengl_id_);
}

void Texture::Bind() const {
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
}
