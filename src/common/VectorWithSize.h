#ifndef WIREBOUNDWORLDCREATOR_SRC_COMMON_VECTORWITHSIZE_H_
#define WIREBOUNDWORLDCREATOR_SRC_COMMON_VECTORWITHSIZE_H_

//TODO: important for UiSlots to track current size, so need wrapper
// to not to forget modify points_data_size_ together with points_data_

//TODO: more clear explanation

//TODO: constexpr...

/// we implement only what we use (sorry, I'm to lazy);
/// methods are named like from std:: but with PascalCase
template <typename T>
class VectorWithSize {
 public:
  using Reference = std::vector<T>::reference;
  using ConstReference = std::vector<T>::const_reference;
  using SizeType = std::vector<T>::size_type;

  using Iterator = std::vector<T>::iterator;
  using ConstIterator = std::vector<T>::const_iterator;

  VectorWithSize() = default;

  /// that's why we're here
  [[nodiscard]] const SizeType& GetSizeRef() const {
    return size_;
  }

  template<class... Args>
  Reference EmplaceBack(Args&&... args) {
    Reference ref = data_.emplace_back(std::forward<Args>(args)...);
    size_ = data_.size();
    return ref;
  }

  SizeType Size() const {
    return size_;
  }

  void PopBack() {
    data_.pop_back();
    size_ = data_.size();
  }

  Iterator Erase(ConstIterator pos) {
    Iterator it = data_.erase(pos);
    size_ = data_.size();
    return it;
  }

  /// nothing interesting here

  Iterator Begin() {
    return data_.begin();
  }

  ConstIterator Begin() const {
    return data_.begin();
  }

  Reference operator[](SizeType pos) {
    return data_[pos];
  }

  ConstReference operator[](SizeType pos) const {
    return data_[pos];
  }

 private:
  std::vector<T> data_;
  SizeType size_{0};
};

#endif  // WIREBOUNDWORLDCREATOR_SRC_COMMON_VECTORWITHSIZE_H_
