#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_

#include <glm/glm.hpp>

#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiText.h"
#include "UiSharedResources.h"

class UiDynamicSprite;

int GetUiEditEntryId(const UiDynamicSprite* sprite, float size,
                     glm::vec2 mouse_pos, float start_height);

class UiEditConfigTerrain {
 public:
  UiEditConfigTerrain(UiSharedResources& ui_shared_resources,
                      TextRenderer& text_renderer);

  UiEditConfigTerrain(UiEditConfigTerrain&& other) noexcept = default;
  UiEditConfigTerrain(const UiEditConfigTerrain& other) = delete;

  UiEditConfigTerrain& operator=(UiEditConfigTerrain&& other) = delete;
  UiEditConfigTerrain& operator=(const UiEditConfigTerrain& other) = delete;

  /// so we could get GetTopBorder & GetBottomBorder and estimate position
  void ResetTransform();

  /// no Press(), Release() <- done in external ui_event_handler
  void Render(float& strength, bool do_invert, bool do_tiling,
              glm::vec2 translate, bool update_strength, data::TextId text_id);

  void RenderPicking(glm::vec2 translate);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  UiDynamicSprite btn_config_;
  UiTextModeId txt_name_;
  UiToggle4 tg_invert_;
  UiToggle4 tg_tiling_;
  UiSliderH2 sl_strength_;
  UiDynamicSprite sp_hmap_;

  int pressed_strength_id_ = -1.0f;

  UiSharedResources& ui_shared_resources_;
};

class UiEditConfigOcean {
 public:
  UiEditConfigOcean(UiSharedResources& ui_shared_resources,
                    TextRenderer& text_renderer);

  UiEditConfigOcean(UiEditConfigOcean&& other) noexcept = default;
  UiEditConfigOcean(const UiEditConfigOcean& other) = delete;

  UiEditConfigOcean& operator=(UiEditConfigOcean&& other) = delete;
  UiEditConfigOcean& operator=(const UiEditConfigOcean& other) = delete;

  /// so we could get GetTopBorder & GetBottomBorder and estimate position
  void ResetTransform();

  /// no Press(), Release() <- done in external ui_event_handler
  void Render(bool do_show, glm::vec2 translate, data::TextId text_id);

  void RenderPicking(glm::vec2 translate);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  UiDynamicSprite btn_config_;
  UiTextModeId txt_name_;
  UiToggle4 tg_visible_;

  UiSharedResources& ui_shared_resources_;
};

class UiEditConfigSlTxt {
 public:
  UiEditConfigSlTxt(UiSharedResources& ui_shared_resources,
                    TextRenderer& text_renderer, UiSliderH2&& sl_strength_,
                    UiTextModeId&& txt_name_);

  UiEditConfigSlTxt(UiEditConfigSlTxt&& other) noexcept = default;
  UiEditConfigSlTxt(const UiEditConfigSlTxt& other) = delete;

  UiEditConfigSlTxt& operator=(UiEditConfigSlTxt&& other) = delete;
  UiEditConfigSlTxt& operator=(const UiEditConfigSlTxt& other) = delete;

  /// so we could get GetTopBorder & GetBottomBorder and estimate position
  void ResetTransform();

  /// no Press(), Release() <- done in external ui_event_handler
  void Render(float& strength, glm::vec2 translate, bool update_strength,
              data::TextId text_id);

  void RenderPicking(glm::vec2 translate);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  UiSliderH2 sl_strength_;
  UiTextModeId txt_name_;

  int pressed_strength_id_ = -1.0f;

  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
