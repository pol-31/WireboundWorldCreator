#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_SKYLINEBOTTOMLEFT_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_SKYLINEBOTTOMLEFT_H_

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <forward_list>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Vec2i {
  int x;
  int y;
};

// in pixels
struct Aabb {
  // we could make num_lim<int>, but there's no need
  int top{0};
  int bottom{0};
  int left{0};
  int right{0};

  [[nodiscard]] int Height() const {
    return top - bottom;
  }
  [[nodiscard]] int Width() const {
    return right - left;
  }
};

struct Sprite {
  std::filesystem::path path;
  int width{0};
  int height{0};
  Aabb aabb;
};

struct SpriteEntry {
  std::string comment;
  const Sprite* sprite;
};

std::vector<Sprite> ParseSpritesDir(
    const std::filesystem::path& dir_path) {
  std::vector<Sprite> sprites;
  for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
    Sprite sprite;
    sprite.path = entry.path();
    stbi_info(sprite.path.string().c_str(), &sprite.width,
              &sprite.height, nullptr);
    if (sprite.width == 0 && sprite.height == 0) {
      std::cerr << "Unable to load sprite at " << sprite.path << std::endl;
      //      throw;
    } else {
      sprites.push_back(std::move(sprite));
    }
  }
  return sprites;
}

/// we need only int
int NearestLessEqualPowerOfTwo(int number) {
  auto n = static_cast<uint32_t>(number);
  --n;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  return static_cast<int>(++n);
}

inline Vec2i ResizeAtlas(Vec2i size) {
  if (size.x < size.y) {
    size.x <<= 1;
  } else {
    size.y <<= 1;
  }
  return size;
}

Vec2i GetAtlasSize(const std::vector<Sprite>& sprites) {
  int atlas_square = 0;
  int max_width = 0;
  int max_height = 0;
  for (const auto& s : sprites) {
    atlas_square += s.width * s.height;
    max_width = std::max(max_width, s.width);
    max_height = std::max(max_height, s.height);
  }
  Vec2i atlas_size = {
      NearestLessEqualPowerOfTwo(max_width),
      NearestLessEqualPowerOfTwo(max_height),
  };
  while (atlas_size.x * atlas_size.y < atlas_square) {
    atlas_size = ResizeAtlas(atlas_size);
  }
  return atlas_size;
}

struct SkylineNode {
  // top-left coor
  int x;
  int y;
  int actual_start;
};

struct PlaceInfo {
  std::forward_list<SkylineNode>::iterator it;
  int possible_width;
};

// it doesn't modify the list, but returns mutable iterator
PlaceInfo FindBestSkyline(
    std::forward_list<SkylineNode>& skylines, int width, int height, int atlas_width) {
  if (++skylines.begin() == skylines.end()) {
    // width of the atlas initially is GUARANTEED to be at least with
    // width and height of the largest sprites,
    // so first element is always will be placed
    return {skylines.begin(), atlas_width};
  }
  using NodePtr = std::forward_list<SkylineNode>::iterator;
  /*std::vector<NodePtr> sorted_skylines{
      skylines.begin(), skylines.end()};*/
  std::vector<NodePtr> sorted_skylines;
  for (auto it = skylines.begin(); it != skylines.end(); ++it) {
    sorted_skylines.push_back(it);
  }

  std::stable_sort(sorted_skylines.begin(), sorted_skylines.end(),
                   [](NodePtr node1, NodePtr node2) {
                     return node1->y > node2->y; // reversed: top->bottom
                   });
  for (auto cur_node : sorted_skylines) {
    if (cur_node->y < height) { // can't place due to y-overflow
      return {skylines.end(), 0};
    }
    auto next_node = cur_node;
    ++next_node;
    if (next_node == skylines.end()) {
      if ((atlas_width - cur_node->x) < width) {
        continue;
      } else {
        return {cur_node, atlas_width - cur_node->x};
      }
    }
    auto it_skyline_stop = next_node;
    for (; it_skyline_stop != skylines.end(); ++it_skyline_stop) {
      if (it_skyline_stop->actual_start > cur_node->x && it_skyline_stop->y < cur_node->y) {
        break;
      }
    }
    int possible_length;
    if (it_skyline_stop == skylines.end()) {
      possible_length = atlas_width - cur_node->x;
    } else {
      possible_length = it_skyline_stop->actual_start - cur_node->x;
    }
    if (possible_length >= width) {
      return {cur_node, possible_length};
    }
  }
  return {skylines.end(), 0}; // can't place due to x-overflow
}

