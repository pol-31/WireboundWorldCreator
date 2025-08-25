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

//TODO: callable, text_id... under the question

class UiBase {
 public:
  using CallableType = std::function<void()>;

  UiBase(data::VboIdMain vbo_texture, CallableType action);

  UiBase(size_t ui_data_id, CallableType&& action);



  ///
  UiBase(UiBase* other) noexcept {
    ui_data_id_ = other->ui_data_id_;
  }
  /// \param other

  UiBase(UiBase&& other) noexcept = default;
  UiBase(const UiBase& other) = default;

  UiBase& operator=(UiBase&& other) = default;
  UiBase& operator=(const UiBase& other) = default;

  /// different components have different params and shaders for Render()
  /// and RenderPicking(), therefore now we can't provide enough support
  //  virtual void Render(float value) {} // can change time (not const)
  //  virtual void RenderPicking() const {}

  /// each ui component has its functionality
  virtual void Press();

  virtual void Release();

  virtual bool Scroll(GLuint id, float yoffset);

  virtual void UpdateTransform() = 0;

  [[nodiscard]] std::uint32_t GetId() const;

  [[nodiscard]] std::size_t GetVboOffset() const;

 private:
  UiBase(data::UiData ui_data, CallableType&& action);

  size_t ui_data_id_;
  std::function<void()> action_;
};

class UiBasePad final : public UiBase {
 public:
  UiBasePad()
      : UiBase(data::VboIdMain::kMenuTerrain, {}) {}
  void Press() override {
    std::cerr << "Pad was called, smt went wrong" << std::endl;
  }

  void UpdateTransform() override {}
};

class IUiEventHandler {
 public:
  virtual bool Press(int id) = 0;

  virtual void Release() = 0;

  virtual bool IsInRange(int id) = 0;
};

template<std::size_t MaxSize>
class UiEventHandler final : public IUiEventHandler {
 public:
  UiEventHandler() = default;

  UiEventHandler(std::initializer_list<UiBase*> widgets) {
    Init(widgets);
  }

  UiEventHandler(UiEventHandler&& other) = default;
  UiEventHandler(const UiEventHandler& other) = default;

  UiEventHandler& operator=(UiEventHandler&& other) = default;
  UiEventHandler& operator=(const UiEventHandler& other) = default;

  bool Press(int id) override {
    if (id < start_ || id > end_) {
      return false;
    }
    /// performance over the memory:
    ///   creates dumps if ids empty, but hit exactly by id on Press()
    widgets_[id - start_]->Press();
    return true;
  }

  void Release() override {
    for (auto widget : widgets_) {
      widget->Release();
    }
  }

  bool IsInRange(int id) override {
    return id >= start_ && id <= end_;
  }

