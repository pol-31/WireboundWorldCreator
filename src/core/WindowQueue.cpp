#include "WindowQueue.h"

#include <stdexcept>

#include "UiComplex.h"

WindowQueue::WindowQueue() : windows_{} { windows_.fill(nullptr); }

WindowQueue::SizeType WindowQueue::PushBack(UiWindowBase* window) {
  for (int i = 0; i < windows_.size(); ++i) {
    if (!windows_[i]) {
      windows_[i] = window;
      selected_id_ = i;
      return i;
    }
  }
  throw std::runtime_error("WindowQueue no space for new window");
}

void WindowQueue::Erase(WindowQueue::SizeType id) {
  windows_[id] = nullptr;
  if (selected_id_ == id) {
    selected_id_ = -1;
  }
}

int WindowQueue::GetSize() const noexcept {
  int counter = 0;
  for (auto window : windows_) {
    if (window) {
      ++counter;
    }
  }
  return counter;
}

bool WindowQueue::Render() {
  bool result = false;
  for (auto window : windows_) {
    if (window && !result) {
      /*result |= */ window->Render();
    }
  }
  return result;
}

void WindowQueue::RenderPicking() {
  for (auto window : windows_) {
    if (window) {
      window->RenderPicking();
    }
  }
}

bool WindowQueue::Press(int id) {
  bool result = false;
  /// this way we won't Hide() new created
  auto windows_copy = windows_;
  for (int i = 0; i < windows_copy.size(); ++i) {
    if (windows_copy[i]) {
      if (windows_copy[i]->Press(id)) {
        result = true;
        selected_id_ = i;
      } else if (!windows_copy[i]->Pinned()) {
        windows_copy[i]->Hide();
      }
    }
  }
  if (!result) {
    selected_id_ = -1;
  }
  return result;
}

void WindowQueue::Release() {
  for (auto window : windows_) {
    if (window) {
      window->Release();
    }
  }
}

bool WindowQueue::Scroll(GLuint id, float yoffset) {
  /* no scrolls
   * if (top_window_) {
    return top_window_->Scroll(id, yoffset);
  }*/
  bool result = false;
  for (auto window : windows_) {
    if (window && !result) {
      result = window->Scroll(id, yoffset);
    }
  }
  return result;
}
