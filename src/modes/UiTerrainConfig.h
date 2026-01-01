#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_

#include <random>
#include <string_view>
#include <span>

#include "../core/UiComplex.h"
#include "TerrainNoiseData.h"

Texture32F GenAndSave(data::TextId tex_id);

class ITerrainNoise {
 public:
  ITerrainNoise(std::string_view shader_path)
      : shader_(shader_path) {}

  virtual std::span<float> GetValueSpan() noexcept = 0;
  virtual std::span<data::TextId> GetTextIdSpan() noexcept = 0;
  virtual data::TextId GetTextId() const noexcept = 0;
  virtual void Randomize(
      std::mt19937& gen,
      std::uniform_real_distribution<float>& dist_float,
      std::bernoulli_distribution& dist_bool) noexcept = 0;

  NoiseDataBase* GetBaseData() {
    return &base_data_;
  }

  void ToggleDoInvert() {
    base_data_.do_invert = !base_data_.do_invert;
  }

  void ToggleDoTiling() {
    base_data_.do_tiling = !base_data_.do_tiling;
  }

 protected:
  Shader shader_;
  NoiseDataBase base_data_;
};

template <size_t gParamNum, data::TextId gTextId>
class TerrainNoiseBase : public ITerrainNoise {
 public:
  TerrainNoiseBase(
      std::string_view shader_path,
      std::array<float, gParamNum> scale,
      std::array<data::TextId, gParamNum> text_id)
      : ITerrainNoise(shader_path),
        value_{{0.0f}},
        scale_(scale),
        text_id_(text_id) {}

  std::span<float> GetValueSpan() noexcept override {
    return value_;
  }

  std::span<data::TextId> GetTextIdSpan() noexcept override {
    return text_id_;
  }
  /// no GetScaleSpan(), scales used internally at Generate()

  void Randomize(
      std::mt19937& gen,
      std::uniform_real_distribution<float>& dist_float,
      std::bernoulli_distribution& dist_bool) noexcept override {
    for (int i = 0; i < value_.size(); ++i) {
      value_[i] = dist_float(gen) * 0.8f + 0.2f; // to avoid small values
    }
    base_data_.strength = dist_float(gen);
    base_data_.do_tiling = dist_bool(gen);
    base_data_.do_invert = dist_bool(gen);
  }

  void SetConfig(const NoiseDataBase* base_data, const float* noise_data) {
    std::copy(noise_data, noise_data + gParamNum, value_.begin());
    base_data_.do_invert = base_data->do_invert;
    base_data_.do_tiling = base_data->do_tiling;
    base_data_.strength = base_data->strength;
  }

  Texture32F Generate(float* noise_data) {
    std::copy(value_.begin(), value_.end(), noise_data);
    shader_.Bind();
    std::cout << "--- " << static_cast<int>(gTextId) << " ---" << std::endl;
    for (int i = 0; i < value_.size(); ++i) {
      std::cout << value_[i] << ' ';
      glUniform1f(i, value_[i] * scale_[i]);
    }
    std::cout << std::endl;
    return GenAndSave(gTextId);
  }

  [[nodiscard]] data::TextId GetTextId() const noexcept override {
    return gTextId;
  }

 private:
  std::array<float, gParamNum> value_;
  std::array<float, gParamNum> scale_;
  std::array<data::TextId, gParamNum> text_id_;
};

using TerrainNoisePerlin = TerrainNoiseBase<3, data::TextId::kPerlin>;
using TerrainNoiseCellular = TerrainNoiseBase<4, data::TextId::kCellular>;
using TerrainNoiseMetaballs = TerrainNoiseBase<4, data::TextId::kMetaballs>;
using TerrainNoiseFbmGrid = TerrainNoiseBase<9, data::TextId::kFbmGrid>;
using TerrainNoiseFbmMulti = TerrainNoiseBase<5, data::TextId::kFbmMulti>;
using TerrainNoiseFbmdPerlin = TerrainNoiseBase<8, data::TextId::kFbmdPerlin>;
using TerrainNoiseFbmWarp = TerrainNoiseBase<10, data::TextId::kFbmWarp>;
using TerrainNoiseFbmPerlinWarp = TerrainNoiseBase<10, data::TextId::kFbmPerlinWarp>;

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
