#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_

#include <glad/glad.h>

#include <array>

#include "../common/Text.h"

class UiWindowBase;
class UiTopWindowBase;

class WindowQueue {
 public:
  using DataType = std::array<UiWindowBase*, 20>;
  using SizeType = DataType::size_type;

  WindowQueue();

  WindowQueue(const WindowQueue& other) = delete;
  WindowQueue(WindowQueue&& other) = delete;
  WindowQueue& operator=(const WindowQueue& other) = delete;
  WindowQueue& operator=(WindowQueue&& other) = delete;

  SizeType PushBack(UiWindowBase* window);

  void Erase(SizeType id);

  [[nodiscard]] int GetSize() const noexcept;

  // ---

  void BtnEnter();

  void BtnEscape();

  // ---

  bool Render();

  void RenderPicking();

  bool Press(int id);

  void Release();

  bool Scroll(GLuint id, float yoffset);

  void SetTopWindow(UiTopWindowBase* window) { top_window_ = window; }

  [[nodiscard]] UiTopWindowBase* GetTopWindow() const noexcept {
    return top_window_;
  }

 private:
  DataType windows_;

  // edit, bake, settings, tips - can be pinned
  // confirmation, file - cannot be pinned
  UiTopWindowBase* top_window_ = nullptr;

  int selected_id_ = -1;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
