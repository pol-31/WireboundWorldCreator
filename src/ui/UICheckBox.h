#pragma once

#include <functional>

#include "UIStaticText.h"
#include "UITexturedQuad.h"

/// Check box control that allows the user to select between true or false
class UICheckBox : public UIStaticText
{
public:
	enum EState
	{
		STATE_UNCHECKED,
		STATE_CHECKED
	};

	using ClickAction = std::function<void(EState)>;

	/// Properties
	void				SetState(EState inState)							{ mState = inState; }
	EState				GetState() const									{ return mState; }
	void				SetClickAction(ClickAction inAction)				{ mClickAction = inAction; }
	void				SetUncheckedStateQuad(const UITexturedQuad &inQuad)	{ mUncheckedState = inQuad; }
	void				SetCheckedStateQuad(const UITexturedQuad &inQuad)	{ mCheckedState = inQuad; }

	/// When added to a parent
	virtual void		OnAdded() override;

	/// Cloning / copying
	//virtual void		CopyTo(UIElement *ioElement) const override;

	/// Actions
	virtual bool		MouseDown(int inX, int inY) override;
	virtual bool		MouseUp(int inX, int inY) override;
	virtual bool		MouseMove(int inX, int inY) override;
	virtual void		MouseCancel() override;

	/// Draw element
	virtual void		Draw() const override;

protected:
	/// Properties
	JPH::Color				mDownTextColor { JPH::Color::sGrey };
	JPH::Color				mHighlightTextColor { JPH::Color::sWhite };
	int					mPaddingBetweenCheckboxAndText = 8;
	ClickAction			mClickAction;
	UITexturedQuad		mUncheckedState;
	UITexturedQuad		mCheckedState;

	/// State
	EState				mState = STATE_UNCHECKED;
	bool				mPressed = false;
};
