#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_OPENGLUTILITY_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_OPENGLUTILITY_H_

#include <glad/glad.h>

namespace utility {

// why now class instances:
// - to reduce amount of single Create/Delete calls
// - to safety - e.g. delete vbo before vao
inline void InitUbo(GLuint& id, GLsizeiptr size, GLenum usage, GLuint bind_id) {
  glGenBuffers(1, &id);
  glBindBuffer(GL_UNIFORM_BUFFER, id);
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
  glBindBufferBase(GL_UNIFORM_BUFFER, bind_id, id);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

inline void UpdateUbo(GLuint id, GLintptr offset, GLsizeiptr size,
                      const void* data) {
  glBindBuffer(GL_UNIFORM_BUFFER, id);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// TODO: deleting N buffers

}  // namespace utility

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_OPENGLUTILITY_H_
