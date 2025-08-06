#ifndef WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
#define WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_

#include <array>
#include <string_view>
#include <stdexcept>

#include <glad/glad.h>

#include "Shader.h"

//TODO: astc format

class Texture {
 public:
  Texture() = default;
  
  Texture(std::string_view path, int format,
          int filter = GL_LINEAR, int wrap = GL_REPEAT);

  Texture(int width, int height, GLint format,
          int filter = GL_LINEAR, int wrap = GL_REPEAT);

  /// in case id already generated, but we want RAII
  Texture(GLuint opengl_id, GLsizei width,
          GLsizei height, GLint format)
    : opengl_id_(opengl_id),
      width_(width),
      height_(height),
      format_(format) {}

  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  virtual void CreateTextureData();

  virtual void LoadTextureData(std::string_view path);

  virtual void GetTextureData(
      GLint format, std::vector<uint8_t>& pixels,
      int component, GLint channels) const;

  ~Texture() {
    DeInit();
  }

  void Bind() const;

  [[nodiscard]] GLuint GetId() const {
    return opengl_id_;
  }

  [[nodiscard]] GLsizei GetWidth() const {
    return width_;
  }
  [[nodiscard]] GLsizei GetHeight() const {
    return height_;
  }
  [[nodiscard]] GLsizei GetFormat() const {
    return format_;
  }

  [[nodiscard]] std::pair<GLsizei, GLsizei> GetSize() const {
    return {width_, height_};
  }

//  [[nodiscard]] GLsizei GetSizeByte() const {
//    return width_ * height_ * channels_;
//  }

  [[nodiscard]] GLsizei GetSizePixels() const {
    return width_ * height_;
  }

  explicit operator bool() const {
    return opengl_id_ != 0;
  }

  /// component id starts from 1;
  /// component = 0 means we store all channels;
  /// We can't use format_, because here OpenGL requires,
  /// for instance, GL_RGBA instead of GL_RGBA32F
  void Store(std::string_view path, GLint channels,
             GLint format, int component = 0) const;

 protected:
  static GLint FormatStbImageToOpenGL(GLint channels) noexcept;

  void DeInit();

  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

class Texture32F final : public Texture {
  public:
   Texture32F() = default;

   Texture32F(std::string_view path, GLint format);

   Texture32F(int width, int height, GLint format);

   Texture32F(int size, GLint format)
       : Texture32F(size, size, format) {}

   void CreateTextureData() override;

   void LoadTextureData(std::string_view path) override;

   void GetTextureData(GLint format, std::vector<uint8_t> &pixels,
                       int component, GLint channels) const override;

   static std::vector<uint8_t> FloatsToUint(
       const std::vector<float>& data, int component, GLint channels);
};

class TextureUi final : public Texture {
 public:
//  using Texture::Texture;
  TextureUi() = default;

  TextureUi(std::string_view path, int format,
          int filter = GL_LINEAR, int wrap = GL_REPEAT);

  TextureUi(int width, int height, GLint format,
          int filter = GL_LINEAR, int wrap = GL_REPEAT);

  void CreateTextureData() override;

  void LoadTextureData(std::string_view path) override;

  void GetTextureData(GLint format, std::vector<uint8_t> &pixels,
                      int component, GLint channels) const override;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
