#ifndef WIREBOUNDWORLDCREATOR_SRC_UI_H_
#define WIREBOUNDWORLDCREATOR_SRC_UI_H_

#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <span>
#include <vector>

#include "../common/LocalTransform.h"
#include "../common/Vbos.h"
#include "../io/Window.h"

/// dbg serializing - stored without rotation
LocalTransformLinear GetParentDbgTransform(size_t id);

extern std::array<data::UiData, data::gVboIdSize> gUiComponents;

class UiBase {
 public:
  using CallableType = std::function<void()>;

  UiBase(data::VboIdMain vbo_texture, CallableType action);

  UiBase(size_t ui_data_id, CallableType&& action);

  UiBase(UiBase&& other) noexcept = default;
  UiBase(const UiBase& other) = default;
  UiBase& operator=(UiBase&& other) = default;
  UiBase& operator=(const UiBase& other) = default;

  virtual ~UiBase() = default;

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

/// fills with nullptr between ids, so may be inefficient in some cases
class UiEventHandler {
 public:
  UiEventHandler() = default;

  UiEventHandler(std::initializer_list<UiBase*> widgets)
      : UiEventHandler(std::span{widgets}) {}

  explicit UiEventHandler(std::span<UiBase* const> widgets) { Init(widgets); }

  UiEventHandler(UiEventHandler&& other) = delete;
  UiEventHandler(const UiEventHandler& other) = delete;
  UiEventHandler& operator=(UiEventHandler&& other) = delete;
  UiEventHandler& operator=(const UiEventHandler& other) = delete;

  bool Press(int id);

  void Release();

  [[nodiscard]] bool IsInRange(int id) const {
    return id >= start_ && id <= end_;
  }

 private:
  void Init(std::span<UiBase* const> widgets);

  int start_ = 0;
  int end_ = -1;
  std::vector<UiBase*> widgets_;
};

class UiHierarchy final : public UiBase {
 public:
  UiBase* parent_;
  std::vector<UiBase*> components_;

  UiHierarchy(UiBase* parent, std::initializer_list<UiBase*> components)
      : UiHierarchy(parent, std::span{components}) {}

  UiHierarchy(UiBase* parent, std::span<UiBase* const> components)
      : UiBase(*parent),
        parent_(parent),
        components_(components.begin(), components.end()) {
    Bind();
  }

  UiHierarchy(UiHierarchy&& other) = delete;
  UiHierarchy(const UiHierarchy& other) = delete;
  UiHierarchy& operator=(UiHierarchy&& other) = delete;
  UiHierarchy& operator=(const UiHierarchy& other) = delete;

  void AddNested(UiBase* parent, std::span<UiBase* const> components);

  void UpdateTransform() override;

 private:
  void Bind();
};

class UiSprite : public UiBase {
 public:
  UiSprite(data::VboIdMain vbo_texture, CallableType action = {});

  UiSprite(UiSprite&& other) noexcept;
  UiSprite(const UiSprite& other) = delete;
  UiSprite& operator=(UiSprite&& other) noexcept = delete;
  UiSprite& operator=(const UiSprite& other) = delete;

  void Render();

  void Rotate(float radians) { local_transform_.rotate += radians; }

  void Scale(float scale) { local_transform_.scale *= scale; }

  void Translate(glm::vec2 translate) {
    local_transform_.translate += translate;
  }

  void SetRotate(float radians) { local_transform_.rotate = radians; }

  void SetScale(float scale) { local_transform_.scale = scale; }

  void SetExtraScale(float scale) { extra_scale_ = scale; }

  void SetTranslate(glm::vec2 translate) {
    local_transform_.translate = translate;
  }

  void SetTransform(LocalTransform transform) { local_transform_ = transform; }

  [[nodiscard]] glm::vec2 GetTranslate() const noexcept {
    return local_transform_.translate;
  }

  [[nodiscard]] LocalTransform GetTransform() const noexcept {
    return local_transform_;
  }

  /// useful for transform-animated parent ui components
  void SetParentTransform(LocalTransform transform) {
    parent_transform_ = transform;
  }

