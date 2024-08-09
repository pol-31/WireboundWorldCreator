#ifndef WIREBOUNDWORLDCREATOR_SRC_IEDITMODE_H_
#define WIREBOUNDWORLDCREATOR_SRC_IEDITMODE_H_

#include "SharedResources.h"

class IEditMode {
 public:
  // not const because mode Tiles calls SetTile() what is mutable call
  explicit IEditMode(SharedResources& shared_resources)
      : shared_resources_(shared_resources) {}

  virtual void Render() = 0;

  // used for mouse scroll, mouse & keyboard btn interaction
  virtual void BindCallbacks() = 0;

 protected:
  SharedResources& shared_resources_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_IEDITMODE_H_
