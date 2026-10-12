#include "Cubemap.h"

#include <filesystem>
#include <array>
#include <iostream>
#include <string_view>

#include <glm/glm.hpp>
#include <stb_image.h>

#include "../common/models/Scene.h"

CubemapRenderData Cubemap::GetRenderData() const noexcept {
  return {vao_, texture_.GetId()};
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

std::array<std::string, 6> gDefaultPaths{
  "../assets/skybox/field_posx.jpg", "../assets/skybox/field_negx.jpg",
  "../assets/skybox/field_posy.jpg", "../assets/skybox/field_negy.jpg",
  "../assets/skybox/field_posz.jpg", "../assets/skybox/field_negz.jpg"};

void Cubemap::LoadZone(const SceneZone* zone) {
  GLuint opengl_id = 0;
  glGenTextures(1, &opengl_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, opengl_id);

  stbi_set_flip_vertically_on_load(true);

  int width = 1;
  int height = 1;
  int channels = 0;

  const float black_pixel[3] = {0.0f, 0.0f, 0.0f};

  for (int i = 0; i < 6; ++i) {
    std::string filename = "../assets/ibl/" + zone->name + "_face_" + std::to_string(i) + ".hdr";

    float* data = nullptr;
    int face_width = 1;
    int face_height = 1;

    if (std::filesystem::exists(filename)) {
      // Force 3 channels (RGB) for HDR sampling
      data = stbi_loadf(filename.c_str(), &face_width, &face_height, &channels, 3);
    }
    if (data) {
      width = face_width;
      height = face_height;
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                   width, height, 0, GL_RGB, GL_FLOAT, data);

      stbi_image_free(data);
    } else {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                   1, 1, 0, GL_RGB, GL_FLOAT, black_pixel);
    }
  }

  // stbi_set_flip_vertically_on_load(false); // Restore engine default

  // Texture Filtering & Clamping
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  texture_ = Texture(opengl_id, width, height, GL_RGB16F, GL_RGB, GL_FLOAT);
}

void Cubemap::LoadDefaultCubemap() {
  std::array<std::string, 6> paths_cubemap{
    "../assets/skybox/field_posx.jpg", "../assets/skybox/field_negx.jpg",
    "../assets/skybox/field_posy.jpg", "../assets/skybox/field_negy.jpg",
    "../assets/skybox/field_posz.jpg", "../assets/skybox/field_negz.jpg"};
  GLuint opengl_id;
  glGenTextures(1, &opengl_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, opengl_id);
  GLint format = GL_RGBA;
  GLint channels = 4;
  // TODO: don't need width, height, channels... so maybe return only id?
  stbi_set_flip_vertically_on_load(false);
  GLsizei width, height;
  for (int i = 0; i < paths_cubemap.size(); ++i) {
    unsigned char* data =
        stbi_load(paths_cubemap[i].data(), &width, &height, &channels, 0);
    if (data) {
      GLint pixel_format = FormatStbImageToOpenGL(channels);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width,
                   height, 0, pixel_format, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: "
                << paths_cubemap[i] << std::endl;
      stbi_image_free(data);
    }
  }
  stbi_set_flip_vertically_on_load(true);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  //// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  texture_ =
      Texture(opengl_id, width, height, format, format, GL_UNSIGNED_BYTE);
}

void Cubemap::Init() {
  float skybox_vertices[] = {
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
}

void Cubemap::DeInit() {
  glDeleteBuffers(1, &vbo_);
  glDeleteVertexArrays(1, &vao_);
}
