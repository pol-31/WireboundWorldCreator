#include "WindowQueue.h"

#include <stdexcept>

#include "UiComplex.h"

WindowQueue::WindowQueue()
    : windows_{} {
  windows_.fill(nullptr);
}

WindowQueue::SizeType WindowQueue::PushBack(UiWindowBase* window) {
  for (int i = 0; i < windows_.size(); ++i) {
    if (!windows_[i]) {
      windows_[i] = window;
      return i;
    }
  }
  throw std::runtime_error("WindowQueue no space for new window");
}

void WindowQueue::Erase(WindowQueue::SizeType id) {
  windows_[id] = nullptr;
}

// ---

bool WindowQueue::Render() {
  bool result = false;
  for (auto window : windows_) {
    if (window && !result) {
      /*result |= */window->Render();
    }
  }
  if (top_window_) {
    top_window_->Render();
  }
  return result;
}

void WindowQueue::RenderPicking() {
  if (top_window_) {
    top_window_->RenderPicking();
    return;
  }
  for (auto window : windows_) {
    if (window) {
      window->RenderPicking();
    }
  }
}

bool WindowQueue::Press(int id) {
  if (top_window_) {
    top_window_->Press(id);
    return true;
  }
  bool result = false;
  for (auto window : windows_) {
    if (window) {
      if (window->Press(id)) {
        result = true;
      } else if (!window->Pinned()) {
        window->Hide();
      }
    }
  }
  return result;
}

void WindowQueue::Release() {
  for (auto window : windows_) {
    if (window) {
      window->Release();
    }
  }
  if (top_window_) {
    top_window_->Release();
  }
}

data::TextId WindowQueue::Hover(int id) {
  if (top_window_) {
    return top_window_->Hover(id);
  }
  data::TextId handled = data::TextId::kNone;
  for (auto window : windows_) {
    if (window && handled == data::TextId::kNone) {
      handled = window->Hover(id);
    }
  }
  return handled;
}

bool WindowQueue::Scroll(GLuint id, float yoffset) {
  if (top_window_) {
    return top_window_->Scroll(id, yoffset);
  }
  bool result = false;
  for (auto window : windows_) {
    if (window && !result) {
      result = window->Scroll(id, yoffset);
    }
  }
  return result;
}
