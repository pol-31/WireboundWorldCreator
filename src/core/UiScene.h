#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>
#include <stb_truetype.h>

#include "../render/Shader.h"
#include "../render/Texture.h"
#include "../ui/UiElement.h"
#include "../ui/UITexturedQuad.h"

const float cActivateScreenTime = 0.2f;

class UiRenderer;

class UiScene : public UIElement {
 public:
  UiScene(UiRenderer* ui_renderer);
  ~UiScene();

  /// --- --- ---
  void Update(float inDeltaTime) override;
  /// Only one layer can be active, if you push a layer it will exit in the background and no longer be updated
  void						PushLayer();
  void						PopLayer();
  int							GetNumLayers() const								{ return (int)mInactiveElements.size() + 1; }
  void						SetDrawInactiveLayers(bool inDraw)					{ mDrawInactiveElements = inDraw; }
  /// Find element by ID
  virtual UIElement *			FindByID(int inID) override;
  /// Actions
  void						SetDeactivatedAction(std::function<void()> inAction)		{ mDeactivatedAction = inAction; }
  /// Event handling (returns true if the event has been handled)
  virtual bool				HandleUIEvent(EUIEvent inEvent, UIElement *inSender) override;
  enum EState
  {
    STATE_INVALID,
    STATE_ACTIVATING,
    STATE_ACTIVE,
    STATE_DEACTIVATING,
    STATE_DEACTIVE
};

  void						SwitchToState(EState inState);
  EState						GetState() const									{ return mState; }

  /// Calculate max horizontal and vertical distance of elements to edge of screen
  void						GetMaxElementDistanceToScreenEdge(int &outMaxH, int &outMaxV);
  /// Drawing
  void						DrawQuad(int inX, int inY, int inWidth, int inHeight, const UITexturedQuad &inQuad, JPH::ColorArg inColor);
  /// Draw a string in screen coordinates (assumes that the projection matrix has been set up correctly)
  void						DrawText(int inX, int inY, const std::string_view &inText, const UiScene *inFont, JPH::ColorArg inColor = JPH::Color::sWhite);
  void Draw() const override;

  [[nodiscard]] int GetCharHeight() const noexcept;
  /// --- --- ---

  /// add text instance to vbo data buffer
  void AddText(std::string_view text, glm::vec2 position_pix,
    glm::vec2 scale, glm::vec4 color);

  void AddText3D() {}

  glm::vec2 MeasureText(std::string_view text) const;

  void AddSprite(const std::string& name, glm::vec2 position_pix,
    glm::vec2 size_pix, glm::vec4 color);


 private:
  UiRenderer* ui_renderer_ = nullptr;

  JPH::Array<UIElementVector>		mInactiveElements;
  bool						mDrawInactiveElements = true;
  std::function<void()>			mDeactivatedAction;

  EState						mState;
  float						mStateTime;
};
