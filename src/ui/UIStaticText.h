#pragma once

#include "UIElement.h"

class UiScene;

/// Static text string
class UIStaticText : public UIElement
{
public:
	/// Cloning / copying
	//virtual void		CopyTo(UIElement *ioElement) const override;

	/// Set properties
	void				SetTextColor(JPH::ColorArg inColor)				{ mTextColor = inColor; }
	void				SetDisabledTextColor(JPH::ColorArg inColor)		{ mDisabledTextColor = inColor; }
	void				SetFont(const UiScene *inFont)					{ mFont = inFont; }
	void				SetText(const std::string_view &inText)			{ mText = inText; }
	void				SetTextPadding(int inTop, int inLeft, int inBottom, int inRight) { mTextPadTop = inTop; mTextPadLeft = inLeft; mTextPadBottom = inBottom; mTextPadRight = inRight; }
	void				SetTextAlignment(EAlignment inAlignment)	{ JPH_ASSERT(inAlignment == LEFT || inAlignment == RIGHT || inAlignment == CENTER); mTextAlignment = inAlignment; }
	void				SetWrap(bool inWrap)						{ mWrap = inWrap; }

	/// Draw element
	virtual void		Draw() const override;

	/// Calculate auto layout
	virtual void		AutoLayout() override;

protected:
	/// Draw element custom
	void				DrawCustom(JPH::ColorArg inColor) const;

	std::string				GetWrappedText() const;

	const UiScene*		mFont;
	std::string				mText;
	JPH::Color				mTextColor { JPH::Color(220, 220, 200) };
	JPH::Color				mDisabledTextColor { JPH::Color::sGrey };
	int					mTextPadLeft = 0;
	int					mTextPadRight = 0;
	int					mTextPadTop = 0;
	int					mTextPadBottom = 0;
	EAlignment			mTextAlignment = LEFT;
	bool				mWrap = false;
};
