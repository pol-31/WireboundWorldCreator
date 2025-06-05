#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <functional>
#include <tuple>
#include <vector>
#include <memory>
#include <limits>

#ifndef NDEBUG
#include <set>
#endif

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/Shader.h"
#include "../common/Colors.h"
#include "../common/Vbos.h"
#include "../common/LocalTransform.h"
#include "../io/Window.h"
#include "../renderers/water/WaterBiome.h"
#include "../modes/UiSharedResources.h"

#include "../common/ArbitraryGraph.h"

#include "../common/Text.h"

/// dbg serializing - stored without rotation
LocalTransformLinear GetParentDbgTransform(size_t id);

extern std::array<data::UiData, data::gVboIdSize> gUiComponents;

class UiCallable {
 public:
  using CallableType = std::function<void()>;

  UiCallable(data::VboIdMain vbo_texture, data::TextId text_id,
             CallableType action);

  UiCallable(size_t ui_data_id, CallableType&& action);

  UiCallable(UiCallable&& other) noexcept = default;
  UiCallable(const UiCallable& other) = delete;

  UiCallable& operator=(UiCallable&& other) = delete;
  UiCallable& operator=(const UiCallable& other) = delete;

  /// different components have different params and shaders for Render()
  /// and RenderPicking(), therefore now we can't provide enough support
  //  virtual void Render(float value) {} // can change time (not const)
  //  virtual void RenderPicking() const {}

  /// each ui component has its functionality
  virtual void Press();

  virtual void Release();

  virtual bool Scroll(GLuint id, float yoffset);

  /// non-virtual - one for all
  virtual size_t Hover();

  [[nodiscard]] std::uint32_t GetId() const;

  [[nodiscard]] std::size_t GetVboOffset() const;

  [[nodiscard]] std::size_t GetTextId() const;

 private:
  UiCallable(data::UiData ui_data, CallableType&& action);

  size_t ui_data_id_;
  std::function<void()> action_;
};

class UiCallablePad final : public UiCallable {
 public:
  UiCallablePad() : UiCallable(data::VboIdMain::kMenuTerrain, data::TextId::kNone, {}) {}

  void Press() override {
    std::cerr << "Pad was called, smt went wrong" << std::endl;
  }
};

template<std::size_t MaxSize>
class UiEventHandler {
 public:
  UiEventHandler(std::initializer_list<UiCallable*> widgets) {
    Init(widgets);
  }

  /// according to UiCallable as a member
  UiEventHandler(UiEventHandler&& other) = delete;
  UiEventHandler(const UiEventHandler& other) = delete;

  UiEventHandler& operator=(UiEventHandler&& other) = delete;
  UiEventHandler& operator=(const UiEventHandler& other) = delete;

  bool Press(int id) {
    if (id < start_ || id > end_) {
      return false;
    }
    widgets_[id - start_]->Press();
    return true;
  }

  void Release() {
    for (auto widget : widgets_) {
      widget->Release();
    }
  }

  size_t Hover(int id) {
    if (id < start_ || id > end_) {
      return static_cast<size_t>(data::TextId::kNone);
    }
    return widgets_[id - start_]->Hover();
  }


 private:
  void Init(std::initializer_list<UiCallable*> widgets) {
    start_ = std::numeric_limits<int>::max();
    end_ = std::numeric_limits<int>::min();

    for (UiCallable* widget : widgets) {
      int id = widget->GetId();
      start_ = std::min(start_, id);
      end_ = std::max(end_, id);
    }

    // Bounds check
    if ((end_ - start_ + 1) > MaxSize) {
      throw std::runtime_error("EventHandler array size exceeded MaxSize");
    }

    widgets_.fill(&ui_callable_pad_);

    for (UiCallable* widget : widgets) {
      int index = widget->GetId() - start_;
#ifndef NDEBUG
      // if exception is here - indices aren't conseq... maybe
      widgets_.at(index) = widget;
#else
      widgets_.[index] = widget;
#endif
    }
  }

  int start_ = 0;
  int end_ = 0;
  std::array<UiCallable*, MaxSize> widgets_;
  inline static UiCallablePad ui_callable_pad_{};
};

/// interface, all ctors default
class UiTransformDbg {
 public:
  virtual void UpdateTransform(
      float x_translate, float y_translate, float scale) = 0;
  virtual void UpdateTransform() = 0;
};

class UiDynamicSprite : public UiCallable, public UiTransformDbg {
 public:
  UiDynamicSprite(data::VboIdMain vbo_texture, data::TextId text_id,
                  CallableType action = {});

