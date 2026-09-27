#pragma once

#include "UIElement.h"
#include "UITexturedQuad.h"

/// A static image UI element
class UIImage : public UIElement {
 public:
  /// Set properties
  void SetImage(std::string_view image_name) { image_name_ = image_name; }

  /// Cloning / copying
  // virtual void		CopyTo(UIElement *ioElement) const override;

  /// Draw element
  virtual void Draw() const override;

 private:
  std::string_view image_name_;
  // UITexturedQuad		mImage;
};
