#ifndef WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
#define WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_

#include <array>
#include <string_view>
#include <stdexcept>

#include "glad/glad.h"

#include "Shader.h"

//TODO: astc format

class Texture {
 public:
  Texture() = default;
  
  Texture(std::string_view path, int format,
          int filter = GL_LINEAR, int wrap = GL_REPEAT,
          bool gen_mipmap = true);

  Texture(int size, int format, int filter = GL_LINEAR,
          int wrap = GL_REPEAT, bool integer = false);

  Texture(GLuint opengl_id, GLsizei width,
          GLsizei height, GLint format)
    : opengl_id_(opengl_id),
      width_(width),
      height_(height),
      format_(format) {}
  
  explicit Texture(const std::array<std::string, 6>& cubemap_paths) {
    LoadCubemap(cubemap_paths);
  }

  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

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

  explicit operator bool() const {
    return opengl_id_ != 0;
  }

  // for dbg (don't need for the majority of texture)
  /// component id starts from 1;
  /// component = 0 means we store all channels
  /// we don't store channels_ and type_, because we would
  /// need to store extra 8 bytes and at each texture loading
  /// compare all OpenGL texture formats (literally all) to obtain
  /// actual channels num and type, what I don't want to implement \-_-/
  void Store(std::string_view path, int channels, GLint format,
             GLenum type, int component = 0) const;

 private:
  /// doesn't matter; as a placeholder for OpenGL calls with nullptr data
  static std::size_t kNoFormat;
  static std::size_t kNoFormatI;
  static std::size_t kNoType;

  void LoadStbImage(std::string_view path, bool gen_mipmap);

  /// for .r16 and .32 extensions; we assume (width == height)
  void LoadRawFloat(std::string_view path, int float_size);

  static GLint FormatStbImageToOpenGL(int channels);

  void LoadCubemap(const std::array<std::string, 6>& cubemap_paths);
  
  void DeInit();

  /*TODO:
   * very often we need only opengl_id to render, but size and format useful
   * for debugging (or framebuffer?), so we can remove it... by declaring
   *   using Texture = DbgTexture or RelTexture;
   * so this way our thousands of textures for models could store only id,
   * BUT we still need format for framebuffer or compute shaders
   * */
  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
