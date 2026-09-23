#pragma once

#include "UIElement.h"
#include "UITexturedQuad.h"

/// A static image UI element
class UIImage : public UIElement
{
public:
	/// Set properties
	void				SetImage(const UITexturedQuad &inImage)			{ mImage = inImage; }

	/// Cloning / copying
	//virtual void		CopyTo(UIElement *ioElement) const override;

	/// Draw element
	virtual void		Draw() const override;

private:
	UITexturedQuad		mImage;
};
