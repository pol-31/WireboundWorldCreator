#ifndef WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
#define WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_

#include <string_view>

#include "glad/glad.h"

#include "Shader.h"

class Texture {
 public:
  Texture() = default;
  explicit Texture(std::string_view path);
  explicit Texture(GLuint opengl_id, GLsizei width,
                   GLsizei height, GLint format);

  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  ~Texture() {
    DeInit();
  }

  // technically it's not const, because internal data
  // can be affected by other OpenGL functions
  void Bind() const;

  // technically it's not const, because opengl_id_ is internal OpenGL id
  // which can be affected by OpenGL functions
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

 private:
  void Init(std::string_view path);
  void DeInit();

  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
