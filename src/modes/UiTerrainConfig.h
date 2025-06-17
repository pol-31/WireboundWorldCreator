#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_

#include "../core/Ui.h"

const int gNoisesNum = 8;

class ITerrainNoise {
 public:
  virtual void Render(glm::vec2 mouse_pos) = 0;

  virtual void RenderPicking() = 0;

  virtual bool Scroll(GLuint id, float yoffset) = 0;

  virtual void UpdateTransform() = 0;

  virtual bool Press(int id) = 0;

  virtual void Release() = 0;

  virtual data::TextId Hover(int id) = 0;
};

template <size_t gSlidersNum, size_t gTogglesNum,
          size_t gTextNum, size_t gWidgetsNum>
class TerrainNoiseBase : public ITerrainNoise {
 public:
  TerrainNoiseBase() = default;

  void Render(glm::vec2 mouse_pos) override {
    for (auto slider : sliders_) {
      slider->Render(mouse_pos);
    }
    for (auto toggle : toggles_) {
      toggle->Render();
    }
    for (auto text : texts_) {
      text->Render();
    }
  }

  void RenderPicking() override {
    for (auto slider : sliders_) {
      slider->RenderPicking();
    }
    for (auto toggle : toggles_) {
      toggle->RenderPicking();
    }
    for (auto text : texts_) {
      text->RenderPicking();
    }
  }

  bool Scroll(GLuint id, float yoffset) override {
    bool handled = false;
    for (auto slider : sliders_) {
      handled || slider->Scroll(id, yoffset);
    }
    return handled;
  }

  void UpdateTransform() override {
    for (auto slider : sliders_) {
      slider->UpdateTransform();
    }
    for (auto toggle : toggles_) {
      toggle->UpdateTransform();
    }
    for (auto text : texts_) {
      text->UpdateTransform();
    }
  }

  bool Press(int id) override {
    return ui_event_handler_.Press(id);
  }

  void Release() override {
    ui_event_handler_.Release();
  }

  data::TextId Hover(int id) override {
    return ui_event_handler_.Hover(id);
  }

 protected:
  void SetParent(GLuint parent_id) {
    for (const auto slider :sliders_) {
      gUiComponents[slider->GetId() - details::kIdOffsetUi].parent_id_
          = parent_id;
    }
    for (const auto toggle : toggles_) {
      gUiComponents[toggle->GetId() - details::kIdOffsetUi].parent_id_
          = parent_id;
    }
    for (const auto text :texts_) {
      gUiComponents[text->GetId() - details::kIdOffsetUi].parent_id_
          = parent_id;
    }
  }

  std::array<UiSliderH2*, gSlidersNum> sliders_;
  std::array<UiToggle*, gTogglesNum> toggles_;
  std::array<UiDynamicSprite*, gTextNum> texts_;

  UiEventHandler<gWidgetsNum> ui_event_handler_;
};

