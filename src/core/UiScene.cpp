#include "UiScene.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../io/Window.h"
#include "../ui/UIAnimationSlide.h"
#include "ReadData.h"
#include "UiRenderer.h"

UiScene::UiScene(UiRenderer* ui_renderer)
: ui_renderer_(ui_renderer),
mState(STATE_INVALID) {
  mManager = this;
  // Set dimensions of the screen
  SetWidth(gWindowWidth);
  SetHeight(gWindowHeight);
}

[[nodiscard]] int UiScene::GetCharHeight() const noexcept {
  return ui_renderer_->GetCharHeight();
}

void UiScene::AddSprite(const std::string& name, glm::vec2 position_pix, glm::vec2 size_pix, glm::vec4 color) {
  ui_renderer_->AddSprite(name, position_pix, size_pix, color);
}

void UiScene::AddText(std::string_view text, glm::vec2 position_pix,
  glm::vec2 scale, glm::vec4 color) {
  ui_renderer_->AddText(text, position_pix, scale, color);
}

glm::vec2 UiScene::MeasureText(std::string_view text) const {
  return ui_renderer_->MeasureText(text);
}

UiScene::~UiScene()
{
	while (!mInactiveElements.empty())
		PopLayer();
}

// Update elements
void UiScene::Update(float inDeltaTime)
{
	// Update inactive elements (array can resize at any time, so no iterators and extra checking here)
	for (int i = (int)mInactiveElements.size() - 1; i >= 0; --i)
		for (int j = 0; i < (int)mInactiveElements.size() && j < (int)mInactiveElements[i].size(); ++j)
			mInactiveElements[i][j]->Update(inDeltaTime);

	// Update elements
	UIElement::Update(inDeltaTime);

	// Update state
	mStateTime += inDeltaTime;
	switch (mState)
	{
	case STATE_ACTIVATING:
		if (mStateTime > cActivateScreenTime)
			SwitchToState(STATE_ACTIVE);
		break;

	case STATE_DEACTIVATING:
		if (mStateTime > cActivateScreenTime)
			SwitchToState(STATE_DEACTIVE);
		break;

	case STATE_ACTIVE:
	case STATE_DEACTIVE:
	case STATE_INVALID:
	default:
		break;
	}
}

void UiScene::Draw() const
{
	// Draw inactive elements first
	if (mDrawInactiveElements)
		for (int i = (int)mInactiveElements.size() - 1; i >= 0; --i)
			for (const UIElement *j : mInactiveElements[i])
				if (j->IsVisible())
					j->Draw();

	// Then draw active elements
	UIElement::Draw();
}

void UiScene::PushLayer()
{
	mInactiveElements.push_back(mChildren);
	mChildren.clear();
}

void UiScene::PopLayer()
{
	Clear();

	mChildren = mInactiveElements.back();
	mInactiveElements.pop_back();
}

UIElement *UiScene::FindByID(int inID)
{
	UIElement *element = UIElement::FindByID(inID);
	if (element != nullptr)
		return element;

	for (int i = (int)mInactiveElements.size() - 1; i >= 0; --i)
		for (int j = 0; j < (int)mInactiveElements[i].size(); ++j)
	{
		element = mInactiveElements[i][j]->FindByID(inID);
		if (element != nullptr)
			return element;
	}

	return nullptr;
}

bool UiScene::HandleUIEvent(EUIEvent inEvent, UIElement *inSender)
{
  return UIElement::HandleUIEvent(inEvent, inSender);

//	if (UIElement::HandleUIEvent(inEvent, inSender))
//		return true;
//	return mListener != nullptr && mListener->HandleUIEvent(inEvent, inSender);
}

void UiScene::GetMaxElementDistanceToScreenEdge(int &outMaxH, int &outMaxV){
	outMaxH = 0;
	outMaxV = 0;

	for (const UIElement *e : mChildren)
		if (e->HasDeactivateAnimation())
		{
			int dl = e->GetX() + e->GetWidth();
			int dr = gWindowWidth - e->GetX();
			outMaxH = std::max(outMaxH, std::min(dl, dr));
			int dt = e->GetY() + e->GetHeight();
			int db = gWindowHeight - e->GetY();
			outMaxV = std::max(outMaxV, std::min(dt, db));
		}
}

