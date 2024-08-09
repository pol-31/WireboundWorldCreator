#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_

#include <glad/glad.h>

#include "IEditMode.h"

class BiomesMode final : public IEditMode {
 public:
  using IEditMode::IEditMode;
  /*
   *
glBindVertexArray(vao_biomes_);
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
    shared_resources_.tile_renderer.Render();
  }
  void BindCallbacks() override {}

 private:
  GLuint vao_biomes_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_BIOMESMODE_H_
