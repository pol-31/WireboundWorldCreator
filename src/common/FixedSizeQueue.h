#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_FIXEDSIZEQUEUE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_FIXEDSIZEQUEUE_H_

#include <array>
#include <functional>
#include <cassert>
#include <algorithm>
#include <iterator>

/// it's like constexpr vector:
/// internally it's std::array, but interface of std::vector
template <typename T, std::size_t N>
class FixedSizeQueue {
 public:
  using SizeType = std::size_t;
  using Iterator = typename std::array<T, N>::iterator;
  using ConstIterator = typename std::array<T, N>::const_iterator;

  FixedSizeQueue() : cur_size_(0) {}

  SizeType PushBack(const T& value) {
    assert(cur_size_ < N && "Queue overflow");
    data_[cur_size_] = value;
    return cur_size_++;
  }

  bool SafePushBack(const T& value) {
    if (cur_size_ < N) {
      data_[cur_size_++] = value;
      return true;
    } else {
      return false;
    }
  }

  SizeType PopBack() {
    assert(cur_size_ > 0 && "Queue is empty, unable to PopBack");
    return --cur_size_;
  }

  bool SafePopBack() {
    if (cur_size_ > 0) {
      --cur_size_;
      return true;
    } else {
      return false;
    }
  }

  void Clear() {
    cur_size_ = 0;
  }

  SizeType Size() const {
    return cur_size_;
  }

  const T& operator[](SizeType index) const {
    assert(index < cur_size_ && "Index out of bounds");
    return data_[index];
  }

  void Erase(SizeType idx) {
    assert(idx < cur_size_ && "Index out of bounds");
    auto first = std::next(data_.begin(), idx);
    auto last = std::next(data_.begin(), cur_size_);
    std::move(first + 1, last, first);
    --cur_size_;
  }

  Iterator begin() {
    return data_.begin();
  }

  Iterator end() {
    return std::next(data_.begin(), cur_size_);
  }

  ConstIterator cbegin() const {
    return data_.cbegin();
  }

  ConstIterator cend() const {
    return std::next(data_.cbegin(), cur_size_);
  }

 private:
  std::array<T, N> data_;
  SizeType cur_size_;
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_FIXEDSIZEQUEUE_H_
