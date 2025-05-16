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

class UiTransformDbg {
 public:
  virtual void UpdateTransform(
      float x_translate, float y_translate, float scale) = 0;
};

class UiDynamicSprite : public UiCallable {
 public:
  UiDynamicSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
                  CallableType action = {})
      : UiCallable(vbo_texture, vbo_text, std::move(action)) {}

  void Render();

  void Rotate(float radians) {
    transform_ = glm::rotate(transform_, radians);
  }

  void Scale(glm::vec2 scale) {
    transform_ = glm::scale(transform_, scale);
  }

  void Translate(glm::vec2 position) {
    transform_ = glm::translate(transform_, position);
  }

  void SetTransform(glm::mat3 transform) {
    transform_ = transform;
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

 private:
  glm::mat3 transform_{1.0f};
};

class UiStaticSprite : public UiCallable {
 public:
  UiStaticSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text,
                 CallableType action = {})
      : UiCallable(vbo_texture, vbo_text, std::move(action)) {}

  void Render();

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;
};

class UiSliderV final : public UiTransformDbg, public UiCallable {
 public:
  UiSliderV(UiStaticSprite&& fill_sprite,
           UiStaticSprite&& back_sprite,
           UiDynamicSprite&& icon_sprite,
            float scale = 1.0f);

  void Render(glm::vec2 mouse_pos);
  void RenderIcon();

  void RenderPicking() const;

  [[nodiscard]] size_t Hover(std::uint32_t id);

  void Press() {
    pressed_ = true;
  }

  void Release() {
    pressed_ = false;
  }

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return fill_sprite_.GetId();
  }

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate, float scale);

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
};

class UiSliderH final : public UiTransformDbg, public UiCallable {
 public:
  UiSliderH(UiStaticSprite&& fill_sprite,
            UiStaticSprite&& back_sprite,
            UiDynamicSprite&& handler_sprite,
            UiStaticSprite&& icon_sprite,
            float scale = 1.0f);

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

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate, float scale);

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void UnHover();

  UiStaticSprite fill_sprite_;
  UiStaticSprite back_sprite_;
  UiStaticSprite icon_sprite_;
  UiDynamicSprite handler_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
};

// UiWIndowBACK

class UiPopUpBase {
 public:
  UiPopUpBase(UiStaticSprite&& sprite,
              SharedResources& shared_resources,
              glm::vec2 start_translate,
              glm::vec2 end_translate,
              glm::vec2 start_scale,
              glm::vec2 end_scale,
              float start_angle,
              float end_angle);

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();

  [[nodiscard]] bool BackIsReady() const {
    return back_ready_;
  }

 protected:
  // cubic interpolation here (not shader)
  glm::mat3 cur_transform_{1.0f};

 private:
  void CubicInterpolation();
  // not dynamic (transformation stored here, not in the sprite)
  UiStaticSprite sprite_;

  // store separately for easiest interpolation
  const glm::vec2 start_translate_;
  const glm::vec2 end_translate_;
  const glm::vec2 start_scale_;
  const glm::vec2 end_scale_;
  const float start_angle_;
  const float end_angle_;

  const float speed_{0.5f};
  float progress_{0.0f};

  bool back_ready_{false};

  SharedResources& shared_resources_; // for shader bindings, mask texture
};

class UiWaterLayerConfig final : public UiPopUpBase {
  using Base = UiPopUpBase;

  UiWaterLayerConfig(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,
      glm::vec2 start_translate,
      glm::vec2 end_translate,
      glm::vec2 start_scale,
      glm::vec2 end_scale,
      float start_angle,
      float end_angle,
      UiStaticSprite&& sprite_layer_,
      UiSliderH&& scale,
      UiSliderH&& fetch,
      UiSliderH&& spread_blend,
      UiSliderH&& swell,
      UiSliderH&& peak_enhancement,
      UiSliderH&& short_waves_fade,
      UiSliderH&& lambda);

  // returns "stop render"
  bool Render(bool show);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

