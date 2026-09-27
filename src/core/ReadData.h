#pragma once

#include <cinttypes>
#include <string_view>
#include <vector>

std::vector<uint8_t> ReadData(std::string_view filename);
