#include "UIHorizontalStack.h"

void UIHorizontalStack::sUniformChildWidth(UIElement *inParent)
{
	std::vector<int> sizes;
	sizes.resize(1, 0);
	for (UIElement *e : inParent->GetChildren())
	{
		e->AutoLayout();

		auto horiz = dynamic_cast<UIHorizontalStack*>(e);
		if (horiz != nullptr)
		{
			if (horiz->GetNumChildren() > (int)sizes.size())
				sizes.resize(horiz->GetNumChildren(), 0);
			for (int i = 0; i < horiz->GetNumChildren(); ++i)
				sizes[i] = std::max(sizes[i], horiz->GetChild(i)->GetWidth());
		}
		else
		{
			sizes[0] = std::max(sizes[0], e->GetWidth());
		}
	}

	for (UIElement *e : inParent->GetChildren())
	{
		auto horiz = dynamic_cast<UIHorizontalStack*>(e);
		if (horiz != nullptr)
		{
			for (int i = 0; i < horiz->GetNumChildren(); ++i)
				horiz->GetChild(i)->SetWidth(sizes[i]);
		}
		else
		{
			e->SetWidth(sizes[0]);
		}
	}
}

void UIHorizontalStack::AutoLayout()
{
	UIElement::AutoLayout();

	mWidth.Set(0, PIXELS);
	for (UIElement *e : mChildren)
		if (e->IsVisible() || mPlaceInvisibleChildren)
		{
			e->SetRelativeX(GetWidth());
			mWidth.Set(GetWidth() + e->GetWidth() + e->GetPaddingRight() + mDeltaX, PIXELS);
		}
}
