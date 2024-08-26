#ifndef WIREBOUNDDEV_SRC_CUBEMAP_H_
#define WIREBOUNDDEV_SRC_CUBEMAP_H_

#include <iostream>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>
#include "stb_image.h"

#include "../shaders/ShaderBase.h"
#include "Texture.h"

class Cubemap {
 public:
  Cubemap() = default;

  /// order: right, left, top, bottom, front, back
  Cubemap(const std::vector<std::string_view>& cubemap_paths,
          std::string_view path_shader_vert,
          std::string_view path_shader_frag)
      : texture_(cubemap_paths),
        shader_(path_shader_vert, path_shader_frag) {
    Init();
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
  void Init() {
    shader_.Bind();
    shader_.SetUniform("tex", 0);
    InitBuffers();
  }
  
  void InitBuffers() {
    float skybox_vertices[] = {
        // coords
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
  ShaderBase shader_;
  GLuint vao_{0};
  GLuint vbo_{0};
};

#endif  // WIREBOUNDDEV_SRC_CUBEMAP_H_