 private:
  UiStaticSprite sprite_layer_;
  UiSliderH scale_;
  UiSliderH fetch_;
  UiSliderH spread_blend_;
  UiSliderH swell_;
  UiSliderH peak_enhancement_;
  UiSliderH short_waves_fade_;
  UiSliderH lambda_;

  // 1 button + 7 hor sliders (x4)
  UiEventHandler<29> ui_event_handler_;

  SharedResources& shared_resources_;
};

class UiToggle : public UiCallable {
 public:
  UiToggle(UiStaticSprite&& off, UiStaticSprite&& on1,
           UiStaticSprite&& on2, UiStaticSprite&& on3);

  void Render();

  void RenderPicking() const;

  void Press() override;

 private:
  UiStaticSprite off_;
  UiStaticSprite on1_;
  UiStaticSprite on2_;
  UiStaticSprite on3_;

  UiStaticSprite* state_{nullptr};

  bool turned_off_{true};

  const float speed_{0.2f};
  float progress_{0.0f};
};

/// loading only on the bottom of the screen (so use UiStaticSprite)
class UiLoading {
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

  void Render(float progress);

  void RenderPicking() const;

 private:
  std::array<UiStaticSprite, 11> sprites_;
};

class UiOceanCascadeConfig {
 public:
  UiOceanCascadeConfig(
      UiStaticSprite&& scale_fill, UiStaticSprite&& scale_back,
      UiDynamicSprite&& scale_handler, UiStaticSprite&& scale_icon,
      float scale_scale,
      UiStaticSprite&& fetch_fill, UiStaticSprite&& fetch_back,
      UiDynamicSprite&& fetch_handler, UiStaticSprite&& fetch_icon,
      float fetch_scale,
      UiStaticSprite&& spread_blend_fill, UiStaticSprite&& spread_blend_back,
      UiDynamicSprite&& spread_blend_handler, UiStaticSprite&& spread_blend_icon,
      float spread_blend_scale,
      UiStaticSprite&& swell_fill, UiStaticSprite&& swell_back,
      UiDynamicSprite&& swell_handler, UiStaticSprite&& swell_icon,
      float swell_scale,
      UiStaticSprite&& peaks_fill, UiStaticSprite&& peaks_back,
      UiDynamicSprite&& peaks_handler, UiStaticSprite&& peaks_icon,
      float peaks_scale,
      UiStaticSprite&& fade_fill, UiStaticSprite&& fade_back,
      UiDynamicSprite&& fade_handler, UiStaticSprite&& fade_icon,
      float fade_scale,
      UiStaticSprite&& lambda_fill, UiStaticSprite&& lambda_back,
      UiDynamicSprite&& lambda_handler, UiStaticSprite&& lambda_icon,
      float lambda_scale);

  //  void Update(); // update ubo not here

  OceanLayerTraits GetOceanLayerTraits() const;

  bool Modified();

  bool Press(int id);

  bool CheckId(int id);

  size_t Hover(int id);

  void Release();

  void Render(glm::vec2 mouse_pos);
  void RenderIcons();

  void RenderPicking() const;

 private:
  bool modified_{false};

  UiSliderH scale_;
  UiSliderH fetch_;
  UiSliderH spread_blend_;
  UiSliderH swell_;
  UiSliderH peak_enhancement_;
  UiSliderH short_waves_fade_;
  UiSliderH lambda_;
};

// --- complex (separate header?) ---
// --- complex (separate header?) ---
// --- complex (separate header?) ---

class UiWindowBase {
 public:
  UiWindowBase(UiStaticSprite&& sprite,
               SharedResources& shared_resources);

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();

 public:
  [[nodiscard]] bool BackIsReady() const {
    return back_ready_;
  }

 private:
  UiStaticSprite sprite_;

  const float speed_{0.5f};
  float progress_{0.0f};

  bool back_ready_{false};

  SharedResources& shared_resources_; // for shader bindings, mask texture
};

class UiSettings final : public UiWindowBase {
 public:
  using Base = UiWindowBase;

