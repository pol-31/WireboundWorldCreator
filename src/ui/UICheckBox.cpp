#include "UICheckBox.h"

#include "../core/UiScene.h"

// void UICheckBox::CopyTo(UIElement *ioElement) const
// {
// 	UIStaticText::CopyTo(ioElement);
//
// 	UICheckBox *element = StaticCast<UICheckBox>(ioElement);
// 	element->mDownTextColor = mDownTextColor;
// 	element->mHighlightTextColor = mHighlightTextColor;
// 	element->mPaddingBetweenCheckboxAndText = mPaddingBetweenCheckboxAndText;
// 	element->mState = mState;
// 	element->mUncheckedState = mUncheckedState;
// 	element->mCheckedState = mCheckedState;
// 	element->mClickAction = mClickAction;
// }

void UICheckBox::OnAdded()
{
	mTextPadLeft = std::max(mUncheckedState.mWidth, mCheckedState.mWidth) + mPaddingBetweenCheckboxAndText;
}

bool UICheckBox::MouseDown(int inX, int inY)
{
	if (UIStaticText::MouseDown(inX, inY))
		return true;

	if (Contains(inX, inY))
	{
		mPressed = true;
		return true;
	}

	return false;
}

bool UICheckBox::MouseUp(int inX, int inY)
{
	if (UIStaticText::MouseUp(inX, inY))
		return true;

	if (mPressed)
	{
		mPressed = false;

		if (Contains(inX, inY))
		{
			mState = mState == STATE_CHECKED? STATE_UNCHECKED : STATE_CHECKED;

			if (mClickAction)
				mClickAction(mState);
		}
		return true;
	}

	return false;
}

bool UICheckBox::MouseMove(int inX, int inY)
{
	if (UIStaticText::MouseMove(inX, inY))
		return true;

	return mPressed;
}

void UICheckBox::MouseCancel()
{
	UIStaticText::MouseCancel();

	mPressed = false;
}

void UICheckBox::Draw() const
{
	JPH::Color color = IsDisabled()? mDisabledTextColor : (mPressed? mDownTextColor : (mIsHighlighted? mHighlightTextColor : mTextColor));

	UIStaticText::DrawCustom(color);

	if (mState == STATE_UNCHECKED)
		GetManager()->DrawQuad(GetX(), GetY() + (GetHeight() - mUncheckedState.mHeight) / 2, mUncheckedState.mWidth, mUncheckedState.mHeight, mUncheckedState, color);
	else if (mState == STATE_CHECKED)
		GetManager()->DrawQuad(GetX(), GetY() + (GetHeight() - mCheckedState.mHeight) / 2, mCheckedState.mWidth, mCheckedState.mHeight, mCheckedState, color);

	// Skip direct base class, we modify the text color
	UIElement::Draw();
}
