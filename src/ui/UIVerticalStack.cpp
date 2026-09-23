#include "UIVerticalStack.h"

void UIVerticalStack::AutoLayout()
{
	UIElement::AutoLayout();

	mHeight.Set(0, PIXELS);
	for (UIElement *e : mChildren)
		if (e->IsVisible() || mPlaceInvisibleChildren)
		{
			e->SetRelativeY(GetHeight());
			mHeight.Set(GetHeight() + e->GetHeight() + e->GetPaddingBottom() + mDeltaY, PIXELS);
		}
}
