#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_

#include "Ui.h"
#include "UiText.h"
#include "WindowQueue.h"

class Camera;
struct ModelData;
class ModelManager;

class UiWindowSlider final : public UiBase {
 public:
  UiWindowSlider(UiDynamicSprite&& sp_track, UiDynamicSprite&& sp_handle,
                 int slots_num, float track_length_factor,
                 float slots_length_factor);

  UiWindowSlider(UiWindowSlider&& other) noexcept;
  UiWindowSlider(const UiWindowSlider& other) = delete;

  UiWindowSlider& operator=(UiWindowSlider&& other) = delete;
  UiWindowSlider& operator=(const UiWindowSlider& other) = delete;

  void SetSlotPtr(const UiDynamicSprite* sp_slot) { sp_slot_ = sp_slot; }

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

  UiDynamicSprite* slot_ = nullptr;

 private:
  const UiDynamicSprite* sp_slot_ = nullptr;
  UiDynamicSprite sp_track_;
  UiDynamicSprite sp_handle_;

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

// NEVER just std::move it, recreate it in all ctors
/// hierarchy blocks; should be created at most derived ui components
class UiHierarchy final : public UiBase {
 public:
  UiBase* parent_;
  std::vector<UiBase*> components_;

  /// due to base class UiBase
  UiHierarchy() = delete;

  /// parent hold id of the first component
  // TODO: UiBase ctor params !!!
  template <typename... Args>
  UiHierarchy(UiBase* parent, Args... components)
      : UiBase(*parent), parent_(parent), components_{components...} {
    auto parent_id = parent_->GetId();
    for (auto component : components_) {
      auto child_id = component->GetId();
      gUiComponents[child_id - details::kIdOffsetUi].parent_id_ = parent_id;
    }
    gUiComponents[parent_id - details::kIdOffsetUi].ui = this;
    UpdateTransform();
  }

  UiHierarchy(UiHierarchy&& other)
      : UiBase(std::move(*this)),
        parent_(other.parent_),
        components_(std::move(other.components_)) {
    gUiComponents[parent_->GetId() - details::kIdOffsetUi].ui = this;
  }

  // cannot remove it: Base class store ::hierarchy_, but
  // can be initialized only from derived (not init list)
  UiHierarchy& operator=(UiHierarchy&& other) {
    parent_ = other.parent_;
    components_ = std::move(other.components_);
    gUiComponents[parent_->GetId() - details::kIdOffsetUi].ui = this;
    return *this;
  }

  template <typename... Args>
  void AddNested(UiBase* parent, Args... components) {
    auto parent_id = parent->GetId();
    int prev_size = components_.size();
    components_.push_back(parent);
    (components_.push_back(components), ...);
    for (int i = prev_size; i < components_.size(); ++i) {
      auto child_id = components_[i]->GetId();
      gUiComponents[child_id - details::kIdOffsetUi].parent_id_ = parent_id;
    }
    gUiComponents[parent_id - details::kIdOffsetUi].parent_id_ =
        parent_->GetId();
    gUiComponents[parent_id - details::kIdOffsetUi].ui = this;
    UpdateTransform();
  }

  void UpdateTransform() override {
    parent_->UpdateTransform();
    for (auto component : components_) {
      component->UpdateTransform();
    }
  }
};

/// loading only on the bottom of the screen (so use UiDynamicSprite)
class UiLoading {
 public:
  UiLoading(UiDynamicSprite&& sprite0, UiDynamicSprite&& sprite10,
            UiDynamicSprite&& sprite20, UiDynamicSprite&& sprite30,
            UiDynamicSprite&& sprite40, UiDynamicSprite&& sprite50,
            UiDynamicSprite&& sprite60, UiDynamicSprite&& sprite70,
            UiDynamicSprite&& sprite80, UiDynamicSprite&& sprite90,
            UiDynamicSprite&& sprite100);

  UiLoading(UiLoading&& other) noexcept;
  UiLoading(const UiLoading& other) = delete;

  UiLoading& operator=(UiLoading&& other) = delete;
  UiLoading& operator=(const UiLoading& other) = delete;

  void Render(float progress);

  void RenderPicking() const;

 private:
  std::array<UiDynamicSprite, 11> sprites_;
  UiHierarchy hierarchy_;
};

/// loading only on the bottom of the screen (so use UiDynamicSprite)
class UiCompass {
 public:
  UiCompass(const Camera* camera, UiDynamicSprite&& sp_compass,
            UiDynamicSprite&& sp_north, UiDynamicSprite&& sp_south,
            UiDynamicSprite&& sp_east, UiDynamicSprite&& sp_west);

  UiCompass(UiCompass&& other) noexcept;
  UiCompass(const UiCompass& other) = delete;

