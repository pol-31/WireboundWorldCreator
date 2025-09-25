#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_

#include <span>

#include "../core/UiComplex.h"
#include "TerrainNoiseData.h"

Texture32F GenAndSave(std::string_view tex_name);

class ITerrainNoise {
 public:
  virtual std::span<float> GetValueSpan() noexcept = 0;
  virtual std::span<data::TextId> GetTextIdSpan() noexcept = 0;
  virtual data::TextId GetTextId() const noexcept = 0;

  NoiseDataBase* GetBaseConfigPtr() {
    return &base_data_;
  }

  void ToggleDoInvert() {
    base_data_.do_invert = !base_data_.do_invert;
  }

  void ToggleDoTiling() {
    base_data_.do_tiling = !base_data_.do_tiling;
  }

 void SetBaseData(
      bool do_invert, bool do_tiling,
      float strength, GLuint hmap_id) {
   base_data_.do_invert = do_invert;
   base_data_.do_tiling = do_tiling;
   base_data_.strength = strength;
   base_data_.hmap_id = hmap_id;
 }

 protected:
  Shader shader_;
  NoiseDataBase base_data_;
};

template <size_t gParamNum>
class TerrainNoiseBase : public ITerrainNoise {
 public:
  std::span<float> GetValueSpan() noexcept override {
    return value_;
  }
  std::span<data::TextId> GetTextIdSpan() noexcept override {
    return text_id_;
  }
  /// no GetScaleSpan(), scale applied internally at glUniform

 protected:
  std::array<float, gParamNum> value_;
  std::array<float, gParamNum> scale_;
  std::array<data::TextId, gParamNum> text_id_;
};

class TerrainNoisePerlin final : public TerrainNoiseBase<3> {
 public:
  TerrainNoisePerlin();

  NoisePerlinData Generate();

  void SetConfig(const NoisePerlinData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseCellular final : public TerrainNoiseBase<4> {
 public:
  TerrainNoiseCellular();

  NoiseCellularData Generate();

  void SetConfig(const NoiseCellularData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseMetaballs final : public TerrainNoiseBase<4> {
 public:
  TerrainNoiseMetaballs();

  NoiseMetaballsData Generate();

  void SetConfig(const NoiseMetaballsData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseFbmGrid final : public TerrainNoiseBase<9> {
 public:
  TerrainNoiseFbmGrid();

  NoiseFbmGridData Generate();

  void SetConfig(const NoiseFbmGridData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseFbmMulti final : public TerrainNoiseBase<5> {
 public:
  TerrainNoiseFbmMulti();

  NoiseFbmMultiData Generate();

  void SetConfig(const NoiseFbmMultiData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseFbmdPerlin final : public TerrainNoiseBase<8> {
 public:
  TerrainNoiseFbmdPerlin();

  NoiseFbmdPerlinData Generate();

  void SetConfig(const NoiseFbmdPerlinData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseFbmWarp final : public TerrainNoiseBase<10> {
 public:
 TerrainNoiseFbmWarp();

  NoiseFbmWarpData Generate();

  void SetConfig(const NoiseFbmWarpData& config);

  data::TextId GetTextId() const noexcept override;
};

class TerrainNoiseFbmPerlinWarp final : public TerrainNoiseBase<10> {
 public:
  TerrainNoiseFbmPerlinWarp();

  NoiseFbmPerlinWarpData Generate();

  void SetConfig(const NoiseFbmPerlinWarpData& config);

  data::TextId GetTextId() const noexcept override;
};


class UiTerrainNoise final : public UiWindowAppear {
 public:
  UiTerrainNoise(
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      TextRenderer& text_renderer);

  UiTerrainNoise(UiTerrainNoise&& other) noexcept;

  bool Render() override;

  void RenderPicking() override;

  bool Scroll(GLuint id, float yoffset) override;

  bool Press(int id) override;

  void Release() override;

  void SetNoise(ITerrainNoise* noise);

 private:
  void RenderSlotsSprites(glm::vec2 mouse_pos);

  void RenderSlotsText();

  void RenderPickingSlotsSprites();

  void RenderPickingSlotsText();

  UiTextModeId name_;
  UiDynamicSprite btn_save_;

//  UiDynamicSprite sp_sl_track_;
//  UiDynamicSprite sp_sl_handle_;
  UiWindowSlider sl_data_;

  UiSliderH2 config_slider_;
  UiTextModeId config_text_;

  //TODO: dublicating the size
  std::span<float> value_;
  std::span<data::TextId> text_id_;

  int sl_pressed_config_ = -1;

  //  UiEventHandler<gWidgetsNum> ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UITERRAINCONFIG_H_
