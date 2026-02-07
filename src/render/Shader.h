#ifndef WIREBOUNDWORLDCREATOR_SRC_RENDER_SHADER_H_
#define WIREBOUNDWORLDCREATOR_SRC_RENDER_SHADER_H_

#include <glad/glad.h>

#include <filesystem>
#include <string>
#include <string_view>

#include "../common/FixedSizeQueue.h"

class Shader {
 public:
  Shader() = default;

  Shader(std::string_view comp_path,
    const std::vector<int>& texture_units);

  Shader(std::string_view vert_path, std::string_view frag_path,
    const std::vector<int>& texture_units);

  Shader(std::string_view vert_path, std::string_view geom_path,
      std::string_view frag_path,
      const std::vector<int>& texture_units);

  Shader(std::string_view vert_path, std::string_view tesc_path,
      std::string_view tese_path, std::string_view frag_path,
      const std::vector<int>& texture_units);

  Shader(const Shader& other) = delete;
  Shader& operator=(const Shader& other) = delete;

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  ~Shader();

  void Bind() const;

  [[nodiscard]] GLuint GetId() const { return id_; }

  void DebugUpdate();

private:
  static void CheckCompilation(GLuint shader, GLenum type);

  static void CheckLinking(GLuint shader);

  static std::string ShaderNameFromType(GLenum type);

  static GLuint AttachShader(
    GLuint program_id, std::string_view path, GLenum type);

  std::filesystem::file_time_type GetLastModificationTime();

  /// sets shader sampler2D uniforms
  void SetTextureUnits() const;

  GLuint id_ = 0;

  /// NDEBUG
  std::filesystem::file_time_type last_modification_time_
    = std::filesystem::file_time_type::min();
  FixedSizeQueue<std::string_view, 4> paths_;
  std::vector<int> textures_units_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_RENDER_SHADER_H_
