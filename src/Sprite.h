#ifndef WIREBOUNDWORLDCREATOR_SRC_SPRITE_H_
#define WIREBOUNDWORLDCREATOR_SRC_SPRITE_H_

#include <glad/glad.h>

#include "Texture.h"
#include "GlWrappers.h"

/// tex_id + tex_coords wrapper, that NOT OWNS texture id (otherwise unnecessary overhead)
/// doesn't keep position, so it should bind from the outside
class Sprite {
 public:
  Sprite(const Texture& tex, const TexCoords& tex_coords)
      : tex_id_(tex.GetId()), tex_coords_(tex_coords) {}

  ~Sprite() = default; // no OpenGL texture deletion, we don't own it

  //TODO: vbo should be already bind (to check it we ifndef_NDEBUG we ask OpenGL is vbo bind
  void Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id_);
    tex_coords_.Bind(); // bind to vbo with offset
  }

 private:
  const TexCoords& tex_coords_; // known at compile-time, find it in Details.h
  GLuint tex_id_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_SPRITE_H_
