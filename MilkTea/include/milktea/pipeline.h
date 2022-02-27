#ifndef LIB_MILKTEA_PIPELINE_H_
#define LIB_MILKTEA_PIPELINE_H_

#include <map>
#include <set>
#include <vector>
#include <initializer_list>
#include <sstream>
#include <functional>

#include <milktea/cursor.h>

namespace MilkTea {

template<typename container_type, typename extra_type>
class Pipeline final {
  using self_type = Pipeline<container_type, extra_type>;
  using value_type = typename container_type::value_type;
  using cursor_type = Cursor<container_type>;
  using command_type = std::function<bool(const value_type &, cursor_type &)>;
 public:
  explicit Pipeline(container_type &container, extra_type &extra)
  : container_(container),
    extra_(extra) {}
  Pipeline(self_type &&another)
  : callbacks_(std::move(another.callbacks_)),
    container_(another.container_),
    extra_(another.extra_) {}
  self_type Append(
      std::initializer_list<value_type> candidate,
      command_type f
    ) && {
    for (auto it : candidate) {
      callbacks_[it] = f;
    }
    return std::move(*this);
  }
  template<typename T, typename S>
  self_type Append(
      std::initializer_list<value_type> candidate,
      T S::* member,
      T value
    ) && {
    return std::move(*this).Append(std::move(candidate), [&extra = extra_, member, value](auto &, auto &) -> bool {
      dynamic_cast<S &>(extra).*member = value;
      return true;
    });
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      void (extra_type::* member)()
    ) && {
    return std::move(*this).Append(std::move(candidate), [&extra = extra_, member](auto &, auto &) -> bool {
      (extra.*member)();
      return true;
    });
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      bool (extra_type::* member)(const value_type &, cursor_type &)
    ) && {
    return std::move(*this).Append(std::move(candidate), [&extra = extra_, member](auto &option, auto &cursor) -> bool {
      return (extra.*member)(option, cursor);
    });
  }
  template<typename S>
  self_type Append(
      std::initializer_list<value_type> candidate,
      value_type S::* member,
      std::function<void(const value_type &)> handler_none
  ) && {
    return std::move(*this).Append(std::move(candidate),
        [&extra = extra_, member, handler_none]
        (auto & option, auto &cursor) -> bool {
      if (!cursor) {
        handler_none(option);
        return false;
      }
      dynamic_cast<S &>(extra).*member = *cursor;
      ++cursor;
      return true;
    });
  }
  template<typename T, typename S>
  self_type Append(
      std::initializer_list<value_type> candidate,
      T S::* member,
      std::function<void(const value_type &)> handler_none,
      std::function<void(const value_type &, const value_type &)> handler_invalid,
      std::map<T, std::vector<value_type>> value_map
    ) && {
    std::map<value_type, T> candidate_map;
    {
      for (auto i = value_map.begin(), n = value_map.end(); i != n; ++i) {
        for (auto &it : i->second) {
          candidate_map[it] = i->first;
        }
      }
    }
    return std::move(*this).Append(std::move(candidate),
        [&extra = extra_, member, handler_none, handler_invalid, handler_map = std::move(candidate_map)]
        (auto &option, auto &cursor) -> bool {
      if (!cursor) {
        handler_none(option);
        return false;
      }
      auto &value = *cursor;
      auto it = handler_map.find(value);
      if (it == handler_map.end()) {
        handler_invalid(option, value);
        return false;
      }
      dynamic_cast<S &>(extra).*member = it->second;
      ++cursor;
      return true;
    });
  }
  bool Start() && {
    for (auto i = container_.begin(); i != container_.end(); ) {
      auto option = *i;
      auto it = callbacks_.find(option);
      if (it == callbacks_.end()) {
        ++i;
        continue;
      }
      i = container_.erase(i);
      auto cursor = cursor_type(container_, i);
      if (!it->second(option, cursor)) {
        return false;
      }
    }
    return true;
  }
 private:
  std::map<value_type, command_type> callbacks_;
  container_type &container_;
  extra_type &extra_;
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_PIPELINE_H_