class TerrainNoisePerlin final
    : public TerrainNoiseBase<
          2, 0, 2,
          static_cast<int>(data::VboIdMain::kTerrainNoisePerlinSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoisePerlinScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      2, 0, 2,
      static_cast<int>(data::VboIdMain::kTerrainNoisePerlinSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoisePerlinScaleText)>;
  explicit TerrainNoisePerlin(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoisePerlinScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoisePerlinScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoisePerlinScaleText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoisePerlinSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoisePerlinSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoisePerlinSeedText, data::TextId::kNone) {
    sliders_ = {&slider_scale_, &slider_seed_};
    texts_ = {&text_scale_, &text_seed_};
    ui_event_handler_ = {&slider_scale_, &slider_seed_};
    SetParent(parent_id);

  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
};

class TerrainNoiseCellular final
    : public TerrainNoiseBase<
          3, 0, 3,
          static_cast<int>(data::VboIdMain::kTerrainNoiseCellularSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseCellularScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      3, 0, 3,
      static_cast<int>(data::VboIdMain::kTerrainNoiseCellularSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseCellularScaleText)>;
  explicit TerrainNoiseCellular(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseCellularScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseCellularScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseCellularScaleText, data::TextId::kNone),
        slider_jitter_(
            {data::VboIdMain::kTerrainNoiseCellularJitterArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseCellularJitterIcon, data::TextId::kNone}
            ),
        text_jitter_(
            data::VboIdMain::kTerrainNoiseCellularJitterText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseCellularSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseCellularSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseCellularSeedText, data::TextId::kNone) {
    Base::sliders_ = {&slider_scale_, &slider_jitter_, &slider_seed_};
    Base::texts_ = {&text_scale_, &text_jitter_, &text_seed_};
    Base::ui_event_handler_ = {&slider_scale_, &slider_jitter_, &slider_seed_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_jitter_;
  UiDynamicSprite text_jitter_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
};

class TerrainNoiseMetaballs final
    : public TerrainNoiseBase<
          4, 0, 4,
          static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      4, 0, 4,
      static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsScaleText)>;
  explicit TerrainNoiseMetaballs(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseMetaballsScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseMetaballsScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseMetaballsScaleText, data::TextId::kNone),
        slider_jitter_(
            {data::VboIdMain::kTerrainNoiseMetaballsJitterArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseMetaballsJitterIcon, data::TextId::kNone}
            ),
        text_jitter_(
            data::VboIdMain::kTerrainNoiseMetaballsJitterText, data::TextId::kNone),
        slider_phase_(
            {data::VboIdMain::kTerrainNoiseMetaballsPhaseArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseMetaballsPhaseIcon, data::TextId::kNone}
            ),
        text_phase_(
            data::VboIdMain::kTerrainNoiseMetaballsPhaseText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseMetaballsSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseMetaballsSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseMetaballsSeedText, data::TextId::kNone) {
    Base::sliders_ = {&slider_scale_, &slider_jitter_,
                      &slider_phase_, &slider_seed_};
    Base::texts_ = {&text_scale_, &text_jitter_,
                    &text_phase_, &text_seed_};
    Base::ui_event_handler_ = {&slider_scale_, &slider_jitter_,
                               &slider_phase_, &slider_seed_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_jitter_;
  UiDynamicSprite text_jitter_;
  UiSliderH2 slider_phase_;
  UiDynamicSprite text_phase_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
};

class TerrainNoiseFbmGrid final
    : public TerrainNoiseBase<
          10, 0, 10,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      10, 0, 10,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridScaleText)>;
  explicit TerrainNoiseFbmGrid(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseFbmGridScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseFbmGridScaleText, data::TextId::kNone),
        slider_octaves_(
            {data::VboIdMain::kTerrainNoiseFbmGridOctavesArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridOctavesIcon, data::TextId::kNone}
            ),
        text_octaves_(
            data::VboIdMain::kTerrainNoiseFbmGridOctavesText, data::TextId::kNone),
        slider_shift_(
            {data::VboIdMain::kTerrainNoiseFbmGridShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridShiftIcon, data::TextId::kNone}
            ),
        text_shift_(
            data::VboIdMain::kTerrainNoiseFbmGridShiftText, data::TextId::kNone),
        slider_time_shift_(
            {data::VboIdMain::kTerrainNoiseFbmGridTimeShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridTimeShiftIcon, data::TextId::kNone}
            ),
        text_time_shift_(
            data::VboIdMain::kTerrainNoiseFbmGridTimeShiftText, data::TextId::kNone),
        slider_gain_(
            {data::VboIdMain::kTerrainNoiseFbmGridGainArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridGainIcon, data::TextId::kNone}
            ),
        text_gain_(
            data::VboIdMain::kTerrainNoiseFbmGridGainText, data::TextId::kNone),
        slider_lacunarity_(
            {data::VboIdMain::kTerrainNoiseFbmGridLacunarityArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridLacunarityIcon, data::TextId::kNone}
            ),
        text_lacunarity_(
            data::VboIdMain::kTerrainNoiseFbmGridLacunarityText, data::TextId::kNone),
        slider_translate_(
            {data::VboIdMain::kTerrainNoiseFbmGridTranslateArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridTranslateIcon, data::TextId::kNone}
            ),
        text_translate_(
            data::VboIdMain::kTerrainNoiseFbmGridTranslateText, data::TextId::kNone),
        slider_warp_strength_(
            {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthIcon, data::TextId::kNone}
            ),
        text_warp_strength_(
            data::VboIdMain::kTerrainNoiseFbmGridWarpStrengthText, data::TextId::kNone),
        slider_octave_factor_(
            {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorIcon, data::TextId::kNone}
            ),
        text_octave_factor_(
            data::VboIdMain::kTerrainNoiseFbmGridOctaveFactorText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseFbmGridSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmGridSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseFbmGridSeedText, data::TextId::kNone) {
    Base::sliders_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_translate_,
        &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
    Base::texts_ = {
        &text_scale_, &text_octaves_, &text_shift_, &text_time_shift_,
        &text_gain_, &text_lacunarity_, &text_translate_,
        &text_warp_strength_, &text_octave_factor_, &text_seed_};
    Base::ui_event_handler_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_translate_,
        &slider_warp_strength_, &slider_octave_factor_, &slider_seed_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_octaves_;
  UiDynamicSprite text_octaves_;
  UiSliderH2 slider_shift_;
  UiDynamicSprite text_shift_;
  UiSliderH2 slider_time_shift_;
  UiDynamicSprite text_time_shift_;
  UiSliderH2 slider_gain_;
  UiDynamicSprite text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiDynamicSprite text_lacunarity_;
  UiSliderH2 slider_translate_;
  UiDynamicSprite text_translate_;
  UiSliderH2 slider_warp_strength_;
  UiDynamicSprite text_warp_strength_;
  UiSliderH2 slider_octave_factor_;
  UiDynamicSprite text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
};

class TerrainNoiseFbmMulti final
    : public TerrainNoiseBase<
          5, 0, 5,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      5, 0, 5,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiScaleText)>;
  explicit TerrainNoiseFbmMulti(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseFbmMultiScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmMultiScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseFbmMultiScaleText, data::TextId::kNone),
        slider_lacunarity_(
            {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmMultiLacunarityIcon, data::TextId::kNone}
            ),
        text_lacunarity_(
            data::VboIdMain::kTerrainNoiseFbmMultiLacunarityText, data::TextId::kNone),
        slider_octaves_(
            {data::VboIdMain::kTerrainNoiseFbmMultiOctavesArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmMultiOctavesIcon, data::TextId::kNone}
            ),
        text_octaves_(
            data::VboIdMain::kTerrainNoiseFbmMultiOctavesText, data::TextId::kNone),
        slider_phases_(
            {data::VboIdMain::kTerrainNoiseFbmMultiPhaseArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmMultiPhaseIcon, data::TextId::kNone}
            ),
        text_phases_(
            data::VboIdMain::kTerrainNoiseFbmMultiPhaseText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseFbmMultiSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseFbmMultiSeedText, data::TextId::kNone) {
    Base::sliders_ = {
        &slider_scale_, &slider_lacunarity_, &slider_octaves_,
        &slider_phases_, &slider_seed_};
    Base::texts_ = {
        &text_scale_, &text_lacunarity_, &text_octaves_,
        &text_phases_, &text_seed_};
    Base::ui_event_handler_ = {
        &slider_scale_, &slider_lacunarity_, &slider_octaves_,
        &slider_phases_, &slider_seed_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_lacunarity_;
  UiDynamicSprite text_lacunarity_;
  UiSliderH2 slider_octaves_;
  UiDynamicSprite text_octaves_;
  UiSliderH2 slider_phases_;
  UiDynamicSprite text_phases_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
};

class TerrainNoiseFbmdPerlin final
    : public TerrainNoiseBase<
          9, 1, 10,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      9, 1, 10,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinScaleText)>;
  explicit TerrainNoiseFbmdPerlin(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinScaleText, data::TextId::kNone),
        slider_octaves_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesIcon, data::TextId::kNone}
            ),
        text_octaves_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinOctavesText, data::TextId::kNone),
        slider_shift_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinShiftIcon, data::TextId::kNone}
            ),
        text_shift_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinShiftText, data::TextId::kNone),
        slider_transform_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinTransformArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinTransformIcon, data::TextId::kNone}
            ),
        text_transform_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinTransformText, data::TextId::kNone),
        slider_gain_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinGainArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinGainIcon, data::TextId::kNone}
            ),
        text_gain_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinGainText, data::TextId::kNone),
        slider_lacunarity_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityIcon, data::TextId::kNone}
            ),
        text_lacunarity_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinLacunarityText, data::TextId::kNone),
        slider_slopeness_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessIcon, data::TextId::kNone}
            ),
        text_slopeness_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinSlopenessText, data::TextId::kNone),
        slider_octave_factor_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorIcon, data::TextId::kNone}
            ),
        text_octave_factor_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinOctaveFactorText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinSeedText, data::TextId::kNone),
        toggle_negative_(
            {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOff, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn1, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn2, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeOn3, data::TextId::kNone}
            ),
        text_negative_(
            data::VboIdMain::kTerrainNoiseFbmdPerlinNegativeText, data::TextId::kNone) {
    Base::sliders_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_transform_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_};
    Base::toggles_ = {&toggle_negative_};
    Base::texts_ = {
        &text_scale_, &text_octaves_, &text_shift_, &text_transform_,
        &text_gain_, &text_lacunarity_, &text_slopeness_,
        &text_octave_factor_, &text_seed_, &text_negative_};
    Base::ui_event_handler_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_transform_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_octaves_;
  UiDynamicSprite text_octaves_;
  UiSliderH2 slider_shift_;
  UiDynamicSprite text_shift_;
  UiSliderH2 slider_transform_;
  UiDynamicSprite text_transform_;
  UiSliderH2 slider_gain_;
  UiDynamicSprite text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiDynamicSprite text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiDynamicSprite text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiDynamicSprite text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;

  UiToggle toggle_negative_;
  UiDynamicSprite text_negative_;
};

