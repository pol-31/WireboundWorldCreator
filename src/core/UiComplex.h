#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_

#include "Ui.h"

/// loading only on the bottom of the screen (so use UiStaticSprite)
class UiLoading final : public UiBase {
 public:
  UiLoading(UiStaticSprite&& sprite0,
            UiStaticSprite&& sprite10,
            UiStaticSprite&& sprite20,
            UiStaticSprite&& sprite30,
            UiStaticSprite&& sprite40,
            UiStaticSprite&& sprite50,
            UiStaticSprite&& sprite60,
            UiStaticSprite&& sprite70,
            UiStaticSprite&& sprite80,
            UiStaticSprite&& sprite90,
            UiStaticSprite&& sprite100);

  UiLoading(UiLoading&& other) noexcept;
  UiLoading(const UiLoading& other) = delete;

  UiLoading& operator=(UiLoading&& other) = delete;
  UiLoading& operator=(const UiLoading& other) = delete;

  void Render(float progress);

  void RenderPicking() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  std::array<UiStaticSprite, 11> sprites_;
};

//used for UiPalette, so X-axis is Hue, Y-asix is Saturation
class UiSlider2D final : public UiBase {
 public:
  UiSlider2D(UiDynamicSprite&& palette,
             UiDynamicSprite&& cursor,
             glm::vec2 scale = glm::vec2{1.0f});

  UiSlider2D(UiSlider2D&& other) noexcept;
  UiSlider2D(const UiSlider2D& other) = delete;

  UiSlider2D& operator=(UiSlider2D&& other) = delete;
  UiSlider2D& operator=(const UiSlider2D& other) = delete;

  void Render(glm::vec2 mouse_pos);
  void RenderIcon();

  void RenderPicking() const;

  [[nodiscard]] data::TextId Hover(std::uint32_t id);

  void Press() override {
    pressed_ = true;
  }

  void Release() override {
    pressed_ = false;
  }

  bool Scroll(GLuint id, float yoffset) override;

  [[nodiscard]] glm::vec2 GetProgress() const;

  [[nodiscard]] float GetProgressX() const;

  [[nodiscard]] float GetProgressY() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

 private:
  void SetMousePos(glm::vec2 mouse_pos);

  void SetProgress(glm::vec2 progress);

  UiDynamicSprite palette_;
  UiDynamicSprite cursor_;

  glm::vec2 progress_ = glm::vec2{0.0f};
  bool pressed_ = false;
  glm::vec2 centre_;
  glm::vec2 length_;
  glm::vec2 scale_;

  static const float kTrackWidthFactor;
  static const float kTrackHeightFactor;
};

class UiWindowBase : public UiBase {
 public:
  using UiBase::UpdateTransform;

  UiWindowBase(UiDynamicSprite&& sprite,
               float size_scale,
               UiSharedResources& ui_shared_resources);

  UiWindowBase(UiWindowBase&& other) noexcept;
  UiWindowBase(const UiWindowBase& other) = delete;

  UiWindowBase& operator=(UiWindowBase&& other) = delete;
  UiWindowBase& operator=(const UiWindowBase& other) = delete;

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();

  [[nodiscard]] bool BackIsReady() const {
    return back_ready_;
  }

  void UpdateTransform() final;

 protected:
  UiDynamicSprite sprite_; // dynamic to set scale

  float speed_{0.5f};
  float progress_{0.0f};

  // how it differs from the nested components
  float size_scale_{1.0f};

  bool back_ready_{false};

  UiSharedResources& ui_shared_resources_; // for shader bindings, mask texture
};

