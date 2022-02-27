#ifndef LIB_MILKTEA_CURSOR_H_
#define LIB_MILKTEA_CURSOR_H_

namespace MilkTea {

template<typename container_type>
class Cursor final {
  using value_type = typename container_type::value_type;
  using iterator = typename container_type::iterator;
public:
  Cursor(container_type &container, iterator &cursor)
  : container_(container),
    cursor_(cursor) {}
  auto operator*() const -> const value_type * {
    if (!operator bool()) {
      return nullptr;
    }
    return cursor_;
  }
  bool operator++() {
    if (!operator bool()) {
      return false;
    }
    cursor_ = container_.erase(cursor_);
    return true;
  }
  operator bool() const {
    return cursor_ != container_.end();
  }
private:
  container_type &container_;
  iterator &cursor_;
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_CURSOR_H_