class TerrainNoiseFbmWarp final
    : public TerrainNoiseBase<
          12, 1, 13,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpRIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      12, 1, 13,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpRIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpScaleText)>;
  explicit TerrainNoiseFbmWarp(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseFbmWarpScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseFbmWarpScaleText, data::TextId::kNone),
        slider_factors_(
            {data::VboIdMain::kTerrainNoiseFbmWarpFactorsArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpFactorsIcon, data::TextId::kNone}
            ),
        text_factors_(
            data::VboIdMain::kTerrainNoiseFbmWarpFactorsText, data::TextId::kNone),
        slider_octaves_(
            {data::VboIdMain::kTerrainNoiseFbmWarpOctavesArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpOctavesIcon, data::TextId::kNone}
            ),
        text_octaves_(
            data::VboIdMain::kTerrainNoiseFbmWarpOctavesText, data::TextId::kNone),
        slider_shift_(
            {data::VboIdMain::kTerrainNoiseFbmWarpShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpShiftIcon, data::TextId::kNone}
            ),
        text_shift_(
            data::VboIdMain::kTerrainNoiseFbmWarpShiftText, data::TextId::kNone),
        slider_time_shift_(
            {data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftIcon, data::TextId::kNone}
            ),
        text_time_shift_(
            data::VboIdMain::kTerrainNoiseFbmWarpTimeShiftText, data::TextId::kNone),
        slider_gain_(
            {data::VboIdMain::kTerrainNoiseFbmWarpGainArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpGainIcon, data::TextId::kNone}
            ),
        text_gain_(
            data::VboIdMain::kTerrainNoiseFbmWarpGainText, data::TextId::kNone),
        slider_lacunarity_(
            {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpLacunarityIcon, data::TextId::kNone}
            ),
        text_lacunarity_(
            data::VboIdMain::kTerrainNoiseFbmWarpLacunarityText, data::TextId::kNone),
        slider_slopeness_(
            {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpSlopenessIcon, data::TextId::kNone}
            ),
        text_slopeness_(
            data::VboIdMain::kTerrainNoiseFbmWarpSlopenessText, data::TextId::kNone),
        slider_octave_factor_(
            {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorIcon, data::TextId::kNone}
            ),
        text_octave_factor_(
            data::VboIdMain::kTerrainNoiseFbmWarpOctaveFactorText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseFbmWarpSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseFbmWarpSeedText, data::TextId::kNone),
        slider_q_(
            {data::VboIdMain::kTerrainNoiseFbmWarpQArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpQIcon, data::TextId::kNone}
            ),
        text_q_(
            data::VboIdMain::kTerrainNoiseFbmWarpQText, data::TextId::kNone),
        slider_r_(
            {data::VboIdMain::kTerrainNoiseFbmWarpRArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpRIcon, data::TextId::kNone}
            ),
        text_r_(
            data::VboIdMain::kTerrainNoiseFbmWarpRText, data::TextId::kNone),
        toggle_negative_(
            {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOff, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn1, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn2, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmWarpNegativeOn3, data::TextId::kNone}
            ),
        text_negative_(
            data::VboIdMain::kSpareText1, data::TextId::kNone) {
    Base::sliders_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
    Base::toggles_ = {&toggle_negative_};
    Base::texts_ = {
        &text_scale_, &text_octaves_, &text_shift_, &text_time_shift_,
        &text_gain_, &text_lacunarity_, &text_slopeness_,
        &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
    Base::ui_event_handler_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_factors_;
  UiDynamicSprite text_factors_;
  UiSliderH2 slider_octaves_;
  UiDynamicSprite text_octaves_;
  UiSliderH2 slider_shift_;
  UiDynamicSprite text_shift_;
  UiSliderH2 slider_time_shift_;
  UiDynamicSprite text_time_shift_;
  UiSliderH2 slider_gain_;
  UiDynamicSprite text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiDynamicSprite text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiDynamicSprite text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiDynamicSprite text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
  UiSliderH2 slider_q_;
  UiDynamicSprite text_q_;
  UiSliderH2 slider_r_;
  UiDynamicSprite text_r_;

  UiToggle toggle_negative_;
  UiDynamicSprite text_negative_;
};

class TerrainNoiseFbmPerlinWarp final
    : public TerrainNoiseBase<
          12, 1, 13,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleText)> {
 public:
  using Base = TerrainNoiseBase<
      12, 1, 13,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleText)>;
  explicit TerrainNoiseFbmPerlinWarp(GLuint parent_id)
      : slider_scale_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleIcon, data::TextId::kNone}
            ),
        text_scale_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpScaleText, data::TextId::kNone),
        slider_factors_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsIcon, data::TextId::kNone}
            ),
        text_factors_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpFactorsText, data::TextId::kNone),
        slider_octaves_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesIcon, data::TextId::kNone}
            ),
        text_octaves_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctavesText, data::TextId::kNone),
        slider_shift_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftIcon, data::TextId::kNone}
            ),
        text_shift_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpShiftText, data::TextId::kNone),
        slider_time_shift_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftIcon, data::TextId::kNone}
            ),
        text_time_shift_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpTimeShiftText, data::TextId::kNone),
        slider_gain_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainIcon, data::TextId::kNone}
            ),
        text_gain_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpGainText, data::TextId::kNone),
        slider_lacunarity_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityIcon, data::TextId::kNone}
            ),
        text_lacunarity_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpLacunarityText, data::TextId::kNone),
        slider_slopeness_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessIcon, data::TextId::kNone}
            ),
        text_slopeness_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpSlopenessText, data::TextId::kNone),
        slider_octave_factor_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorIcon, data::TextId::kNone}
            ),
        text_octave_factor_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpOctaveFactorText, data::TextId::kNone),
        slider_seed_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedIcon, data::TextId::kNone}
            ),
        text_seed_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpSeedText, data::TextId::kNone),
        slider_q_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpQIcon, data::TextId::kNone}
            ),
        text_q_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpQText, data::TextId::kNone),
        slider_r_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRArea, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon, data::TextId::kNone}
            ),
        text_r_(
            data::VboIdMain::kTerrainNoiseFbmPerlinWarpRText, data::TextId::kNone),
        toggle_negative_(
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOff, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn1, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn2, data::TextId::kNone},
            {data::VboIdMain::kTerrainNoiseFbmPerlinWarpNegativeOn3, data::TextId::kNone}
            ),
        text_negative_(
            data::VboIdMain::kSpareText2, data::TextId::kNone) {
    Base::sliders_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
    Base::toggles_ = {&toggle_negative_};
    Base::texts_ = {
        &text_scale_, &text_octaves_, &text_shift_, &text_time_shift_,
        &text_gain_, &text_lacunarity_, &text_slopeness_,
        &text_octave_factor_, &text_seed_, &text_q_, &text_r_, &text_negative_};
    Base::ui_event_handler_ = {
        &slider_scale_, &slider_octaves_, &slider_shift_, &slider_time_shift_,
        &slider_gain_, &slider_lacunarity_, &slider_slopeness_,
        &slider_octave_factor_, &slider_seed_, &slider_q_, &slider_r_};
    SetParent(parent_id);
  }

 private:
  UiSliderH2 slider_scale_;
  UiDynamicSprite text_scale_;
  UiSliderH2 slider_factors_;
  UiDynamicSprite text_factors_;
  UiSliderH2 slider_octaves_;
  UiDynamicSprite text_octaves_;
  UiSliderH2 slider_shift_;
  UiDynamicSprite text_shift_;
  UiSliderH2 slider_time_shift_;
  UiDynamicSprite text_time_shift_;
  UiSliderH2 slider_gain_;
  UiDynamicSprite text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiDynamicSprite text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiDynamicSprite text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiDynamicSprite text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiDynamicSprite text_seed_;
  UiSliderH2 slider_q_;
  UiDynamicSprite text_q_;
  UiSliderH2 slider_r_;
  UiDynamicSprite text_r_;

  UiToggle toggle_negative_;
  UiDynamicSprite text_negative_;
};

