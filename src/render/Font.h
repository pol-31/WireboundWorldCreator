#ifndef WIREBOUNDWORLDCREATOR_RENDER_FONT_H
#define WIREBOUNDWORLDCREATOR_RENDER_FONT_H

#include <Jolt/Core/Color.h>
#include <Jolt/Jolt.h>

#include <glm/glm.hpp>
#include <memory>

#include "PipelineState.h"
#include "RenderPrimitive.h"

class Renderer;
class Texture;

/// Font class, used to display text in 3D mode. Does variable width fonts with
/// kerning. Font names are identical to the Windows font names.
class Font {
 public:
  /// Constants
  static const int cBeginChar =
      ' ';  ///< First character that is drawable in the character set
  static const int cEndChar =
      256;  ///< Last character + 1 that is drawable in the character set
  static const int cNumChars =
      cEndChar -
      cBeginChar;  ///< Number of drawable characters in the character set

  /// Constructor
  Font(const char *inFontName, int inCharHeight);  // TODO: create bitmap

  /// Properties
  const std::string &GetFontName() const { return mFontName; }
  int GetCharHeight() const { return mCharHeight; }

  /// Get extents of a string, assuming the height of the text is 1 and with the
  /// normal aspect ratio of the font
  glm::vec2 MeasureText(const std::string_view &inText) const;

  /// Draw a string at a specific location
  /// If the string is drawn with the identity matrix, it's top left will start
  /// at (0, 0, 0) The text width is in the X direction and the text height is
  /// in the Y direction and it will have a height of 1
  void DrawText3D(glm::mat4 inTransform, const std::string_view &inText,
                  JPH::ColorArg inColor = JPH::Color::sWhite) const;

 private:
  /// Create a primitive for a string
  bool CreateString(glm::mat4 inTransform, const std::string_view &inText,
                    JPH::ColorArg inColor, RenderPrimitive &ioPrimitive) const;

  struct FontVertex {
    glm::vec3 mPosition;
    glm::vec2 mTexCoord;
    JPH::Color mColor;
  };

  /// Properties of the font
  std::string mFontName;  ///< Name of the font
  int mCharHeight;        ///< Height of a character
  int mHorizontalTexels;  ///< Number of texels horizontally, determines the
                          ///< scale of mStartU, mWidth and mSpacing
  int mVerticalTexels;    ///< Number of texels vertically, determines the scale
                          ///< of mStartV
  std::uint16_t mStartU[cNumChars] = {};  ///< Start U in texels
  std::uint16_t mStartV[cNumChars] = {};  ///< Start V in texels
  std::uint8_t mWidth[cNumChars] = {};    ///< Width of character in texels
  std::uint8_t mSpacing[cNumChars][cNumChars] =
      {};  ///< Spacing between characters in texels

  /// Structures used for drawing
  Renderer *mRenderer;                ///< Our renderer
  const Texture *mTexture = nullptr;  ///< The texture containing all characters
  std::unique_ptr<PipelineState>
      mPipelineState;  ///< The state used to render characters
};

#endif  // WIREBOUNDWORLDCREATOR_RENDER_FONT_H
