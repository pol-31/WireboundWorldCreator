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
/*
class UiColorPalette final : public UiBase {
 public:
  UiColorPalette(UiDynamicSprite&& palette,
                 UiDynamicSprite&& cursor_color,
                 UiDynamicSprite&& cursor_brightness);

  UiColorPalette(UiColorPalette&& other) noexcept;
  UiColorPalette(const UiColorPalette& other) = delete;

  UiColorPalette& operator=(UiColorPalette&& other) = delete;
  UiColorPalette& operator=(const UiColorPalette& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking() const;

  [[nodiscard]] data::TextId Hover(std::uint32_t id);

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetHue() const {
    return hue_;
  }

  [[nodiscard]] float GetSaturation() const {
    return saturation_;
  }

  [[nodiscard]] float GetBrightness() const {
    return brightness_;
  }

  [[nodiscard]] glm::vec3 GetRbgColor() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  UiDynamicSprite palette_;
  UiDynamicSprite cursor_color_;
  UiDynamicSprite cursor_brightness_;

  float hue_ = 0.0f;
  float saturation_ = 0.0f;
  float brightness_ = 0.0f;

  bool pressed_color_ = 0.0f;
  bool pressed_brightness_ = 0.0f;

  float hue_centre_;
  float saturation_centre_;
  float brightness_centre_;

  float hue_length_;
  float saturation_length_;
  float brightness_length_;

  static const float kTrackWidthFactor;
  static const float kTrackHeightFactor;
  // separate hue+saturation from brightness
  static const float kSeparatorFactor;
};*/

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
      UiSliderH&& scale,
      UiSliderH&& fetch,
      UiSliderH&& spread_blend,
      UiSliderH&& swell,
      UiSliderH&& peak_enhancement,
      UiSliderH&& short_waves_fade,
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
  UiSliderH scale_;
  UiSliderH fetch_;
  UiSliderH spread_blend_;
  UiSliderH swell_;
  UiSliderH peak_enhancement_;
  UiSliderH short_waves_fade_;
  UiSliderH lambda_;

  bool modified_{false};

  bool hovered_{false};

  UiEventHandler<28> ui_event_handler_;
};

// ---

class IUiEdit : public UiBase {
 public:
  IUiEdit();
};

class UiEditStub final : public IUiEdit {
 public:
  UiEditStub();

  UiEditStub(UiEditStub&& other) noexcept;
  UiEditStub(const UiEditStub& other) = delete;

  UiEditStub& operator=(UiEditStub&& other) = delete;
  UiEditStub& operator=(const UiEditStub& other) = delete;

  void Render();

  void RenderPicking() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:/*
  UiDynamicSprite desk_;
  UiDynamicSprite accept_;
  UiDynamicSprite close_;
  UiDynamicSprite name_;
  UiDynamicSprite name_back_;
  UiDynamicSprite color_;

//  UiColorPalette palette_;
  UiDynamicSprite color_cursor_;
  UiDynamicSprite brightness_cursor_;

  UiDynamicSprite type_back_;
  UiDynamicSprite type_text_;
  UiDynamicSprite type_prev_;
  UiDynamicSprite type_next_;*/
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
