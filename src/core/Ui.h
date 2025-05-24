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
#include "../common/TextRenderer.h"
#include "../common/Colors.h"
#include "../common/Vbos.h"
#include "../common/LocalTransform.h"
#include "../io/Window.h"
#include "../renderers/water/WaterBiome.h"
#include "../modes/SharedResources.h"

//todo: relevant?
/** All components stores vbo offset and provides AABB-like functions:
 * GetLeftBorder(), GetRightBorder(), GetTopBorder(), GetBottomBorder()
 * for more effective collision detection (let's take Terrain edit mode;
 * then because we know that ui located on the left side, we can discard
 * everything on the right side before ui_component.GetRightBorder()
 * */

/// dbg serializing - stored without rotation
LocalTransformLinear GetParentDbgTransform(size_t id);

extern std::array<vbos::UiData, vbos::gVboIdSize> gUiComponents;

class UiCallable {
 public:
  using CallableType = std::function<void()>;

  UiCallable(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
         CallableType action)
      : UiCallable(vbos::GetUiData(vbo_texture, vbo_text),
                   std::move(action)) {}

  explicit UiCallable(vbos::UiData ui_data, CallableType&& action)
      : ui_data_(ui_data),
        action_(std::move(action)) {
    gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  }

  UiCallable(UiCallable&& other) noexcept = default;
  UiCallable(const UiCallable& other) = delete;

  UiCallable& operator=(UiCallable&& other) = delete;
  UiCallable& operator=(const UiCallable& other) = delete;

  /// different components have different params and shaders for Render()
  /// and RenderPicking(), therefore now we can't provide enough support
  //  virtual void Render(float value) {} // can change time (not const)
  //  virtual void RenderPicking() const {}

  /// each ui component has its functionality
  virtual void Press() {
    action_();
  }

  virtual void Release() {} // can change everything

  /// one for all
  size_t Hover() {
    return GetTextVboOffset();
  }

  [[nodiscard]] std::uint32_t GetId() const {
    return ui_data_.id;
  }

  [[nodiscard]] std::size_t GetVboOffset() const {
    return ui_data_.vbo_offset;
  }

  [[nodiscard]] std::size_t GetTextVboOffset() const {
    return ui_data_.text_vbo_offset_;
  }

  // no getter \_-_-_/
  vbos::UiData ui_data_;

 private:
  std::function<void()> action_;
};

class UiCallablePad final : public UiCallable {
 public:
  UiCallablePad() : UiCallable(
            vbos::VboIdMain::kMenuTerrain,
            vbos::VboIdText::kNone, {}) {}

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
  UiEventHandler(UiEventHandler&& other) noexcept = default;
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
  UiDynamicSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
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

  void SetTranslate(glm::vec2 translate) {
    local_transform_.translate = translate;
  }

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
};

class UiStaticSprite : public UiCallable, public UiTransformDbg {
 public:
  UiStaticSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
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

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

  void UpdateTransform() override;

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

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

  UiWindowBase(UiStaticSprite&& sprite,
               SharedResources& shared_resources);

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
  UiStaticSprite sprite_;

  const float speed_{0.5f};
  float progress_{0.0f};

  bool back_ready_{false};

  SharedResources& shared_resources_; // for shader bindings, mask texture
};

class UiTabMenu final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiTabMenu(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,

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
      static_cast<int>(vbos::VboIdMain::kMenuSettings) -
      static_cast<int>(vbos::VboIdMain::kMenuTerrain) + 1
      > ui_event_handler_;

  UiDynamicSprite cross_;
};

class UiSettings final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiSettings(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,
      UiSliderH&& resolution,
      UiSliderH&& music,
      UiSliderH&& sound,
      UiSliderH&& sensitivity,
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

  void UpdateTransform(float x_translate, float y_translate,
                       float scale) override;

 private:
  UiSliderH resolution_;
  UiSliderH music_;
  UiSliderH sound_;
  UiSliderH sensitivity_;
  UiStaticSprite keyboard_;

  UiToggle toggle_music_;
  UiToggle toggle_sound_;

  UiEventHandler<
      static_cast<int>(vbos::VboIdMain::kSettingsMusicOn3) -
      static_cast<int>(vbos::VboIdMain::kSettingsResolutionFill) + 1
      > ui_event_handler_;

  UiDynamicSprite cross_;
};

