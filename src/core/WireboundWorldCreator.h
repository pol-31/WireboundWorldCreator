#ifndef WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
#define WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_

#include "../common/Paths.h"
#include "../io/Camera.h"
#include "../io/Cursor.h"
#include "../common/Cubemap.h"
#include "Interface.h"

// should be created only after OpenGL context initialization
class WireboundWorldCreator {
 public:
  WireboundWorldCreator(const Paths& paths, Map& map);

  void RunRenderLoop();

 private:
  void Init(const Paths& paths);

  Cursor cursor_;
  Camera camera_;
  Cubemap cubemap_;

  Interface interface_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_WIREBOUNDWORLDCREATOR_H_
