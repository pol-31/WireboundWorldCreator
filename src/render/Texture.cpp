#include "Texture.h"

#include <stb_image.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include "../common/Details.h"
#include "../io/Window.h"

struct Format {
  GLsizei width;
  GLsizei height;
  GLenum format = GL_NONE;
  GLint format_internal = GL_NONE;
  GLenum type = GL_NONE;
};

static Format sFormats[]{
    Format(details::gTerrainSize, details::gTerrainSize, GL_RGBA, GL_RGBA32F,
           GL_FLOAT),
    Format(details::gTerrainSize, details::gTerrainSize, GL_RED, GL_R32F,
           GL_FLOAT),
    Format(details::gTerrainSize, details::gTerrainSize, GL_RGBA, GL_RGBA8,
           GL_UNSIGNED_BYTE),
    Format(details::gTerrainSize, details::gTerrainSize, GL_RG, GL_RG8,
           GL_UNSIGNED_BYTE),
    Format(details::gTerrainSize, details::gTerrainSize, GL_RED, GL_R8,
           GL_UNSIGNED_BYTE),
    Format(details::gWaterSize, details::gWaterSize, GL_RG, GL_RG32F, GL_FLOAT),
    Format(static_cast<int>(std::log2(details::gWaterSize)),
           details::gWaterSize, GL_RGBA, GL_RGBA32F, GL_FLOAT),
    Format(0, 0, GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT),
    Format(0, 0, GL_RED, GL_R8, GL_UNSIGNED_BYTE),
    Format(0, 0, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE),
    Format(0, 0, GL_RGBA, GL_RGBA16F, GL_FLOAT),
};

void Texture::CreateOpenGlTexture(Type type, GLint filter, GLint wrap) {
  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  Format format = sFormats[static_cast<int>(type)];
  format_ = format.format;
  format_internal_ = format.format_internal;
  type_ = format.type;
  if (format.width == 0 && format.height == 0) {
    width_ = gWindowWidth;
    height_ = gWindowHeight;
  } else {
    width_ = format.width;
    height_ = format.height;
  }
}

Texture::Texture(Type type, GLint filter, GLint wrap) {
  CreateOpenGlTexture(type, filter, wrap);
  glTexImage2D(GL_TEXTURE_2D, 0, format_internal_, width_, height_, 0, format_,
               type_, nullptr);
  Clear();
}

Texture::Texture(std::string_view path, Type type, GLint filter, GLint wrap) {
  CreateOpenGlTexture(type, filter, wrap);
  if (type_ == GL_FLOAT) {
    LoadImageF(path);
  } else if (type_ == GL_UNSIGNED_BYTE) {
    LoadImage8(path);
  } else {
    throw "unsupported texture data type";
  }
}

Texture::Texture(Texture&& other) noexcept {
  id_ = other.id_;
  other.id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_internal_ = other.format_internal_;
  format_ = other.format_;
  type_ = other.type_;
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
  id_ = other.id_;
  other.id_ = 0;
  width_ = other.width_;
  height_ = other.height_;
  format_internal_ = other.format_internal_;
  format_ = other.format_;
  type_ = other.type_;
  return *this;
}

Texture::~Texture() { glDeleteTextures(1, &id_); }

void Texture::BindSampler(GLenum unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::BindImage(GLuint unit, GLenum access) const {
  glBindImageTexture(unit, id_, 0, GL_FALSE, 0, access, format_internal_);
}

void Texture::Clear() const {
  glClearTexImage(id_, 0, format_, type_, nullptr);
}

void Texture::StoreData(std::string_view path, GLint channels) const {
  if (type_ == GL_FLOAT) {
    std::vector<float> data(GetChannelSize() * channels);
    glGetTextureImage(id_, 0, GL_RED, GL_FLOAT, data.size() * sizeof(float),
                      data.data());
    std::ofstream out_file(path.data(), std::ios::out | std::ios::binary);
    if (out_file.is_open()) {
      out_file.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(float));
    } else {
      std::cerr << "unable to write " << path << std::endl;
    }
  } else if (type_ == GL_UNSIGNED_BYTE) {
    std::vector<uint8_t> data(GetChannelSize() * channels);
    glGetTextureImage(id_, 0, format_, GL_UNSIGNED_BYTE,
                      data.size() * sizeof(uint8_t), data.data());
    stbi_write_png(path.data(), width_, height_, channels, data.data(), 0);
  } else {
    throw "unsupported texture data type";
  }
}

