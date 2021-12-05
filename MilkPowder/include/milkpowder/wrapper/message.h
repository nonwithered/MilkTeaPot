#ifndef LIB_MILKPOWDER_WRAPPER_MESSAGE_H_
#define LIB_MILKPOWDER_WRAPPER_MESSAGE_H_

#include <milkpowder/wrapper/event.h>
#include <milkpowder/wrapper/meta.h>
#include <milkpowder/wrapper/sysex.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Message> {
  using mapping = Mapping::Message;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  uint32_t GetDelta() const {
    uint32_t result = 0;
    MilkTea_panic(mapping::raw_get_delta(get(), &result));
    return result;
  }
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_panic(mapping::raw_get_type(get(), &result));
    return result;
  }
  template<typename T>
  bool Is() const {
    bool result = false;
    MilkTea_panic(T::raw_message_is(get(), &result));
    return result;
  }
  bool IsEvent() const {
    return Is<Mapping::Event>();
  }
  bool IsMeta() const {
    return Is<Mapping::Meta>();
  }
  bool IsSysex() const {
    return Is<Mapping::Sysex>();
  }
  template<typename T>
  ConstWrapper<T> To() const {
    const typename T::raw_type *result = nullptr;
    MilkTea_panic(T::raw_message_to(get(), &result));
    return ConstWrapper<T>(result);
  }
  ConstWrapper<Mapping::Event> ToEvent() const {
    return To<Mapping::Event>();
  }
  ConstWrapper<Mapping::Meta> ToMeta() const {
    return To<Mapping::Meta>();
  }
  ConstWrapper<Mapping::Sysex> ToSysex() const {
    return To<Mapping::Sysex>();
  }
};

using MessageConstWrapper = ConstWrapper<Mapping::Message>;

template<>
class MutableInterface<Mapping::Message> {
  using mapping = Mapping::Message;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  void SetDelta(uint32_t delta) {
    MilkTea_panic(mapping::raw_set_delta(get(), delta));
  }
  static MutableWrapper<Mapping::Message> Parse(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) {
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_parse(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke, last));
    return self;
  }
  template<typename T>
  static MutableWrapper<Mapping::Message> From(MutableWrapper<T> &&another) {
    raw_type *self = nullptr;
    MilkTea_panic(T::raw_message_from(&self, another.release()));
    return self;
  }
};

using MessageMutableWrapper = MutableWrapper<Mapping::Message>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_MESSAGE_H_