class UiTabMenu final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiTabMenu(
      UiDynamicSprite&& back_desk,
      float size_scale,
      UiSharedResources& ui_shared_resources,

      UiStaticSprite&& btn_mode_terrain,
      UiStaticSprite&& btn_mode_water,
      UiStaticSprite&& btn_mode_roads,
      UiStaticSprite&& btn_mode_fences,
      UiStaticSprite&& btn_mode_placement,
      UiStaticSprite&& btn_mode_objects,
      UiStaticSprite&& btn_mode_biomes,
      UiStaticSprite&& btn_mode_tiles,

      UiToggle&& toggle_terrain,
      UiToggle&& toggle_water,
      UiToggle&& toggle_roads,
      UiToggle&& toggle_fences,
      UiToggle&& toggle_placement,
      UiToggle&& toggle_objects,
      UiToggle&& toggle_biomes,
      UiToggle&& toggle_tiles,

      UiStaticSprite&& btn_shader_wirebound,
      UiToggle&& toggle_shaders,

      UiDynamicSprite&& arrow_select,
      UiDynamicSprite&& arrow_selected,
      UiDynamicSprite&& save_data,
      UiDynamicSprite&& load_data);

  UiTabMenu(UiTabMenu&& other) noexcept;
  UiTabMenu(const UiTabMenu& other) = delete;

  UiTabMenu& operator=(UiTabMenu&& other) = delete;
  UiTabMenu& operator=(const UiTabMenu& other) = delete;

  // returns "stop render"
  bool Render(bool show, float angle_select, float angle_selected);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

  void Release() {
    ui_event_handler_.Release();
  }

  data::TextId Hover(int id) {
    return ui_event_handler_.Hover(id);
  }

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiStaticSprite btn_mode_terrain_;
  UiStaticSprite btn_mode_water_;
  UiStaticSprite btn_mode_roads_;
  UiStaticSprite btn_mode_fences_;
  UiStaticSprite btn_mode_placement_;
  UiStaticSprite btn_mode_objects_;
  UiStaticSprite btn_mode_biomes_;
  UiStaticSprite btn_mode_tiles_;

  UiToggle toggle_terrain_;
  UiToggle toggle_water_;
  UiToggle toggle_roads_;
  UiToggle toggle_fences_;
  UiToggle toggle_placement_;
  UiToggle toggle_objects_;
  UiToggle toggle_biomes_;
  UiToggle toggle_tiles_;

  UiStaticSprite btn_shader_wirebound_;
  UiToggle toggle_shaders_;

  UiDynamicSprite arrow_select_;
  UiDynamicSprite arrow_selected_;
  UiDynamicSprite save_data_;
  UiDynamicSprite load_data_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kMenuLoad) -
      static_cast<int>(data::VboIdMain::kMenuTerrain) + 1
      > ui_event_handler_;
};

class UiConfirmation final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiConfirmation(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      UiStaticSprite&& btn_accept,
      UiStaticSprite&& btn_decline);

  UiConfirmation(UiConfirmation&& other) noexcept;
  UiConfirmation(const UiConfirmation& other) = delete;

  UiConfirmation& operator=(UiConfirmation&& other) = delete;
  UiConfirmation& operator=(const UiConfirmation& other) = delete;

  // returns "stop render"
  bool Render(bool show);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

  void Release() {
    ui_event_handler_.Release();
  }

  data::TextId Hover(int id) {
    return ui_event_handler_.Hover(id);
  }

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiStaticSprite btn_accept_;
  UiStaticSprite btn_decline_;

  // 3 buttons
  UiEventHandler<3> ui_event_handler_;
};

/// only one scale allowed (no x or y scale)
class UiPopUpBase : public UiBase {
 public:
  using UiBase::UpdateTransform;

  UiPopUpBase(UiDynamicSprite&& sprite,
              float size_scale,
              UiSharedResources& ui_shared_resources,
              LocalTransform start_transform,
              LocalTransform end_transform,
              UiToggle2&& pin);

  UiPopUpBase(UiPopUpBase&& other) noexcept;
  UiPopUpBase(const UiPopUpBase& other) = delete;

  UiPopUpBase& operator=(UiPopUpBase&& other) = delete;
  UiPopUpBase& operator=(const UiPopUpBase& other) = delete;

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();

  [[nodiscard]] bool BackIsReady() const {
    return back_ready_;
  }

  void UpdateTransform() final;

 protected:
  // cubic interpolation here (not shader)
  LocalTransform cur_transform_;
  UiDynamicSprite sprite_; // dynamic to set scale

  UiToggle2 pin_;

  // for shader bindings, mask texture
  UiSharedResources& ui_shared_resources_;

 private:
  void CubicInterpolation();
  // not dynamic (transformation stored here, not in the sprite)

  // store separately for easiest interpolation
  LocalTransform start_transform_;
  LocalTransform end_transform_;

  // how it differs from the nested components
  float size_scale_{1.0f};

  const float speed_{0.5f};
  float progress_{0.0f};

  bool back_ready_ = false;
};


class UiSettings final : public UiPopUpBase {
 public:
  using Base = UiPopUpBase;

