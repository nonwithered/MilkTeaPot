#ifndef LIB_MILKTEA_DISPATCHER_H_
#define LIB_MILKTEA_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>
#include <sstream>

namespace MilkTea {

template<typename container_type, typename context_type>
struct DispatcherHelper {
  static constexpr char TAG[] = "MilkTea::Dispatcher";
  using value_type = typename container_type::value_type;
  using command_type = std::function<void(container_type &, context_type &)>;
  using map_type = std::map<value_type, command_type>;
};

template<typename container_type, typename context_type, typename ...T>
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
template<typename container_type, typename context_type, typename T, typename ...Y>
struct DispatcherGenerator<container_type, context_type, T, Y...> final {
  using helper_type = DispatcherHelper<container_type, context_type>;
  static constexpr auto TAG = helper_type::TAG;
  using map_type = typename helper_type::map_type;
  static map_type Make() {
    auto m = DispatcherGenerator<container_type, context_type, Y...>::Make();
    auto n = T::Name();
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
  template<typename ...T>
  bool Start(T...) && {
    auto m = DispatcherGenerator<container_type, context_type, T...>::Make();
    if (m.empty()) {
      return false;
    }
    if (auto h = container_.begin(); h != container_.end()) {
      if (auto i = m.find(*h); i != m.end()) {
        container_.erase(h);
        i->second(container_, context_);
        return true;
      }
    }
    if (auto v = value_type(); m.count(v) != 0) {
      m[v](container_, context_);
      return true;
    }
    return false;
  }
  Dispatcher(container_type &container, context_type &context)
  : container_(container),
    context_(context) {}
 private:
  container_type &container_;
  context_type &context_;
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_DISPATCHER_H_
