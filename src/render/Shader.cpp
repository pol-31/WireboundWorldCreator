#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

GLuint Shader::AttachShader(GLuint program_id, std::string_view path,
                            GLenum type) {
  std::string code;
  try {
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    shader_file.open(path.data());
    std::stringstream shader_stream;
    shader_stream << shader_file.rdbuf();
    code = shader_stream.str();
  } catch (const std::ifstream::failure& e) {
    std::cerr << "shader reading error: " << e.what() << std::endl;
  }
  GLuint shader_id_ = glCreateShader(type);
  const char* path_c = code.c_str();
  glShaderSource(shader_id_, 1, &path_c, nullptr);
  glCompileShader(shader_id_);
  CheckCompilation(shader_id_, type);
  glAttachShader(program_id, shader_id_);
  return shader_id_;
}

Shader::Shader(std::string_view comp_path,
               const std::vector<int>& textures_units) {
  id_ = glCreateProgram();
  GLuint id_comp = AttachShader(id_, comp_path, GL_COMPUTE_SHADER);
  glLinkProgram(id_);
  CheckLinking(id_);
  glDeleteShader(id_comp);

  // NDEBUG
  paths_.PushBack(comp_path);
  last_modification_time_ = GetLastModificationTime();
  textures_units_ = textures_units;
  SetTextureUnits();
}

Shader::Shader(std::string_view vert_path, std::string_view frag_path,
               const std::vector<int>& textures_units) {
  id_ = glCreateProgram();
  GLuint id_vert = AttachShader(id_, vert_path, GL_VERTEX_SHADER);
  GLuint id_frag = AttachShader(id_, frag_path, GL_FRAGMENT_SHADER);
  glLinkProgram(id_);
  CheckLinking(id_);
  glDeleteShader(id_vert);
  glDeleteShader(id_frag);

  // NDEBUG
  paths_.PushBack(vert_path);
  paths_.PushBack(frag_path);
  last_modification_time_ = GetLastModificationTime();
  textures_units_ = textures_units;
  SetTextureUnits();
}

Shader::Shader(std::string_view vert_path, std::string_view frag_path,
               std::string_view geom_path,
               const std::vector<int>& textures_units) {
  id_ = glCreateProgram();
  GLuint id_vert = AttachShader(id_, vert_path, GL_VERTEX_SHADER);
  GLuint id_geom = AttachShader(id_, frag_path, GL_GEOMETRY_SHADER);
  GLuint id_frag = AttachShader(id_, geom_path, GL_FRAGMENT_SHADER);
  glLinkProgram(id_);
  CheckLinking(id_);
  glDeleteShader(id_vert);
  glDeleteShader(id_geom);
  glDeleteShader(id_frag);

  // NDEBUG
  paths_.PushBack(vert_path);
  paths_.PushBack(geom_path);
  paths_.PushBack(frag_path);
  last_modification_time_ = GetLastModificationTime();
  textures_units_ = textures_units;
  SetTextureUnits();
}

Shader::Shader(std::string_view vert_path, std::string_view tesc_path,
               std::string_view tese_path, std::string_view frag_path,
               const std::vector<int>& textures_units) {
  id_ = glCreateProgram();
  GLuint id_vert = AttachShader(id_, vert_path, GL_VERTEX_SHADER);
  GLuint id_tesc = AttachShader(id_, tesc_path, GL_TESS_CONTROL_SHADER);
  GLuint id_tese = AttachShader(id_, tese_path, GL_TESS_EVALUATION_SHADER);
  GLuint id_frag = AttachShader(id_, frag_path, GL_FRAGMENT_SHADER);
  glLinkProgram(id_);
  CheckLinking(id_);
  glDeleteShader(id_vert);
  glDeleteShader(id_tesc);
  glDeleteShader(id_tese);
  glDeleteShader(id_frag);

  // NDEBUG
  paths_.PushBack(vert_path);
  paths_.PushBack(tesc_path);
  paths_.PushBack(tese_path);
  paths_.PushBack(frag_path);
  last_modification_time_ = GetLastModificationTime();
  textures_units_ = textures_units;
  SetTextureUnits();
}

Shader::Shader(Shader&& other) noexcept {
  id_ = other.id_;
  other.id_ = 0;
  last_modification_time_ = std::move(other.last_modification_time_);
  paths_ = std::move(other.paths_);
  textures_units_ = std::move(other.textures_units_);
}

Shader& Shader::operator=(Shader&& other) noexcept {
  if (id_ != 0) {
    glDeleteProgram(id_);
  }
  id_ = other.id_;
  other.id_ = 0;
  last_modification_time_ = std::move(other.last_modification_time_);
  paths_ = std::move(other.paths_);
  textures_units_ = std::move(other.textures_units_);
  return *this;
}

Shader::~Shader() { glDeleteProgram(id_); }

void Shader::Bind() const { glUseProgram(id_); }

void Shader::CheckCompilation(GLuint shader, GLenum type) {
  GLint success;
  GLchar buffer[256];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, std::size(buffer), nullptr, buffer);
    std::cerr << "Shader compiling error at " << ShaderNameFromType(type)
              << " shader:\n"
              << buffer << std::endl;
  }
}

void Shader::CheckLinking(GLuint shader) {
  GLint success;
  GLchar buffer[256];
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, std::size(buffer), nullptr, buffer);
    std::cerr << "Shader linking error:\n" << buffer << std::endl;
  }
}

std::string Shader::ShaderNameFromType(GLenum type) {
  switch (type) {
    case GL_VERTEX_SHADER:
      return "vertex";
    case GL_FRAGMENT_SHADER:
      return "fragment";
    case GL_GEOMETRY_SHADER:
      return "geometry";
    case GL_COMPUTE_SHADER:
      return "compute";
    case GL_TESS_CONTROL_SHADER:
      return "tess control";
    case GL_TESS_EVALUATION_SHADER:
      return "tess evaluation";
    default:
      throw std::runtime_error("incorrect shader type");
  }
}

std::filesystem::file_time_type Shader::GetLastModificationTime() {
  namespace fs = std::filesystem;
  fs::file_time_type new_time = fs::file_time_type::min();
  for (int i = 0; i < paths_.Size(); ++i) {
    new_time = std::max(new_time, fs::last_write_time(paths_[i]));
  }
  return new_time;
}

void Shader::DebugUpdate() {
  namespace fs = std::filesystem;
  auto new_time = GetLastModificationTime();
  if (new_time <= last_modification_time_) {
    return;
  }
  Shader new_shader;
  if (paths_.Size() == 1) {
    new_shader = Shader(paths_[0], textures_units_);
  } else if (paths_.Size() == 2) {
    new_shader = Shader(paths_[0], paths_[1], textures_units_);
  } else if (paths_.Size() == 3) {
    new_shader = Shader(paths_[0], paths_[1], paths_[2], textures_units_);
  } else if (paths_.Size() == 4) {
    new_shader =
        Shader(paths_[0], paths_[1], paths_[2], paths_[3], textures_units_);
  }
  *this = std::move(new_shader);
}

void Shader::SetTextureUnits() const {
  Bind();
  for (auto id : textures_units_) {
    glUniform1i(id, id);  // for glActiveTexture(id)
  }
}