  UiSettings(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      LocalTransform start_transform,
      LocalTransform end_transform,
      UiToggle2&& pin,
      UiDynamicSprite&& resolution_label,
      UiDynamicSprite&& resolution_left,
      UiDynamicSprite&& resolution_right,
      UiDynamicSprite&& resolution_slot,
      UiDynamicSprite&& resolution,
      UiToggle&& toggle_fullscreen,
      UiDynamicSprite sensitivity_icon,
      UiSliderH2 sensitivity,
      UiDynamicSprite keyboard,
      UiDynamicSprite&& sound_icon,
      UiSliderH2&& sound,
      UiToggle&& toggle_sound,
      UiDynamicSprite&& music_icon,
      UiSliderH2&& music,
      UiToggle&& toggle_music,
      UiDynamicSprite&& tip_info_label,
      UiDynamicSprite&& tip_info,
      UiToggle&& toggle_tip_info);

  UiSettings(UiSettings&& other) noexcept;
  UiSettings(const UiSettings& other) = delete;

  UiSettings& operator=(UiSettings&& other) = delete;
  UiSettings& operator=(const UiSettings& other) = delete;

  // returns "stop render"
  bool Render();

  void RenderPicking();

  void Press(int id);

  void Release();

  data::TextId Hover(int id);

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  //todo; replace by component
  UiDynamicSprite resolution_label_;
  UiDynamicSprite resolution_left_;
  UiDynamicSprite resolution_right_;
  UiDynamicSprite resolution_slot_;

  UiDynamicSprite resolution_;
  UiToggle toggle_fullscreen_;

  UiDynamicSprite sensitivity_icon_;
  UiSliderH2 sensitivity_;

  UiDynamicSprite keyboard_;

  UiDynamicSprite sound_icon_;
  UiSliderH2 sound_;
  UiToggle toggle_sound_;

  UiDynamicSprite music_icon_;
  UiSliderH2 music_;
  UiToggle toggle_music_;

  UiDynamicSprite tip_info_label_;
  UiDynamicSprite tip_info_;
  UiToggle toggle_tip_info_;


  UiEventHandler<
      static_cast<int>(data::VboIdMain::kSettingsTipInfoOn3) -
      static_cast<int>(data::VboIdMain::kSettingsDesk) + 1
      > ui_event_handler_;

  bool hovered_ = false;
};

class UiWaterLayerConfig final : public UiPopUpBase {
 public:
  using Base = UiPopUpBase;

  UiWaterLayerConfig(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      LocalTransform start_transform,
      LocalTransform end_transform,
      UiToggle2&& pin,
      UiDynamicSprite&& sprite_layer,
      UiDynamicSprite&& text_layer,
      UiToggle&& toggle_layer,
      UiDynamicSprite&& scale_text,
      UiSliderH&& scale,
      UiDynamicSprite&& fetch_text,
      UiSliderH&& fetch,
      UiDynamicSprite&& spread_blend_text,
      UiSliderH&& spread_blend,
      UiDynamicSprite&& swell_text,
      UiSliderH&& swell,
      UiDynamicSprite&& peak_enhancement_text,
      UiSliderH&& peak_enhancement,
      UiDynamicSprite&& short_waves_fade_text,
      UiSliderH&& short_waves_fade,
      UiDynamicSprite&& lambda_text,
      UiSliderH&& lambda);

  UiWaterLayerConfig(UiWaterLayerConfig&& other) noexcept;
  UiWaterLayerConfig(const UiWaterLayerConfig& other) = delete;

  UiWaterLayerConfig& operator=(UiWaterLayerConfig&& other) = delete;
  UiWaterLayerConfig& operator=(const UiWaterLayerConfig& other) = delete;

  // returns "stop render"
  bool Render();

  void Release();

  bool Scroll(GLuint id, float yoffset) override;

  void RenderPicking();

  void Press(int id) {
    modified_ = ui_event_handler_.Press(id);
  }

  data::TextId Hover(int id);

  bool Modified();

  OceanLayerTraits GetOceanLayerTraits() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiDynamicSprite sprite_layer_;
  UiDynamicSprite text_layer_;
  UiToggle toggle_layer_;
  UiSliderH scale_;
  UiDynamicSprite scale_text_;
  UiSliderH fetch_;
  UiDynamicSprite fetch_text_;
  UiSliderH spread_blend_;
  UiDynamicSprite spread_blend_text_;
  UiSliderH swell_;
  UiDynamicSprite swell_text_;
  UiSliderH peak_enhancement_;
  UiDynamicSprite peak_enhancement_text_;
  UiSliderH short_waves_fade_;
  UiDynamicSprite short_waves_fade_text_;
  UiSliderH lambda_;
  UiDynamicSprite lambda_text_;

  bool modified_{false};