 private:
  void Init(std::initializer_list<UiBase*> widgets) {
    start_ = std::numeric_limits<int>::max();
    end_ = std::numeric_limits<int>::min();

    for (UiBase* widget : widgets) {
      int id = widget->GetId();
      start_ = std::min(start_, id);
      end_ = std::max(end_, id);
    }

    // Bounds check
    if ((end_ - start_ + 1) > MaxSize) {
      throw std::runtime_error("EventHandler array size exceeded MaxSize");
    }

    widgets_.fill(&ui_callable_pad_);

    for (UiBase* widget : widgets) {
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
  std::array<UiBase*, MaxSize> widgets_;
  inline static UiBasePad ui_callable_pad_{};
};

class UiDynamicSprite : public UiBase {
 public:
  UiDynamicSprite(data::VboIdMain vbo_texture, CallableType action = {});

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

class UiStaticSprite : public UiBase {
 public:
  UiStaticSprite(data::VboIdMain vbo_texture, CallableType action = {});

  UiStaticSprite(UiStaticSprite&& other) noexcept;
  UiStaticSprite(const UiStaticSprite& other) = delete;

  UiStaticSprite& operator=(UiStaticSprite&& other) noexcept = default;
  UiStaticSprite& operator=(const UiStaticSprite& other) = delete;

  void Render();

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  //TODO: inilne
  void RenderPicking() const;

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

class UiSliderV final : public UiBase {
 public:
  UiSliderV(UiDynamicSprite&& fill_sprite,
            UiDynamicSprite&& back_sprite,
            UiDynamicSprite&& icon_sprite,
            float scale = 1.0f);

  UiSliderV(UiSliderV&& other) noexcept;
  UiSliderV(const UiSliderV& other) = delete;

  UiSliderV& operator=(UiSliderV&& other) = delete;
  UiSliderV& operator=(const UiSliderV& other) = delete;

  void Render(glm::vec2 mouse_pos);
  void RenderIcon();

  void RenderPicking() const;

  void Press() override {
    pressed_ = true;
  }

  void Release() override {
    pressed_ = false;
  }

  bool Scroll(GLuint id, float yoffset) override;

  [[nodiscard]] float GetProgress() const;

  [[nodiscard]] float GetProgressUnscaled() const;

  void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

  void Set(float progress);

  void SetMouseDiff(float remembered_progress,
                    glm::vec2 cursor_start, glm::vec2 cursor_end);

 private:
  void Set(glm::vec2 mouse_pos);

  UiDynamicSprite fill_sprite_;
  UiDynamicSprite back_sprite_;
  UiDynamicSprite icon_sprite_;

  float progress_{0.0f};
  bool pressed_ = false;
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH final : public UiBase {
 public:
  UiSliderH(UiDynamicSprite&& fill_sprite,
            UiDynamicSprite&& back_sprite,
            UiDynamicSprite&& handler_sprite,
            float scale = 1.0f);

  UiSliderH(UiSliderH&& other) noexcept;
  UiSliderH(const UiSliderH& other) = delete;

  UiSliderH& operator=(UiSliderH&& other) = delete;
  UiSliderH& operator=(const UiSliderH& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderIcon();

  void RenderPicking() const;

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

  void UpdateTransform() override;

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  /// we want to use UiDynamicSprite only for handler_sprite_,
  /// BUT to use it in complex hierarhies we need all to be UiDynamicSprite
//  UiStaticSprite fill_sprite_;
//  UiStaticSprite back_sprite_;
//  UiStaticSprite icon_sprite_;
//  UiDynamicSprite handler_sprite_;

  UiDynamicSprite fill_sprite_;
  UiDynamicSprite back_sprite_;
  UiDynamicSprite handler_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH3 final : public UiBase {
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

  void UpdateTransform() override;

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

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

class UiSliderH2 final : public UiBase {
 public:
  UiSliderH2(UiDynamicSprite&& back_sprite,
             UiDynamicSprite&& icon_sprite,
             float scale = 1.0f);

  UiSliderH2(UiSliderH2&& other) noexcept;
  UiSliderH2(const UiSliderH2& other) = delete;

  UiSliderH2& operator=(UiSliderH2&& other) = delete;
  UiSliderH2& operator=(const UiSliderH2& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderIcon();

  void RenderPicking() const;

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  /// UiSlider has the same id as a track_, so it's like its wrapper.
  /// We don't render UiSlider id, but
  /// for comparison (e.g. in key callback) we directly slider.GetId()
  [[nodiscard]] std::uint32_t GetTrackId() const {
    return back_sprite_.GetId();
  }

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform() override;

  void SetValue(float value);

 private:
  // if hor slider - use mouse_pos.x, otherwise mouse_pos.y
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  UiDynamicSprite back_sprite_;
  UiDynamicSprite icon_sprite_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
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

/// You should pass action to off_ sprite (see Press());
class UiToggle final : public UiBase {
 public:
  UiToggle(UiDynamicSprite&& off, UiDynamicSprite&& on1,
           UiDynamicSprite&& on2, UiDynamicSprite&& on3);

  UiToggle(UiToggle&& other) noexcept;
  UiToggle(const UiToggle& other) = delete;

  UiToggle& operator=(UiToggle&& other) = delete;
  UiToggle& operator=(const UiToggle& other) = delete;

  void Render();

  void RenderPicking() const;

  void Press() override;

  void UpdateTransform() override;

  void UpdateState();

  void SetParentTransform(LocalTransform transform);

  void SetTranslate(glm::vec2 translate);

  [[nodiscard]] bool IsOff() const noexcept {
    return turned_off_;
  }

  void Set(bool value) {
    if (turned_off_ != value) {
      Press();
    }
  }

 private:
  UiDynamicSprite off_;
  UiDynamicSprite on1_;
  UiDynamicSprite on2_;
  UiDynamicSprite on3_;

  UiDynamicSprite* state_{nullptr};

  bool turned_off_{true};

  float speed_{0.2f};
  float progress_{0.0f};
};

class UiToggle2 final : public UiBase {
 public:
  UiToggle2(UiDynamicSprite&& off, UiDynamicSprite&& on);

  UiToggle2(UiToggle2&& other) noexcept;
  UiToggle2(const UiToggle2& other) = delete;

  UiToggle2& operator=(UiToggle2&& other) = delete;
  UiToggle2& operator=(const UiToggle2& other) = delete;

  void Render();

  void RenderPicking() const;

  void Press() override;

  void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

  void SetTranslate(glm::vec2 translate);

  [[nodiscard]] bool TurnedOn() const noexcept {
    return !turned_off_;
  }

  void Set(bool value) {
    if (turned_off_ == value) {
      Press();
    }
  }

 private:
  UiDynamicSprite off_;
  UiDynamicSprite on_;
  bool turned_off_{true};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