void UiScene::SwitchToState(EState inState)
{
  // Clean up old state
  switch (mState)
  {
    case STATE_ACTIVATING:
    case STATE_DEACTIVATING:
      for (UIElement *e : mChildren)
        e->StopAnimation(UiAnimationType::Slide);
      break;

    case STATE_ACTIVE:
    case STATE_DEACTIVE:
    case STATE_INVALID:
    default:
      break;
  }

  // Store new state
  mState = inState;
  mStateTime = 0.0f;

  // Calculate max horizontal and vertical distance of elements to edge of screen
  int max_h, max_v;
  GetMaxElementDistanceToScreenEdge(max_h, max_v);

  switch (inState)
  {
    case STATE_ACTIVATING:
      for (UIElement *e : mChildren)
        if (e->HasActivateAnimation())
          e->StartAnimation(new UIAnimationSlide(UIAnimationSlide::SLIDE_ON_SCREEN, max_h, max_v, 0.0f, cActivateScreenTime));
      break;

    case STATE_DEACTIVATING:
      for (UIElement *e : mChildren)
        if (e->HasDeactivateAnimation())
          e->StartAnimation(new UIAnimationSlide(UIAnimationSlide::SLIDE_OFF_SCREEN, max_h, max_v, 0.0f, cActivateScreenTime));
      break;

    case STATE_DEACTIVE:
      HandleUIEvent(EVENT_MENU_DEACTIVATED, this);

      if (mDeactivatedAction)
        mDeactivatedAction();
      break;

    case STATE_ACTIVE:
    case STATE_INVALID:
    default:
      break;
  }
}

