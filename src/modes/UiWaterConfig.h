#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERCONFIG_H_

#include <random>
#include <span>

#include "../common/Text.h"

/// oceanographic spectra
struct OceanLayerTraits {
  float scale{1.0f};
  float fetch{1.0f}; // TODO: does it related to terrain?
  float spreadBlend{0.5f};
  float swell{0.5f};
  float peakEnhancement{0.5f};
  float shortWavesFade{0.5f};
  float lambda{1.0f};
  bool visible = true;
  //3 bytes padding?
};

struct OceanTraits {
  float height = 0.0f;
  OceanLayerTraits near;
  OceanLayerTraits mid;
  OceanLayerTraits far;
};

class OceanLayerConfig {
 public:
  OceanLayerConfig(data::TextId layer_name_id)
      : layer_name_id_(layer_name_id),
        value_{{0.0f}},
        scale_{{100.0f, 10000.0f, 1.0f, 1.0f, 4.0f, 1.0f, 1.0f}},
        text_id_(
            {data::TextId::kScaleWater, data::TextId::kFetch,
             data::TextId::kSpreadBlend, data::TextId::kSwell,
             data::TextId::kPeakEnhancement, data::TextId::kShortWavesFade,
             data::TextId::kLambda}) {}

  std::span<float> GetValueSpan() noexcept {
    return value_;
  }

  std::span<data::TextId> GetTextIdSpan() noexcept {
    return text_id_;
  }
  /// no GetScaleSpan(), scales used internally at Generate()

  void Randomize(
      std::mt19937& gen,
      std::uniform_real_distribution<float>& dist_float) noexcept {
    for (int i = 0; i < value_.size(); ++i) {
      value_[i] = dist_float(gen);
    }
  }

  void SetConfig(const OceanLayerTraits* data) {
    auto data_ptr = reinterpret_cast<const float*>(data);
    std::copy(data_ptr, data_ptr + 7, value_.begin());
    visible_ = data->visible;
  }

  OceanLayerTraits GetConfig() {
    return {
      value_[0] * scale_[0], value_[1] * scale_[1],
      value_[2] * scale_[2], value_[3] * scale_[3],
      value_[4] * scale_[4], value_[5] * scale_[5],
      value_[6] * scale_[6], visible_
    };
  }

  bool GetVisible() const noexcept {
    return visible_;
  }

  data::TextId GetTextId() const noexcept {
    return layer_name_id_;
  }

  void ToggleVisible() {
    visible_ = !visible_;
  }

 private:
  data::TextId layer_name_id_;
  std::array<float, 7> value_;
  std::array<float, 7> scale_;
  std::array<data::TextId, 7> text_id_;
  bool visible_ = true;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIWATERCONFIG_H_
