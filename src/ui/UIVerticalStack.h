#pragma once

#include "UIElement.h"

/// Layout class that will automatically layout child elements vertically, stacking them
class UIVerticalStack : public UIElement
{
public:
	/// Calculate auto layout
	virtual void		AutoLayout() override;

private:
	int					mDeltaY = 0;
	bool				mPlaceInvisibleChildren = false;
};