  UiSettings(
      UiStaticSprite&& sprite,
      SharedResources& shared_resources,
      UiStaticSprite&& resolution,
      UiStaticSprite&& music,
      UiStaticSprite&& sound,
      UiStaticSprite&& sensitivity,
      UiStaticSprite&& keyboard,
      UiToggle&& toggle_music,
      UiToggle&& toggle_sound,
      UiToggle&& toggle_3,
      UiToggle&& toggle_4,
      UiDynamicSprite&& cross);

  // returns "stop render"
  bool Render(bool show);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

 private:
  //TODO: UiSliderH
  UiStaticSprite resolution_;
  UiStaticSprite music_;
  UiStaticSprite sound_;
  UiStaticSprite sensitivity_;
  UiStaticSprite keyboard_;

  UiToggle toggle_music_;
  UiToggle toggle_sound_;
  UiToggle toggle_3_;
  UiToggle toggle_4_;

  // 5 buttons + 4 toggles (x4)
  UiEventHandler<21> ui_event_handler_;

  UiDynamicSprite cross_;

  SharedResources& shared_resources_; // for shader bindings
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

      UiStaticSprite&& btn_vision_terrain,
      UiStaticSprite&& btn_vision_water,
      UiStaticSprite&& btn_vision_roads,
      UiStaticSprite&& btn_vision_fences,
      UiStaticSprite&& btn_vision_placement,
      UiStaticSprite&& btn_vision_objects,
      UiStaticSprite&& btn_vision_biomes,
      UiStaticSprite&& btn_vision_tiles,

      UiStaticSprite&& btn_settings,
      UiStaticSprite&& btn_shader_wirebound,

      UiDynamicSprite&& cross);

  // returns "stop render"
  bool Render(bool show);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

 private:
  UiStaticSprite btn_mode_terrain_;
  UiStaticSprite btn_mode_water_;
  UiStaticSprite btn_mode_roads_;
  UiStaticSprite btn_mode_fences_;
  UiStaticSprite btn_mode_placement_;
  UiStaticSprite btn_mode_objects_;
  UiStaticSprite btn_mode_biomes_;
  UiStaticSprite btn_mode_tiles_;

  UiStaticSprite btn_vision_terrain_;
  UiStaticSprite btn_vision_water_;
  UiStaticSprite btn_vision_roads_;
  UiStaticSprite btn_vision_fences_;
  UiStaticSprite btn_vision_placement_;
  UiStaticSprite btn_vision_objects_;
  UiStaticSprite btn_vision_biomes_;
  UiStaticSprite btn_vision_tiles_;

  UiStaticSprite btn_settings_;
  UiStaticSprite btn_shader_wirebound_;

  // 18 buttons
  UiEventHandler<18> ui_event_handler_;

  UiDynamicSprite cross_;

  SharedResources& shared_resources_; // for shader bindings
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

  // returns "stop render"
  bool Render(bool show);

  void RenderPicking();

  void Press(int id) {
    ui_event_handler_.Press(id);
  }

 private:
  UiStaticSprite btn_close_;
  UiStaticSprite btn_accept_;
  UiStaticSprite btn_decline_;

  // 3 buttons
  UiEventHandler<3> ui_event_handler_;

  UiDynamicSprite cross_;

  SharedResources& shared_resources_; // for shader bindings
};


//TODO: WIP, no ideas
//  --- --- --- ---- --- ---- --- --- --- -- -- --
//  --- --- --- ---- --- ---- --- --- --- -- -- --
//  --- --- --- ---- --- ---- --- --- --- -- -- --
//  --- --- --- ---- --- ---- --- --- --- -- -- --
//  --- --- --- ---- --- ---- --- --- --- -- -- --
//  --- --- --- ---- --- ---- --- --- --- -- -- --

//TODO: UiSlots, UiBiomes, UiTiles, UiObjects - all drawn as:
// - static background texture;
// - N x M equally alligned clickable sprites
// so we have std::array<InstanceData, n * m>,
// - single vbo_offset

