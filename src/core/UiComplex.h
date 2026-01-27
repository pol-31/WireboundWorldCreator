#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_

#include "../modes/UiSharedResources.h"
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
class UiCompass {
 public:
  UiCompass(const Camera* camera, UiSprite&& sp_compass, UiSprite&& sp_north,
            UiSprite&& sp_south, UiSprite&& sp_east, UiSprite&& sp_west);

  UiCompass(UiCompass&& other) = delete;
  UiCompass(const UiCompass& other) = delete;

  UiCompass& operator=(UiCompass&& other) = delete;
  UiCompass& operator=(const UiCompass& other) = delete;

  void Render();

  void RenderPicking() const;

 private:
  const Camera* camera_;
  UiSprite sp_compass_;
  UiSprite sp_north_;
  UiSprite sp_south_;
  UiSprite sp_east_;
  UiSprite sp_west_;
  UiHierarchy hierarchy_;
};

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

// single interactable window; can add few
class UiWindowBase {
 public:
  virtual ~UiWindowBase() = default;
  UiWindowBase(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue);

  UiWindowBase(UiWindowBase&& other) = delete;
  UiWindowBase(const UiWindowBase& other) = delete;

  UiWindowBase& operator=(UiWindowBase&& other) = delete;
  UiWindowBase& operator=(const UiWindowBase& other) = delete;

  UiWindowBase* GetWindowPtr();

  void Show();

  void Hide();

  void ForceHide();

  virtual bool Render() = 0;

  virtual void RenderPicking() = 0;

  virtual bool Press(int id) = 0;

  // no override
  virtual void Release() = 0;

  virtual bool Scroll(GLuint id, float yoffset) { return false; }

  [[nodiscard]] bool Pinned() const noexcept;

  [[nodiscard]] bool BackIsReady() const { return back_ready_; }

 protected:
  UiSprite background_;
  // how it differs from the nested components
  float size_scale_{1.0f};
  UiToggle2 pin_;
  UiSharedResources& ui_shared_resources_;  // for shader bindings, mask texture

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
  UiWindowAppear(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
                 UiSharedResources& ui_shared_resources,
                 WindowQueue& window_queue);

  UiWindowAppear(UiWindowAppear&& other) = delete;
  UiWindowAppear(const UiWindowAppear& other) = delete;

  UiWindowAppear& operator=(UiWindowAppear&& other) = delete;
  UiWindowAppear& operator=(const UiWindowAppear& other) = delete;

  /// back_ready_==false when appearing or disappearing animation
  /// returs false when disappearing fading is over
  bool RenderBack(bool show);

  void RenderPickingBack();
};

class UiObjectInfo final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;
  using Base::Show;

  UiObjectInfo(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue,

               UiSprite&& sp_enemy, UiSprite&& sp_friend, UiSprite&& sp_neutral,
               UiSprite&& sp_obstacle, UiText&& txt_name,
               UiText&& txt_characteristic, UiText&& txt_value);

  UiObjectInfo(UiObjectInfo&& other) = delete;
  UiObjectInfo(const UiObjectInfo& other) = delete;

  UiObjectInfo& operator=(UiObjectInfo&& other) = delete;
  UiObjectInfo& operator=(const UiObjectInfo& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  void Show(bool enemy_selected, bool friend_selected, bool neutral_selected,
            bool obstacle_selected, int selected_num);

  // if selected_num == 1
  void Show(bool enemy_selected, bool friend_selected, bool neutral_selected,
            bool obstacle_selected, const ModelData* mdl_data);

 private:
  struct ObjParam {
    std::string_view name;
    float value;
  };

  void RenderParams();

  void RenderPickingParams();

  UiSprite sp_enemy_;
  UiSprite sp_friend_;
  UiSprite sp_neutral_;
  UiSprite sp_obstacle_;
  UiText txt_name_;
  UiText txt_characteristic_;
  UiText txt_value_;
  UiEventHandler ui_event_handler_;

  bool enemy_selected_ = false;
  bool friend_selected_ = false;
  bool neutral_selected_ = false;
  bool obstacle_selected_ = false;

  int selected_num_ = 0;
  const ModelData* mdl_data_ = nullptr;

  std::vector<ObjParam> render_params_;
  UiHierarchy hierarchy_;
};

class UiTipWindow final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTipWindow(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
              UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
              UiSprite&& text);

  UiTipWindow(UiTipWindow&& other) = delete;
  UiTipWindow(const UiTipWindow& other) = delete;

  UiTipWindow& operator=(UiTipWindow&& other) = delete;
  UiTipWindow& operator=(const UiTipWindow& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  void SetText(data::TextId text_id);

  void UpdateHoverState(int id);

 private:
  UiSprite text_;
  bool hovered_ = false;
  UiHierarchy hierarchy_;
};

// -
/// only one scale allowed (no x or y scale)
class UiWindowPopUp : public UiWindowBase {
 public:
  UiWindowPopUp(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
                UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue, LocalTransform start_transform,
                LocalTransform end_transform);

  UiWindowPopUp(UiWindowPopUp&& other) = delete;
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

  UiSettings(UiSprite&& sprite, float size_scale, UiToggle2&& pin,
             UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
             LocalTransform start_transform, LocalTransform end_transform,
             UiSprite&& resolution_label, UiSprite&& resolution_left,
             UiSprite&& resolution_right, UiSprite&& resolution,
             UiToggle4&& toggle_fullscreen, UiSliderH2 sensitivity,
             UiSliderH2&& sound, UiToggle4&& toggle_sound, UiSliderH2&& music,
             UiToggle4&& toggle_music, UiSprite&& tip_info_label,
             UiSprite&& tip_info, UiToggle4&& toggle_tip_info);

  UiSettings(UiSettings&& other) = delete;
  UiSettings(const UiSettings& other) = delete;

  UiSettings& operator=(UiSettings&& other) = delete;
  UiSettings& operator=(const UiSettings& other) = delete;

  // returns "stop render"
  bool Render() override;

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateHoverState(int id);

 private:
  // todo; replace by component
  UiSprite resolution_label_;
  UiSprite resolution_left_;
  UiSprite resolution_right_;

  UiSprite resolution_;
  UiToggle4 toggle_fullscreen_;

  UiSliderH2 sensitivity_;

  UiSliderH2 sound_;
  UiToggle4 toggle_sound_;

  UiSliderH2 music_;
  UiToggle4 toggle_music_;

  UiSprite tip_info_label_;
  UiSprite tip_info_;
  UiToggle4 toggle_tip_info_;
  UiEventHandler ui_event_handler_;
  UiHierarchy hierarchy_;
};

class UiPlayerMap final : public UiWindowPopUp {
 public:
  using Base = UiWindowPopUp;

  UiPlayerMap(UiSharedResources& ui_shared_resources,
              WindowQueue& window_queue);

  UiPlayerMap(UiPlayerMap&& other) = delete;
  UiPlayerMap(const UiPlayerMap& other) = delete;

  UiPlayerMap& operator=(UiPlayerMap&& other) = delete;
  UiPlayerMap& operator=(const UiPlayerMap& other) = delete;

  bool Render() override;

  void Render(ModelManager* mdl_manager);

  void RenderPicking() override;

  bool Press(int id) override;

  void Release() override;

  bool Scroll(GLuint id, float yoffset) override;

  void UpdateHoverState(int id);

 private:
  UiSprite sp_player_;
  UiSprite sp_enemy_;
  UiSprite sp_friend_;
  UiSprite sp_neutral_;
  UiSprite sp_obstacle_;
  UiHierarchy hierarchy_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
