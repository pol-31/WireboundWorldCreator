#include "UIImage.h"

#include "../core/UiScene.h"

void UIImage::Draw() const
{
	GetManager()->DrawQuad(GetX(), GetY(), GetWidth(), GetHeight(), mImage, JPH::Color::sWhite);

	UIElement::Draw();
}

// void UIImage::CopyTo(UIElement *ioElement) const
// {
// 	UIElement::CopyTo(ioElement);
//
// 	UIImage *element = StaticCast<UIImage>(ioElement);
// 	element->mImage = mImage;
// }