class UiTerrainNoiseConfig {
 public:
  UiTerrainNoiseConfig(GLuint parent_id)
      : perlin_(parent_id),
        cellular_(parent_id),
        metaballs_(parent_id),
        fbm_grid_(parent_id),
        fbm_multi_(parent_id),
        fmbd_perlin_(parent_id),
        fbm_warp_(parent_id),
        fmb_perlin_warp_(parent_id) {
    noises_ = {&perlin_, &cellular_, &metaballs_, &fbm_grid_, &fbm_multi_,
               &fmbd_perlin_, &fbm_warp_, &fmb_perlin_warp_};
    SetCurId(0);
  }

  void Render(glm::vec2 mouse_pos) {
    noises_[cur_id_]->Render(mouse_pos);
  }

  void RenderPicking() {
    noises_[cur_id_]->RenderPicking();
  }

  bool Scroll(GLuint id, float yoffset) {
    return noises_[cur_id_]->Scroll(id, yoffset);
  }

  void UpdateTransform() {
    noises_[cur_id_]->UpdateTransform();
  }

  bool Press(int id) {
    return noises_[cur_id_]->Press(id);
  }

  void Release() {
    noises_[cur_id_]->Release();
  }

  data::TextId Hover(int id) {
    return noises_[cur_id_]->Hover(id);
  }

