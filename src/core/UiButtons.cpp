#include "UiButtons.h"

int UiButtons::gIndentMod = 12;
int UiButtons::gIndentKey = 10;

UiButtons::UiButtons(UiSharedResources& ui_shared_resources,
                     TextRenderer& text_renderer)
    : ui_shared_resources_(ui_shared_resources),
      txt_keys_(text_renderer, data::VboIdMain::kButtonsTextKeys),
      txt_mods_(text_renderer, data::VboIdMain::kButtonsTextMods),
      sp_key_(data::VboIdMain::kPressedKey) {}

void UiButtons::Press(int key, int action) {
  if (action == GLFW_REPEAT) {
    return;
  }
  if (key >= GLFW_KEY_LEFT_SHIFT && key <= GLFW_KEY_RIGHT_ALT) {
    /// alt, ctrl, shift mods; doesn't matter GLFW_PRESS or GLFW_RELEASE,
    /// we simply toggle the state
    alt_ ^= key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT;
    ctrl_ ^= key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL;
    shift_ ^= key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT;
    UpdateModsText();
  } else if (key > GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT) {
    if (action == GLFW_RELEASE) {
      RemoveKey(key);
    } else {  // GLFW_PRESS, GLFW_REPEAT
      AddKey(key);
    }
  }
}

void UiButtons::UpdateModsText() {
  text_mods_.clear();
  int total_mods = static_cast<int>(alt_) + static_cast<int>(ctrl_) +
                   static_cast<int>(shift_);
  if (!total_mods) {
    return;
  }
  std::string alt_text = " alt";
  std::string ctrl_text = "ctrl";
  std::string shift_text = "shift";
  int half_1, half_2;
  half_1 = static_cast<int>(gIndentMod / 2.0f);
  half_2 = gIndentMod - half_1;
  /// wrap
  for (int i = 0; i < half_1; ++i) {
    text_mods_ += ' ';
  }
  if (total_mods == 1) {
    if (alt_) {
      text_mods_ += alt_text;
    } else if (ctrl_) {
      text_mods_ += ctrl_text;
    } else {
      text_mods_ += shift_text;
    }
  } else if (total_mods == 2) {
    if (alt_) {
      text_mods_ += alt_text;
      for (int i = 0; i < gIndentMod; ++i) {
        text_mods_ += ' ';
      }
      if (ctrl_) {
        text_mods_ += ctrl_text;
      } else {
        text_mods_ += shift_text;
      }
    } else {
      text_mods_ += ctrl_text;
      for (int i = 0; i < gIndentMod; ++i) {
        text_mods_ += ' ';
      }
      text_mods_ += shift_text;
    }
  } else {  // 3
    text_mods_ += alt_text;
    for (int i = 0; i < gIndentMod; ++i) {
      text_mods_ += ' ';
    }
    text_mods_ += ctrl_text;
    for (int i = 0; i < gIndentMod; ++i) {
      text_mods_ += ' ';
    }
    text_mods_ += shift_text;
  }
  /// unwrap
  for (int i = 0; i < half_2; ++i) {
    text_mods_ += ' ';
  }
}

void UiButtons::UpdateKeyText() {
  text_keys_.clear();
  for (auto id : symbol_ids_) {
    // glfw code to ascii
    text_keys_ += static_cast<char>(id - GLFW_KEY_A + 97);
    for (int i = 0; i < gIndentKey; ++i) {
      text_keys_ += ' ';
    }
  }
}

void UiButtons::AddKey(int key) {
  symbol_ids_.push_back(key);
  UpdateKeyText();
}

void UiButtons::RemoveKey(int idx) {
  for (auto it = symbol_ids_.begin(); it != symbol_ids_.end(); ++it) {
    if (*it == idx) {
      symbol_ids_.erase(it);
      break;
    }
  }
  UpdateKeyText();
}

void UiButtons::Render() {
  ui_shared_resources_.tex_ui_.Bind();
  ui_shared_resources_.shader_sp_.Bind();
  sp_key_.SetScale(0.9f);  // 1.0f for modifiers (todo; gVar)
  // wrt scale
  glm::vec2 half_length(
      (sp_key_.GetRightBorder() - sp_key_.GetLeftBorder()) / 2.0f, 0.0f);
  glm::vec2 offset(-half_length);
  int total_mods = static_cast<int>(alt_) + static_cast<int>(ctrl_) +
                   static_cast<int>(shift_);
  for (int i = 0; i < total_mods; ++i) {
    offset += half_length;
    sp_key_.SetTranslate(offset);
    sp_key_.Render();
    offset += half_length;
  }
  sp_key_.SetScale(0.65f);  // 1.0f for keys (todo; gVar)
  half_length = glm::vec2(
      (sp_key_.GetRightBorder() - sp_key_.GetLeftBorder()) / 2.0f, 0.0f);
  LocalTransform keys_offset;
  keys_offset.translate = offset + half_length;
  txt_keys_.SetParentTransform(keys_offset);
  for (int i = 0; i < symbol_ids_.size(); ++i) {
    offset += half_length;
    sp_key_.SetTranslate(offset);
    sp_key_.Render();
    offset += half_length;
  }
  txt_mods_.SetText(text_mods_);
  txt_mods_.Render();
  txt_keys_.SetText(text_keys_);
  txt_keys_.Render();
}

void UiButtons::RenderModifier(bool value, std::string_view text) {
  if (!value) {
    return;
  }
}

void UiButtons::RenderPicking() {
  ui_shared_resources_.shader_sp_picking_.Bind();
  sp_key_.RenderPicking();
  txt_mods_.RenderPicking();
  txt_keys_.RenderPicking();
}
