#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_

#include <glm/glm.hpp>
#include <span>

#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiText.h"
#include "UiSharedResources.h"

class UiDynamicSprite;

int GetUiEditEntryId(float height, float size, glm::vec2 mouse_pos,
                     float start_height);

class UiEditConfigSlCfg {
 public:
  struct Trait {
    float* value;
    data::TextId label;
    bool tg1;
    bool tg2;
  };

  UiEditConfigSlCfg(UiSharedResources& ui_shared_resources,
                    TextRenderer& text_renderer);

  UiEditConfigSlCfg(UiEditConfigSlCfg&& other) noexcept = default;
  UiEditConfigSlCfg(const UiEditConfigSlCfg& other) = delete;

  UiEditConfigSlCfg& operator=(UiEditConfigSlCfg&& other) = delete;
  UiEditConfigSlCfg& operator=(const UiEditConfigSlCfg& other) = delete;

  void Release();

  void Render(float& strength, bool do_invert, bool do_tiling,
              glm::vec2 translate, bool update_strength, data::TextId text_id);

  void RenderPicking(float sprite_height, int num);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  void ResetTransform();

  UiDynamicSprite btn_config_;
  UiTextModeId txt_name_;
  UiToggle4 tg1_;
  UiToggle4 tg2_;
  UiSliderH2 sl_strength_;
  int pressed_strength_id_ = -1.0f;

 private:
  void RenderPickingEntry(glm::vec2 translate);

  UiSharedResources& ui_shared_resources_;
};

class UiEditConfigSlTxt {
 public:
  struct Trait {
    float* value;
    data::TextId label;
  };

  UiEditConfigSlTxt(UiSharedResources& ui_shared_resources,
                    TextRenderer& text_renderer);

  UiEditConfigSlTxt(UiEditConfigSlTxt&& other) noexcept = default;
  UiEditConfigSlTxt(const UiEditConfigSlTxt& other) = delete;

  UiEditConfigSlTxt& operator=(UiEditConfigSlTxt&& other) = delete;
  UiEditConfigSlTxt& operator=(const UiEditConfigSlTxt& other) = delete;

  bool Press(int id, float height, int num);

  void Release();

  void Render(float sprite_height, std::span<Trait> traits);

  void RenderPicking(float sprite_height, int num);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  UiSliderH2 sl_strength_;
  UiTextModeId txt_name_;

 private:
  void RenderEntry(float& strength, glm::vec2 translate, bool update_strength,
                   data::TextId text_id);

  void RenderPickingEntry(glm::vec2 translate);

  void ResetTransform();

  int pressed_strength_id_ = -1.0f;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
