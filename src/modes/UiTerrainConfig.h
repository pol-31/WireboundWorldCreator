#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_

#include "../core/Ui.h"
#include "../core/UiText.h"

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
  std::array<UiTextLabelId*, gTextNum> texts_;

  UiEventHandler<gWidgetsNum> ui_event_handler_;
  Shader shader_;
};

Texture GenAndSave(
    glm::vec2 resolution, std::vector<unsigned char>& buffer,
    std::string_view tex_name);

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

  explicit TerrainNoisePerlin(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
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

  explicit TerrainNoiseCellular(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_jitter_;
  UiTextLabelId text_jitter_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
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

  explicit TerrainNoiseMetaballs(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_jitter_;
  UiTextLabelId text_jitter_;
//  UiSliderH2 slider_phase_; remove
//  UiTextLabelId text_phase_; remove
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
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

  explicit TerrainNoiseFbmGrid(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
  UiSliderH2 slider_shift_;
  UiTextLabelId text_shift_;
//  UiSliderH2 slider_time_shift_; remove
//  UiTextLabelId text_time_shift_; remove
  UiSliderH2 slider_gain_;
  UiTextLabelId text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
//  UiSliderH2 slider_translate_; remove
//  UiTextLabelId text_translate_; remove
  UiSliderH2 slider_warp_strength_;
  UiTextLabelId text_warp_strength_;
  UiSliderH2 slider_octave_factor_;
  UiTextLabelId text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
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

  explicit TerrainNoiseFbmMulti(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
//  UiSliderH2 slider_phases_; remove
//  UiTextLabelId text_phases_; remove
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
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

  explicit TerrainNoiseFbmdPerlin(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
//  UiSliderH2 slider_shift_; remove
//  UiTextLabelId text_shift_; remove
//  UiSliderH2 slider_transform_; remove
//  UiTextLabelId text_transform_; remove
  UiSliderH2 slider_gain_;
  UiTextLabelId text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiTextLabelId text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiTextLabelId text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;

  UiToggle toggle_negative_;
  UiTextLabelId text_negative_;
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

  explicit TerrainNoiseFbmWarp(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
//  UiSliderH2 slider_factors_; remove
//  UiTextLabelId text_factors_; remove
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
//  UiSliderH2 slider_shift_; remove
//  UiTextLabelId text_shift_; remove
//  UiSliderH2 slider_time_shift_; remove
//  UiTextLabelId text_time_shift_; remove
  UiSliderH2 slider_gain_;
  UiTextLabelId text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiTextLabelId text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiTextLabelId text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
  UiSliderH2 slider_q_;
  UiTextLabelId text_q_;
  UiSliderH2 slider_r_;
  UiTextLabelId text_r_;

  UiToggle toggle_negative_;
  UiTextLabelId text_negative_;
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

  explicit TerrainNoiseFbmPerlinWarp(TextRenderer& text_renderer, GLuint parent_id);

  Texture Generate(
      glm::vec2 resolution, std::vector<unsigned char>& buffer,
      std::string_view tex_name);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  // UiSliderH2 slider_scale_y_;
  // UiTextLabelId text_scale_y_;
//  UiSliderH2 slider_factors_; remove
//  UiTextLabelId text_factors_; remove
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
//  UiSliderH2 slider_shift_; remove
//  UiTextLabelId text_shift_; remove
//  UiSliderH2 slider_time_shift_; remove
//  UiTextLabelId text_time_shift_; remove
  UiSliderH2 slider_gain_;
  UiTextLabelId text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
  UiSliderH2 slider_slopeness_;
  UiTextLabelId text_slopeness_;
  UiSliderH2 slider_octave_factor_;
  UiTextLabelId text_octave_factor_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
  UiSliderH2 slider_q_;
  UiTextLabelId text_q_;
  UiSliderH2 slider_r_;
  UiTextLabelId text_r_;

  UiToggle toggle_negative_;
  UiTextLabelId text_negative_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
