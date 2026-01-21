#ifndef WIREBOUNDWORLDCREATOR_UIOCEANCONFIG_H
#define WIREBOUNDWORLDCREATOR_UIOCEANCONFIG_H

#include <random>
#include <span>

#include "../common/BaseInstanceData.h"
#include "../common/MapPoint.h"
#include "../common/Text.h"
#include "traits/OceanTraits.h"

class OceanLayerConfig {
 public:
  OceanLayerConfig(data::TextId layer_name_id, std::array<float, 8> scales)
      : layer_name_id_(layer_name_id),
        scale_(scales),
        text_id_({data::TextId::kScaleWater, data::TextId::kFetch,
                  data::TextId::kFetch, data::TextId::kSpreadBlend,
                  data::TextId::kSwell, data::TextId::kPeakEnhancement,
                  data::TextId::kShortWavesFade, data::TextId::kLambda}) {}

  std::span<float> GetValueSpan() noexcept { return value_; }

  std::span<data::TextId> GetTextIdSpan() noexcept { return text_id_; }
  /// no GetScaleSpan(), scales used internally at Generate()

  void Randomize(std::mt19937& gen,
                 std::uniform_real_distribution<float>& dist_float) noexcept {
    for (int i = 0; i < value_.size(); ++i) {
      value_[i] = dist_float(gen);
    }
  }

  void SetConfig(OceanLayerTraits* data) {
    value_ = {&data->scale, 8};
    visible_ = data->visible;
  }

  OceanLayerTraits GetConfig() {
    return {value_[0] * scale_[0] + 0.2f,
            value_[1] * scale_[1] + 2000.0f,
            value_[2] * scale_[2] + 1.0f,
            value_[3] * scale_[3] + 0.2f,
            value_[4] * scale_[4] + 0.1f,
            value_[5] * scale_[5] + 1.0f,
            value_[6] * scale_[6] + 0.1f,
            value_[7] * scale_[7] + 0.1f,
            visible_};
  }

  bool GetVisible() const noexcept { return visible_; }

  bool GetVisible2() const noexcept { return visible2_; }

  float& GetStrengthRef() { return strength_; }

  data::TextId GetTextId() const noexcept { return layer_name_id_; }

  void ToggleVisible() { visible_ = !visible_; }
  void ToggleVisible2() { visible2_ = !visible2_; }

 private:
  data::TextId layer_name_id_;
  std::span<float> value_;
  std::array<float, 8> scale_;
  std::array<data::TextId, 8> text_id_;
  bool visible_ = true;

  // not used yet
  bool visible2_ = true;
  float strength_ = 0.0f;
};

#endif  // WIREBOUNDWORLDCREATOR_UIOCEANCONFIG_H
