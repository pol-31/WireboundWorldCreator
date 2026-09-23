#pragma once

#include "UITextButton.h"
#include "UITexturedQuad.h"

/// Button with a background image and text on it
class UIButton : public UITextButton
{
public:

	/// Cloning / copying
	//virtual void		CopyTo(UIElement *ioElement) const override;

	/// Draw element
	virtual void		Draw() const override;

	/// Set quad
	void				SetButtonQuad(const UITexturedQuad &inQuad);

private:
	UITexturedQuad		mUpQuad;
	JPH::Color				mUpColor { JPH::Color(220, 220, 220) };
	UITexturedQuad		mDownQuad;
	JPH::Color				mDownColor { JPH::Color::sGrey };
	UITexturedQuad		mHighlightQuad;
	JPH::Color				mHighlightColor { JPH::Color::sWhite };
	UITexturedQuad		mSelectedQuad;
	JPH::Color				mSelectedColor { JPH::Color::sWhite };
	UITexturedQuad		mDisabledQuad;
	JPH::Color				mDisabledColor { JPH::Color::sGrey };
};
