#include "UIAnimationSlide.h"
#include "UIElement.h"
#include "../core/UiRenderer.h"
#include "../io/Window.h"

UIAnimationSlide::UIAnimationSlide(EMode inMode, int inSlideDistanceH, int inSlideDistanceV, float inTimeBeforeSlide, float inSlideTime) :
	mSlideMode(inMode),
	mSlideDistanceH(inSlideDistanceH),
	mSlideDistanceV(inSlideDistanceV),
	mTimeBeforeSlide(inTimeBeforeSlide),
	mSlideTime(inSlideTime)
{
  type_ = UiAnimationType::Slide;
}

void UIAnimationSlide::Init(UIElement *inElement)
{
	mTargetRelativeX = inElement->GetRelativeX();
	mTargetRelativeY = inElement->GetRelativeY();

	int dl = inElement->GetX();
	int dr = gWindowWidth - (inElement->GetX() + inElement->GetWidth());
	int dt = inElement->GetY();
	int db = gWindowHeight - (inElement->GetY() + inElement->GetHeight());

	if (std::min(dl, dr) < std::min(dt, db))
	{
		mInitialRelativeX = mTargetRelativeX + (dl < dr? -mSlideDistanceH : mSlideDistanceH);
		mInitialRelativeY = mTargetRelativeY;
	}
	else
	{
		mInitialRelativeX = mTargetRelativeX;
		mInitialRelativeY = mTargetRelativeY + (dt < db? -mSlideDistanceV : mSlideDistanceV);
	}

	if (mSlideMode == SLIDE_ON_SCREEN)
		inElement->SetAnimatedVisible(true);

	mTime = 0.0f;
}

bool UIAnimationSlide::Update(UIElement *inElement, float inDeltaTime)
{
	mTime += inDeltaTime;

	float factor = (mTime - mTimeBeforeSlide) / mSlideTime;
	if (factor >= 1.0f)
		return false;
	if (factor < 0.0f)
		factor = 0.0f;

	if (mSlideMode == SLIDE_OFF_SCREEN)
		factor = 1.0f - factor;

	float x = mInitialRelativeX * (1.0f - factor) + mTargetRelativeX * factor;
	float y = mInitialRelativeY * (1.0f - factor) + mTargetRelativeY * factor;

	inElement->SetRelativeX((int)x);
	inElement->SetRelativeY((int)y);
	return true;
}

void UIAnimationSlide::Exit(UIElement *inElement)
{
	inElement->SetRelativeX(mTargetRelativeX);
	inElement->SetRelativeY(mTargetRelativeY);

	inElement->SetAnimatedVisible(mSlideMode == SLIDE_ON_SCREEN);
}