// inline static void sDrawQuad(Vertex *&v ,float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2, JPH::ColorArg inColor)
// {
// 	v->mPosition = Float3(x1, y1, 0);
// 	v->mTexCoord = Float2(tx1, ty1);
// 	v->mColor = inColor;
// 	++v;
//
// 	v->mPosition = Float3(x1, y2, 0);
// 	v->mTexCoord = Float2(tx1, ty2);
// 	v->mColor = inColor;
// 	++v;
//
// 	v->mPosition = Float3(x2, y2, 0);
// 	v->mTexCoord = Float2(tx2, ty2);
// 	v->mColor = inColor;
// 	++v;
//
// 	v->mPosition = Float3(x1, y1, 0);
// 	v->mTexCoord = Float2(tx1, ty1);
// 	v->mColor = inColor;
// 	++v;
//
// 	v->mPosition = Float3(x2, y2, 0);
// 	v->mTexCoord = Float2(tx2, ty2);
// 	v->mColor = inColor;
// 	++v;
//
// 	v->mPosition = Float3(x2, y1, 0);
// 	v->mTexCoord = Float2(tx2, ty1);
// 	v->mColor = inColor;
// 	++v;
// }
//
// void UiRenderer::DrawQuad(int inX, int inY, int inWidth, int inHeight, const UITexturedQuad &inQuad, ColorArg inColor)
// {
// 	// Outer area - screen coordinates
// 	float x1 = float(inX);
// 	float y1 = float(inY);
// 	float x2 = float(inX + inWidth);
// 	float y2 = float(inY + inHeight);
//
// 	if (inQuad.mTexture != nullptr)
// 	{
// 		bool has_inner = inQuad.HasInnerPart();
//
// 		Ref<RenderPrimitive> primitive = mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Triangle);
// 		primitive->CreateVertexBuffer(has_inner? 9 * 6 : 6, sizeof(Vertex));
//
// 		float w = float(inQuad.mTexture->GetWidth()), h = float(inQuad.mTexture->GetHeight());
//
// 		// Outer area - texture coordinates
// 		float tx1 = float(inQuad.mX);
// 		float ty1 = float(inQuad.mY);
// 		float tx2 = float(inQuad.mX + inQuad.mWidth);
// 		float ty2 = float(inQuad.mY + inQuad.mHeight);
// 		tx1 /= w; ty1 /= h;
// 		tx2 /= w; ty2 /= h;
//
// 		Vertex *v = (Vertex *)primitive->LockVertexBuffer();
//
// 		if (has_inner)
// 		{
// 			// Inner area - screen coordinates
// 			float ix1 = float(inX + inQuad.mInnerX - inQuad.mX);
// 			float iy1 = float(inY + inQuad.mInnerY - inQuad.mY);
// 			float ix2 = float(inX + inWidth - (inQuad.mWidth - inQuad.mInnerWidth - (inQuad.mInnerX - inQuad.mX)));
// 			float iy2 = float(inY + inHeight - (inQuad.mHeight - inQuad.mInnerHeight - (inQuad.mInnerY - inQuad.mY)));
//
// 			// Inner area - texture coordinates
// 			float itx1 = float(inQuad.mInnerX);
// 			float ity1 = float(inQuad.mInnerY);
// 			float itx2 = float(inQuad.mInnerX + inQuad.mInnerWidth);
// 			float ity2 = float(inQuad.mInnerY + inQuad.mInnerHeight);
// 			itx1 /= w; ity1 /= h;
// 			itx2 /= w; ity2 /= h;
//
// 			sDrawQuad(v, x1, y1, ix1, iy1, tx1, ty1, itx1, ity1, inColor);
// 			sDrawQuad(v, ix1, y1, ix2, iy1, itx1, ty1, itx2, ity1, inColor);
// 			sDrawQuad(v, ix2, y1, x2, iy1, itx2, ty1, tx2, ity1, inColor);
//
// 			sDrawQuad(v, x1, iy1, ix1, iy2, tx1, ity1, itx1, ity2, inColor);
// 			sDrawQuad(v, ix1, iy1, ix2, iy2, itx1, ity1, itx2, ity2, inColor);
// 			sDrawQuad(v, ix2, iy1, x2, iy2, itx2, ity1, tx2, ity2, inColor);
//
// 			sDrawQuad(v, x1, iy2, ix1, y2, tx1, ity2, itx1, ty2, inColor);
// 			sDrawQuad(v, ix1, iy2, ix2, y2, itx1, ity2, itx2, ty2, inColor);
// 			sDrawQuad(v, ix2, iy2, x2, y2, itx2, ity2, tx2, ty2, inColor);
// 		}
// 		else
// 		{
// 			sDrawQuad(v, x1, y1, x2, y2, tx1, ty1, tx2, ty2, inColor);
// 		}
//
// 		primitive->UnlockVertexBuffer();
// 		inQuad.mTexture->Bind();
// 		mTextured->Activate();
// 		primitive->Draw();
// 	}
// 	else
// 	{
// 		Ref<RenderPrimitive> primitive = mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Triangle);
// 		primitive->CreateVertexBuffer(6, sizeof(Vertex));
// 		Vertex *v = (Vertex *)primitive->LockVertexBuffer();
// 		sDrawQuad(v, x1, y1, x2, y2, 0, 0, 0, 0, inColor);
// 		primitive->UnlockVertexBuffer();
// 		mUntextured->Activate();
// 		primitive->Draw();
// 	}
// }
//
// void UiRenderer::DrawText(int inX, int inY, const string_view &inText, const UiRenderer *inFont, ColorArg inColor)
// {
// 	Vec4 pos(float(inX), float(inY), 0.0f, 1.0f);
// 	Vec4 right(float(inFont->GetCharHeight()), 0.0f, 0.0f, 0.0f);
// 	Vec4 up(0.0f, float(-inFont->GetCharHeight()), 0.0f, 0.0f);
// 	Vec4 forward(0.0f, 0.0f, 1.0f, 0.0f);
// 	Mat44 transform(right, up, forward, pos);
// 	inFont->DrawText3D(transform, inText, inColor);
// }
void UiScene::DrawQuad(int inX, int inY, int inWidth, int inHeight, const UITexturedQuad &inQuad, JPH::ColorArg inColor)
{
  AddSprite("DeskHuge", glm::vec2(inX, inY), glm::vec2(inWidth, inHeight), glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));
}

void UiScene::DrawText(int inX, int inY, const std::string_view &inText, const UiScene *inFont, JPH::ColorArg inColor)
{
  AddText(inText, glm::vec2(inX, inY), glm::vec2(1.0f),glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));

}