// guarantee to fit
// (check for enough space was in FindBestSkyline());
// invalidates best_node
void InsertSkyline(std::forward_list<SkylineNode>& skylines,
                   PlaceInfo best_node,
                   Aabb aabb, int atlas_width) {
  int prev_y = best_node.it->y;
  best_node.it->y -= aabb.Height();
  best_node.it->actual_start = best_node.it->x;
  int skyline_stop = best_node.it->x + aabb.Width();
  auto next_node = best_node.it;
  ++next_node;
  if (next_node == skylines.end()) {
    if (aabb.Width() + best_node.it->x != atlas_width) {
      skylines.emplace_after(best_node.it, skyline_stop, prev_y, skyline_stop);
    }
  } else if (best_node.possible_width == aabb.Width()) {
    if (next_node == skylines.end()) {
      skylines.erase_after(best_node.it);

    } else {
      while (next_node->y > prev_y) {
        skylines.erase_after(best_node.it);
        next_node = best_node.it;
        ++next_node;
        if (next_node == skylines.end()) {
          break;
        }
      }
    }
  } else {
    /// merge with next? - no, we remove everything until
    /// we can merge only with prev (they've been "shadowed")
    for (; next_node != skylines.end(); ++next_node) {
      // best node 100% fits (FindBestSkyline()),
      // but do we need to create/erase a node:
      if (next_node->actual_start > skyline_stop) {
        skylines.emplace_after(best_node.it, skyline_stop, prev_y, skyline_stop);
        break;
      } else if (next_node->actual_start == skyline_stop) {
        break;
      } else {
        // shadowing
        auto next_next_node = next_node;
        ++next_next_node;
        if (next_next_node != skylines.end()) {
          skylines.erase_after(best_node.it);
          next_node = best_node.it; // recover
        } else {
          next_node->x = skyline_stop;
          //          break;
        }
      }
    }
  }
  // if in next y_is_<_our and x_is_<=our

  next_node = best_node.it;
  ++next_node;
  for (; next_node != skylines.end(); ++next_node) {
    if (next_node->y <= best_node.it->y) {
      break;
    }
    if (/*next_node->y > best_node.it->y && */next_node->x <= best_node.it->x) {
      next_node->x = skyline_stop;
    }
    // if equal we don't remove |_-_-_-_-_-|
  }

  if (best_node.it != skylines.begin()) {
    // merge with previous
    auto prev = skylines.before_begin();
    auto most_left_node = best_node.it;
    SkylineNode most_left_node_data = *(best_node.it);
    int nodes_to_remove = 0;
    for (auto it = skylines.begin(); it != best_node.it; ++prev, ++it) {
      if (it->x >= best_node.it->x && it->y > prev_y) {
        if (most_left_node != best_node.it) {
          ++nodes_to_remove;
        } else {
          most_left_node = it;
        }
      }
    }
    if (most_left_node != best_node.it) {
      while (--nodes_to_remove >= 0) {
        skylines.erase_after(most_left_node);
      }
      *most_left_node = most_left_node_data;
      best_node.it = most_left_node; // recover
    }
    //TODO: need to recover prev node
    if (prev->y == best_node.it->y) {
      skylines.erase_after(prev);
      best_node.it = prev; // recover (valid)
    }
    // merge with next
    next_node = best_node.it;
    ++next_node;
    if (next_node != skylines.end() && next_node->y == best_node.it->y) {
      skylines.erase_after(best_node.it); // no need to recover next
    }
    // to-left
    int x = 0;
    auto most_left_higher_node = best_node.it;
    for (auto prev_node = skylines.begin();
         prev_node != best_node.it; ++prev_node) {
      if (prev_node->y < best_node.it->y) {
        most_left_higher_node = prev_node;
        ++most_left_higher_node; // we need next x
        x = most_left_higher_node->x;
      }
    }
    best_node.it->x = x;
  }
}