void Texture::StoreImage(std::string_view path, GLint channels,
                         int component) const {
  if (channels < component || component < -1) {
    throw std::runtime_error("specified wrong channel component");
  }
  std::vector<uint8_t> pixels(GetChannelSize() * channels);
  if (type_ == GL_FLOAT) {
    std::vector<float> float_pixels(GetChannelSize() * channels);
    glGetTextureImage(id_, 0, format_, GL_FLOAT,
                      float_pixels.size() * sizeof(float), float_pixels.data());
    pixels = FloatsToUint(float_pixels, component, channels);
  } else if (type_ == GL_UNSIGNED_BYTE) {
    glGetTextureImage(id_, 0, format_, GL_UNSIGNED_BYTE,
                      pixels.size() * sizeof(uint8_t), pixels.data());
  } else {
    throw "unsupported texture data type";
  }
  if (component == -1) {
    stbi_write_png(path.data(), width_, height_, channels, pixels.data(), 0);
  } else {
    std::vector<uint8_t> selected_pixels(GetChannelSize());
    for (int i = 0; i < GetChannelSize(); ++i) {
      selected_pixels[i] = pixels[i * channels + component];
    }
    stbi_write_png(path.data(), width_, height_, 1, selected_pixels.data(), 0);
  }
}

std::vector<uint8_t> Texture32F::FloatsToUint(const std::vector<float>& data,
                                              GLint channels, int component) {
  float min_value = std::numeric_limits<float>::max();
  float max_value = std::numeric_limits<float>::min();
  bool select_all = component == -1;
  if (select_all) {
    auto [min_it, max_it] = std::minmax_element(data.begin(), data.end());
    min_value = *min_it;
    max_value = *max_it;
  } else {
    for (std::size_t i = 0; i < data.size(); i += channels) {
      min_value = std::min(data[i + component], min_value);
      max_value = std::max(data[i + component], max_value);
    }
  }
  std::size_t size = data.size();
  std::vector<uint8_t> uint8_data(size, min_value);
  if (max_value == min_value) {
    std::cerr << "texture uniform values: " << min_value << std::endl;
    return uint8_data;
  }
  /// normalization to 0-255
  for (size_t i = 0; i < size; ++i) {
    uint8_data[i] = static_cast<uint8_t>(255.0f * (data[i] - min_value) /
                                         (max_value - min_value));
  }
  return uint8_data;
}

GLint DeduceChannelsNum(GLenum format) noexcept {
  int channels = 0;
  switch (format) {
    case GL_RED:
      channels = 1;
      break;
    case GL_RG:
      channels = 2;
      break;
    case GL_RGB:
      channels = 3;
      break;
    case GL_RGBA:
      channels = 4;
      break;
  }
  return channels;
}

void Texture::LoadImage8(std::string_view path) {
  GLint channels;
  GLint desired_channels = DeduceChannelsNum(format_);
  unsigned char* data =
      stbi_load(path.data(), &width_, &height_, &channels, desired_channels);
  if ((width_ & 3) || (height_ & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size " << width_ << ' '
              << height_ << std::endl;
  }
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format_internal_, width_, height_, 0,
                 format_, type_, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    std::cerr << "failed to load texture " << path << std::endl;
  }
}

void Texture::LoadImageF(std::string_view path) {
  /// ate to get file size
  std::ifstream file(path.data(), std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "failed to open " << path << std::endl;
    return;
  }
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  size_t num_floats = file_size / sizeof(float);
  std::vector<float> data(num_floats);

  file.read(reinterpret_cast<char*>(data.data()), file_size);
  if (file.fail()) {
    std::cerr << "failed to read data from " << path << std::endl;
    return;
  }
  int dim = static_cast<int>(std::sqrt(num_floats));
  if (dim * dim != num_floats) {
    throw "non-square heightmap, or corrupted file";
  }
  width_ = dim;
  height_ = dim;
  glTexImage2D(GL_TEXTURE_2D, 0, format_internal_, width_, height_, 0, format_,
               type_, data.data());
}