  void SetCurId(GLuint id) {
    if (id >= 0 && id < gNoisesNum) {
      cur_id_ = id;
    }
    throw "wrong terrain noise id";
  }

 private:
  std::array<ITerrainNoise*, gNoisesNum> noises_;
  GLuint cur_id_;

  TerrainNoisePerlin perlin_;
  TerrainNoiseCellular cellular_;
  TerrainNoiseMetaballs metaballs_;
  TerrainNoiseFbmGrid fbm_grid_;
  TerrainNoiseFbmMulti fbm_multi_;
  TerrainNoiseFbmdPerlin fmbd_perlin_;
  TerrainNoiseFbmWarp fbm_warp_;
  TerrainNoiseFbmPerlinWarp fmb_perlin_warp_;
};

class UiTerrainSlotsConfig {
 public:
  UiTerrainSlotsConfig()
      : sprite_desk_(
            data::VboIdMain::kTerrainEditDesk, data::TextId::kNone),
        btn_accept_(
            data::VboIdMain::kTerrainEditAccept, data::TextId::kNone),
        btn_close_(
            data::VboIdMain::kTerrainEditClose, data::TextId::kNone),
        input_slot_name_(
            data::VboIdMain::kTerrainEditName, data::TextId::kNone),
        sprite_name_back_(
            data::VboIdMain::kTerrainEditNameBack, data::TextId::kNone),
        palette_(
            {data::VboIdMain::kTerrainColorPalette, data::TextId::kNone},
            {data::VboIdMain::kTerrainColorBrightnessCursor, data::TextId::kNone},
            {data::VboIdMain::kTerrainColorColorCursor, data::TextId::kNone}),
        color_indicator_(
            data::VboIdMain::kTerrainColorIndicator, data::TextId::kNone),
        sprite_heightmap_(
            data::VboIdMain::kTerrainEditHeightmap, data::TextId::kNone),
        btn_random_generate_(
            data::VboIdMain::kTerrainEditRandomGeneration, data::TextId::kNone),
        input_translate_x_(
            data::VboIdMain::kTerrainEditTranslateXText, data::TextId::kNone),
        input_translate_y_(
            data::VboIdMain::kTerrainEditTranslateYText, data::TextId::kNone),
        input_translate_z_(
            data::VboIdMain::kTerrainEditTranslateZText, data::TextId::kNone),
        input_scale_x_(
            data::VboIdMain::kTerrainEditScaleXText, data::TextId::kNone),
        input_scale_y_(
            data::VboIdMain::kTerrainEditScaleYText, data::TextId::kNone),
        input_scale_z_(
            data::VboIdMain::kTerrainEditScaleZText, data::TextId::kNone),
        input_rotate_x_(
            data::VboIdMain::kTerrainEditRotateXText, data::TextId::kNone),
        input_rotate_y_(
            data::VboIdMain::kTerrainEditRotateYText, data::TextId::kNone),
        input_rotate_z_(
            data::VboIdMain::kTerrainEditRotateZText, data::TextId::kNone),
        text_invert_(
            data::VboIdMain::kTerrainEditLabelInvert, data::TextId::kNone),
        noises_config_(
            {
                {data::VboIdMain::kTerrainEditNoise1Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise1Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise1InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise1InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise1InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise1InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise1StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise1StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise1StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise2Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise2Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise2InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise2InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise2InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise2InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise2StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise2StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise2StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise3Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise3Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise3InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise3InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise3InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise3InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise3StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise3StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise3StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise4Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise4Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise4InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise4InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise4InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise4InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise4StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise4StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise4StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise5Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise5Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise5InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise5InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise5InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise5InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise5StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise5StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise5StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise6Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise6Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise6InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise6InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise6InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise6InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise6StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise6StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise6StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise7Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise7Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise7InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise7InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise7InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise7InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise7StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise7StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise7StrengthIcon, data::TextId::kNone}
                },
            },
            {
                {data::VboIdMain::kTerrainEditNoise8Config, data::TextId::kNone},
                {data::VboIdMain::kTerrainEditNoise8Name, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise8InvertOff, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise8InvertOn1, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise8InvertOn2, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise8InvertOn3, data::TextId::kNone}
                },
                {data::VboIdMain::kTerrainEditNoise8StrengthText, data::TextId::kNone},
                {{data::VboIdMain::kTerrainEditNoise8StrengthArea, data::TextId::kNone},
                 {data::VboIdMain::kTerrainEditNoise8StrengthIcon, data::TextId::kNone}
                },
            }) {
    //    gUiComponents[sprite_heightmap_.GetId() - details::kIdOffsetUi].parent_id_
    //        = sprite_desk_.GetId();
    //    gUiComponents[GetId() - details::kIdOffsetUi].ui =
    //        static_cast<UiTransformDbg*>(this);
    //    Base::UpdateTransform();
  }

 private:
  struct UiNoiseConfig {
    UiStaticSprite btn_noise_config_;
    UiTextLabel text_noise_name_;
    UiToggle toggle_noise_invert_;
    UiTextLabel text_noise_strength_;
    UiSliderH2 text_noise_slider_;
  };

  UiStaticSprite sprite_desk_;

  UiStaticSprite btn_accept_;
  UiStaticSprite btn_close_;

  UiTextInput input_slot_name_;
  UiStaticSprite sprite_name_back_;

  UiPalette palette_;

  UiColorIndicator color_indicator_;

  UiHeightmapSprite sprite_heightmap_;

  UiStaticSprite btn_random_generate_;

  UiNumberInput input_translate_x_;
  UiNumberInput input_translate_y_;
  UiNumberInput input_translate_z_;
  UiNumberInput input_scale_x_;
  UiNumberInput input_scale_y_;
  UiNumberInput input_scale_z_;
  UiNumberInput input_rotate_x_;
  UiNumberInput input_rotate_y_;
  UiNumberInput input_rotate_z_;

  UiTextLabel text_invert_;

  std::array<UiNoiseConfig, gNoisesNum> noises_config_;
};