class UiConfirmation final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiConfirmation(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,

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

  UiPopUpBase(UiStaticSprite&& sprite,
              SharedResources& shared_resources,
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
  UiStaticSprite sprite_;

  // for shader bindings, mask texture
  SharedResources& shared_resources_;

 private:
  void CubicInterpolation();
  // not dynamic (transformation stored here, not in the sprite)

  // store separately for easiest interpolation
  LocalTransform start_transform_;
  LocalTransform end_transform_;

  const float speed_{0.5f};
  float progress_{0.0f};

  bool back_ready_{false};
};

class UiWaterLayerConfig final : public UiPopUpBase {
 public:
  using Base = UiPopUpBase;

  UiWaterLayerConfig(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,
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

  void RenderPicking();

  void Press(int id) {
    modified_ = ui_event_handler_.Press(id);
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

class UiSlots {};

/*
/// no shaders managing
class UiSlot {
 public:
  void Render() {
    slot_.Render();
    selected_.Render();
    remove_.Render();
  }
  void RenderPicking() {
    slot_.RenderPicking();
    selected_.RenderPicking();
    remove_.RenderPicking();
  }

  void Update(LocalTransform transform) {
    slot_.SetParentTransform(transform_);
    selected_.SetParentTransform(transform_);
    remove_.SetParentTransform(transform_);
  }

 private:
  UiDynamicSprite slot_;
  UiDynamicSprite selected_;
  UiDynamicSprite remove_;
  LocalTransform transform_;
  //TODO: each one should have a name or picture, etc...
};


// UiSlots transform -> UiSlot transform -> Sprite transform (in-slot)
//TODO: static sprites + reusing amond Modes
class UiSlots final : public UiTransformDbg, public UiCallable {
 public:
  UiSlots(SharedResources& shared_resources, // TODO:
          float scale = 1.0f);

  UiSlots(UiSlots&& other) noexcept;
  UiSlots(const UiSlots& other) = delete;

  UiSlots& operator=(UiSlots&& other) = delete;
  UiSlots& operator=(const UiSlots& other) = delete;

  void Set(glm::vec2 cursor_pos) {
    //...
    LocalTransform transform;
    float prev_y = ;
    glm::vec2 local_translate = glm::vec2{1.0f};
    for (auto& slot : slots_) {
      local_translate = prev_y - slot_height;
      slot.Update();
      prev_y -= slot_height;
      // set parent transform
    }
  }

  void Render() {
    /// --- render static part ---
    shared_resources_.static_sprite_shader_.Bind();
    back_.Render();
    create_.Render();
    flip_select_edit_back_.Render();
    flip_point_edge_back_.Render();

    //rotated (scale -> -1)
    flip_select_edit_.Render();
    flip_point_edge_.Render();

    /// --- render dynamic part ---
    shared_resources_.dynamic_sprite_shader_.Bind();
    handle_.Render(); // slide
    flip_point_edge_.Render(); // rotate
    flip_select_edit_.Render(); // rotate

    /// --- render slots ---
    for (auto& slot : slots_) {
      slot.Render();
    }
  }

 private:
  UiStaticSprite back_;
  UiStaticSprite create_;
  UiStaticSprite flip_select_edit_back_;
  UiStaticSprite flip_point_edge_back_;

  UiDynamicSprite handle_;
  UiDynamicSprite flip_select_edit_; // just scale to -1 to flip vertically (and hor?)
  UiDynamicSprite flip_point_edge_;

  glm::mat3 transform_;

  //TODO:

  float progress_{0.0f};

  /// we starts with 5 slots, so nothing scrolls;
  /// therefore the centre is the middle of the track (0.5f out of [0.0f;1.0f]);
  /// therefore the offset is 0.5f on two sides
  float slots_progress_{0.5f}; // the centre, so we don't reorder the slots
  float slots_safe_offset{0.5f}; // how much we can deviate

  /// max 6 on the screen, but 7 for convenience (spare top & bottom)
  std::array<UiSlot, 7> slots_;



  UiEventHandler<
      static_cast<int>(vbos::VboIdMain::kSettingsMusicOn3) -
      static_cast<int>(vbos::VboIdMain::kSettingsResolutionFill) + 1
      > ui_event_handler_;
  SharedResources& shared_resources_; // for shader bindings
};*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
