#ifndef LIB_MILKPOWDER_HOLDER_H_
#define LIB_MILKPOWDER_HOLDER_H_

#ifdef __cplusplus
#include <milkpowder/core.h>
#include <utility>
namespace MilkPowder {

template<typename T>
struct DestroyMap;
#define milkpowder_destroy_map_(T) \
template<> \
struct DestroyMap<MilkPowder_##T##_t> final { \
  using Destroy = MilkTea_Exception_t (*)(MilkPowder_##T##_t *); \
  static constexpr Destroy destroy = MilkPowder_##T##_Destroy; \
};
milkpowder_destroy_map_(Midi)
milkpowder_destroy_map_(Track)
milkpowder_destroy_map_(Message)
milkpowder_destroy_map_(Event)
milkpowder_destroy_map_(Meta)
milkpowder_destroy_map_(Sysex)
#undef milkpowder_destroy_map_

template<typename T>
struct CloneMap;
#define milkpowder_clone_map_(T) \
template<> \
struct CloneMap<MilkPowder_##T##_t> final { \
  using Clone = MilkTea_Exception_t (*)(const MilkPowder_##T##_t *, MilkPowder_##T##_t **); \
  static constexpr Clone clone = MilkPowder_##T##_Clone; \
};
milkpowder_clone_map_(Midi)
milkpowder_clone_map_(Track)
milkpowder_clone_map_(Message)
milkpowder_clone_map_(Event)
milkpowder_clone_map_(Meta)
milkpowder_clone_map_(Sysex)
#undef milkpowder_clone_map_

template<typename T>
struct DumpMap;
#define milkpowder_dump_map_(T) \
template<> \
struct DumpMap<MilkPowder_##T##_t> final { \
  using Dump = MilkTea_Exception_t (*)(const MilkPowder_##T##_t *, void *, void (MilkTea_CALL *)(void *, const uint8_t *, size_t )); \
  static constexpr Dump dump = MilkPowder_##T##_Dump; \
};
milkpowder_dump_map_(Midi)
milkpowder_dump_map_(Track)
milkpowder_dump_map_(Message)
milkpowder_dump_map_(Event)
milkpowder_dump_map_(Meta)
milkpowder_dump_map_(Sysex)
#undef milkpowder_dump_map_

#define MilkPowder_HolderRef(T) MilkPowder::HolderRef<MilkPowder_##T##_t>

template<typename T>
class HolderRef {
 public:
  explicit HolderRef(const T *ref) : ref_(ref) {}
  const T *get() const {
    return ref_;
  }
  operator const T *() const {
    return get();
  }
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(DumpMap<T>::dump(get(), &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
 private:
  const T *ref_;
};

#define MilkPowder_Holder(T) MilkPowder::Holder<MilkPowder_##T##_t>

template<typename T>
class Holder {
 public:
  explicit Holder(T *ptr = nullptr) : ptr_(ptr) {
  }
  explicit Holder(const HolderRef<T> &ref) {
    MilkTea_panic(CloneMap<T>::clone(ref, &*this));
  }
  Holder(const Holder<T> &another) {
    MilkTea_panic(CloneMap<T>::clone(another, &*this));
  }
  Holder(Holder<T> &&another) : ptr_(another.release()) {
  }
  void operator=(const Holder<T> &another) {
    if (ptr_ == another.ptr_) {
      return;
    }
    this->~Holder();
    CloneMap<T>::clone(another.ptr_, &ptr_);
  }
  void operator=(Holder<T> &&another) {
    if (ptr_ == another.ptr_) {
      return;
    }
    this->~Holder();
    std::swap(ptr_, another.ptr_);
  }
  virtual ~Holder() {
    if (ptr_ != nullptr) {
      MilkTea_panic(DestroyMap<T>::destroy(release()));
    }
  }
  operator T *() {
    return get();
  }
  operator const T *() const {
    return get();
  }
  T **operator&() {
    return &ptr_;
  }
  T *get() {
    return ptr_;
  }
  const T *get() const {
    return ptr_;
  }
  T *release() {
    T *ptr = get();
    ptr_ = nullptr;
    return ptr;
  }
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    HolderRef<T>(*this).Dump(callback);
  }
 private:
  T *ptr_;
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_HOLDER_H_
