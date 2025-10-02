#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_UIBUTTONS_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_UIBUTTONS_H_

#include <array>
#include <vector>

#include "Ui.h"
#include "UiText.h"

class UiButtons {
 public:
  UiButtons(UiSharedResources& ui_shared_resources,
            TextRenderer& text_renderer);

  void Press(int key, int action);

  void Render();

  void RenderPicking();

 private:
  void RenderModifier(bool value, std::string_view text);

  void AddKey(int key);

  void RemoveKey(int idx);

  void UpdateModsText();

  void UpdateKeyText();

  UiDynamicSprite sp_key_;
  UiText txt_mods_;
  UiText txt_keys_;
  // we could add shift_r_, shift_l_ to handle both sides,
  // but let's toggle it so both pressed ALTs == neither of them
  bool alt_ = false;
  bool ctrl_ = false;
  bool shift_ = false;

  static int gIndentMod;
  static int gIndentKey;

  // text components: "alt ctrl W G R" from "alt ctrl " and "W G R"
  std::string text_mods_;
  std::string text_keys_;

  std::vector<int> symbol_ids_;
  UiSharedResources& ui_shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_UIBUTTONS_H_