  [[nodiscard]] LocalTransform GetParentTransform() const noexcept {
    return parent_transform_;
  }

  /// when we operate on arrays of buttons we don't want
  /// bind the same shader 20 times, so this function don't bind shader
  // TODO: inilne
  void RenderPicking() const;

  void UpdateTransform() override;

  [[nodiscard]] float GetLeftBorder() const;

  [[nodiscard]] float GetRightBorder() const;

  [[nodiscard]] float GetTopBorder() const;

  [[nodiscard]] float GetBottomBorder() const;

  [[nodiscard]] float GetHeight() const;

  [[nodiscard]] float GetWidth() const;

  [[nodiscard]] glm::vec2 GetCentre() const;

  [[nodiscard]] float GetLeftBorderUnScaled() const;

  [[nodiscard]] float GetRightBorderUnScaled() const;

  [[nodiscard]] float GetTopBorderUnScaled() const;

  [[nodiscard]] float GetBottomBorderUnScaled() const;

 private:
  // used for slider handlers, stored separately from gUiTransform &
  // gUiComponents
  /// dynamic, so do has rotations
  LocalTransform local_transform_;

  // to update only once at UpdateTransform() - not every time at Render()
  /// filled with SetParentTransform, includes rotation
  LocalTransform parent_transform_;

  /// collected from gUiTransform & gUiComponents, so dbg - no rotation
  LocalTransformLinear final_dbg_transform_;

  /// how x differs from y (useful for text or special resolution)
  float extra_scale_{1.0f};
};

class UiSpriteTransformation {
 public:
  UiSpriteTransformation(UiSprite& sprite, LocalTransform start,
                         LocalTransform end);

  void Render();

  void RenderPicking();

  void RunAnimation(bool looping = false);

  void StopAnimation();

  [[nodiscard]] LocalTransform GetStart() const noexcept { return start_; }

  [[nodiscard]] LocalTransform GetEnd() const noexcept { return end_; }

  [[nodiscard]] LocalTransform GetCur() const noexcept { return cur_; }

  void SetStart(LocalTransform transform) { start_ = transform; }

  void SetEnd(LocalTransform transform) { end_ = transform; }

  void SetCur(LocalTransform transform) { cur_ = transform; }

  // manual looping
  void SwapStartEnd();

 private:
  void CubicInterpolation();

  void UpdateAnimation();

  // take by ref to avoid UpdateTransform / parent dependency issues
  UiSprite& sprite_;
  LocalTransform start_;
  LocalTransform end_;
  LocalTransform cur_;

  const float speed_;
  float progress_{0.0f};
  bool looping_{false};
};

class UiSliderV3 final : public UiBase {
 public:
  UiSliderV3(UiSprite&& sp_fill, UiSprite&& sp_track, UiSprite&& sp_handle,
             float scale = 1.0f);

  UiSliderV3(UiSliderV3&& other) noexcept;
  UiSliderV3(const UiSliderV3& other) = delete;

