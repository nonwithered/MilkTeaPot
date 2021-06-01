#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <functional>
#include <fstream>
#include <string_view>

#include <milkpowder.h>

namespace Milk {

template<typename T>
struct MilkPowderDeletor final {
  using Deletor = MilkPowder_Errno_t (*)(T *);
  static constexpr Deletor deletor = nullptr;
};

#define milkpowder_deletor_map_(T) \
template<> \
struct MilkPowderDeletor<MilkPowder_##T##_t> final { \
  using Deletor = MilkPowder_Errno_t (*)(MilkPowder_##T##_t *); \
  static constexpr Deletor deletor = MilkPowder_##T##_Destroy; \
};
milkpowder_deletor_map_(Midi)
milkpowder_deletor_map_(Track)
milkpowder_deletor_map_(Message)
milkpowder_deletor_map_(Event)
milkpowder_deletor_map_(Meta)
milkpowder_deletor_map_(Sysex)
#undef milkpowder_deletor_

template<typename T>
class MilkPowderHolder final {
 public:
  MilkPowderHolder(T *ptr = nullptr) : ptr_(ptr) {
  }
  MilkPowderHolder(MilkPowderHolder &&another) : ptr_(another.release()) {
  }
  ~MilkPowderHolder() {
    if (get() != nullptr) {
      Milk::MilkPowderDeletor<T>::deletor(release());
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
  MilkPowderHolder(const MilkPowderHolder &) = delete;
  MilkPowderHolder &operator=(const MilkPowderHolder &) = delete;
  MilkPowderHolder &operator=(MilkPowderHolder &&) = delete;
};

class InputReader {
 public:
  static bool read(void *obj, uint8_t *byte) {
    std::ifstream &ifs = reinterpret_cast<InputReader *>(obj)->ifs_;
    if (ifs.eof()) {
      return false;
    }
    ifs.read(reinterpret_cast<char *>(byte), 1);
    return true;
  }
  InputReader(std::string_view filename) : ifs_(filename.data(), std::ios::binary) {
  }
  bool NonOpen() const {
    return !ifs_.is_open();
  }
 private:
  std::ifstream ifs_;
};

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