  bool hovered_{false};

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kWater1LambdaHandler) -
      static_cast<int>(data::VboIdMain::kWaterLayer1) + 1
      > ui_event_handler_;
};

// ---

//class IUiEdit : public UiBase {
// public:
//  IUiEdit();
//};

class UiEditFences final : public UiBase {
 public:
  UiEditFences(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& desk,
      UiDynamicSprite&& accept,
      UiDynamicSprite&& name,
      UiDynamicSprite&& name_back,
      UiSlider2D&& color_palette,
      UiSliderH2&& color_brightness,
      UiDynamicSprite&& color_indicator,
      UiDynamicSprite&& type_back,
      UiDynamicSprite&& type_text,
      UiDynamicSprite&& type_prev,
      UiDynamicSprite&& type_next);

  UiEditFences(UiEditFences&& other) noexcept;
  UiEditFences(const UiEditFences& other) = delete;

  UiEditFences& operator=(UiEditFences&& other) = delete;
  UiEditFences& operator=(const UiEditFences& other) = delete;

  data::TextId Hover(int id);

  bool Press(int id);

  void Release();

  bool Scroll(GLuint id, float yoffset);

  void Render();

  void RenderPicking();

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  UiDynamicSprite desk_;
  UiDynamicSprite accept_;
  UiDynamicSprite name_;
  UiDynamicSprite name_back_;

  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;

  UiDynamicSprite type_back_;
  UiDynamicSprite type_text_;
  UiDynamicSprite type_prev_;
  UiDynamicSprite type_next_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kFencesTypeNext) -
      static_cast<int>(data::VboIdMain::kFencesEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

class UiEditTerrain final : public UiBase {
 public:
  UiEditTerrain(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& desk,
      UiDynamicSprite&& accept,
      UiDynamicSprite&& name,
      UiDynamicSprite&& name_back,
      UiSlider2D&& color_palette,
      UiSliderH2&& color_brightness,
      UiDynamicSprite&& color_indicator,
      UiDynamicSprite&& heightmap,
      UiDynamicSprite&& random_generate);

  UiEditTerrain(UiEditTerrain&& other) noexcept;
  UiEditTerrain(const UiEditTerrain& other) = delete;

  UiEditTerrain& operator=(UiEditTerrain&& other) = delete;
  UiEditTerrain& operator=(const UiEditTerrain& other) = delete;

  data::TextId Hover(int id);

  bool Press(int id);

  void Release();

  bool Scroll(GLuint id, float yoffset);

  void Render();

  void RenderPicking();

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  UiDynamicSprite desk_;
  UiDynamicSprite accept_;
  UiDynamicSprite name_;
  UiDynamicSprite name_back_;

  UiSlider2D color_palette_;
  UiSliderH2 color_brightness_;
  UiDynamicSprite color_indicator_;

  // vao, vbo the same (kTerrainEditHeightmap for fullscreen tx)
  // we simply need Texture which bind to, AND update it somehow
  Texture tex_heightmap_;
  UiDynamicSprite heightmap_;
  UiDynamicSprite random_generate_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainEditNoise8StrengthIcon) -
      static_cast<int>(data::VboIdMain::kTerrainEditDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

class UiTerrainBake final : public UiBase {
 public:
  UiTerrainBake(
      UiSharedResources& ui_shared_resources,
      UiDynamicSprite&& desk,
      UiDynamicSprite&& accept,
      UiDynamicSprite&& heightmap,
      UiDynamicSprite&& erosion_label,
      UiDynamicSprite&& erosion_input,
      UiDynamicSprite&& weathering_label,
      UiDynamicSprite&& weathering_input);

  UiTerrainBake(UiTerrainBake&& other) noexcept;
  UiTerrainBake(const UiTerrainBake& other) = delete;

  UiTerrainBake& operator=(UiTerrainBake&& other) = delete;
  UiTerrainBake& operator=(const UiTerrainBake& other) = delete;

  data::TextId Hover(int id);

  bool Press(int id);

  void Release();

  bool Scroll(GLuint id, float yoffset);

  void Render();

  void RenderPicking();

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  UiDynamicSprite desk_;
  UiDynamicSprite accept_;

  Texture tex_heightmap_;
  UiDynamicSprite heightmap_;

  UiDynamicSprite erosion_label_;
  UiDynamicSprite erosion_input_;
  UiDynamicSprite weathering_label_;
  UiDynamicSprite weathering_input_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kTerrainBakeHeightmap) -
      static_cast<int>(data::VboIdMain::kTerrainBakeDesk) + 1
      > ui_event_handler_;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
