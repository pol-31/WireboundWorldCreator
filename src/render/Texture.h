#ifndef WIREBOUNDWORLDCREATOR_TEXTURE_H
#define WIREBOUNDWORLDCREATOR_TEXTURE_H

#include <cinttypes>
#include <string_view>
#include <vector>

#include <glad/glad.h>

class Texture;

using Texture32F = Texture;
using TextureUi = Texture;

class Texture {
public:
  Texture() = default;

  ~Texture();

  Texture(std::string_view path, GLint format, GLint format_internal,
    GLenum type, GLint filter = GL_LINEAR, GLint wrap = GL_REPEAT);

  Texture(GLsizei width, GLsizei height, GLint format,
    GLint format_internal, GLenum type)
      : Texture(width, height, format, format_internal,
        type, GL_LINEAR, GL_REPEAT) {}

  Texture(GLsizei width, GLsizei height, GLint format, GLint format_internal,
    GLenum type, GLint filter, GLint wrap);

  /// in case id already generated, but we want RAII
  Texture(GLuint id, GLsizei width, GLsizei height,
    GLint format, GLenum format_internal, GLenum type)
      : id_(id),
        width_(width),
        height_(height),
        format_internal_(format_internal),
        format_(format),
        type_(type) {}

  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  /// narrows floats to uint8, can store specified component
  void StoreImage(std::string_view path, GLint channels, int component = -1) const;

  /// stores image with all components, float to raw .r32
  void StoreData(std::string_view path, GLint channels) const;

  /// shader's sampler2D
  void BindSampler(GLenum unit) const;

  /// comp shader's image2D
  void BindImage(GLuint unit, GLenum access) const;

  void Clear() const;

  [[nodiscard]] GLuint GetId() const { return id_; }

  [[nodiscard]] GLsizei GetWidth() const { return width_; }

  [[nodiscard]] GLsizei GetHeight() const { return height_; }

  [[nodiscard]] GLsizei GetChannelSize() const { return width_ * height_; }

  [[nodiscard]] GLsizei GetFormatInternal() const { return format_internal_; }

  [[nodiscard]] GLenum GetFormat() const { return format_; }

  [[nodiscard]] GLenum GetType() const { return type_; }

  explicit operator bool() const { return id_ != 0; }

private:
  void LoadImage8(std::string_view path);

  void LoadImageF(std::string_view path);

  static std::vector<uint8_t> FloatsToUint(const std::vector<float>& data,
    GLint channels, int component);

  GLuint id_ = 0;
  GLsizei width_ = 0;
  GLsizei height_ = 0;
  GLint format_internal_ = GL_NONE;
  GLenum format_ = GL_NONE;
  GLenum type_ = GL_NONE;
};

#endif  // WIREBOUNDWORLDCREATOR_TEXTURE_H
