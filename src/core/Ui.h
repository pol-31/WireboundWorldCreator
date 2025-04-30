#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <vector>
#include <memory>
#include <limits>

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

// TODO: we can accelerate our search by binary search, but
//  looks like the best way to do this is "in-place" binary search and
//  direct if-else branching

/** All components stores vbo offset and provides AABB-like functions:
 * GetLeftBorder(), GetRightBorder(), GetTopBorder(), GetBottomBorder()
 * for more effective collision detection (let's take Terrain edit mode;
 * then because we know that ui located on the left side, we can discard
 * everything on the right side before ui_component.GetRightBorder()
 * */

/// if function doesn't have shader in parameters,
/// you should bind it before the call

// details::kUiVboDataMain store data both for Dynamic and Static sprites
// in the same format, the only difference is how class initialize its data

// Let's draw the entire ui component with the same shader,
// so if there's dynamic component - we draw with dynamic shader only
// to reduce switching)

class UiTransformDbg {
 public:
  virtual void UpdateTransform(
      float x_translate, float y_translate, float scale) = 0;
};

extern std::array<vbos::UiData, vbos::gVboIdSize> gUiComponents;

// <--------- NOT IN USE BY NOW
class UiDynamicSprite {
 public:
  UiDynamicSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text)
      : UiDynamicSprite(vbos::GetUiData(vbo_texture, vbo_text)) {}

  void Render() const;

  [[nodiscard]] size_t Hover() const {
    return GetTextVboOffset();
  }

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

  [[nodiscard]] std::uint32_t GetId() const {
    return ui_data_.id;
  }

  [[nodiscard]] std::size_t GetVboOffset() const {
    return ui_data_.vbo_offset;
  }

  [[nodiscard]] std::size_t GetTextVboOffset() const {
    return ui_data_.text_vbo_offset_;
  }

  vbos::UiData ui_data_;

 private:
  explicit UiDynamicSprite(vbos::UiData ui_data)
      : ui_data_(ui_data) {
    gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  }

  //TODO: we need only tex coords (?)
  glm::mat3 transform_{1.0f};
};

class UiStaticSprite {
 public:
  UiStaticSprite(vbos::VboIdMain vbo_texture, vbos::VboIdText vbo_text)
      : UiStaticSprite(vbos::GetUiData(vbo_texture, vbo_text)) {}

  void Render() const;

  [[nodiscard]] size_t Hover() const {
    return GetTextVboOffset();
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

  [[nodiscard]] std::uint32_t GetId() const {
    return ui_data_.id;
  }

  [[nodiscard]] std::size_t GetVboOffset() const {
    return ui_data_.vbo_offset;
  }

  [[nodiscard]] std::size_t GetTextVboOffset() const {
    return ui_data_.text_vbo_offset_;
  }

  vbos::UiData ui_data_;

 private:
  explicit UiStaticSprite(vbos::UiData ui_data)
      : ui_data_(ui_data) {
    gUiComponents[ui_data_.id - details::kIdOffsetUi] = ui_data_;
  }
};

class UiSlider final : public UiTransformDbg {
 public:
  UiSlider(UiStaticSprite&& fill_icon_sprite,
           UiStaticSprite&& background_sprite,
           float scale = 1.0f);

  void Render(float related_pos);

  void RenderPicking();

  [[nodiscard]] size_t Hover(std::uint32_t id) const;

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
    return fill_icon_sprite_.GetId();
  }

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform(float x_translate, float y_translate, float scale);

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(float related_pos);

  void UnHover();

  UiStaticSprite background_sprite_;
  UiStaticSprite fill_icon_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
};

class UiOceanCascadeConfig {
 public:
  UiOceanCascadeConfig(
      UiStaticSprite&& scale_fill, UiStaticSprite&& scale_wheel, float scale_scale,
      UiStaticSprite&& fetch_fill, UiStaticSprite&& fetch_wheel, float fetch_scale,
      UiStaticSprite&& spread_blend_fill, UiStaticSprite&& spread_blend_wheel, float spread_blend_scale,
      UiStaticSprite&& swell_fill, UiStaticSprite&& swell_wheel, float swell_scale,
      UiStaticSprite&& peaks_fill, UiStaticSprite&& peaks_wheel, float peaks_scale,
      UiStaticSprite&& fade_fill, UiStaticSprite&& fade_wheel, float fade_scale,
      UiStaticSprite&& lambda_fill, UiStaticSprite&& lambda_wheel, float lambda_scale);

  //  void Update(); // update ubo not here

  OceanLayerTraits GetOceanLayerTraits() const;

  bool Modified();

  bool Press(int id);

  bool CheckId(int id);

  size_t Hover(int id);

  void Release();

  void Render(float related_pos);

  void RenderPicking();

 private:
  bool modified_{false};

  UiSlider scale_;
  UiSlider fetch_;
  UiSlider spread_blend_;
  UiSlider swell_;
  UiSlider peak_enhancement_;
  UiSlider short_waves_fade_;
  UiSlider lambda_;
};

/// position lerp based on time
class UiTabAnimation {
 public:
  UiTabAnimation(UiDynamicSprite&& sprite, glm::vec2 start_pos,
                 glm::vec2 end_pos, float speed = 1.0f);

  void Render();

  void RenderPicking() {
    sprite_.RenderPicking();
  }

  [[nodiscard]] size_t Hover() {
    return sprite_.Hover();
  }

  [[nodiscard]] std::uint32_t GetId() const {
    return sprite_.GetId();
  }

  void Start();

  void End();

 private:
  UiDynamicSprite sprite_;
  glm::vec2 start_pos_;
  glm::vec2 end_pos_;

  // 0-1 for progress; 1 means either "not active" or end_pos_
  float progress_{1.0f};
  float speed_;
};

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

  void Render() const;

  void RenderPicking() const;

  void RenderIds() const;

  /// return true if there was a button with such id
  bool Press(std::uint32_t id);

  [[nodiscard]] size_t Hover(std::uint32_t id) const;

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

//TODO; we need different type of UiSprite: with vbo offset only for texture,
//  but we still can use the same UiStaticSprite, but we don't need description

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
