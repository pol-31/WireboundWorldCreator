#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_

#include <glad/glad.h>

#include "IEditMode.h"

class TilesMode final : public IEditMode {
 public:
  using IEditMode::IEditMode;
  /*
   *
glBindVertexArray(vao_tiles_);
glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced_); // TODO: is it correct buffer (instances)
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                      reinterpret_cast<void*>(0));
glEnableVertexAttribArray(0); // position
glBindBuffer(GL_ARRAY_BUFFER, vbo_ui_);
// we don't update it, use uniform texture coords
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                      reinterpret_cast<void*>(TODO:HERE_OUR_UNIFORM_TEX_COORDS));
glEnableVertexAttribArray(1); // tex coords
   * */
  //TODO: glDeleteVertexArrays(1, vao);

  void Render() override {
    // we don't render tile_renderer_.*
  }

  //TODO: modify tile_renderer_.SetTile(id);
  void BindCallbacks() override {}

 private:
  GLuint vao_tiles_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_TILESMODE_H_
