#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_

#ifndef NDEBUG
#include <filesystem>
#endif
#include <string>
#include <string_view>

#include <glad/glad.h>

#ifndef NDEBUG
#include "FixedSizeQueue.h"
#endif

class Shader {
 public:
  Shader() = default;

  explicit Shader(std::string_view comp_path) {
#ifndef NDEBUG
    last_modification_time_ = std::filesystem::last_write_time(comp_path);
    paths_.PushBack(comp_path);
#endif
    Init(comp_path);
  }

  Shader(std::string_view vert_path, std::string_view frag_path) {
#ifndef NDEBUG
    last_modification_time_ = std::filesystem::last_write_time(vert_path);
    paths_.PushBack(vert_path);
    paths_.PushBack(frag_path);
#endif
    Init(vert_path, frag_path);
  }

  Shader(std::string_view vert_path, std::string_view frag_path,
         std::string_view geom_path) {
#ifndef NDEBUG
    last_modification_time_ = std::filesystem::last_write_time(vert_path);
    paths_.PushBack(vert_path);
    paths_.PushBack(frag_path);
    paths_.PushBack(geom_path);
#endif
    Init(vert_path, frag_path, geom_path);
  }

  Shader(std::string_view vert_path, std::string_view tesc_path,
         std::string_view tese_path, std::string_view frag_path) {
#ifndef NDEBUG
    last_modification_time_ = std::filesystem::last_write_time(vert_path);
    paths_.PushBack(vert_path);
    paths_.PushBack(tesc_path);
    paths_.PushBack(tese_path);
    paths_.PushBack(frag_path);
#endif
    Init (vert_path, tesc_path, tese_path, frag_path);
  }

  Shader(const Shader& other) = delete;
  Shader& operator=(const Shader& other) = delete;

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  ~Shader();

  void Bind() const;

#ifndef NDEBUG
  bool Update();
#endif

 private:
  class ShaderObject {
   public:
    ShaderObject() = default;
    ShaderObject(const std::string& path, GLenum type);
    ~ShaderObject();

    ShaderObject(const ShaderObject& other) = delete;
    ShaderObject& operator=(const ShaderObject& other) = delete;

    ShaderObject(ShaderObject&& other) noexcept;
    ShaderObject& operator=(ShaderObject&& other) noexcept;

    [[nodiscard]] GLuint GetId() const {
      return opengl_id_;
    }

   private:
    GLuint opengl_id_{0};
  };

  void Init(std::string_view comp_path);

  void Init(std::string_view vert_path, std::string_view frag_path);

  void Init(std::string_view vert_path, std::string_view frag_path,
            std::string_view geom_path);

  void Init(std::string_view vert_path, std::string_view tesc_path,
            std::string_view tese_path, std::string_view frag_path);

  static std::string LoadShader(std::string_view path);

  static void CheckCompilation(GLuint shader, GLenum type);
  static void CheckLinking(GLuint shader);

  /// for dbg message
  static std::string ShaderNameFromType(GLenum type);

  GLuint opengl_id_{0};

#ifndef NDEBUG
  std::filesystem::file_time_type last_modification_time_;
  FixedSizeQueue<std::string_view, 4> paths_;
#endif
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_
