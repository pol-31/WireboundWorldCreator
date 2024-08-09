#ifndef WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAINSTANCED_H_
#define WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAINSTANCED_H_

#include <array>

enum class UiVboDataInstancedId;
inline consteval std::size_t GetUiDataInstancedOffset(
    UiVboDataInstancedId id);

enum class UiVboDataInstancedId {
  kObjects,
  kBiomes,
  kTiles,
};

inline consteval std::size_t GetUiDataInstancedOffset(
    UiVboDataInstancedId id) {
  switch(id) {
    case UiVboDataInstancedId::kObjects:
      return 0;
    case UiVboDataInstancedId::kBiomes:
      return 8 * 4 * 8;
    case UiVboDataInstancedId::kTiles:
      return (8 * 4 + 8) * 8;
  }
}

namespace details {

/// here position instanced data (only for Modes "Objects", "Biomes", "Tiles")
inline constexpr std::array<float, (8 * 4 + 8 + 6 * 6) * 8>
    kUiVboDataInstanced = {
    // Objects slots (8 x 4)
    0.4f, 0.6f, 0.4f, 0.8f, 0.2f, 0.6f, 0.2f, 0.8f,
    0.6f, 0.6f, 0.6f, 0.8f, 0.4f, 0.6f, 0.4f, 0.8f,
    0.8f, 0.6f, 0.8f, 0.8f, 0.6f, 0.6f, 0.6f, 0.8f,
    1.0f, 0.6f, 1.0f, 0.8f, 0.8f, 0.6f, 0.8f, 0.8f,

    0.4f, 0.4f, 0.4f, 0.6f, 0.2f, 0.4f, 0.2f, 0.6f,
    0.6f, 0.4f, 0.6f, 0.6f, 0.4f, 0.4f, 0.4f, 0.6f,
    0.8f, 0.4f, 0.8f, 0.6f, 0.6f, 0.4f, 0.6f, 0.6f,
    1.0f, 0.4f, 1.0f, 0.6f, 0.8f, 0.4f, 0.8f, 0.6f,

    0.4f, 0.2, 0.4f, 0.4f, 0.2f, 0.2f, 0.2f, 0.4f,
    0.6f, 0.2, 0.6f, 0.4f, 0.4f, 0.2f, 0.4f, 0.4f,
    0.8f, 0.2, 0.8f, 0.4f, 0.6f, 0.2f, 0.6f, 0.4f,
    1.0f, 0.2, 1.0f, 0.4f, 0.8f, 0.2f, 0.8f, 0.4f,

    0.4f, 0.0f, 0.4f, 0.2f, 0.2f, 0.0f, 0.2f, 0.2f,
    0.6f, 0.0f, 0.6f, 0.2f, 0.4f, 0.0f, 0.4f, 0.2f,
    0.8f, 0.0f, 0.8f, 0.2f, 0.6f, 0.0f, 0.6f, 0.2f,
    1.0f, 0.0f, 1.0f, 0.2f, 0.8f, 0.0f, 0.8f, 0.2f,

    0.4f, -0.2f, 0.4f, 0.0f, 0.2f, -0.2f, 0.2f, 0.0f,
    0.6f, -0.2f, 0.6f, 0.0f, 0.4f, -0.2f, 0.4f, 0.0f,
    0.8f, -0.2f, 0.8f, 0.0f, 0.6f, -0.2f, 0.6f, 0.0f,
    1.0f, -0.2f, 1.0f, 0.0f, 0.8f, -0.2f, 0.8f, 0.0f,

    0.4f, -0.4f, 0.4f, -0.2f, 0.2f, -0.4f, 0.2f, -0.2f,
    0.6f, -0.4f, 0.6f, -0.2f, 0.4f, -0.4f, 0.4f, -0.2f,
    0.8f, -0.4f, 0.8f, -0.2f, 0.6f, -0.4f, 0.6f, -0.2f,
    1.0f, -0.4f, 1.0f, -0.2f, 0.8f, -0.4f, 0.8f, -0.2f,

    0.4f, -0.6f, 0.4f, -0.4f, 0.2f, -0.6f, 0.2f, -0.4f,
    0.6f, -0.6f, 0.6f, -0.4f, 0.4f, -0.6f, 0.4f, -0.4f,
    0.8f, -0.6f, 0.8f, -0.4f, 0.6f, -0.6f, 0.6f, -0.4f,
    1.0f, -0.6f, 1.0f, -0.4f, 0.8f, -0.6f, 0.8f, -0.4f,

    0.4f, -0.8f, 0.4f, -0.6f, 0.2f, -0.8f, 0.2f, -0.6f,
    0.6f, -0.8f, 0.6f, -0.6f, 0.4f, -0.8f, 0.4f, -0.6f,
    0.8f, -0.8f, 0.8f, -0.6f, 0.6f, -0.8f, 0.6f, -0.6f,
    1.0f, -0.8f, 1.0f, -0.6f, 0.8f, -0.8f, 0.8f, -0.6f,

    // Biomes slots (8)
    1.0f, 0.6f, 1.0f, 0.8f, 0.2f, 0.6f, 0.2f, 0.8f,
    1.0f, 0.4f, 1.0f, 0.6f, 0.2f, 0.4f, 0.2f, 0.6f,
    1.0f, 0.2, 1.0f, 0.4f, 0.2f, 0.2f, 0.2f, 0.4f,
    1.0f, 0.0f, 1.0f, 0.2f, 0.2f, 0.0f, 0.2f, 0.2f,

    1.0f, -0.2f, 1.0f, 0.0f, 0.2f, -0.2f, 0.2f, 0.0f,
    1.0f, -0.4f, 1.0f, -0.2f, 0.2f, -0.4f, 0.2f, -0.2f,
    1.0f, -0.6f, 1.0f, -0.4f, 0.2f, -0.6f, 0.2f, -0.4f,
    1.0f, -0.8f, 1.0f, -0.6f, 0.2f, -0.8f, 0.2f, -0.6f,

    // Tiles slots (6 x 6)
    -0.8f, 0.2f, -0.8f, 0.3f, -0.9f, 0.2f, -0.9f, 0.3f,
    -0.7f, 0.2f, -0.7f, 0.3f, -0.8f, 0.2f, -0.8f, 0.3f,
    -0.6f, 0.2f, -0.6f, 0.3f, -0.7f, 0.2f, -0.7f, 0.3f,
    -0.5f, 0.2f, -0.5f, 0.3f, -0.6f, 0.2f, -0.6f, 0.3f,
    -0.4f, 0.2f, -0.4f, 0.3f, -0.5f, 0.2f, -0.5f, 0.3f,
    -0.3f, 0.2f, -0.3f, 0.3f, -0.4f, 0.2f, -0.4f, 0.3f,

    -0.8f, 0.1f, -0.8f, 0.2f, -0.9f, 0.1f, -0.9f, 0.2f,
    -0.7f, 0.1f, -0.7f, 0.2f, -0.8f, 0.1f, -0.8f, 0.2f,
    -0.6f, 0.1f, -0.6f, 0.2f, -0.7f, 0.1f, -0.7f, 0.2f,
    -0.5f, 0.1f, -0.5f, 0.2f, -0.6f, 0.1f, -0.6f, 0.2f,
    -0.4f, 0.1f, -0.4f, 0.2f, -0.5f, 0.1f, -0.5f, 0.2f,
    -0.3f, 0.1f, -0.3f, 0.2f, -0.4f, 0.1f, -0.4f, 0.2f,

    -0.8f, 0.0f, -0.8f, 0.1f, -0.9f, 0.0f, -0.9f, 0.1f,
    -0.7f, 0.0f, -0.7f, 0.1f, -0.8f, 0.0f, -0.8f, 0.1f,
    -0.6f, 0.0f, -0.6f, 0.1f, -0.7f, 0.0f, -0.7f, 0.1f,
    -0.5f, 0.0f, -0.5f, 0.1f, -0.6f, 0.0f, -0.6f, 0.1f,
    -0.4f, 0.0f, -0.4f, 0.1f, -0.5f, 0.0f, -0.5f, 0.1f,
    -0.3f, 0.0f, -0.3f, 0.1f, -0.4f, 0.0f, -0.4f, 0.1f,

    -0.8f, -0.1f, -0.8f, 0.0f, -0.9f, -0.1f, -0.9f, 0.0f,
    -0.7f, -0.1f, -0.7f, 0.0f, -0.8f, -0.1f, -0.8f, 0.0f,
    -0.6f, -0.1f, -0.6f, 0.0f, -0.7f, -0.1f, -0.7f, 0.0f,
    -0.5f, -0.1f, -0.5f, 0.0f, -0.6f, -0.1f, -0.6f, 0.0f,
    -0.4f, -0.1f, -0.4f, 0.0f, -0.5f, -0.1f, -0.5f, 0.0f,
    -0.3f, -0.1f, -0.3f, 0.0f, -0.4f, -0.1f, -0.4f, 0.0f,

    -0.8f, -0.2f, -0.8f, -0.1f, -0.9f, -0.2f, -0.9f, -0.1f,
    -0.7f, -0.2f, -0.7f, -0.1f, -0.8f, -0.2f, -0.8f, -0.1f,
    -0.6f, -0.2f, -0.6f, -0.1f, -0.7f, -0.2f, -0.7f, -0.1f,
    -0.5f, -0.2f, -0.5f, -0.1f, -0.6f, -0.2f, -0.6f, -0.1f,
    -0.4f, -0.2f, -0.4f, -0.1f, -0.5f, -0.2f, -0.5f, -0.1f,
    -0.3f, -0.2f, -0.3f, -0.1f, -0.4f, -0.2f, -0.4f, -0.1f,

    -0.8f, -0.3f, -0.8f, -0.2f, -0.9f, -0.3f, -0.9f, -0.2f,
    -0.7f, -0.3f, -0.7f, -0.2f, -0.8f, -0.3f, -0.8f, -0.2f,
    -0.6f, -0.3f, -0.6f, -0.2f, -0.7f, -0.3f, -0.7f, -0.2f,
    -0.5f, -0.3f, -0.5f, -0.2f, -0.6f, -0.3f, -0.6f, -0.2f,
    -0.4f, -0.3f, -0.4f, -0.2f, -0.5f, -0.3f, -0.5f, -0.2f,
    -0.3f, -0.3f, -0.3f, -0.2f, -0.4f, -0.3f, -0.4f, -0.2f,
};

} // namespace details

#endif  // WIREBOUNDWORLDCREATOR_SRC_VBO_UIDATAINSTANCED_H_