  UiCompass& operator=(UiCompass&& other) = delete;
  UiCompass& operator=(const UiCompass& other) = delete;

  void Render();

  void RenderPicking() const;

 private:
  const Camera* camera_;
  UiDynamicSprite sp_compass_;
  UiDynamicSprite sp_north_;
  UiDynamicSprite sp_south_;
  UiDynamicSprite sp_east_;
  UiDynamicSprite sp_west_;
  UiHierarchy hierarchy_;
};

class UiSelectedSprite {
 public:
  UiSelectedSprite(UiDynamicSprite&& sp_selected,
                   const UiDynamicSprite* sp_ptr_top);

  UiSelectedSprite(UiSelectedSprite&& other) noexcept;
  UiSelectedSprite(const UiSelectedSprite& other) = delete;

  UiSelectedSprite& operator=(UiSelectedSprite&& other) = delete;
  UiSelectedSprite& operator=(const UiSelectedSprite& other) = delete;

  void Render();

  void SetSelected(int id);

 private:
  UiDynamicSprite sp_selected_;
  const UiDynamicSprite* sp_ptr_top_ = nullptr;
};

// cannot be pinned, cannot be hovered above,
// single to interact - on very top; shadow all other
class UiTopWindowBase {
 public:
  UiTopWindowBase(UiDynamicSprite&& desk, float size_scale,
                  UiSharedResources& ui_shared_resources,
                  WindowQueue& window_queue);

  UiTopWindowBase(UiTopWindowBase&& other) noexcept;
  UiTopWindowBase(const UiTopWindowBase& other) = delete;

  UiTopWindowBase& operator=(UiTopWindowBase&& other) = delete;
  UiTopWindowBase& operator=(const UiTopWindowBase& other) = delete;

  void Show();

  void Hide();

  virtual bool Render() = 0;

  virtual void RenderPicking() = 0;

  virtual void Press(int id) = 0;

  // no override
  virtual void Release() = 0;

  virtual void BtnEnter() {}

  virtual void BtnEscape() {}

 protected:
  UiDynamicSprite desk_;
  // Cancel() if not important; otherwise skip

  float size_scale_{1.0f};

  // for shader bindings, mask texture
  UiSharedResources& ui_shared_resources_;
  WindowQueue& window_queue_;
  UiHierarchy hierarchy_;
};

class UiCaution final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiCaution(UiDynamicSprite&& desk, float size_scale,
            UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
            UiDynamicSprite&& text);

  UiCaution(UiCaution&& other) noexcept;
  UiCaution(const UiCaution& other) = delete;

  UiCaution& operator=(UiCaution&& other) = delete;
  UiCaution& operator=(const UiCaution& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  void Press(int id) override;

  void Release() override;

 private:
  UiDynamicSprite text_;
};

class UiConfirmation final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiConfirmation(UiDynamicSprite&& desk, float size_scale,
                 UiSharedResources& ui_shared_resources,
                 WindowQueue& window_queue, UiDynamicSprite&& btn_accept,
                 UiDynamicSprite&& btn_decline, UiTextMenuId&& text);

  UiConfirmation(UiConfirmation&& other) noexcept;
  UiConfirmation(const UiConfirmation& other) = delete;

  UiConfirmation& operator=(UiConfirmation&& other) = delete;
  UiConfirmation& operator=(const UiConfirmation& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  void Press(int id) override;

  void Release() override;

  void SetText(data::TextId text_id);

  void SetCallable(std::function<void()>&& callable);

  void BtnEnter() override;

  void BtnEscape() override;

 private:
  UiTextMenuId text_;
  UiDynamicSprite btn_accept_;
  UiDynamicSprite btn_decline_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kConfirmationDecline) -
                 static_cast<int>(data::VboIdMain::kConfirmationDesk) + 1>
      ui_event_handler_;

  std::function<void()> callable_;
};

class UiFile final : public UiTopWindowBase {
 public:
  using Base = UiTopWindowBase;

  UiFile(UiDynamicSprite&& desk, float size_scale,
         UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
         UiDynamicSprite&& btn_accept, UiDynamicSprite&& btn_decline,
         UiText&& label, UiTextInput&& text);

  UiFile(UiFile&& other) noexcept;
  UiFile(const UiFile& other) = delete;

  UiFile& operator=(UiFile&& other) = delete;
  UiFile& operator=(const UiFile& other) = delete;

  bool Render() override;

  void RenderPicking() override;

  void Press(int id) override;

  void Release() override;

 private:
  UiDynamicSprite btn_accept_;
  UiDynamicSprite btn_decline_;  // TODO: appears in case of overwrite
  UiText label_;
  UiTextInput text_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kFileAccept) -
                 static_cast<int>(data::VboIdMain::kFileDesk) + 1>
      ui_event_handler_;
};

