#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_

#include "Ui.h"
#include "UiText.h"
#include "WindowQueue.h"

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

// cannot be pinned, cannot be hovered above,
// single to interact - on very top; shadow all other
class UiTopWindowBase : public UiBase {
 public:
  using UiBase::UpdateTransform;

  UiTopWindowBase(
      UiDynamicSprite&& desk,
      UiDynamicSprite&& shadow,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue);

  UiTopWindowBase(UiTopWindowBase&& other) noexcept;
  UiTopWindowBase(const UiTopWindowBase& other) = delete;

  UiTopWindowBase& operator=(UiTopWindowBase&& other) = delete;
  UiTopWindowBase& operator=(const UiTopWindowBase& other) = delete;

  void Show();

  void Hide();

  void UpdateTransform() final;

  virtual bool Render() = 0;

  virtual void RenderPicking() = 0;

  virtual void Press(int id) = 0;

  virtual void Release() = 0;

  virtual data::TextId Hover(int id) = 0;

 protected:
  UiDynamicSprite desk_;
  // Cancel() if not important; otherwise skip
  UiDynamicSprite shadow_;

  float size_scale_{1.0f};

  // for shader bindings, mask texture
  UiSharedResources& ui_shared_resources_;

  WindowQueue& window_queue_;
};

class UiCaution final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiCaution(
      UiDynamicSprite&& desk,
      UiDynamicSprite&& shadow,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      UiDynamicSprite&& text);

  UiCaution(UiCaution&& other) noexcept;
  UiCaution(const UiCaution& other) = delete;

  UiCaution& operator=(UiCaution&& other) = delete;
  UiCaution& operator=(const UiCaution& other) = delete;

  bool Render();

  void RenderPicking();

  void Press(int id);

  void Release();

  data::TextId Hover(int id);

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiDynamicSprite text_;
};

class UiConfirmation final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiConfirmation(
      UiDynamicSprite&& desk,
      UiDynamicSprite&& shadow,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      UiDynamicSprite&& btn_accept,
      UiDynamicSprite&& btn_decline,
      UiDynamicSprite&& text);

  UiConfirmation(UiConfirmation&& other) noexcept;
  UiConfirmation(const UiConfirmation& other) = delete;

  UiConfirmation& operator=(UiConfirmation&& other) = delete;
  UiConfirmation& operator=(const UiConfirmation& other) = delete;

  bool Render();

  void RenderPicking();

  void Press(int id);

  void Release();

  data::TextId Hover(int id);

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiDynamicSprite text_;
  UiDynamicSprite btn_accept_;
  UiDynamicSprite btn_decline_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kConfirmationDecline) -
      static_cast<int>(data::VboIdMain::kConfirmationDesk) + 1
      > ui_event_handler_;
};

class UiFile final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiFile(
      UiDynamicSprite&& desk,
      UiDynamicSprite&& shadow,
      float size_scale,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      UiDynamicSprite&& btn_accept,
      UiDynamicSprite&& btn_decline,
      UiTextLabel&& label,
      UiTextInput&& text);

  UiFile(UiFile&& other) noexcept;
  UiFile(const UiFile& other) = delete;

  UiFile& operator=(UiFile&& other) = delete;
  UiFile& operator=(const UiFile& other) = delete;

  bool Render();

  void RenderPicking();

  void Press(int id);

  void Release();

  data::TextId Hover(int id);

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiDynamicSprite btn_accept_;
  UiDynamicSprite btn_decline_;
  UiTextLabel label_;
  UiTextInput text_;

  UiEventHandler<
      static_cast<int>(data::VboIdMain::kFileAccept) -
      static_cast<int>(data::VboIdMain::kFileDesk) + 1
      > ui_event_handler_;
};

// single interactable window; can add few
class UiWindowBase : public UiBase {
 public:
  using UiBase::UpdateTransform;

  UiWindowBase(UiDynamicSprite&& sprite,
               float size_scale,
               UiToggle2&& pin,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue);

  UiWindowBase(UiWindowBase&& other) noexcept;
  UiWindowBase(const UiWindowBase& other) = delete;

  UiWindowBase& operator=(UiWindowBase&& other) = delete;
  UiWindowBase& operator=(const UiWindowBase& other) = delete;

  UiWindowBase* GetWindowPtr();

