#include "DebugUI.h"

#include <fstream>

#include "../core/UiScene.h"
#include "UICheckBox.h"
#include "UIHorizontalStack.h"
#include "UIImage.h"
#include "UIStaticText.h"
#include "UITextButton.h"
#include "UIVerticalStack.h"

DebugUI::DebugUI(UiScene *inUiRenderer, const Texture *texture)
    : mUI(inUiRenderer), mUITexture(texture) {
  mUI->SetDeactivatedAction([this]() { mUI->PopLayer(); });
  mUI->SetDrawInactiveLayers(false);
}

UIElement *DebugUI::CreateMenu() {
  mUI->PushLayer();

  UIImage *background = new UIImage();
  // background->SetPaddingRight(100);
  // background->SetPaddingBottom(-50);
  background->SetImage("DeskHuge");
  // background->SetWidth(400);
  // background->SetHeight(400);
  // background->SetImage(UITexturedQuad(mUITexture, 0, 0, 33, 30, 4, 4, 24,
  // 21));
  mUI->Add(background);

  UIVerticalStack *stack = new UIVerticalStack();
  stack->SetRelativeX(60);
  stack->SetRelativeY(100);
  stack->SetPaddingRight(100);
  stack->SetPaddingBottom(100);
  // stack->SetWidth(400);
  background->Add(stack);

  return stack;
}

UIStaticText *DebugUI::CreateStaticText(UIElement *inMenu,
                                        const std::string_view &inText) {
  UIStaticText *text = new UIStaticText();
  text->SetText(inText);
  // text->SetFont(mFont);
  inMenu->Add(text);
  return text;
}

UITextButton *DebugUI::CreateTextButton(UIElement *inMenu,
                                        const std::string_view &inName,
                                        UITextButton::ClickAction inAction) {
  UITextButton *button = new UITextButton();
  button->SetText(inName);
  // button->SetHeight(8);
  //  button->SetFont(mFont);
  button->SetClickAction(inAction);
  button->SetTextPadding(0, 24, 0, 0);
  button->SetPaddingRight(24);
  inMenu->Add(button);
  return button;
}

UICheckBox *DebugUI::CreateCheckBox(UIElement *inMenu,
                                    const std::string_view &inName,
                                    bool inInitiallyChecked,
                                    UICheckBox::ClickAction inAction) {
  UICheckBox *check_box = new UICheckBox();
  check_box->SetUncheckedStateQuad(UITexturedQuad(mUITexture, 48, 0, 16, 16));
  check_box->SetCheckedStateQuad(UITexturedQuad(mUITexture, 65, 0, 16, 16));
  // check_box->SetFont(mFont);
  check_box->SetText(inName);
  check_box->SetClickAction(inAction);
  check_box->SetState(inInitiallyChecked ? UICheckBox::STATE_CHECKED
                                         : UICheckBox::STATE_UNCHECKED);
  check_box->SetPaddingRight(24);
  inMenu->Add(check_box);
  return check_box;
}

