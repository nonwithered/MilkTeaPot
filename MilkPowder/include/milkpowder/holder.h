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
struct RawCreateMap;
template<>
struct RawCreateMap<TypeSet::Midi> {
  static constexpr auto target = MilkPowder_Midi_Create;
};
template<>
struct RawCreateMap<TypeSet::Track> {
  static constexpr auto target = MilkPowder_Track_Create;
};
template<>
struct RawCreateMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Event_Create;
};
template<>
struct RawCreateMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Meta_Create;
};
template<>
struct RawCreateMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Sysex_Create;
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
struct RawParseMap;
template<>
struct RawParseMap<TypeSet::Midi> {
  static constexpr auto target = MilkPowder_Midi_Parse;
};
template<>
struct RawParseMap<TypeSet::Track> {
  static constexpr auto target = MilkPowder_Track_Parse;
};
template<>
struct RawParseMap<TypeSet::Message> {
  static constexpr auto target = MilkPowder_Message_Parse;
};
template<>
struct RawParseMap<TypeSet::Event> {
  static constexpr auto target = MilkPowder_Event_Parse;
};
template<>
struct RawParseMap<TypeSet::Meta> {
  static constexpr auto target = MilkPowder_Meta_Parse;
};
template<>
struct RawParseMap<TypeSet::Sysex> {
  static constexpr auto target = MilkPowder_Sysex_Parse;
};

template<TypeSet T>
class ConstInterface {
  using RawType = typename RawTypeMap<T>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  virtual ~ConstInterface() = default;
};

template<TypeSet T>
class ConstWrapper final : public ConstInterface<T> {
  using RawType = typename RawTypeMap<T>::target;
 private:
  const RawType *const self_;
 public:
  const RawType *get() const final {
    return self_;
  }
 public:
  ConstWrapper(const RawType *self) : self_(self) {}
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(RawDumpMap<T>::target(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
};

template<TypeSet T>
class MutableInterface {
  using RawType = typename RawTypeMap<T>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  virtual ~MutableInterface() = default;
};

template<TypeSet T>
class MutableWrapper final : public ConstInterface<T>, public MutableInterface<T> {
  using RawType = typename RawTypeMap<T>::target;
 private:
  RawType *self_;
 public:
  const RawType *get() const final {
    return self_;
  }
  RawType *get() final {
    return self_;
  }
 public:
  MutableWrapper(RawType *self) : self_(self) {}
  ~MutableWrapper() {
    RawType *self = nullptr;
    std::swap(self_, self);
    if (self == nullptr) {
      return;
    }
    MilkTea_panic(RawDestroyMap<T>::target(self));
  }
  MutableWrapper(const ConstWrapper<T> &another) : MutableWrapper(nullptr) {
    const RawType *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    RawType *self = nullptr;
    MilkTea_panic(RawCloneMap<T>::target(another_, &self));
    std::swap(self_, self);
  }
  MutableWrapper(const MutableWrapper<T> &another) : MutableWrapper(nullptr) {
    const RawType *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    RawType *self = nullptr;
    MilkTea_panic(RawCloneMap<T>::target(another_, &self));
    std::swap(self_, self);
  }
  MutableWrapper(MutableWrapper<T> &&another) : MutableWrapper(nullptr) {
    std::swap(self_, another.self_);
  }
  void operator=(const ConstWrapper<T> &another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(another);
  }
  void operator=(const MutableWrapper<T> &another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(another);
  }
  void operator=(MutableWrapper<T> &&another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(std::forward<MutableWrapper<T>>(another));
  }
  RawType *release() {
    RawType *self = nullptr;
    std::swap(self_, self);
    return self;
  }
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(RawDumpMap<T>::target(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
};

} // namespace MilkPowder

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_HOLDER_H_
