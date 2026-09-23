#include "UIStaticText.h"

#include <glm/glm.hpp>

#include "../core/UiScene.h"

void StringToVector(const std::string& text, std::vector<std::string>& lines, const std::string& delimiter = "\n") {
  lines.clear();
  if (text.empty()) return;

  // Handle empty delimiter edge case by storing the entire string
  if (delimiter.empty()) {
    lines.push_back(text);
    return;
  }

  std::size_t start = 0;
  std::size_t end = text.find(delimiter);

  while (end != std::string::npos) {
    lines.push_back(text.substr(start, end - start));
    start = end + delimiter.length();
    end = text.find(delimiter, start);
  }

  // Push the remaining part of the string after the last delimiter
  lines.push_back(text.substr(start));
}

// void UIStaticText::CopyTo(UIElement *ioElement) const
// {
// 	UIElement::CopyTo(ioElement);
//
// 	UIStaticText *element = StaticCast<UIStaticText>(ioElement);
// 	element->GetManager() = GetManager();
// 	element->mText = mText;
// 	element->mTextColor = mTextColor;
// 	element->mDisabledTextColor = mDisabledTextColor;
// 	element->mTextPadLeft = mTextPadLeft;
// 	element->mTextPadRight = mTextPadRight;
// 	element->mTextPadTop = mTextPadTop;
// 	element->mTextPadBottom = mTextPadBottom;
// 	element->mTextAlignment = mTextAlignment;
// 	element->mWrap = mWrap;
// }

void UIStaticText::Draw() const
{
	DrawCustom(IsDisabled()? mDisabledTextColor : mTextColor);

	UIElement::Draw();
}

void UIStaticText::AutoLayout()
{
	UIElement::AutoLayout();

	// Update size
	if (GetManager() != nullptr)
	{
		glm::vec2 size = GetManager()->MeasureText(GetWrappedText());
		int w = int(size.x * GetManager()->GetCharHeight()) + mTextPadLeft + mTextPadRight;
		int h = int(size.y * GetManager()->GetCharHeight()) + mTextPadTop + mTextPadBottom;
		if (GetWidth() <= 0)
			mWidth.Set(w, PIXELS);
		if (GetHeight() <= 0)
			mHeight.Set(h, PIXELS);
	}
}

std::string UIStaticText::GetWrappedText() const
{
	std::string text;

	if (mWrap)
	{
		int width = GetWidth() - mTextPadLeft - mTextPadRight;

		size_t start_pos = 0, prev_end_pos = size_t(-1);
		for (;;)
		{
			// Find next space or end of text
			size_t end_pos = mText.find(' ', prev_end_pos + 1);
			if (end_pos == std::string::npos)
				end_pos = mText.length();

			// Get line to test for width
			std::string sub = mText.substr(start_pos, end_pos - start_pos);

			// Measure width
			glm::vec2 size = GetManager()->MeasureText(sub);
			int w = int(size.x * GetManager()->GetCharHeight());

			// Check if still fits
			if (width < w)
			{
				// If nothing was found yet, add the last word anyhow so that we don't get into an infinite loop
				if (start_pos >= prev_end_pos
					|| prev_end_pos == size_t(-1))
					prev_end_pos = end_pos;

				// Add current line
				text += mText.substr(start_pos, prev_end_pos - start_pos);
				text += "\n";

				// Start here for new line
				start_pos = prev_end_pos + 1;
			}
			else
			{
				// Store last fitting line
				prev_end_pos = end_pos;
			}

			// Check end
			if (end_pos >= mText.length())
				break;
		}

		// Add last line
		if (start_pos < mText.length())
			text += mText.substr(start_pos, mText.length() - start_pos);
	}
	else
	{
		// Don't autowrap
		text = mText;
	}

	return text;
}

void UIStaticText::DrawCustom(JPH::ColorArg inColor) const
{
	if (GetManager() != nullptr && !mText.empty())
	{
		std::string text = GetWrappedText();

		int y = GetY() + mTextPadTop;

		if (mTextAlignment == LEFT)
		{
				GetManager()->AddText(text, glm::vec2(GetX() + mTextPadLeft, y), glm::vec2(1.0f),glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));
			// GetManager()->DrawText(GetX() + mTextPadLeft, y, text, GetManager(), inColor);
		}
		else if (mTextAlignment == CENTER)
		{
			// Split lines
			std::vector<std::string> lines;
			StringToVector(text, lines, "\n");

			// Amount of space we have horizontally
			int width = GetWidth() - mTextPadLeft - mTextPadRight;

			// Center each line individually
			for (const std::string &l : lines)
			{
				glm::vec2 size = GetManager()->MeasureText(l);
				int w = int(size.x * GetManager()->GetCharHeight());
				GetManager()->AddText(l, glm::vec2(GetX() + (width - w) / 2 + mTextPadLeft, y), glm::vec2(1.0f),glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));
				//GetManager()->DrawText(GetX() + (width - w) / 2 + mTextPadLeft, y, l, GetManager(), inColor);
				y += GetManager()->GetCharHeight();
			}
		}
		else
		{
			JPH_ASSERT(mTextAlignment == RIGHT);

			// Split lines
			std::vector<std::string> lines;
			StringToVector(text, lines, "\n");

			// Center each line individually
			for (const std::string &l : lines)
			{
				glm::vec2 size = GetManager()->MeasureText(l);
				int w = int(size.x * GetManager()->GetCharHeight());
				GetManager()->AddText(l, glm::vec2(GetX() + GetWidth() - mTextPadRight - w, y), glm::vec2(1.0f),glm::vec4(inColor.r, inColor.g, inColor.b, inColor.a));
				// GetManager()->DrawText(GetX() + GetWidth() - mTextPadRight - w, y, l, GetManager(), inColor);
				y += GetManager()->GetCharHeight();
			}
		}
	}
}
