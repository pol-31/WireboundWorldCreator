#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Core/Array.h"

class UIElement;

enum class UiAnimationType {
  Slide
};

/// Base class for UI element animations
class UIAnimation
{
public:
	/// Destructor
	virtual			~UIAnimation() = default;

	///@name Interface
	virtual void	Init(UIElement *inElement)							{ }
	virtual bool	Update(UIElement *inElement, float inDeltaTime)		{ return true; } ///< Returns false when done
	virtual void	Exit(UIElement *inElement)							{ }

  [[nodiscard]] UiAnimationType GetType() const noexcept {
	  return type_;
	}

protected:
  UiAnimationType type_ = UiAnimationType::Slide;
};

using UIAnimationVector = JPH::Array<UIAnimation *>;
