#include "Texture.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Details.h"

std::size_t Texture::kNoFormat = GL_RGBA;
std::size_t Texture::kNoFormatI = GL_RGBA_INTEGER;
std::size_t Texture::kNoType = GL_UNSIGNED_BYTE;

Texture::Texture(std::string_view path, int format, int filter,
                 int wrap, bool gen_mipmap)
    : format_(format) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  if (path.ends_with(".r16")) {
    LoadRawFloat(path, 16);
  } else if(path.ends_with(".r32")) {
    LoadRawFloat(path, 32);
  } else {
    LoadStbImage(path, gen_mipmap);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int size, int format, int filter, int wrap, bool integer)
    : width_(size), height_(size), format_(format) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  if (integer) {
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_,
                 height_, 0, kNoFormatI, kNoType, nullptr);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_,
                 height_, 0, kNoFormat, kNoType, nullptr);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

// * * * * TIPS * * * *
// --- --- --- --- load
// ../assets/map_ and GL_RGBA - internal = GL_RGBA8
// ../assets/poisson and GL_RGB - internal = GL_RGBA8
// --- --- --- --- generate
// float::
// height map - 1024, GL_RED, GL_REAREST
// rgbaf size, GL_RGBA32F, GL_NEAREST
// rgf size, GL_RG32F, GL_NEAREST
// int::
// placement map - 1024, GL_R8UI, GL_REAREST
// id texture - kWindowWidth, GL_R32UI, GL_LINEAR --- DONE

GLint Texture::FormatStbImageToOpenGL(int channels) {
  GLint format;
  switch (channels) {
    case 1:
      format = GL_RED;
      std::cout << "red" << std::endl;
      break;
    case 2:
      format = GL_RG;
      std::cout << "rg" << std::endl;
      break;
    case 3:
      format = GL_RGB;
      std::cout << "rgb" << std::endl;
      break;
    default:
      std::cout << "rgba" << std::endl;
      format = GL_RGBA;
  }
  return format;
}

void Texture::LoadStbImage(std::string_view path, bool gen_mipmap) {
  int channels;
  unsigned char* data = stbi_load(path.data(), &width_,
                                  &height_, &channels, 0);
  if (!(width_ & 3) && !(height_ & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size "
              << width_ << ' ' << height_ << std::endl;
  }
  if (data) {
    std::cout << path << ' ';
    GLint pixel_format = FormatStbImageToOpenGL(channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0,
                 pixel_format, GL_UNSIGNED_BYTE, data);
    if (gen_mipmap) {
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
  } else {
//    throw std::runtime_error("Failed to load texture");
    std::cerr << "failed to load texture " << path << std::endl;
  }
}

void Texture::LoadRawFloat(std::string_view path, int float_size) {
  std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "failed to open " << path << std::endl;
    return;
  }
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  size_t num_floats = file_size / float_size;
  width_ = height_ = static_cast<int>(std::sqrt(num_floats));
  std::vector<float> data(num_floats);

  file.read(reinterpret_cast<char*>(data.data()), file_size);
  if (file.fail()) {
    std::cerr << "failed to read data from " << path << std::endl;
    return;
  }

  if (float_size == 16) {
    format_ = GL_R16F;
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0,
                 GL_RED, GL_HALF_FLOAT, data.data());
  } else {
    format_ = GL_R32F;
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0,
                 GL_RED, GL_FLOAT, data.data());
  }
}

void Texture::LoadCubemap(const std::array<std::string, 6>& cubemap_paths) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, opengl_id_);
  int channels;
  format_ = GL_RGBA;
  //TODO: don't need width, height, channels... so maybe return only id?
  for (int i = 0; i < cubemap_paths.size(); ++i) {
    unsigned char* data = stbi_load(
        cubemap_paths[i].data(), &width_, &height_, &channels, 0);
    if (data) {
      GLint pixel_format = FormatStbImageToOpenGL(channels);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format_,
                   width_, height_, 0, pixel_format, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: "
                << cubemap_paths[i] << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::Texture(Texture&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
}

Texture& Texture::operator=(Texture&& other) noexcept {
  opengl_id_ = other.opengl_id_;
  other.opengl_id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_ = other.format_;
  return *this;
}

void Texture::DeInit() {
  glDeleteTextures(1, &opengl_id_);
}

void Texture::Bind() const {
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
}

void Texture::Store(std::string_view path, int channels,
                    GLenum type, int component) const {
  if (channels < component || component < 0) {
    throw std::runtime_error("specified wrong channel component");
  }
  int channels_num;
  if (component == 0) {
    channels_num = channels;
  } else {
    channels_num = 1;
  }
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  std::vector<float> pixels(width_ * height_ * channels);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
  std::vector<uint8_t> converted_data(width_ * height_ * channels_num);
  if (type == GL_UNSIGNED_BYTE) {
    if (component != 0) {
      for (int i = 0; i < width_ * width_ * channels; i+=channels) {
        auto set_value = static_cast<uint8_t>(pixels[i + component - 1]);
        converted_data[i / channels] = set_value;
      }
    } else {
      for (int i = 0; i < width_ * width_ * channels; ++i) {
        auto set_value = static_cast<uint8_t>(pixels[i]);
        converted_data[i] = set_value;
      }
    }
  } else if (type == GL_FLOAT) {
    // we could map all float type range into [0;255], but
    // clamping was more useful for me
    if (component != 0) {
      for (int i = 0; i < width_ * width_ * channels; i+=channels) {
        auto set_value = static_cast<uint8_t>(
            std::clamp(static_cast<int>(std::log(pixels[i + component - 1])), 0, 255));
        converted_data[i / channels] = set_value;
      }
    } else {
      float min_value = -0.05f;  // Adjust according to your dataset
      float max_value = 0.05f;   // Adjust according to your dataset

      for (int i = 0; i < width_ * width_ * channels; ++i) {
        float scaled_value = (pixels[i + component - 1] - min_value) / (max_value - min_value);
        int byte_value = std::clamp(static_cast<int>(scaled_value * 255.0f), 0, 255);
//        auto set_value = static_cast<uint8_t>(
//            std::clamp(static_cast<int>(pixels[i]) * 40000 + 120, 0, 255));
        converted_data[i] = byte_value;
      }
    }
  } else {
    std::cerr << "texture serializing for type " << static_cast<int>(type)
              << " hasn't been implemented yet" << std::endl;
  }
  stbi_write_png(path.data(), width_, height_,
                 channels_num, converted_data.data(), 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
