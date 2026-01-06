#include "Texture.h"

#include <stb_image.h>
#include <stb_image_write.h>

#include <algorithm>  // clamp
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Details.h"

Texture::Texture(std::string_view path, GLint format, int filter, int wrap)
    : format_(format) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  LoadTextureData(path);
  glBindTexture(GL_TEXTURE_2D, 0);
}

TextureUi::TextureUi(std::string_view path, GLint format, int filter, int wrap)
    : Texture() {
  format_ = format;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  LoadTextureData(path);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture32F::Texture32F(std::string_view path, GLint format) : Texture() {
  format_ = format;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  LoadTextureData(path);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(int width, int height, int format, int filter, int wrap)
    : width_(width), height_(height), format_(format) {
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  CreateTextureData();
  glBindTexture(GL_TEXTURE_2D, 0);
}

TextureUi::TextureUi(int width, int height, int format, int filter, int wrap)
    : Texture() {
  width_ = width;
  height_ = height;
  format_ = format;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  CreateTextureData();
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture32F::Texture32F(int width, int height, int format) : Texture() {
  width_ = width;
  height_ = height;
  format_ = format;
  glGenTextures(1, &opengl_id_);
  glBindTexture(GL_TEXTURE_2D, opengl_id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  CreateTextureData();
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

void Texture::LoadTextureData(std::string_view path) {
  GLint channels;
  unsigned char* data = stbi_load(path.data(), &width_, &height_, &channels, 0);
  if ((width_ & 3) || (height_ & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size " << width_ << ' '
              << height_ << std::endl;
  }
  if (data) {
    GLint pixel_format = FormatStbImageToOpenGL(channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, pixel_format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);  // always
    stbi_image_free(data);
  } else {
    std::cerr << "failed to load texture " << path << std::endl;
  }
}

void TextureUi::LoadTextureData(std::string_view path) {
  GLint channels;
  unsigned char* data = stbi_load(path.data(), &width_, &height_, &channels, 0);
  if ((width_ & 3) || (height_ & 3)) {
    std::cerr << "need GL_UNPACK_ALIGNMENT for size " << width_ << ' '
              << height_ << std::endl;
  }
  if (data) {
    GLint pixel_format = FormatStbImageToOpenGL(channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, pixel_format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);  // always
    stbi_image_free(data);
  } else {
    std::cerr << "failed to load texture " << path << std::endl;
  }
}

void Texture::CreateTextureData() {
  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
}

void TextureUi::CreateTextureData() {
  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, GL_RGBA_INTEGER,
               GL_UNSIGNED_BYTE, nullptr);
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

void Texture::DeInit() { glDeleteTextures(1, &opengl_id_); }

void Texture::Bind() const { glBindTexture(GL_TEXTURE_2D, opengl_id_); }

void Texture::GetTextureData(GLint format, std::vector<uint8_t>& pixels,
                             int component, GLint channels) const {
  glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels.data());
}

void TextureUi::GetTextureData(GLint format, std::vector<uint8_t>& pixels,
                               int component, GLint channels) const {
  glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels.data());
}

void Texture::Store(std::string_view path, GLint channels, GLint format,
                    int component) const {
  if (channels < component || component < 0) {
    throw std::runtime_error("specified wrong channel component");
  }
  glBindTexture(GL_TEXTURE_2D, opengl_id_);

  std::vector<uint8_t> pixels(width_ * height_ * channels);
  GetTextureData(format, pixels, component, channels);
  glBindTexture(GL_TEXTURE_2D, 0);

  bool select_all = component == 0;
  int out_channels_num = select_all ? channels : 1;
  std::vector<uint8_t> selected_pixels(width_ * height_ * out_channels_num);
  if (select_all) {
    selected_pixels = pixels;
  } else {
    for (int i = 0; i < width_ * height_; ++i) {
      selected_pixels[i] =
          static_cast<uint8_t>(pixels[i * channels + component - 1]);
    }
  }
  stbi_write_png(path.data(), width_, height_, out_channels_num,
                 selected_pixels.data(), 0);
}

void Texture32F::CreateTextureData() {
  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, GL_RGBA, GL_FLOAT,
               nullptr);
}

void Texture32F::LoadTextureData(std::string_view path) {
  std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "failed to open " << path << std::endl;
    return;
  }
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  size_t num_floats = file_size / sizeof(float);
  32;
  width_ = height_ = static_cast<int>(std::sqrt(num_floats));
  std::vector<float> data(num_floats);

  file.read(reinterpret_cast<char*>(data.data()), file_size);
  if (file.fail()) {
    std::cerr << "failed to read data from " << path << std::endl;
    return;
  }
  int dim = static_cast<int>(std::sqrt(num_floats));
  if (dim * dim != num_floats) {
    throw "Warning: non-square heightmap, or corrupt file.";
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, GL_RED, GL_FLOAT,
               data.data());
}

void Texture32F::GetTextureData(GLint format, std::vector<uint8_t>& pixels,
                                int component, GLint channels) const {
  std::vector<float> float_pixels(width_ * height_ * channels);
  glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT, float_pixels.data());
  pixels = FloatsToUint(float_pixels, component, channels);
}

std::vector<uint8_t> Texture32F::FloatsToUint(const std::vector<float>& data,
                                              int component, GLint channels) {
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
      minValue = std::min(data[i + component - 1], minValue);
      maxValue = std::max(data[i + component - 1], maxValue);
    }
  }

  std::size_t size = data.size();
  std::vector<uint8_t> pngData(size, 100);
  if (maxValue == minValue /* || data.empty()*/) {
    std::cerr << "Texture::Store() uniform values: " << minValue
              << "; stored as black" << std::endl;
    return pngData;
  }
  // simple normalization to [0, 255]
  for (size_t i = 0; i < size; ++i) {
    // in case of non-selected elements we'll simply have trash
    // (trash discarded in Store())
    pngData[i] = static_cast<uint8_t>(255.0f * (data[i] - minValue) /
                                      (maxValue - minValue));
    // TODO: temp
    //    pngData[i] = static_cast<uint8_t>(data[i]);
  }
  return pngData;
}
