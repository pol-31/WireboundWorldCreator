#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_

#include "../core/UiComplex.h"
#include "TerrainNoiseData.h"

Texture32F GenAndSave(std::string_view tex_name);

template <size_t gSlidersNum, size_t gTogglesNum,
          size_t gTextNum, size_t gWidgetsNum>
class TerrainNoiseBase : public UiWindowAppear {
 public:
  TerrainNoiseBase(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      UiDynamicSprite&& btn_save,
      UiTextLabelId&& name)
      : UiWindowAppear(std::move(sprite), size_scale, std::move(pin),
                       ui_shared_resources, window_queue),
        btn_save_(std::move(btn_save)),
        name_(std::move(name)) {}

  bool Render() override {
    RenderBack(true);
    btn_save_.Render();
    auto mouse_pos =
        ui_shared_resources_.global_glfw_callback_data_.cursor_pos_tex_norm_;
    for (auto slider : sliders_) {
      slider->Render(mouse_pos);
    }
    for (auto toggle : toggles_) {
      toggle->Render();
    }
    for (auto text : texts_) {
      text->Render();
    }
    name_.Render();
    return false;
  }

  void RenderPicking() override {
    RenderPickingBack();
    btn_save_.RenderPicking();
    for (auto slider : sliders_) {
      slider->RenderPicking();
    }
    for (auto toggle : toggles_) {
      toggle->RenderPicking();
    }
    for (auto text : texts_) {
      text->RenderPicking();
    }
    name_.RenderPicking();
  }

  bool Scroll(GLuint id, float yoffset) override {
    bool handled = false;
    for (auto slider : sliders_) {
      handled || slider->Scroll(id, yoffset);
    }
    return handled;
  }

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override {
    sprite_.UpdateTransform();
    pin_.UpdateTransform();
    btn_save_.UpdateTransform();
    for (auto slider : sliders_) {
      slider->UpdateTransform();
    }
    for (auto toggle : toggles_) {
      toggle->UpdateTransform();
    }
    for (auto text : texts_) {
      text->UpdateTransform();
    }
    name_.UpdateTransform();
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
    gUiComponents[pin_.GetId() - details::kIdOffsetUi].parent_id_
        = parent_id;
    gUiComponents[name_.GetId() - details::kIdOffsetUi].parent_id_
        = parent_id;
    gUiComponents[btn_save_.GetId() - details::kIdOffsetUi].parent_id_
        = parent_id;
    gUiComponents[sprite_.GetId() - details::kIdOffsetUi].ui
        = static_cast<UiBase*>(this);
    //TODO: move ctor
  }

  UiTextLabelId name_;
  UiDynamicSprite btn_save_;

  //TODO:
//  UiDynamicSprite hmap_;
//  Texture32F& hmap_ref_;

  std::array<UiSliderH2*, gSlidersNum> sliders_;
  std::array<UiToggle*, gTogglesNum> toggles_;
  std::array<UiTextLabelId*, gTextNum> texts_;

  UiEventHandler<gWidgetsNum> ui_event_handler_;
  Shader shader_;
};

class TerrainNoisePerlin final
    : public TerrainNoiseBase<
          3, 0, 3,
          static_cast<int>(data::VboIdMain::kTerrainNoisePerlinSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoisePerlinDesk)> {
 public:
  using Base = TerrainNoiseBase<
      3, 0, 3,
      static_cast<int>(data::VboIdMain::kTerrainNoisePerlinSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoisePerlinDesk)>;

  explicit TerrainNoisePerlin(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoisePerlinData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoisePerlinData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
};

class TerrainNoiseCellular final
    : public TerrainNoiseBase<
          4, 0, 4,
          static_cast<int>(data::VboIdMain::kTerrainNoiseCellularSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseCellularDesk)> {
 public:
  using Base = TerrainNoiseBase<
      4, 0, 4,
      static_cast<int>(data::VboIdMain::kTerrainNoiseCellularSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseCellularDesk)>;

  explicit TerrainNoiseCellular(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseCellularData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseCellularData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_jitter_;
  UiTextLabelId text_jitter_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
};

class TerrainNoiseMetaballs final
    : public TerrainNoiseBase<
          4, 0, 4,
          static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsDesk)> {
 public:
  using Base = TerrainNoiseBase<
      4, 0, 4,
      static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseMetaballsDesk)>;

  explicit TerrainNoiseMetaballs(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseMetaballsData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseMetaballsData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_jitter_;
  UiTextLabelId text_jitter_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
};

class TerrainNoiseFbmGrid final
    : public TerrainNoiseBase<
          9, 0, 9,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridDesk)> {
 public:
  using Base = TerrainNoiseBase<
      9, 0, 9,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmGridDesk)>;

  explicit TerrainNoiseFbmGrid(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseFbmGridData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseFbmGridData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
  UiSliderH2 slider_shift_;
  UiTextLabelId text_shift_;
  UiSliderH2 slider_gain_;
  UiTextLabelId text_gain_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
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
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiDesk)> {
 public:
  using Base = TerrainNoiseBase<
      5, 0, 5,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmMultiDesk)>;

  explicit TerrainNoiseFbmMulti(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseFbmMultiData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseFbmMultiData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_lacunarity_;
  UiTextLabelId text_lacunarity_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
  UiSliderH2 slider_seed_;
  UiTextLabelId text_seed_;
};

class TerrainNoiseFbmdPerlin final
    : public TerrainNoiseBase<
          8, 1, 9,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinDesk)> {
 public:
  using Base = TerrainNoiseBase<
      8, 1, 9,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinSeedIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmdPerlinDesk)>;

  explicit TerrainNoiseFbmdPerlin(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseFbmdPerlinData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseFbmdPerlinData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
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
          10, 1, 11,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpRIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpDesk)> {
 public:
  using Base = TerrainNoiseBase<
      10, 1, 11,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpRIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmWarpDesk)>;

  explicit TerrainNoiseFbmWarp(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseFbmWarpData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseFbmWarpData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
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
          10, 1, 11,
          static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon)
              - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpDesk)> {
 public:
  using Base = TerrainNoiseBase<
      10, 1, 11,
      static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpRIcon)
          - static_cast<int>(data::VboIdMain::kTerrainNoiseFbmPerlinWarpDesk)>;

  explicit TerrainNoiseFbmPerlinWarp(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  NoiseFbmPerlinWarpData Generate(
      glm::vec2 resolution, std::string_view tex_name);

  void SetConfig(const NoiseFbmPerlinWarpData& config);

 private:
  UiSliderH2 slider_scale_x_;
  UiTextLabelId text_scale_x_;
  UiSliderH2 slider_scale_y_;
  UiTextLabelId text_scale_y_;
  UiSliderH2 slider_octaves_;
  UiTextLabelId text_octaves_;
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
