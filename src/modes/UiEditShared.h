#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_

#include <glm/glm.hpp>
#include <span>

#include "../core/Ui.h"
#include "../core/UiComplex.h"
#include "../core/UiText.h"
#include "../ui/UiRenderData.h"

class UiSprite;

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

  UiEditConfigSlCfg(UiRenderData& render_data, TextRenderer& text_renderer);

  void Release();

  void Render(float& strength, bool do_invert, bool do_tiling,
              glm::vec2 translate, bool update_strength, data::TextId text_id);

  void RenderPicking(float sprite_height, int num);

  void AttachToHierarchy(UiHierarchy& hierarchy);

  void ResetTransform();

  UiSprite btn_config_;
  UiTextModeId txt_name_;
  UiToggle4 tg1_;
  UiToggle4 tg2_;
  UiSliderH2 sl_strength_;
  int pressed_strength_id_ = -1.0f;

 private:
  void RenderPickingEntry(glm::vec2 translate);

  UiRenderData& render_data_;
};

class UiEditConfigSlTxt {
 public:
  struct Trait {
    float* value;
    data::TextId label;
  };

  UiEditConfigSlTxt(UiRenderData& render_data, TextRenderer& text_renderer);

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
  UiRenderData& render_data_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_UIEDITSHARED_H_
