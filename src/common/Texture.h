#ifndef WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
#define WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_

#include <array>
#include <string_view>

#include "glad/glad.h"

#include "Shader.h"

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
    kPlacementMap,
  };
 
  Texture() = default;
  
  explicit Texture(std::string_view path) {
    Init(path);
  }
  
  explicit Texture(Type type) {
    switch (type) {
      case Type::kId:
        InitIdTexture();
        break;
      case Type::kHeightMap:
        InitHeightMap();
        break;
      case Type::kPlacementMap:
        InitPlacementMap();
        break;
    }
  }
  
  Texture(GLuint opengl_id, GLsizei width,
          GLsizei height, GLint format)
    : opengl_id_(opengl_id),
      width_(width),
      height_(height),
      format_(format) {}
  
  Texture(const std::array<std::string, 6>& cubemap_paths) {
    Init(cubemap_paths);
  }

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

  void InitPlacementMap();
  
  void Init(const std::array<std::string, 6>& cubemap_paths);
  
  void DeInit();

  GLuint opengl_id_{0};
  GLsizei width_{0};
  GLsizei height_{0};
  GLint format_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC__TEXTURE_H_