// true if there's enough space to place;
// writes to SpriteInfo::aabb
bool TrySkylineBottomLeft(std::vector<Sprite>& sprites,
                          Vec2i size) {
  // left-to-right
  std::forward_list<SkylineNode> skylines;
  skylines.emplace_front(0, size.y); // start from the top
  int counter = 0;
  for (auto& s : sprites) {
    if (counter == 1) {
      std::cout << "now log" << std::endl;
    }
    auto best_node = FindBestSkyline(skylines, s.width, s.height, size.x);
    if (best_node.it == skylines.end()) {
      // return false if at least one doesn't fit,
      // so outside we could resize atlas
      return false;
    }
    s.aabb.left = best_node.it->x;
    s.aabb.right = best_node.it->x + s.width;
    s.aabb.top = best_node.it->y;
    s.aabb.bottom = best_node.it->y - s.height;
    InsertSkyline(skylines, best_node, s.aabb, size.x); // invalidates best_node
    if (--counter == 0) {
      break;
    }
  }
  std::cout << "skylines:" << std::endl;
  for (const auto& s : skylines) {
    std::cout << s.x << "; " << s.y << '\n';
  }
  std::cout << std::endl;
  return true;
}

void StoreAtlas(const std::vector<Sprite>& sprites,
                Vec2i size,
                std::string_view path) {
  std::vector<std::byte> data(size.x * size.y * 4, static_cast<std::byte>(0));
  for (const auto& s : sprites) {
    int x, y;
    int channels = 4;
    unsigned char* sprite_data = stbi_load(
        s.path.string().c_str(), &x, &y, nullptr, channels);
    if (!sprite_data) {
      std::cerr << "Unable to load sprite at " << s.path << std::endl;
      throw;
    }
    int offset = channels * ((size.y - s.aabb.top) * size.x + s.aabb.left);
    int stride = channels * (s.aabb.left + (size.x - s.aabb.right));
    int row_length = channels * (s.aabb.Width());
    for (int i = 0; i < (s.aabb.Height()); ++i) {
      std::memcpy(data.data() + offset + i * (stride + row_length),
                  sprite_data + i * row_length, row_length);
    }
  }
  stbi_write_png(path.data(), size.x, size.y, 4, data.data(), 0);
}

void StoreTexCoords(const std::vector<SpriteEntry>& sprite_entries,
                    Vec2i size, std::string_view path) {
  std::ofstream file(path.data());
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file: " << path << std::endl;
    return;
  }

  float atlas_ratio = static_cast<float>(size.x) / static_cast<float>(size.y);
  /*
   * // name
   * pos_x,pos_y,coord_x,coord_y x4 (rb-rt-lb-lt)
   * */
  std::ostringstream oss;
  for (const auto& s : sprite_entries) {
    float sprite_ratio = static_cast<float>(s.sprite->width) / static_cast<float>(s.sprite->height);
    Aabb aabb = s.sprite->aabb;
    float width = 0.2f;
    float height = width * atlas_ratio * sprite_ratio;
    float half_width = width / 2;
    float half_height = height / 2;
    oss << "// " << s.comment << '\n'
        << std::to_string(half_width) << ',' << std::to_string(-half_height) << ','
        << std::to_string(static_cast<float>(aabb.right) / size.x) << ','
        << std::to_string(static_cast<float>(aabb.bottom) / size.y) << ",\n"
        << std::to_string(half_width) << ',' << std::to_string(half_height) << ','
        << std::to_string(static_cast<float>(aabb.right) / size.x) << ','
        << std::to_string(static_cast<float>(aabb.top) / size.y) << ",\n"
        << std::to_string(-half_width) << ',' << std::to_string(-half_height) << ','
        << std::to_string(static_cast<float>(aabb.left) / size.x) << ','
        << std::to_string(static_cast<float>(aabb.bottom) / size.y) << ",\n"
        << std::to_string(-half_width) << ',' << std::to_string(half_height) << ','
        << std::to_string(static_cast<float>(aabb.left) / size.x) << ','
        << std::to_string(static_cast<float>(aabb.top) / size.y) << ",\n\n";
  }
  file << oss.str();
}

