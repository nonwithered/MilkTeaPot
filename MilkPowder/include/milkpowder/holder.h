#ifndef LIB_MILKPOWDER_HOLDER_H_
#define LIB_MILKPOWDER_HOLDER_H_

#ifdef __cplusplus

#include <milkpowder/core.h>
#include <utility>

namespace MilkPowder {

enum class TypeSet {
 Midi,
 Track,
 Message,
 Event,
 Meta,
 Sysex,
};

template<TypeSet T>
struct RawTypeMap;
template<>
struct RawTypeMap<TypeSet::Midi> {
  using target = MilkPowder_Midi_t;
};
template<>
struct RawTypeMap<TypeSet::Track> {
  using target = MilkPowder_Track_t;
};
template<>
struct RawTypeMap<TypeSet::Message> {
  using target = MilkPowder_Message_t;
};
template<>
struct RawTypeMap<TypeSet::Event> {
  using target = MilkPowder_Event_t;
};
template<>
struct RawTypeMap<TypeSet::Meta> {
  using target = MilkPowder_Meta_t;
};
template<>
struct RawTypeMap<TypeSet::Sysex> {
  using target = MilkPowder_Sysex_t;
};

template<TypeSet T>
struct RawDestroyMap;
template<>
struct RawDestroyMap<TypeSet::Midi> {
  static constexpr auto target = MilkPowder_Midi_Destroy;
};
template<>
struct RawDestroyMap<TypeSet::Track> {
  static constexpr auto target = MilkPowder_Track_Destroy;
};
template<>
struct RawDestroyMap<TypeSet::Message> {
  static constexpr auto target = MilkPowder_Message_Destroy;
};
template<>
struct RawDestroyMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Event_Destroy;
};
template<>
struct RawDestroyMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Meta_Destroy;
};
template<>
struct RawDestroyMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Sysex_Destroy;
};

template<TypeSet T>
struct RawCloneMap;
template<>
struct RawCloneMap<TypeSet::Midi> {
  static constexpr auto target = MilkPowder_Midi_Clone;
};
template<>
struct RawCloneMap<TypeSet::Track> {
  static constexpr auto target = MilkPowder_Track_Clone;
};
template<>
struct RawCloneMap<TypeSet::Message> {
  static constexpr auto target = MilkPowder_Message_Clone;
};
template<>
struct RawCloneMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Event_Clone;
};
template<>
struct RawCloneMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Meta_Clone;
};
template<>
struct RawCloneMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Sysex_Clone;
};

template<TypeSet T>
struct RawDumpMap;
template<>
struct RawDumpMap<TypeSet::Midi> {
  static constexpr auto target = MilkPowder_Midi_Dump;
};
template<>
struct RawDumpMap<TypeSet::Track> {
  static constexpr auto target = MilkPowder_Track_Dump;
};
template<>
struct RawDumpMap<TypeSet::Message> {
  static constexpr auto target = MilkPowder_Message_Dump;
};
template<>
struct RawDumpMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Event_Dump;
};
template<>
struct RawDumpMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Meta_Dump;
};
template<>
struct RawDumpMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Sysex_Dump;
};

template<TypeSet T>
class TypeConstInterface {
 protected:
  using RawType = typename RawTypeMap<T>::target;
  virtual const RawType *Self() const = 0;
 public:
  virtual ~TypeConstInterface() = default;
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(RawDumpMap<T>::target(Self(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
 protected:
  static RawType *Copy(const TypeConstInterface<T> &it) {
    const RawType *another = it.Self();
    if (another == nullptr) {
      return nullptr;
    }
    RawType *addr = nullptr;
    MilkTea::Defer defer([&addr]() -> void {
      if (addr == nullptr) {
        return;
      }
      MilkTea_panic(RawDestroyMap<T>::target(addr));
    });
    MilkTea_panic(RawCloneMap<T>::target(another, &addr));
    RawType *result = nullptr;
    std::swap(result, addr);
    return result;
  }
};

template<TypeSet T>
class TypeInterface : virtual public TypeConstInterface<T> {
 protected:
  using RawType = typename RawTypeMap<T>::target;
  virtual RawType *Self() = 0;
  virtual void Self(RawType *) = 0;
 public:
  virtual ~TypeInterface() = default;
 protected:
  void Delete() {
    RawType *self = Self();
    if (self == nullptr) {
      return;
    }
    MilkTea_panic(RawDestroyMap<T>::target(self));
    Self(nullptr);
  }
  static RawType *Move(TypeInterface<T> &&it) {
    RawType *result = it.Self();
    it.Self(nullptr);
    return result;
  }
};

#define TypeConstWrapper(T, C) \
 protected: \
  using RawType = typename RawTypeMap<TypeSet::T>::target; \
  const RawType *Self() const final { \
    return self_; \
  } \
 private: \
  explicit C(const RawType *self) : self_(self) {} \
  const RawType *self_;

#define TypeWrapper(T, C) \
 protected: \
  using RawType = typename RawTypeMap<TypeSet::T>::target; \
  const RawType *Self() const final { \
    return self_; \
  } \
  RawType *Self() final { \
    return self_; \
  } \
  void Self(RawType *self) final { \
    self_ = self; \
  } \
 private: \
  RawType *self_; \
 public: \
  explicit C(RawType *self = nullptr) : self_(self) {} \
  ~C() final { \
    Delete(); \
  } \
  C(const TypeConstInterface<TypeSet::T> &another) : C(Copy(another)) {} \
  C(TypeInterface<TypeSet::T> &&another) : C(Move(std::forward<TypeInterface<TypeSet::T>>(another))) {} \
  void operator=(const TypeConstInterface<TypeSet::T> &another) { \
    Delete(); \
    Self(Copy(another)); \
  } \
  void operator=(TypeInterface<TypeSet::T> &&another) { \
    Delete(); \
    Self(Move(std::forward<TypeInterface<TypeSet::T>>(another))); \
  } \
 private: \
  RawType **addr() { \
    return &self_; \
  }

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_HOLDER_H_