// single interactable window; can add few
class UiWindowBase {
 public:
  UiWindowBase(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue);

  UiWindowBase(UiWindowBase&& other) noexcept;
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

  virtual void BtnEnter() {
    std::cout << "enter" << std::endl;
    Hide();
  }

  virtual void BtnEscape() {
    std::cout << "escape" << std::endl;
    Hide();
  }

 protected:
  UiDynamicSprite sprite_;
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

  UiHierarchy hierarchy_;
};

class UiWindowAppear : public UiWindowBase {
 public:
  UiWindowAppear(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
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

class UiObjectInfo final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;
  using Base::Show;

  UiObjectInfo(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
               UiSharedResources& ui_shared_resources,
               WindowQueue& window_queue,

               UiDynamicSprite&& sp_enemy, UiDynamicSprite&& sp_friend,
               UiDynamicSprite&& sp_neutral, UiDynamicSprite&& sp_obstacle,
               UiText&& txt_name, UiText&& txt_characteristic,
               UiText&& txt_value);

  UiObjectInfo(UiObjectInfo&& other) noexcept;
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

  UiDynamicSprite sp_enemy_;
  UiDynamicSprite sp_friend_;
  UiDynamicSprite sp_neutral_;
  UiDynamicSprite sp_obstacle_;
  UiText txt_name_;
  UiText txt_characteristic_;
  UiText txt_value_;
  UiEventHandler<static_cast<int>(data::VboIdMain::kPlayerGameObjInfoValue) -
                 static_cast<int>(data::VboIdMain::kPlayerGameObjInfoDesk) + 1>
      ui_event_handler_;

  bool enemy_selected_ = false;
  bool friend_selected_ = false;
  bool neutral_selected_ = false;
  bool obstacle_selected_ = false;

  int selected_num_ = 0;
  const ModelData* mdl_data_ = nullptr;

  std::vector<ObjParam> render_params_;
};

class UiTipWindow final : public UiWindowAppear {
 public:
  using Base = UiWindowAppear;

  UiTipWindow(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
              UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
              UiDynamicSprite&& text);

  UiTipWindow(UiTipWindow&& other) noexcept;
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
  UiDynamicSprite text_;
  bool hovered_ = false;
};

// -
/// only one scale allowed (no x or y scale)
class UiWindowPopUp : public UiWindowBase {
 public:
  UiWindowPopUp(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
                UiSharedResources& ui_shared_resources,
                WindowQueue& window_queue, LocalTransform start_transform,
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

  UiSettings(UiDynamicSprite&& sprite, float size_scale, UiToggle2&& pin,
             UiSharedResources& ui_shared_resources, WindowQueue& window_queue,
             LocalTransform start_transform, LocalTransform end_transform,
             UiDynamicSprite&& resolution_label,
             UiDynamicSprite&& resolution_left,
             UiDynamicSprite&& resolution_right, UiDynamicSprite&& resolution,
             UiToggle4&& toggle_fullscreen, UiSliderH2 sensitivity,
             UiSliderH2&& sound, UiToggle4&& toggle_sound, UiSliderH2&& music,
             UiToggle4&& toggle_music, UiDynamicSprite&& tip_info_label,
             UiDynamicSprite&& tip_info, UiToggle4&& toggle_tip_info);

  UiSettings(UiSettings&& other) noexcept;
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
  UiDynamicSprite resolution_label_;
  UiDynamicSprite resolution_left_;
  UiDynamicSprite resolution_right_;

  UiDynamicSprite resolution_;
  UiToggle4 toggle_fullscreen_;

  UiSliderH2 sensitivity_;

  UiSliderH2 sound_;
  UiToggle4 toggle_sound_;

  UiSliderH2 music_;
  UiToggle4 toggle_music_;

  UiDynamicSprite tip_info_label_;
  UiDynamicSprite tip_info_;
  UiToggle4 toggle_tip_info_;

  UiEventHandler<static_cast<int>(data::VboIdMain::kSettingsTipInfoOn3) -
                 static_cast<int>(data::VboIdMain::kSettingsDesk) + 1>
      ui_event_handler_;
};

class UiPlayerMap final : public UiWindowPopUp {
 public:
  using Base = UiWindowPopUp;

  UiPlayerMap(UiSharedResources& ui_shared_resources,
              WindowQueue& window_queue);

  UiPlayerMap(UiPlayerMap&& other) noexcept;
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
  UiDynamicSprite sp_player_;
  UiDynamicSprite sp_enemy_;
  UiDynamicSprite sp_friend_;
  UiDynamicSprite sp_neutral_;
  UiDynamicSprite sp_obstacle_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UICOMPLEX_H_
