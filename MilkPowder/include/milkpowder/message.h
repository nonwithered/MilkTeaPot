#ifndef LIB_MILKPOWDER_MESSAGE_H_
#define LIB_MILKPOWDER_MESSAGE_H_

#ifdef __cplusplus

#include <milkpowder/event.h>
#include <milkpowder/meta.h>
#include <milkpowder/sysex.h>

namespace MilkPowder {

template<TypeSet T>
struct RawIsMessageMap;
template<>
struct RawIsMessageMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Message_IsEvent;
};
template<>
struct RawIsMessageMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Message_IsMeta;
};
template<>
struct RawIsMessageMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Message_IsSysex;
};

template<TypeSet T>
struct RawToMessageMap;
template<>
struct RawToMessageMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Message_ToEvent;
};
template<>
struct RawToMessageMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Message_ToMeta;
};
template<>
struct RawToMessageMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Message_ToSysex;
};

template<>
class ConstInterface<TypeSet::Message> {
  using RawType = typename RawTypeMap<TypeSet::Message>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  uint32_t GetDelta() const {
    uint32_t result = 0;
    MilkTea_panic(MilkPowder_Message_GetDelta(get(), &result));
    return result;
  }
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_panic(MilkPowder_Message_GetType(get(), &result));
    return result;
  }
  template<TypeSet T>
  bool IsMessage() const {
    bool result = false;
    MilkTea_panic(RawIsMessageMap<T>::target(get(), &result));
    return result;
  }
  bool IsEvent() const {
    return IsMessage<TypeSet::Event>();
  }
  bool IsMeta() const {
    return IsMessage<TypeSet::Meta>();
  }
  bool IsSysex() const {
    return IsMessage<TypeSet::Sysex>();
  }
  template<TypeSet T>
  ConstWrapper<T> ToMessage() const {
    const typename RawTypeMap<T>::target *result = nullptr;
    MilkTea_panic(RawToMessageMap<T>::target(get(), &result));
    return ConstWrapper<T>(result);
  }
  ConstWrapper<TypeSet::Event> ToEvent() const {
    return ToMessage<TypeSet::Event>();
  }
  ConstWrapper<TypeSet::Meta> ToMeta() const {
    return ToMessage<TypeSet::Meta>();
  }
  ConstWrapper<TypeSet::Sysex> ToSysex() const {
    return ToMessage<TypeSet::Sysex>();
  }
};

using MessageConstWrapper = ConstWrapper<TypeSet::Message>;

template<TypeSet T>
struct RawFromMessageMap;
template<>
struct RawFromMessageMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Message_FromEvent;
};
template<>
struct RawFromMessageMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Message_FromMeta;
};
template<>
struct RawFromMessageMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Message_FromSysex;
};

template<>
class MutableInterface<TypeSet::Message> {
  using RawType = typename RawTypeMap<TypeSet::Message>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  void SetDelta(uint32_t delta) {
    MilkTea_panic(MilkPowder_Message_SetDelta(get(), delta));
  }
  static MutableWrapper<TypeSet::Message> Parse(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Message>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke, last));
    return self;
  }
  template<TypeSet T>
  static MutableWrapper<TypeSet::Message> Make(MutableWrapper<T> &&another) {
    RawType *self = nullptr;
    MilkTea_panic(RawFromMessageMap<T>::target(&self, another.release()));
    return self;
  }
};

using MessageMutableWrapper = MutableWrapper<TypeSet::Message>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MESSAGE_H_
