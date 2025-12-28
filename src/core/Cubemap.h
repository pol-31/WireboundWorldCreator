#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_

#include <array>
#include <iostream>
#include <string_view>

#include <glm/glm.hpp>
#include <stb_image.h>

#include "../common/Shader.h"
#include "../common/ShadersBinding.h"
#include "../common/Texture.h"

class Cubemap {
 public:
  Cubemap() = default;

  /// order: right, left, top, bottom, front, back
  Cubemap(const std::array<std::string, 6>& cubemap_paths,
          std::string_view path_shader_vert,
          std::string_view path_shader_frag)
      : texture_(),
        shader_(path_shader_vert, path_shader_frag) {
    Init(cubemap_paths);
  }

  void LoadTextures(const std::array<std::string, 6>& cubemap_paths) {
    GLuint opengl_id;
    glGenTextures(1, &opengl_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, opengl_id);
    GLint format = GL_RGBA;
    GLint channels = 4;
    //TODO: don't need width, height, channels... so maybe return only id?
    stbi_set_flip_vertically_on_load(false);
    GLsizei width, height;
    for (int i = 0; i < cubemap_paths.size(); ++i) {
      unsigned char* data = stbi_load(
          cubemap_paths[i].data(), &width, &height, &channels, 0);
      if (data) {
        GLint pixel_format = FormatStbImageToOpenGL(channels);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                     width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
      } else {
        std::cout << "Cubemap texture failed to load at path: "
                  << cubemap_paths[i] << std::endl;
        stbi_image_free(data);
      }
    }
    stbi_set_flip_vertically_on_load(true);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    texture_ = Texture(opengl_id, width, height, format);
  }


  GLint FormatStbImageToOpenGL(GLint channels) noexcept {
    GLint format;
    switch (channels) {
      case 1:
        format = GL_RED;
        break;
      case 2:
        format = GL_RG;
        break;
      case 3:
        format = GL_RGB;
        break;
      default:
        format = GL_RGBA;
    }
    return format;
  }

  void Render() {
    // we've set GL_LEQUAL by default for the whole app
    shader_.Bind();
    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_.GetId());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }

 private:
  void Init(const std::array<std::string, 6>& cubemap_paths) {
    LoadTextures(cubemap_paths);
    shader_.Bind();
    glUniform1i(shader::kCubemapTexture, 0);
    InitBuffers();
  }
  
  void InitBuffers() {
    float skybox_vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
                 &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          reinterpret_cast<void*>(0));
  }

  Texture texture_;
  Shader shader_;
  GLuint vao_{0};
  GLuint vbo_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_CUBEMAP_H_
