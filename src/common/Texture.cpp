#include "Texture.h"

#include <algorithm> // clamp
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Details.h"

std::size_t Texture::kNoFormat = GL_RGBA;
std::size_t Texture::kNoFormatI = GL_RGBA_INTEGER;
std::size_t Texture::kNoType = GL_UNSIGNED_BYTE;

Texture::Texture(std::string_view path, GLint format,
                 int filter, int wrap, bool gen_mipmap)
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

Texture::Texture(int width, int height, int format,
                 int filter, int wrap, bool integer)
    : width_(width), height_(height), format_(format) {
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

GLint Texture::FormatStbImageToOpenGL(GLint channels) noexcept {
  GLint format;
  switch (channels) {
    case 1:
      format = GL_RED;
//      std::cout << "channels:: red" << std::endl;
      break;
    case 2:
      format = GL_RG;
//      std::cout << "channels:: rg" << std::endl;
      break;
    case 3:
      format = GL_RGB;
//      std::cout << "channels:: rgb" << std::endl;
      break;
    default:
//      std::cout << "channels:: rgba" << std::endl;
      format = GL_RGBA;
  }
  return format;
}

void Texture::LoadStbImage(std::string_view path, bool gen_mipmap) {
  GLint channels;
  unsigned char* data = stbi_load(path.data(), &width_,
                                  &height_, &channels, 0);
  if ((width_ & 3) || (height_ & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size "
              << width_ << ' ' << height_ << std::endl;
  }
  if (data) {
//    std::cout << path << ' ';
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
  format_ = GL_RGBA;
  GLint channels = 4;
  //TODO: don't need width, height, channels... so maybe return only id?
  stbi_set_flip_vertically_on_load(false);
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
  stbi_set_flip_vertically_on_load(true);
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

std::vector<uint8_t> Texture::FloatsToUint(
    const std::vector<float>& data, int component, GLint channels) {
  float minValue = std::numeric_limits<float>::max();
  float maxValue = std::numeric_limits<float>::min();
  bool select_all = component == 0;
  if (select_all) {
    // TODO: valid iterators after?
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end());
    minValue = *minIt;
    maxValue = *maxIt;
  } else {
    // channels start from 1
    for (std::size_t i = 0; i < data.size(); i += channels) {
      minValue = std::min(data[i + component-1], minValue);
      maxValue = std::max(data[i + component-1], maxValue);
    }
  }

  std::size_t size = data.size();
  std::vector<uint8_t> pngData(size, 100);
  if (maxValue == minValue/* || data.empty()*/) {
    std::cerr << "Texture::Store() uniform values; stored as black" << std::endl;
    return pngData;
  }
  // simple normalization to [0, 255]
  for (size_t i = 0; i < size; ++i) {
    // in case of non-selected elements we'll simply have trash
    // (trash discarded in Store())
//    pngData[i] = static_cast<uint8_t>(
//        255.0f * (data[i] - minValue) / (maxValue - minValue)
//    );
    //TODO: temp
    pngData[i] = static_cast<uint8_t>(data[i]);

  }
  return pngData;
}

void Texture::Store(std::string_view path, GLint channels, GLint format,
                    GLenum type, int component) const {
  if (channels < component || component < 0) {
    throw std::runtime_error("specified wrong channel component");
  }
  glBindTexture(GL_TEXTURE_2D, opengl_id_);

  std::vector<uint8_t> pixels(width_ * height_ * channels);
  if (type == GL_FLOAT) {
    std::vector<float> float_pixels(width_ * height_ * channels);
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, float_pixels.data());
    pixels = FloatsToUint(float_pixels, component, channels);
  } else if (type == GL_UNSIGNED_BYTE) {
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, pixels.data());
  } else {
    std::cerr << "texture serializing for type " << static_cast<int>(type)
              << " hasn't been implemented yet" << std::endl;
    return;
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  bool select_all = component == 0;
  int out_channels_num = select_all ? channels : 1;
  std::vector<uint8_t> selected_pixels(width_ * height_ * out_channels_num);
  if (select_all) {
    selected_pixels = pixels;
  } else {
    for (int i = 0; i < width_ * height_; ++i) {
      selected_pixels[i] = static_cast<uint8_t>(
          pixels[i * channels + component - 1]);
    }
  }
  stbi_write_png(path.data(), width_, height_,
                 out_channels_num, selected_pixels.data(), 0);
}
