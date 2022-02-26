#ifndef LIB_TEA_SLICE_H_
#define LIB_TEA_SLICE_H_

#ifdef __cplusplus

#include <tea/life.h>

namespace tea {

template<typename T>
class slice {
 public:
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reference = value_type &;
  using const_reference = const value_type &;
  slice(iterator begin, iterator end) : begin_(begin), end_(end) {}
  slice(iterator begin, std::size_t len) : slice(begin, begin + len) {}
  auto begin() -> iterator {
    return begin_;
  }
  auto end() -> iterator {
    return end_;
  }
  auto begin() const -> const_iterator {
    return cbegin();
  }
  auto end() const -> const_iterator {
    return cend();
  }
  auto cbegin() const -> const_iterator {
    return begin_;
  }
  auto cend() const -> const_iterator {
    return end_;
  }
  auto size() const -> std::size_t {
    return end() - begin();
  }
  auto empty() const -> bool {
    return size() == 0;
  }
  auto operator[](std::size_t index) -> reference {
    return *(begin() + index);
  }
  auto operator[](std::size_t index) const -> const_reference {
    return *(begin() + index);
  }
  auto data() -> pointer {
    return begin();
  }
  auto data() const -> const_pointer {
    return begin();
  }
 private:
  const iterator begin_;
  const iterator end_;
};

template<typename T>
class slice<const T> {
 public:
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reference = value_type &;
  using const_reference = const value_type &;
  slice(const_iterator begin, const_iterator end) : begin_(begin), end_(end) {}
  slice(const_iterator begin, std::size_t len) : slice(begin, begin + len) {}
  auto begin() const -> const_iterator {
    return cbegin();
  }
  auto end() const -> const_iterator {
    return cend();
  }
  auto cbegin() const -> const_iterator {
    return begin_;
  }
  auto cend() const -> const_iterator {
    return end_;
  }
  auto size() const -> std::size_t {
    return end() - begin();
  }
  auto empty() const -> bool {
    return size() == 0;
  }
  auto operator[](std::size_t index) const -> const_reference {
    return *(begin() + index);
  }
  auto data() const -> const_pointer {
    return begin();
  }
 private:
  const const_iterator begin_;
  const const_iterator end_;
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_SLICE_H_
