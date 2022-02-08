#ifndef LIB_CAMELLIA_CURSOR_H_
#define LIB_CAMELLIA_CURSOR_H_

#include <milktea.h>

namespace Camellia {

template<typename container_type>
class Cursor final {
  static constexpr char TAG[] = "Camellia::Cursor";
  using value_type = typename container_type::value_type;
  using iterator = typename container_type::iterator;
public:
  Cursor(container_type &container, iterator &cursor)
  : container_(container),
    cursor_(cursor) {}
  const value_type &operator*() const {
    if (!operator bool()) {
      MilkTea_throw(LogicError, "try to get but no more");
    }
    return *cursor_;
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

} // namespace Camellia

#endif // ifndef LIB_CAMELLIA_CURSOR_H_
