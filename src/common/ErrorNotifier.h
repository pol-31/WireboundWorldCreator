#ifndef WIREBOUNDWORLDCREATOR_SRC_ERRORNOTIFIER_H_
#define WIREBOUNDWORLDCREATOR_SRC_ERRORNOTIFIER_H_

#include <string_view>

#include "glad/glad.h"

#include "Texture.h"
#include "Shader.h"
#include "Details.h"

class ErrorNotifier {
 public:
  ErrorNotifier(const Shader& static_sprite_shader,
                std::string_view prerendered_text_texture_path)
      : shader_(static_sprite_shader),
        texture_(prerendered_text_texture_path) {}

  /// render text in the middle of the screen
  void Notify(std::string_view msg) {
    /*TexCoords tex_coords = FindPrerenderedText(msg);
    shader_.Bind();
    glActiveTexture(GL_TEXTURE0);
    texture_.Bind();
    tex_coords.Bind();
    tex_coords.BindAsPosition();
//    details::kErrorMsgPos.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
  }

 private:/*
  TexCoords FindPrerenderedText(std::string_view msg) {
    //TODO (see WireboundTextBaker):
    //  if not found - simply throw/terminate
  }*/

  const Shader& shader_;
  Texture texture_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_ERRORNOTIFIER_H_