//TODO: let's not draw sprites of objects, but only text name
// (we can also add search bar).
// So then we have uniform type for these four:
// 1 sprite for background; n sprites depends on mode (config, remove, rename),
// what about Render() and collision detection?

/// to check was it pressed see UiSlots::Press(id)
class UiSlots {
 public:
  UiSlots(const std::size_t& total_size, UiStaticSprite&& btn_next,
          UiStaticSprite&& btn_prev, UiStaticSprite&& slot1,
          UiStaticSprite&& slot2, UiStaticSprite&& slot3,
          UiStaticSprite&& slot4, UiStaticSprite&& slot5,
          int& edit_mode_selected_sample_id,
          const TextRenderer& text_renderer);

  void Render();

  void RenderPicking() const;

  void RenderIds() const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] size_t Hover(std::uint32_t id);

 private:
  void InitTextIdsPositions();

  // 5 x slots, btn_next, btn_prev
  std::array<UiStaticSprite, 7> slots_;
  std::array<glm::vec2, 5> text_ids_positions_; // TODO: init

  // we don't own point/graph data and all what we need is size (dynamically)
  const std::size_t& total_size_;
  int start_idx_{0}; // from where in view to start
  int& edit_mode_selected_sample_id_;

  const TextRenderer& text_renderer_;
};

/*class UiBiomesList {
 public:
  UiTable(UiDynamicSprite&& background, glm::vec2 position)
      : background_(background),
        position_(position) {
    float width = vbos::GetElementWidth(background_.GetVboOffset());
    float height = vbos::GetElementHeight(background_.GetVboOffset());
    offset_ = {width / (columns + 1), height / (rows + 1)};
  }

  // we can't use instanced draw call, because anyway we need set ids to
  // picking framebuffer, what will make drawing confusing and non-uniform

  //TODO; so let's just draw single texture with those boxes instead of 36 boxes,
  // while __clickable__ objects draw here also separately;
  // THEN we just set differet sprites if btn_next or btn_prev were pressed
  std::array<UiStaticSprite, 36> biomes_;
  UiStaticSprite background_;

  //TODO: we need vbo with tex coords only like Text.glsl

  void Render() {
    glUniform2fv(0, 1, glm::value_ptr(position_));
    background_;
    //bind
  }

 private:
  std::array<UiDynamicSprite, rows * columns> elements_;
  UiDynamicSprite background_;
  const glm::vec2 position_;
  // offset before first; padding between elements is the same
  glm::vec2 offset_;
};

// !!!
//TODO: you should remember that id is related to Vbos.h,
// not order of creation

//TODO: their difference in paging and num


/// UiBiomesList, UiObjectTable, UiTilesMap all use instanced vbo
/// from stc/vbos/UiDataInstanced.h

/// to check was it pressed see UiSlots::Press(id)
class UiBiomesList {
 public:
  // vbo_offset for instanced
  UiBiomesList(int vbo_offset, int instances_num, UiStaticSprite&& btn_next,
               UiStaticSprite&& btn_prev) {
    Init();
  }

  void Render() const {
    glBindVertexArray(vao_);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, number);
  }

  void RenderPicking() const {
    glBindVertexArray(vao_);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, number);
  }

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  int Hover(std::uint32_t id) const;

 private:


  void Init() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, ,);
    glVertexAttribDivisor(0, 0);
    glEnableVertexArrayAttrib(0);
    // tex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, ,);
    glVertexAttribDivisor(1, 1);
    glEnableVertexArrayAttrib(1);
  }

  GLuint vao_;

  int vbo_offset_;
  int instances_num_;

  UiStaticSprite btn_next_;
  UiStaticSprite btn_prev_;

  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8 (8 biomes at once)
};

class UiObjectTable {
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 8x4 (if we have n=8x4 buttons)
};
class UiTilesMap {
  int cur_page_offset_{-1}; // when scrolling pages
  int selected_id_{-1}; // from 0 to 6x6 (if we have n=6x6 tiles at once)
};*/

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
