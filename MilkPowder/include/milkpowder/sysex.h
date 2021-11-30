#ifndef LIB_MILKPOWDER_SYSEX_H_
#define LIB_MILKPOWDER_SYSEX_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>
#include <tuple>

namespace MilkPowder {

template<>
class ConstInterface<TypeSet::Sysex> {
  using RawType = typename RawTypeMap<TypeSet::Sysex>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> GetArgs() const {
    std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> result;
    std::function<void(uint32_t, const uint8_t *, uint32_t)> callback = [&result](uint32_t delta, const uint8_t *argv, uint32_t argc) -> void {
      result.push_back(std::make_tuple(delta, std::vector<uint8_t>(argv, argv + argc)));
    };
    MilkTea_panic(MilkPowder_Sysex_GetArgs(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return result;
  }
};

using SysexConstWrapper = ConstWrapper<TypeSet::Sysex>;

template<>
class MutableInterface<TypeSet::Sysex> {
  using RawType = typename RawTypeMap<TypeSet::Sysex>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  static MutableWrapper<TypeSet::Sysex> Parse(std::function<bool(uint8_t *)> callback) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Sysex>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<TypeSet::Sysex> Make(std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> vec) {
    RawType *self = nullptr;
    uint32_t size = vec.size();
    std::vector<uint32_t> delta(size);
    std::vector<const uint8_t *> args(size);
    std::vector<uint32_t> length(size);
    for (uint32_t i = 0; i != size; ++i) {
      delta[i] = std::get<0>(vec[i]);
      args[i] = std::get<1>(vec[i]).data();
      length[i] = std::get<1>(vec[i]).size();
    }
    MilkTea_panic(RawCreateMap<TypeSet::Sysex>::target(&self, delta.data(), args.data(), length.data(), size));
    return self;
  }
};

using SysexMutableWrapper = MutableWrapper<TypeSet::Sysex>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_SYSEX_H_
