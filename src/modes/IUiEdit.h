#ifndef WIREBOUNDWORLDCREATOR_SRC_MODES_IUIEDIT_H_
#define WIREBOUNDWORLDCREATOR_SRC_MODES_IUIEDIT_H_

#include "../core/UiComplex.h"

class IUiEdit : public UiWindowAppear {
 public:
  using UiWindowAppear::UiWindowAppear;
  virtual void UpdateConfig() = 0;         // e.g. UpdateHmap()
  virtual void SetInstanceId(int id) = 0;  // insctances_[id]
  virtual void HideAll() = 0;
  virtual void CreateInstance() = 0;
  virtual void RemoveInstance(GLuint id) {}
  virtual void Reset() = 0;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_MODES_IUIEDIT_H_
