#ifndef LIB_MILKPOWDER_HOLDER_H_
#define LIB_MILKPOWDER_HOLDER_H_

#ifdef __cplusplus
#include <milkpowder/core.h>
namespace MilkPowder {

template<typename T>
struct DeletorMap final {};

#define milkpowder_deletor_map_(T) \
template<> \
struct DeletorMap<MilkPowder_##T##_t> final { \
  using Deletor = MilkTea_Exception_t (*)(MilkPowder_##T##_t *); \
  static constexpr Deletor deletor = MilkPowder_##T##_Destroy; \
};
milkpowder_deletor_map_(Midi)
milkpowder_deletor_map_(Track)
milkpowder_deletor_map_(Message)
milkpowder_deletor_map_(Event)
milkpowder_deletor_map_(Meta)
milkpowder_deletor_map_(Sysex)
#undef milkpowder_deletor_map_

#define MilkPowder_Holder(T) MilkPowder::Holder<MilkPowder_##T##_t>

template<typename T>
class Holder final {
 public:
  Holder(T *ptr = nullptr) : ptr_(ptr) {
  }
  Holder(Holder &&another) : ptr_(another.release()) {
  }
  ~Holder() {
    if (get() != nullptr) {
      MilkPowder::DeletorMap<T>::deletor(release());
    }
  }
  operator T *() const {
    return get();
  }
  T **operator&() {
    return &ptr_;
  }
  T *get() const {
    return ptr_;
  }
  T *release() {
    T *ptr = get();
    ptr_ = nullptr;
    return ptr;
  }
 private:
  T *ptr_;
  Holder(const Holder &) = delete;
  Holder &operator=(const Holder &) = delete;
  Holder &operator=(Holder &&) = delete;
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_HOLDER_H_
