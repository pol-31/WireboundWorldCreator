#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_

#include "../ui/UiRenderData.h"
#include "Ui.h"
#include "UiText.h"
#include "WindowQueue.h"

class Camera;
struct ModelData;
class ModelManager;

class UiWindowSlider final : public UiBase {
 public:
  UiWindowSlider(UiSprite&& sp_track, UiSprite&& sp_handle, int slots_num,
                 float track_length_factor, float slots_length_factor);

  UiWindowSlider(UiWindowSlider&& other) = delete;
  UiWindowSlider(const UiWindowSlider& other) = delete;

  UiWindowSlider& operator=(UiWindowSlider&& other) = delete;
  UiWindowSlider& operator=(const UiWindowSlider& other) = delete;

  void SetSlotPtr(const UiSprite* sp_slot) { sp_slot_ = sp_slot; }

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

  void UpdateRenderData();

  int GetSlotId(glm::vec2 mouse_pos);

  void Set(glm::vec2 mouse_pos);

  void Set(float progress);

  void SetEntryNum(int entry_num);

  void FocusOnSelected(int slot_id);

  void SetUpScissors() const;

  /// ctor req
  float track_length_factor_ = 1.0f;
  float slots_length_factor_ = 1.0f;
  int slots_num_ = 0;

  int entry_num_ = 0;

  UiSprite* slot_ = nullptr;

 private:
  const UiSprite* sp_slot_ = nullptr;
  UiSprite sp_track_;
  UiSprite sp_handle_;

  float centre_ = 0.0f;
  float length_ = 0.0f;
  float length_slots_ = 0.0f;

  float progress_ = 0.0f;
  bool pressed_ = false;

 public:
  float slot_height_ = 0.0f;
  int scissors_start_ = 0;
  int scissors_length_ = 0;

  /// so we could get id related to slots (0-5)
  int cur_slots_offset_ = 0;
  glm::vec2 start_slot_translate_ = glm::vec2(0.0f);
};

/// loading only on the bottom of the screen (so use UiDynamicSprite)
class UiLoading {
 public:
  UiLoading(UiSprite&& sprite0, UiSprite&& sprite10, UiSprite&& sprite20,
            UiSprite&& sprite30, UiSprite&& sprite40, UiSprite&& sprite50,
            UiSprite&& sprite60, UiSprite&& sprite70, UiSprite&& sprite80,
            UiSprite&& sprite90, UiSprite&& sprite100);

  UiLoading(UiLoading&& other) = delete;
  UiLoading(const UiLoading& other) = delete;

  UiLoading& operator=(UiLoading&& other) = delete;
  UiLoading& operator=(const UiLoading& other) = delete;

  void Render(float progress);

  void RenderPicking() const;

 private:
  std::array<UiSprite, 11> sprites_;
  UiHierarchy hierarchy_;
};

/// loading only on the bottom of the screen (so use UiDynamicSprite)

class UiSelectedSprite {
 public:
  UiSelectedSprite(UiSprite&& sp_selected, const UiSprite* sp_ptr_top);

  UiSelectedSprite(UiSelectedSprite&& other) = delete;
  UiSelectedSprite(const UiSelectedSprite& other) = delete;

  UiSelectedSprite& operator=(UiSelectedSprite&& other) = delete;
  UiSelectedSprite& operator=(const UiSelectedSprite& other) = delete;

  void Render();

  void SetSelected(int id);

 private:
  UiSprite sp_selected_;
  const UiSprite* sp_ptr_top_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
