#pragma once

#include <string_view>

#include "UiElement.h"
#include "UICheckBox.h"
#include "UITextButton.h"
#include "UISlider.h"
#include "UIComboBox.h"
#include "UIStaticText.h"

#include "../render/Texture.h"
#include "../common/Font.h"

class Texture;
class UiScene;
class Keyboard;

class DebugUI
{
public:
	/// Constructor
						DebugUI(UiScene *inUiRenderer,
						  const Texture* texture);

	/// Create a new (sub) menu
	UIElement *			CreateMenu();

	/// Add items to the menu
	UIStaticText *		CreateStaticText(UIElement *inMenu, const std::string_view &inText);
	UITextButton *		CreateTextButton(UIElement *inMenu, const std::string_view &inName, UITextButton::ClickAction inAction);
	UICheckBox *		CreateCheckBox(UIElement *inMenu, const std::string_view &inName, bool inInitiallyChecked, UICheckBox::ClickAction inAction);
	UISlider *			CreateSlider(UIElement *inMenu, const std::string_view &inName, float inInitialValue, float inMinValue, float inMaxValue, float inStepValue, UISlider::ValueChangedAction inAction);
	UIComboBox *		CreateComboBox(UIElement *inMenu, const std::string_view &inName, const std::vector<std::string> &inItems, int inInitialItem, UIComboBox::ItemChangedAction inAction);

	/// Show it
	void				ShowMenu(UIElement *inMenu);

	/// Go back to the main menu
	void				BackToMain();

	/// Show or hide the entire menu
	void				ToggleVisibility();

private:
	UiScene* mUI;
	const Texture* mUITexture;
};