  UiSliderV3& operator=(UiSliderV3&& other) = delete;
  UiSliderV3& operator=(const UiSliderV3& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking() const;

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform() override;

  void SetMouseDiff(float prev_progress, glm::vec2 start, glm::vec2 end);

  void SetValue(float value);

 private:
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  UiSprite sp_fill_;
  UiSprite sp_track_;
  UiSprite sp_handle_;

  float progress_{0.0f};
  bool pressed_ = false;
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH3 final : public UiBase {
 public:
  UiSliderH3(UiSprite&& sp_fill, UiSprite&& sp_track, UiSprite&& sp_handle,
             float scale = 1.0f);

  UiSliderH3(UiSliderH3&& other) noexcept;
  UiSliderH3(const UiSliderH3& other) = delete;

  UiSliderH3& operator=(UiSliderH3&& other) = delete;
  UiSliderH3& operator=(const UiSliderH3& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking() const;

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform() override;

  void SetValue(float value);

 private:
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  UiSprite sp_fill_;
  UiSprite sp_track_;
  UiSprite sp_handle_;

  float progress_{0.0f};
  bool pressed_{false};
  float centre_;
  float length_;
  float scale_{1.0f};
  static const float kTrackLengthFactor;
};

class UiSliderH2 final : public UiBase {
 public:
  UiSliderH2(UiSprite&& sp_track, UiSprite&& sp_handle, float scale = 1.0f);

  UiSliderH2(UiSliderH2&& other) noexcept;
  UiSliderH2(const UiSliderH2& other) = delete;

  UiSliderH2& operator=(UiSliderH2&& other) = delete;
  UiSliderH2& operator=(const UiSliderH2& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking() const;

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void SetTranslate(glm::vec2 translate);

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] float GetProgress() const;

  void UpdateTransform() override;

  void SetValue(float value);

  // used for UiWindowSlider (UiComplex.h) as a slot height
  [[nodiscard]] const UiSprite* GetTrackPtr() const noexcept {
    return &sp_track_;
  }
  // used for UiWindowSlider (UiComplex.h) do setup a group of sliders
  void Render() {
    sp_track_.Render();
    sp_handle_.Render();
  }

 private:
  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  UiSprite sp_track_;
  UiSprite sp_handle_;

  float progress_ = 0.0f;
  bool pressed_ = false;
  float centre_;
  float length_;
  float scale_ = 1.0f;
  static const float kTrackLengthFactor;
};

/// used for ui palette, so X-axis is Hue, Y-axis is Saturation
class UiSlider2D final : public UiBase {
 public:
  UiSlider2D(UiSprite&& sp_palette, UiSprite&& sp_handle,
             glm::vec2 scale = glm::vec2{1.0f});

  UiSlider2D(UiSlider2D&& other) noexcept;
  UiSlider2D(const UiSlider2D& other) = delete;

  UiSlider2D& operator=(UiSlider2D&& other) = delete;
  UiSlider2D& operator=(const UiSlider2D& other) = delete;

  void Render(glm::vec2 mouse_pos);

  void RenderPicking() const;

  void Press() override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void SetParentTransform(LocalTransform transform);

  [[nodiscard]] glm::vec2 GetProgress() const;

  [[nodiscard]] float GetProgressX() const;

  [[nodiscard]] float GetProgressY() const;

  void UpdateTransform() override;

  void SetValue(glm::vec2 value);

 private:
  void SetMousePos(glm::vec2 mouse_pos);

  void SetProgress(glm::vec2 progress);

  UiSprite sp_track_;
  UiSprite sp_handle_;

  glm::vec2 progress_ = glm::vec2{0.0f};
  bool pressed_ = false;
  glm::vec2 centre_;
  glm::vec2 length_;
  glm::vec2 scale_;

  static const float kTrackWidthFactor;
  static const float kTrackHeightFactor;
};

/// You should pass action to off_ sprite (see Press());
class UiToggle4 final : public UiBase {
 public:
  UiToggle4(UiSprite&& off, UiSprite&& on1, UiSprite&& on2, UiSprite&& on3);

  UiToggle4(UiToggle4&& other) noexcept;
  UiToggle4(const UiToggle4& other) = delete;

  UiToggle4& operator=(UiToggle4&& other) = delete;
  UiToggle4& operator=(const UiToggle4& other) = delete;

  void Render();

  void Render(bool state);

  void RenderPicking() const;

  void Press() override;

  void UpdateTransform() override;

  void SetParentTransform(LocalTransform transform);

  void SetTranslate(glm::vec2 translate);

  [[nodiscard]] bool TurnedOn() const noexcept { return !turned_off_; }

  void Set(bool value);

 private:
  void UpdateState();

  UiSprite sp_off_;
  UiSprite sp_on1_;
  UiSprite sp_on2_;
  UiSprite sp_on3_;

  UiSprite* state_ = nullptr;

  bool turned_off_ = true;

  float speed_ = 0.2f;
  float progress_ = 0.0f;
};

class UiToggle2 final : public UiBase {
 public:
  UiToggle2(UiSprite&& off, UiSprite&& on);

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

  [[nodiscard]] bool TurnedOn() const noexcept { return !turned_off_; }

  void Set(bool value);

 private:
  UiSprite sp_off_;
  UiSprite sp_on_;
  bool turned_off_ = true;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_UI_H_
