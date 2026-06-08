#include "ReadData.h"

#include <fstream>
#include <format>

std::vector<uint8_t> ReadData(std::string_view filename) {
  std::ifstream input(std::string(filename), std::ios::in | std::ios::binary);
  if (!input.is_open()) {
    throw std::runtime_error(std::format("Unable to open file: {}", filename));
  }
  input.seekg(0, std::ios_base::end);
  std::streamsize length = input.tellg();
  input.seekg(0, std::ios_base::beg);
  std::vector<uint8_t> data(length);
  if (input.read(reinterpret_cast<char*>(data.data()), length)) {
    return data;
  }
  throw std::runtime_error(std::format("Unable to read file: {}", filename));
}