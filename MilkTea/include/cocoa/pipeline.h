#ifndef LIB_COCOA_PIPELINE_H_
#define LIB_COCOA_PIPELINE_H_

#include <map>
#include <set>
#include <vector>
#include <initializer_list>
#include <sstream>
#include <functional>

#include <cocoa/cursor.h>

namespace Cocoa {

template<typename attribute_type, typename container_type>
class Pipeline final {
  static constexpr char TAG[] = "Cocoa::Pipeline";
  using self_type = Pipeline<attribute_type, container_type>;
  using value_type = typename container_type::value_type;
  using cursor_type = Cursor<container_type>;
 public:
  explicit Pipeline(attribute_type &attribute, container_type &container)
  : attribute_(attribute),
    container_(container) {}
  Pipeline(self_type &&another)
  : callbacks_(std::move(another.callbacks_)),
    attribute_(another.attribute_),
    container_(another.container_) {}
  self_type Append(
      std::initializer_list<value_type> candidate,
      std::function<bool(const value_type &, cursor_type &)> f
    ) && {
    for (auto it : candidate) {
      if (callbacks_.count(it) != 0) {
        std::string s;
        {
          std::stringstream ss;
          ss << it;
          ss >> s;
        }
        MilkTea_throwf(InvalidParam, "append a redundant candidate: %s", s.data());
      }
      callbacks_[it] = f;
    }
    return std::forward<self_type>(*this);
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      std::function<bool(cursor_type &)> f
    ) && {
    return std::forward<self_type>(*this).Append(std::move(candidate), [f](auto &, auto &cursor) -> bool {
      return f(cursor);
    });
  }
  template<typename T, typename S>
  self_type Append(
      std::initializer_list<value_type> candidate,
      T S::* member,
      T value
    ) && {
    return std::forward<self_type>(*this).Append(std::move(candidate), [&attribute = attribute_, member, value](auto &) -> bool {
      dynamic_cast<S &>(attribute).*member = value;
      return true;
    });
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      void (attribute_type::* member)()
    ) && {
    return std::forward<self_type>(*this).Append(std::move(candidate), [&attribute = attribute_, member](auto &) -> bool {
//      std::bind(member, attribute_)();
      (attribute.*member)();
      return true;
    });
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      bool (attribute_type::* member)(const value_type &, cursor_type &)
    ) && {
    return std::forward<self_type>(*this).Append(std::move(candidate), [&attribute = attribute_, member](auto &cmd, auto &cursor) -> bool {
      return (attribute.*member)(cmd, cursor);
    });
  }
  self_type Append(
      std::initializer_list<value_type> candidate,
      bool (attribute_type::* member)(cursor_type &)
    ) && {
    return std::forward<self_type>(*this).Append(std::move(candidate), [&attribute = attribute_, member](auto &cursor) -> bool {
      return (attribute.*member)(cursor);
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
          if (candidate_map.count(it) != 0) {
            std::string s;
            {
              std::stringstream ss;
              ss << it;
              ss >> s;
            }
            MilkTea_throwf(InvalidParam, "append a redundant sub candidate: %s", s.data());
          }
          candidate_map[it] = i->first;
        }
      }
    }
    return std::forward<self_type>(*this).Append(std::move(candidate),
        [&attribute = attribute_, member, handler_none, handler_invalid, handler_map = std::move(candidate_map)]
        (auto cmd, auto &cursor) -> bool {
      if (!cursor) {
        handler_none(cmd);
        return false;
      }
      auto &value = *cursor;
      auto it = handler_map.find(value);
      if (it == handler_map.end()) {
        handler_invalid(cmd, value);
        return false;
      }
      dynamic_cast<S &>(attribute).*member = it->second;
      ++cursor;
      return true;
    });
  }
  bool Launch(const value_type &cmd) && {
    for (auto i = container_.begin(); i != container_.end(); ) {
      auto it = callbacks_.find(*i);
      if (it == callbacks_.end()) {
        ++i;
        continue;
      }
      i = container_.erase(i);
      auto cursor = cursor_type(container_, i);
      if (!it->second(cmd, cursor)) {
        return false;
      }
    }
    return true;
  }
 private:
  std::map<value_type, std::function<bool(const value_type &, cursor_type &)>> callbacks_;
  attribute_type &attribute_;
  container_type &container_;
  MilkTea_NonCopy(Pipeline);
  MilkTea_NonMoveAssign(Pipeline);
};

} // namespace Cocoa

#endif // ifndef LIB_COCOA_PIPELINE_H_