std::vector<SpriteEntry> ParseSpritesInfo(
    std::string_view path, const std::vector<Sprite>& sprites) {
  std::ifstream file(path.data());
  if (!file.is_open()) {
    std::cerr << "unable to open sprites info file: " << path << std::endl;
    throw;
  }
  std::vector<SpriteEntry> sprite_entries;
  std::string line, comment, entry_path;
  std::istringstream iss;
  std::getline(file, line); // skip first
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }
    iss.str(line);
    std::getline(iss, comment, ';');
    std::getline(iss, entry_path, ';');
    SpriteEntry sprite_entry(comment, nullptr);
    for (const auto& s : sprites) {
      if (entry_path == s.path.filename().string()) {
        sprite_entry.sprite = &s;
        sprite_entries.push_back(std::move(sprite_entry));
        break;
      }
    }
    if (!sprite_entry.sprite) {
      std::cerr << "specified wrong sprite path: " << entry_path << std::endl;
      throw;
    }
    iss.str();
    iss.clear();
  }
  return sprite_entries;
}

/*
 *TODO: generate sprite_info.txt alongside with sprites,
 * so we can reuse the same sprite:
 * // sprite_info.txt
 * id;ui_name;path;
 * 0;menu btn "Terrain Mode";../sprites/ModeTerrain.png;
 * 1;menu btn "Water Mode";../sprites/ModeWater.png;
 * 2;menu btn "Roads Mode";../sprites/ModeRoads.png;
 * */

int main() {
  //  std::string dir_path = R"(C:\Users\Pavlo\Desktop\test_sprites)";
  //  std::string sprites_info_file = R"(C:\Users\Pavlo\Desktop\test_sprites/SpritesInfo.txt)";
  std::string dir_path = R"(C:\Users\Pavlo\Desktop\sprites)";
  std::string sprites_info_file = R"(C:\Users\Pavlo\Desktop\sprites/SpritesInfo.txt)";
  std::vector<Sprite> sprites;
  std::vector<SpriteEntry> sprite_entries;
  try {
    sprites = ParseSpritesDir(dir_path);
    //     cache-locality Vs better "placing"
    std::stable_sort(sprites.begin(), sprites.end(),
                     [](const Sprite& sprite1, const Sprite& sprite2) {
                       return sprite1.width > sprite2.width;
                       //                       return sprite1.height > sprite2.height;
                       //                       return (sprite1.width * sprite1.height) > (sprite2.width * sprite2.height);
                     });
    sprite_entries = ParseSpritesInfo(sprites_info_file, sprites);
    Vec2i atlas_size = GetAtlasSize(sprites);
    while (!TrySkylineBottomLeft(sprites, atlas_size)) {
      atlas_size = ResizeAtlas(atlas_size);
    }
    StoreAtlas(sprites, atlas_size, "TexAtlas.png");
    StoreTexCoords(sprite_entries, atlas_size, "TexCoordsInfo.txt");
  } catch (...) {
    return -1;
  }
  return 0;
}

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_SKYLINEBOTTOMLEFT_H_
