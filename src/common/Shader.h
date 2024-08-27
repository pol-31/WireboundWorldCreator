#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_

#include <glad/glad.h>

#include <string>
#include <string_view>

class Shader {
 public:
  Shader() = default;

  explicit Shader(std::string_view comp_path) {
    Init(comp_path);
  }

  Shader(std::string_view vert_path, std::string_view frag_path) {
    Init(vert_path, frag_path);
  }

  Shader(std::string_view vert_path, std::string_view frag_path,
         std::string_view geom_path) {
    Init(vert_path, frag_path, geom_path);
  }

  Shader(std::string_view vert_path, std::string_view tesc_path,
         std::string_view tese_path, std::string_view frag_path) {
    Init (vert_path, tesc_path, tese_path, frag_path);
  }

  ~Shader();

  Shader(const Shader& other) = delete;
  Shader& operator=(const Shader& other) = delete;

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  void Bind() const;

 protected:
  GLuint opengl_id_{0};

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
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_SHADER_H_
