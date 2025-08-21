#ifndef WIREBOUNDWORLDCREATOR_SRC_CORE_EVENTQUEUE_H_
#define WIREBOUNDWORLDCREATOR_SRC_CORE_EVENTQUEUE_H_

#include <functional>
#include <vector>

class Event {
 public:
  std::function<void()> callable_;
  int id_ = -1;

  Event() = delete;

  Event(std::function<void()>&& callable)
      : callable_(std::move(callable)) {}

  void operator()() const {
    callable_();
  }
};

class EventQueue {
 public:
  EventQueue() {
    queue_.fill(nullptr);
  }

  void Append(Event& event) {
    if (event.id_ != -1) {
      throw "EventQueue::Append() wrong logic";
    }
    for (int i = 0; i < gQueueSize; ++i) {
      if (!queue_[i]) {
        queue_[i] = &event;
        event.id_ = i;
        return;
      }
    }
    throw "EventQueue::Append() queue overflow";
  }

  void Remove(Event& event) {
    if (event.id_ == -1) {
      throw "EventQueue::Remove() wrong logic";
    }
    queue_[event.id_] = nullptr;
    event.id_ = -1;
  }

  void Process() {
    for (auto event : queue_) {
      if (event) {
        event->callable_();
      }
    }
  }

  static constexpr int gQueueSize = 10;
 private:
  std::array<Event*, gQueueSize> queue_;
};

// I want this:
// event_queue.Append([]() {data.SetTransform(0, 1, 4);})
// event_queue.Append(ev_move_selected_);
// then it should looks like
// event_queue.Remove(HOW?)
// event_queue.Remove(ev_move_selected_);
// IDK, so lambdas were discarded

// BUT!!!!
// class UiTerrainMode {
// ...
// #1: (nothing)
// #2:
// Event ev_move_selected_;
// Event ev_scale_selected_;
// Event ev_rotate_selected_;
// Event ev_scale_cursor_falloff_;
// Event ev_scale_cursor_size_;
// (etc...)

/*
 * тобто ти хочеш щоб я спочатку провірив TopWindow типу UiConfirmation,
 * потім якось визначив selected_window_id й додав методи Enter(), Escape()?
 * Щоб визначить selected_id потрібно при кожному Press(id) обновлять
 * провіряючи вкладені id вікна... тобто або це if-else-if в десятки рядків,
 * або зберігать start_id/end_id як static constexpr, бо треба ця інфа
 * на compile-time. Це щось може поламать... сподіваюсь не багато
 * */

#endif  // WIREBOUNDWORLDCREATOR_SRC_CORE_EVENTQUEUE_H_