UISlider *DebugUI::CreateSlider(UIElement *inMenu,
                                const std::string_view &inName,
                                float inInitialValue, float inMinValue,
                                float inMaxValue, float inStepValue,
                                UISlider::ValueChangedAction inAction) {
  UIHorizontalStack *horiz = new UIHorizontalStack();
  // horiz->SetPaddingRight(24);
  inMenu->Add(horiz);

  UIStaticText *text = new UIStaticText();
  // text->SetFont(mFont);
  // text->SetTextPadding(0, 24, 0, 0);
  // text->SetHeight(24);
  //text->SetWidth(300);
  text->SetText(inName);
  // text->SetPaddingRight(20);
  horiz->Add(text);

  UISlider *slider = new UISlider();
  slider->SetWidth(150);
  slider->SetPaddingRight(20);
  slider->SetValue(inInitialValue);
  slider->SetRange(inMinValue, inMaxValue, inStepValue);
  slider->SetValueChangedAction(inAction);
  slider->SetSlider(UITexturedQuad(mUITexture, 44, 37, 1, 9));
  slider->SetThumb(UITexturedQuad(mUITexture, 31, 32, 11, 19));
  horiz->Add(slider);

  UIButton *decr_button = new UIButton();
  decr_button->SetRepeat(0.5f, 0.2f);
  decr_button->SetButtonQuad(UITexturedQuad(mUITexture, 0, 31, 17, 21));
  decr_button->SetHeight(20);
  slider->Add(decr_button);
  slider->SetDecreaseButton(decr_button);

  UIButton *incr_button = new UIButton();
  incr_button->SetRepeat(0.5f, 0.2f);
  incr_button->SetButtonQuad(UITexturedQuad(mUITexture, 13, 31, 17, 21));
  incr_button->SetHeight(20);
  slider->Add(incr_button);
  slider->SetIncreaseButton(incr_button);

  UIImage *image = new UIImage();
  image->SetImage("Loading0");
  image->SetHeight(20);
  // image->SetImage(UITexturedQuad(mUITexture, 34, 0, 13, 24, 36, 2, 9, 20));
  horiz->Add(image);

  UIStaticText *value = new UIStaticText();
  // value->SetWidth(75);
  value->SetTextPadding(0, 20, 0, 0);
  // value->SetWrap(true);
  // value->SetHeight(20);
  // value->SetTextAlignment(UIElement::CENTER);
  // value->SetFont(mFont);
  image->Add(value);
  slider->SetStaticText(value);

  return slider;
}

UIComboBox *DebugUI::CreateComboBox(UIElement *inMenu,
                                    const std::string_view &inName,
                                    const std::vector<std::string> &inItems,
                                    int inInitialItem,
                                    UIComboBox::ItemChangedAction inAction) {
  UIHorizontalStack *horiz = new UIHorizontalStack();
  horiz->SetPaddingRight(24);
  inMenu->Add(horiz);

  UIStaticText *text = new UIStaticText();
  // text->SetFont(mFont);
  text->SetTextPadding(0, 24, 0, 0);
  text->SetText(inName);
  text->SetPaddingRight(20);
  horiz->Add(text);

  UIComboBox *combo = new UIComboBox();
  combo->SetHeight(24);
  combo->SetWidth(250);
  combo->SetPaddingRight(20);
  combo->SetItems(inItems);
  combo->SetCurrentItem(inInitialItem);
  combo->SetItemChangedAction(inAction);
  horiz->Add(combo);

  UIButton *prev_button = new UIButton();
  prev_button->SetRepeat(0.5f, 0.2f);
  prev_button->SetButtonQuad(UITexturedQuad(mUITexture, 0, 31, 17, 21));
  combo->Add(prev_button);
  combo->SetPreviousButton(prev_button);

  UIButton *next_button = new UIButton();
  next_button->SetRepeat(0.5f, 0.2f);
  next_button->SetButtonQuad(UITexturedQuad(mUITexture, 13, 31, 17, 21));
  combo->Add(next_button);
  combo->SetNextButton(next_button);

  UIStaticText *value = new UIStaticText();
  value->SetTextPadding(0, 5, 0, 5);
  value->SetWrap(false);
  value->SetTextAlignment(UIElement::CENTER);
  // value->SetFont(mFont);
  combo->Add(value);
  combo->SetStaticText(value);

  return combo;
}

void DebugUI::ShowMenu(UIElement *inMenu) {
  UIHorizontalStack::sUniformChildWidth(inMenu);
  mUI->AutoLayout();
  mUI->SwitchToState(UiScene::STATE_ACTIVATING);
}

void DebugUI::BackToMain() {
  while (mUI->GetNumLayers() > 2) mUI->PopLayer();
}

void DebugUI::ToggleVisibility() {
  if (mUI->GetNumLayers() > 2)
    mUI->SwitchToState(UiScene::STATE_DEACTIVATING);
  else
    mUI->SetVisible(!mUI->IsVisible());
}