  UiDynamicSprite(UiDynamicSprite&& other) noexcept;
  UiDynamicSprite(const UiDynamicSprite& other) = delete;

  UiDynamicSprite& operator=(UiDynamicSprite&& other) noexcept = default;
  UiDynamicSprite& operator=(const UiDynamicSprite& other) = delete;

  void Render();

  void Rotate(float radians) {
    local_transform_.rotate += radians;
  }

  void Scale(float scale) {
    local_transform_.scale *= scale;
  }

  void Translate(glm::vec2 translate) {
    local_transform_.translate += translate;
  }

  void SetRotate(float radians) {
    local_transform_.rotate = radians;
  }

  void SetScale(float scale) {
    local_transform_.scale = scale;
  }

  void SetExtraScale(float scale) {
    extra_scale_ = scale;
  }

  void SetTranslate(glm::vec2 translate) {
    local_transform_.translate = translate;
  }

  /// useful for transform-animated parent ui components
  void SetParentTransform(LocalTransform transform) {
    parent_transform_ = transform;
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void UpdateTransform() override;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

 private:
  // used for slider handlers, stored separately from gUiTransform & gUiComponents
  /// dynamic, so do has rotations
  LocalTransform local_transform_;

  // to update only once at UpdateTransform() - not every time at Render()
  /// filled with SetParentTransform, includes rotation
  LocalTransform parent_transform_;

  /// collected from gUiTransform & gUiComponents, so dbg - no rotation
  LocalTransformLinear final_dbg_transform_;

  // TODO: affects only x or y, kostyl
  float extra_scale_{1.0f};
};

class UiStaticSprite : public UiCallable, public UiTransformDbg {
 public:
  UiStaticSprite(data::VboIdMain vbo_texture, data::TextId text_id,
                 CallableType action = {});

  UiStaticSprite(UiStaticSprite&& other) noexcept;
  UiStaticSprite(const UiStaticSprite& other) = delete;

  UiStaticSprite& operator=(UiStaticSprite&& other) noexcept = default;
  UiStaticSprite& operator=(const UiStaticSprite& other) = delete;

  void Render();

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void UpdateTransform() override;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

 private:
  glm::mat3 final_transform_;
};

class UiSpriteTransformation {
 public:
  UiSpriteTransformation(UiDynamicSprite& sprite,
                         LocalTransform start, LocalTransform end);

  void Render();

  void RenderPicking();

  void RunAnimation(bool looping = false);

  void StopAnimation();

  [[nodiscard]] LocalTransform GetStart() const noexcept {
    return start_;
  }

  [[nodiscard]] LocalTransform GetEnd() const noexcept {
    return end_;
  }

  [[nodiscard]] LocalTransform GetCur() const noexcept {
    return cur_;
  }

  void SetStart(LocalTransform transform) {
    start_ = transform;
  }

  void SetEnd(LocalTransform transform) {
    end_ = transform;
  }

  void SetCur(LocalTransform transform) {
    cur_ = transform;
  }

  // manual looping
  void SwapStartEnd();

 private:
  void CubicInterpolation();

  void UpdateAnimation();

  // take by ref to avoid UpdateTransform / parent dependency issues
  UiDynamicSprite& sprite_;
  LocalTransform start_;
  LocalTransform end_;
  LocalTransform cur_;

  const float speed_;
  float progress_{0.0f};
  bool looping_{false};
};

class UiSliderV final : public UiTransformDbg, public UiCallable {
 public:
  UiSliderV(UiStaticSprite&& fill_sprite,
           UiStaticSprite&& back_sprite,
           UiDynamicSprite&& icon_sprite,
            float scale = 1.0f);

  UiSliderV(UiSliderV&& other) noexcept;
  UiSliderV(const UiSliderV& other) = delete;

  UiSliderV& operator=(UiSliderV&& other) = delete;
  UiSliderV& operator=(const UiSliderV& other) = delete;

  void Render(glm::vec2 mouse_pos);
  void RenderIcon();

  void RenderPicking() const;

  [[nodiscard]] size_t Hover(std::uint32_t id);

  void Press() override {
    pressed_ = true;
  }

  void Release() override {
    pressed_ = false;
  }

  bool Scroll(GLuint id, float yoffset) override;

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  void UnHover();

