#ifndef WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_
#define WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Sprite.h"

//TODO: this like sprite... and... with Contains() method... idk how to name
/// class used for all "buttons", so here we render and say, was cursor_pos click on it
class UiCollisionSprite {
 public:
  bool Contains(); // based on position

  //TODO: shader should be already bind? ... let's give an appropriate name
  bool Bind(const Shader& already_bind_shader) {
    already_bind_shader.SetUniformMat2v("transform", 1, false, glm::value_ptr(pos_));
    sprite_.Bind(); // here bind both tex_id_ and tex_coords_
  }

 private:
  glm::mat2 pos_; // mat2 is enough, because we need only scale and transform in 2d (no rotation)
  Sprite sprite_;
};


// total num: 5 is enough;
// don't store buttons, only idx, which MUST be invalidated with Add() or Remove()
class UiSlots {
 public:
  // render only ui component
  // vao, vbo of single rectangular texture as well as shader
  void Render(const Shader& shader) {
    shader.SetUniform("brightness", 0.5f); // for all non-selected
    if (total_ < 5) {
      //TODO: we need gray
    }
    if (selected_ != -1) {
      //TODO: how to... brightness!
      shader.SetUniform("brightness", 1.0f);
    }
    // there's 6 colors in total: black, orange, green, red, blue, grey (details:: ?);
    // where grey means "empty slot".
    // all painted by modulus, so if start_idx == 0, we have black, orange, green, red, blue,
    // while for start_idx == 3, we have red, blue, black, orange, green

    //TODO: for this purpose we can simply use one 1x5 texture and GL_REPEAT on GL_WRAP_S
  }

  // returns id of point_set, so caller MODE could set it
  int Press() { // this function called from GlfwCallback on GLFW_MOUSE_LEFT && GLFW_PRESS
    if (cur_pos.y < btn_next.y) {
      if (total_ - start_idx > 5) {
        ++start_idx_;
      }
      selected_ = -1; // reset current_point_set
    } else if (cur_pos.y < btn_prev.y) {
      if (start_idx > 0) {
        --start_idx_;
      }
      selected_ = -1; // reset current_point_set
    } else {
      //TODO:
      /*if (<10) {} else if (<20) {} else if ... () <100 // wrt non-existing
            selected_ = "1-5";*/
    }
    return selected_;
  }

  // after each Add() or Remove() from the outside we should update it
  void Add() {
    selected_ = total_;
    ++total_;
    start_idx_ = std::max(total_ - 5, 0);
  }
  void Remove() {
    if (total_ < 1) {
      return;
    }
    --total_;
    start_idx_ = std::max(total_ - 5, 0);
    selected_ = -1;
  }

 protected:
  // point_sets_ as protected or as public in getter const& ?
 private:
  TexPosition position_; // where to draw

  // 1x5 colors for each slow tigh GL_WRAP_S==GL_REPEAT; color_grey set in shader...
  // no, let's use single texture and the same shader as static_sprite_shader;
  Texture texture_;

  int total_{0};
  int selected_{-1};
  int start_idx_{0}; // from where in view to start

  std::vector<std::vector<GLuint>> point_sets_; // point idx in ccw order, convex polygons only
};

class UiSlider; // TODO:

#endif  // WIREBOUNDWORLDCREATOR_SRC_UICOMPONENTS_H_
