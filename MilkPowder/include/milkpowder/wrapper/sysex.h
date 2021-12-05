#ifndef LIB_MILKPOWDER_WRAPPER_SYSEX_H_
#define LIB_MILKPOWDER_WRAPPER_SYSEX_H_

#include <functional>
#include <tuple>

#include <milkpowder/wrapper/holder.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Sysex> {
  using mapping = Mapping::Sysex;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> GetArgs() const {
    std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> result;
    std::function<void(uint32_t, const uint8_t *, uint32_t)> callback = [&result](uint32_t delta, const uint8_t *argv, uint32_t argc) -> void {
      result.push_back(std::make_tuple(delta, std::vector<uint8_t>(argv, argv + argc)));
    };
    MilkTea_panic(mapping::raw_get_args(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return result;
  }
};

using SysexConstWrapper = ConstWrapper<Mapping::Sysex>;

template<>
class MutableInterface<Mapping::Sysex> {
  using mapping = Mapping::Sysex;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<mapping> Parse(std::function<bool(uint8_t *)> callback) {
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_parse(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<mapping> Make(std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> vec) {
    raw_type *self = nullptr;
    uint32_t size = vec.size();
    std::vector<uint32_t> delta(size);
    std::vector<const uint8_t *> args(size);
    std::vector<uint32_t> length(size);
    for (uint32_t i = 0; i != size; ++i) {
      delta[i] = std::get<0>(vec[i]);
      args[i] = std::get<1>(vec[i]).data();
      length[i] = std::get<1>(vec[i]).size();
    }
    MilkTea_panic(mapping::raw_create(&self, delta.data(), args.data(), length.data(), size));
    return self;
  }
};

using SysexMutableWrapper = MutableWrapper<Mapping::Sysex>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_SYSEX_H_