  UiStaticSprite fill_sprite_;
  UiStaticSprite back_sprite_;
  UiDynamicSprite icon_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH final : public UiTransformDbg, public UiCallable {
 public:
  UiSliderH(UiDynamicSprite&& fill_sprite,
            UiDynamicSprite&& back_sprite,
            UiDynamicSprite&& handler_sprite,
            UiDynamicSprite&& icon_sprite,
            float scale = 1.0f);

  UiSliderH(UiSliderH&& other) noexcept;
  UiSliderH(const UiSliderH& other) = delete;

  UiSliderH& operator=(UiSliderH&& other) = delete;
  UiSliderH& operator=(const UiSliderH& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderIcon();

  void RenderPicking() const;

  [[nodiscard]] size_t Hover(std::uint32_t id);

  void Press() override {
    pressed_ = true;
  }

  void Release() override {
    pressed_ = false;
  }

  bool Scroll(GLuint id, float yoffset) override;

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return fill_sprite_.GetId();
  }

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  void UnHover();

  /// we want to use UiDynamicSprite only for handler_sprite_,
  /// BUT to use it in complex hierarhies we need all to be UiDynamicSprite
//  UiStaticSprite fill_sprite_;
//  UiStaticSprite back_sprite_;
//  UiStaticSprite icon_sprite_;
//  UiDynamicSprite handler_sprite_;

  UiDynamicSprite fill_sprite_;
  UiDynamicSprite back_sprite_;
  UiDynamicSprite icon_sprite_;
  UiDynamicSprite handler_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH3 final : public UiTransformDbg, public UiCallable {
 public:
  UiSliderH3(UiDynamicSprite&& fill_sprite,
             UiDynamicSprite&& back_sprite,
             UiDynamicSprite&& icon_sprite,
            float scale = 1.0f);

  UiSliderH3(UiSliderH3&& other) noexcept;
  UiSliderH3(const UiSliderH3& other) = delete;

  UiSliderH3& operator=(UiSliderH3&& other) = delete;
  UiSliderH3& operator=(const UiSliderH3& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderIcon();

  void RenderPicking() const;

  [[nodiscard]] size_t Hover(std::uint32_t id);

  void Press() override {
    pressed_ = true;
  }

  void Release() override {
    pressed_ = false;
  }

  bool Scroll(GLuint id, float yoffset) override;

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return fill_sprite_.GetId();
  }

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  void UnHover();

  /// we want to use UiDynamicSprite only for handler_sprite_,
  /// BUT to use it in complex hierarhies we need all to be UiDynamicSprite
  //  UiStaticSprite fill_sprite_;
  //  UiStaticSprite back_sprite_;
  //  UiStaticSprite icon_sprite_;
  //  UiDynamicSprite handler_sprite_;

  UiDynamicSprite fill_sprite_;
  UiDynamicSprite back_sprite_;
  UiDynamicSprite icon_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

/// You should pass action to off_ sprite (see Press());
class UiToggle final : public UiTransformDbg, public UiCallable {
 public:
  UiToggle(UiStaticSprite&& off, UiStaticSprite&& on1,
           UiStaticSprite&& on2, UiStaticSprite&& on3);

  UiToggle(UiToggle&& other) noexcept;
  UiToggle(const UiToggle& other) = delete;

  UiToggle& operator=(UiToggle&& other) = delete;
  UiToggle& operator=(const UiToggle& other) = delete;

  void Render();

  void RenderPicking() const;

  void Press() override;

  size_t Hover(std::uint32_t id) {
    return off_.Hover();
  }

  void UpdateTransform(
      float x_translate, float y_translate, float scale) override;

  void UpdateTransform() override;

  void UpdateState();

 private:
  UiStaticSprite off_;
  UiStaticSprite on1_;
  UiStaticSprite on2_;
  UiStaticSprite on3_;

  UiStaticSprite* state_{nullptr};

  bool turned_off_{true};

  float speed_{0.2f};
  float progress_{0.0f};
};

/// loading only on the bottom of the screen (so use UiStaticSprite)
class UiLoading final : public UiTransformDbg, public UiCallable {
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

class UiWindowBase : public UiTransformDbg, public UiCallable {
 public:
  using UiTransformDbg::UpdateTransform;

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

  size_t Hover(int id) {
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

  size_t Hover(int id) {
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

  size_t Hover(int id) {
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
class UiPopUpBase : public UiTransformDbg, public UiCallable {
 public:
  using UiTransformDbg::UpdateTransform;

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
  bool Render(bool show);

  void Release();

  bool Scroll(GLuint id, float yoffset) override;

  void RenderPicking();

  void Press(int id) {
    modified_ = ui_event_handler_.Press(id);
  }

  size_t Hover(int id) {
    return ui_event_handler_.Hover(id);
  }

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

  UiEventHandler<28> ui_event_handler_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