  void Show();

  void Hide();

  void UpdateTransform() final;

  virtual bool Render() = 0;

  virtual void RenderPicking() = 0;

  virtual void Press(int id) = 0;

  virtual void Release() = 0;

  virtual data::TextId Hover(int id) = 0;

  [[nodiscard]] bool Pinned() const noexcept;

  [[nodiscard]] bool BackIsReady() const {
    return back_ready_;
  }

 protected:
  UiDynamicSprite sprite_;
  // how it differs from the nested components
  float size_scale_{1.0f};
  UiToggle2 pin_;
  UiSharedResources& ui_shared_resources_; // for shader bindings, mask texture

  WindowQueue& window_queue_;
  int window_queue_id_ = -1;
  // useful if window still pinned, but menu already hidden
  bool do_show_ = false;

  // animation related (mask-appearing / transformation)
  float speed_{0.5f};
  float progress_{0.0f};
  bool back_ready_{false};
};

class UiWindowAppear : public UiWindowBase {
 public:
  using UiBase::UpdateTransform;

  UiWindowAppear(UiDynamicSprite&& sprite,
                 float size_scale,
                 UiToggle2&& pin,
                 UiSharedResources& ui_shared_resources,
                 WindowQueue& window_queue);

  UiWindowAppear(UiWindowAppear&& other) noexcept;
  UiWindowAppear(const UiWindowAppear& other) = delete;

  UiWindowAppear& operator=(UiWindowAppear&& other) = delete;
  UiWindowAppear& operator=(const UiWindowAppear& other) = delete;

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();
};

class UiTabMenu final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTabMenu(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,

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

  bool Render() override;

  void RenderPicking() override;

  void Press(int id) override;

  void Release() override;

  data::TextId Hover(int id) override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void SetSelectedArrow(float angle);

  void SetSelectArrow(float angle);

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

  float arrow_select_angle_ = 0;
  float arrow_selected_angle_ = 0;
};

class UiTipWindow final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTipWindow(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      UiDynamicSprite&& text);

  UiTipWindow(UiTipWindow&& other) noexcept;
  UiTipWindow(const UiTipWindow& other) = delete;

  UiTipWindow& operator=(UiTipWindow&& other) = delete;
  UiTipWindow& operator=(const UiTipWindow& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  void Press(int id) override;

  void Release() override;

  data::TextId Hover(int id) override;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void SetText(data::TextId text_id);

 private:
  UiDynamicSprite text_;
  bool hovered_ = false;
};

// -
/// only one scale allowed (no x or y scale)
class UiWindowPopUp : public UiWindowBase {
 public:
  using UiBase::UpdateTransform;

  UiWindowPopUp(UiDynamicSprite&& sprite,
                float size_scale,
                UiToggle2&& pin,
                UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue,
                LocalTransform start_transform,
                LocalTransform end_transform);

  UiWindowPopUp(UiWindowPopUp&& other) noexcept;
  UiWindowPopUp(const UiWindowPopUp& other) = delete;

  UiWindowPopUp& operator=(UiWindowPopUp&& other) = delete;
  UiWindowPopUp& operator=(const UiWindowPopUp& other) = delete;

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();

 protected:
  // cubic interpolation here (not shader)
  LocalTransform cur_transform_;
  bool hovered_ = false;

 private:
  void CubicInterpolation();
  // not dynamic (transformation stored here, not in the sprite)

  // store separately for easiest interpolation
  LocalTransform start_transform_;
  LocalTransform end_transform_;
};


class UiSettings final : public UiWindowPopUp {
 public:
  using Base = UiWindowPopUp;

  UiSettings(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      LocalTransform start_transform,
      LocalTransform end_transform,
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
};

class UiWaterLayerConfig final : public UiWindowPopUp {
 public:
  using Base = UiWindowPopUp;

  UiWaterLayerConfig(
      UiDynamicSprite&& sprite,
      float size_scale,
      UiToggle2&& pin,
      UiSharedResources& ui_shared_resources,
      WindowQueue& window_queue,
      LocalTransform start_transform,
      LocalTransform end_transform,
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
  bool Render() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void RenderPicking() override;

  void Press(int id) override {
    modified_ = ui_event_handler_.Press(id);
  }

  data::TextId Hover(int id) override;

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
