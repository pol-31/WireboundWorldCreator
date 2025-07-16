#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_

#include <array>

#include <glad/glad.h>

#include "../common/Text.h"

class UiWindowBase;
class UiTopWindowBase;

class WindowQueue {
 public:
  using SizeType = std::array<UiWindowBase*, 5>::size_type;

  WindowQueue();

  SizeType PushBack(UiWindowBase* window);

  void Erase(SizeType id);

  // ---

  bool Render();

  void RenderPicking();

  bool Press(int id);

  void Release();

  data::TextId Hover(int id);

  bool Scroll(GLuint id, float yoffset);

  void SetTopWindow(UiTopWindowBase* window) {
    top_window_ = window;
  }

  [[nodiscard]] UiTopWindowBase* GetTopWindow() const noexcept {
    return top_window_;
  }

 private:
  std::array<UiWindowBase*, 5> windows_;

  // edit, bake, settings, tips - can be pinned
  // confirmation, file - cannot be pinned
  UiTopWindowBase* top_window_ = nullptr;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
