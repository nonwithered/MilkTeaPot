#ifndef LIB_COCOA_DISPATCHER_H_
#define LIB_COCOA_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>
#include <sstream>

#include <milktea.h>

namespace Cocoa {

template<typename container_type, typename context_type>
struct DispatcherHelper {
  static constexpr char TAG[] = "Cocoa::Dispatcher";
  using value_type = typename container_type::value_type;
  using command_type = std::function<void(value_type, container_type &, context_type &)>;
  using map_type = std::map<value_type, command_type>;
};

template<typename container_type, typename context_type, typename... T>
struct DispatcherGenerator final {
  using helper_type = DispatcherHelper<container_type, context_type>;
};
template<typename container_type, typename context_type>
struct DispatcherGenerator<container_type, context_type> final {
  using helper_type = DispatcherHelper<container_type, context_type>;
  using map_type = typename helper_type::map_type;
  static map_type Make() {
    return map_type();
  }
};
template<typename container_type, typename context_type, typename T, typename... Y>
struct DispatcherGenerator<container_type, context_type, T, Y...> final {
  using helper_type = DispatcherHelper<container_type, context_type>;
  static constexpr auto TAG = helper_type::TAG;
  using map_type = typename helper_type::map_type;
  static map_type Make() {
    auto m = DispatcherGenerator<container_type, context_type, Y...>::Make();
    auto n = T::Name();
    if (m.count(n) != 0) {
      auto s = MilkTea::ToString::From(n);
      MilkTea_throwf(InvalidParam, "Make: duplicate %s", s.data());
    }
    m[n] = T::Start;
    return m;
  }
};

template<typename container_type, typename context_type>
class Dispatcher final {
  using helper_type = DispatcherHelper<container_type, context_type>;
  static constexpr auto TAG = helper_type::TAG;
  using value_type = typename helper_type::value_type;
  using map_type = typename helper_type::map_type;
 public:
  template<typename... T>
  void Start(T...) && {
    auto m = DispatcherGenerator<container_type, context_type, T...>::Make();
    if (m.empty()) {
      MilkTea_throw(InvalidParam, "Start need commands");
    }
    auto h = container_.begin();
    if (h != container_.end()) {
      auto i = m.find(*h);
      if (i != m.end()) {
        container_.erase(h);
        i->second(i->first, container_, context_);
        return;
      }
    }
    m.begin()->second(value_type(), container_, context_);
  }
  Dispatcher(container_type container, context_type context)
  : container_(container),
    context_(context) {}
  Dispatcher(container_type &container, context_type &context)
  : container_(container),
    context_(context) {}
 private:
  container_type &container_;
  context_type &context_;
  MilkTea_NonCopy(Dispatcher);
  MilkTea_NonMove(Dispatcher);
};

} // namespace Cocoa

#endif // ifndef LIB_COCOA_DISPATCHER_H_
