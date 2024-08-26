#ifndef WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
#define WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_

#include <string_view>
#include <vector>

#include "glad/glad.h"

#include "../shaders/ShaderBase.h"

/*TODO:
 * - cubemap
 * - heightmap (A)
 * - rgba32ui for framebuffer
 * */

class Texture {
 public:
  enum class Type {
    kId,
    kHeightMap,
  }
 
  Texture() = default;
  
  explicit Texture(std::string_view path) {
    Init(path);
  }
  
  explicit Texture(Type type) {
    switch (type) {
      cast Type::kId:
        InitIdTexture();
        break;
      cast Type::kHeightMap:
        InitHeightMap();
        break;
    }
  }
  
  Texture(GLuint opengl_id, GLsizei width,
          GLsizei height, GLint format)
    : opengl_id_(opengl_id),
      width_(width),
      height_(height),
      format_(format) {}
  
  Texture(const std::vector<std::string_view>& cubemap_paths);

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
  
  void InitIdTexture();
  
  void InitHeightMap();
  
  void Init(const std::vector<std::string_view>& cubemap_paths);
  
  void DeInit();

  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
