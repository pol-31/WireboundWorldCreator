#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_

#include <glad/glad.h>

#include <array>

class UiWindowBase;

class WindowQueue {
 public:
  using DataType = std::array<UiWindowBase*, 5>;
  using SizeType = DataType::size_type;

  WindowQueue();

  WindowQueue(const WindowQueue& other) = delete;
  WindowQueue(WindowQueue&& other) = delete;
  WindowQueue& operator=(const WindowQueue& other) = delete;
  WindowQueue& operator=(WindowQueue&& other) = delete;

  SizeType PushBack(UiWindowBase* window);

  void Erase(SizeType id);

  [[nodiscard]] int GetSize() const noexcept;

  bool Render();

  void RenderPicking();

  bool Press(int id);

  void Release();

  bool Scroll(GLuint id, float yoffset);

 private:
  DataType windows_;
  int selected_id_ = -1;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_WINDOWQUEUE_H_