class UiTerrainBakeConfig {
 public:
  UiTerrainBakeConfig(GLuint parent_id)
      : sprite_desk_(
            data::VboIdMain::kTerrainBakeDesk, data::TextId::kNone),
        btn_close_(
            data::VboIdMain::kTerrainBakeClose, data::TextId::kNone),
        btn_accept_(
            data::VboIdMain::kTerrainBakeAccept, data::TextId::kNone),
        label_erosion_steps_(
            data::VboIdMain::kTerrainBakeErosionStepLabel, data::TextId::kNone),
        label_weathering_steps_(
            data::VboIdMain::kTerrainBakeErosinoStepInput, data::TextId::kNone),
        erosion_steps_(
            data::VboIdMain::kTerrainBakeWeatheringStepLabel, data::TextId::kNone),
        weathering_steps_(
            data::VboIdMain::kTerrainBakeWeatheringStepInput, data::TextId::kNone),
        sprite_heightmap_(
            data::VboIdMain::kTerrainBakeHeightmap, data::TextId::kNone) {
    gUiComponents[btn_close_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[btn_accept_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[label_erosion_steps_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[label_weathering_steps_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[erosion_steps_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[weathering_steps_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[sprite_heightmap_.GetId() - details::kIdOffsetUi].parent_id_
        = sprite_desk_.GetId();
    gUiComponents[GetId() - details::kIdOffsetUi].ui =
        static_cast<UiTransformDbg*>(this);
    Base::UpdateTransform();
  }

  void Render() {
    sprite_desk_.Render();
    btn_close_.Render();
    btn_accept_.Render();

    label_erosion_steps_.Render();
    input_erosion_steps_.Render();

    label_weathering_steps_.Render();
    input_weathering_steps_.Render();

    sprite_heightmap_.Render();
  }

 private:
  UiStaticSprite sprite_desk_;
  UiStaticSprite btn_close_;
  UiStaticSprite btn_accept_;

  UiTextLabel label_erosion_steps_;
  UiTextLabel label_weathering_steps_;

  UiTextInput erosion_steps_;
  UiTextInput weathering_steps_;

  UiHeightmapSprite sprite_heightmap_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainBakeHeightmap)
      - static_cast<int>(data::VboIdMain::kTerrainBake)>
      ui_event_handler_;
};

class UiTerrainEditConfig final : public UiBase {
 public:
  UiTerrainEditConfig() {
    gUiComponents[edit_back_.GetId() - details::kIdOffsetUi].ui =
        static_cast<UiBase*>(this);
    UpdateTransform();
  }

  //TODO: move ctor update .ui = this

  void Render() {
    edit_back_.Render();
    if (render_noises_edit_) {
      ui_noises_.Render();
    } else {
      ui_slots_edit_.Render();
    }
  }

  void UpdateTransform() {
    edit_back_.UpdateTransform();
    noise_config_.UpdateTransform();
  }

 private:
  UiStaticSprite edit_back_;
  UiTerrainNoiseConfig noise_config_;
  UiTerrainSlotsConfig slots_config_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
