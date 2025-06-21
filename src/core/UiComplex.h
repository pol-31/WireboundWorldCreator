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
      UiDynamicSprite&& sprite,
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

      UiStaticSprite&& btn_settings,
      UiStaticSprite&& btn_shader_wirebound,

      UiToggle&& toggle_shaders,

      UiDynamicSprite&& cross);

  UiTabMenu(UiTabMenu&& other) noexcept;
  UiTabMenu(const UiTabMenu& other) = delete;

  UiTabMenu& operator=(UiTabMenu&& other) = delete;
  UiTabMenu& operator=(const UiTabMenu& other) = delete;

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

  UiStaticSprite btn_settings_;
  UiStaticSprite btn_shader_wirebound_;

  UiToggle toggle_shaders_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kMenuSettings) -
      static_cast<int>(data::VboIdMain::kMenuTerrain) + 1
      > ui_event_handler_;

  UiDynamicSprite cross_;
};

class UiSettings final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiSettings(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      UiSliderH3&& resolution,
      UiSliderH3&& music,
      UiSliderH3&& sound,
      UiSliderH3&& sensitivity,
      UiStaticSprite&& keyboard,
      UiToggle&& toggle_music,
      UiToggle&& toggle_sound,
      UiDynamicSprite&& cross);

  UiSettings(UiSettings&& other) noexcept;
  UiSettings(const UiSettings& other) = delete;

  UiSettings& operator=(UiSettings&& other) = delete;
  UiSettings& operator=(const UiSettings& other) = delete;

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

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiSliderH3 resolution_;
  UiSliderH3 music_;
  UiSliderH3 sound_;
  UiSliderH3 sensitivity_;
  UiStaticSprite keyboard_;

  UiToggle toggle_music_;
  UiToggle toggle_sound_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kSettingsMusicOn3) -
      static_cast<int>(data::VboIdMain::kSettingsResolutionFill) + 1
      > ui_event_handler_;

  UiDynamicSprite cross_;
};

class UiConfirmation final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiConfirmation(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiSharedResources& ui_shared_resources,

      UiStaticSprite&& btn_close,
      UiStaticSprite&& btn_accept,
      UiStaticSprite&& btn_decline,

      UiDynamicSprite&& cross);

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
  UiStaticSprite btn_close_;
  UiStaticSprite btn_accept_;
  UiStaticSprite btn_decline_;

  // 3 buttons
  UiEventHandler<3> ui_event_handler_;

  UiDynamicSprite cross_;
};

/// only one scale allowed (no x or y scale)
class UiPopUpBase : public UiBase {
 public:
  using UiBase::UpdateTransform;

  UiPopUpBase(UiDynamicSprite&& sprite,
              float size_scale,
              UiSharedResources& ui_shared_resources,
              LocalTransform start_transform,
              LocalTransform end_transform);

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

  bool back_ready_{false};
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

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
