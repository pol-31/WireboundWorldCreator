#ifndef WIREBOUNDWORLDCREATOR_UIANIMATION_H
#define WIREBOUNDWORLDCREATOR_UIANIMATION_H

#include <vector>

#include "../common/Vbos.h"

// TODO:
//  - sprite sheet animation (check box - furniture fire)
//  - sprite sheet animation by steps (ui_loading)
//  - sprite transformation / pop_up animation (ui_pop_up_window)

class IUiAnimation {
 public:
  virtual ~IUiAnimation() = default;

  virtual void Update(float delta_time) {
    if (!running_) {
      return;
    }
    progress_ += delta_time * speed_;
    if (progress_ >= 1.0f) {
      if (looping_) {
        progress_ = 0.0f;
      } else {
        running_ = false;
        progress_ = 1.0f;
      }
    }
  }

  /// we might want to get start transform of end transform without update,
  /// so then we StopAnimation() and e.g. SetProgress(1.0f)
  void SetProgress(float progress) { progress_ = progress; }

  void SetSpeed(float speed) { speed_ = speed; }

  void SetLooping(bool value) { looping_ = value; }

  void StartAnimation() { running_ = true; }

  void StopAnimation() { running_ = false; }

 protected:
  float progress_ = 0.0f;  // 0.0f - 1.0f
  float speed_ = 1.0f;
  bool looping_ = false;
  bool running_ = false;
};

class UiAnimationGif final : public IUiAnimation {
 public:
  UiAnimationGif() = default;

  void Update(float delta_time) override {
    IUiAnimation::Update(delta_time);
    auto num = sprites_.size();
    currect_id_ = static_cast<int>(num * progress_);
  }

  data::UiId GetCurrentSpriteId() { return sprites_[currect_id_]; }

  void SetSprites(const std::vector<data::UiId>& sprites) {
    sprites_ = sprites;
  }

 private:
  size_t currect_id_ = 0;
  std::vector<data::UiId> sprites_;
};

class UiAnimationTransform final : public IUiAnimation {
 public:
  struct Transform {
    glm::vec2 translate = glm::vec2(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;
  };

  enum class Interpolation {
    Linear,
    Cubic,
  };

  UiAnimationTransform() = default;

  void Update(float delta_time) override {
    IUiAnimation::Update(delta_time);
    switch (interpolation_) {
      case Interpolation::Linear:
        InterpolateLinear();
        break;
      case Interpolation::Cubic:
        InterpolateCubic();
        break;
    }
  }

  [[nodiscard]] Transform GetCurrentTransform() const noexcept {
    return cur_transform_;
  }

  void SetTranslation(glm::vec2 start, glm::vec2 end) {
    start_transform_.translate = start;
    end_transform_.translate = end;
  }

  void SetScale(glm::vec2 start, glm::vec2 end) {
    start_transform_.scale = start;
    end_transform_.scale = end;
  }

  void SetRotation(float start, float end) {
    start_transform_.rotation = start;
    end_transform_.rotation = end;
  }

  void SetInterpolation(Interpolation type) { interpolation_ = type; }

  void RunForward(bool value) { run_forward_ = value; }

 private:
  void InterpolateLinear() {
    auto start = run_forward_ ? start_transform_ : end_transform_;
    auto end = run_forward_ ? end_transform_ : start_transform_;
    cur_transform_.translate =
        glm::mix(start.translate, end.translate, progress_);
    cur_transform_.scale = glm::mix(start.scale, end.scale, progress_);
    cur_transform_.rotation = glm::mix(start.rotation, end.rotation, progress_);
  }

  void InterpolateCubic() {
    auto start = run_forward_ ? start_transform_ : end_transform_;
    auto end = run_forward_ ? end_transform_ : start_transform_;
    float t = progress_;
    float cubic_t = t * t * (3.0f - 2.0f * t);
    cur_transform_.translate =
        start.translate + cubic_t * (end.translate - start.translate);
    cur_transform_.rotation =
        start.rotation + cubic_t * (end.rotation - start.rotation);
    cur_transform_.scale = start.scale + cubic_t * (end.scale - start.scale);
  }

  Transform start_transform_;
  Transform end_transform_;
  Transform cur_transform_;
  Interpolation interpolation_ = Interpolation::Cubic;
  bool run_forward_ = true;  // in case we want animate roll-back
};

#endif  // WIREBOUNDWORLDCREATOR_UIANIMATION_H
