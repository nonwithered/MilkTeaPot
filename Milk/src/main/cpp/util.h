#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <functional>
#include <fstream>
#include <string_view>
#include <string>
#include <cctype>
#include <cstdio>
#include <cinttypes>

#include <milkpowder.h>

namespace Milk {

constexpr char kVersion[] = "0.0.1-snapshot";

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
  static bool Reader(void *obj, uint8_t *byte) {
    InputReader &self= *reinterpret_cast<InputReader *>(obj);
    return self.Read(byte, 1) != 0;
  }
  InputReader(std::string_view filename)
    : ifs_(filename.data(), std::ios::binary),
      count_(0) {
  }
  size_t Count() const {
    return count_;
  }
  bool NonOpen() const {
    return !ifs_.is_open();
  }
  size_t Read(uint8_t *buf, size_t size) {
    if (ifs_.eof()) {
      return 0;
    }
    if (buf != nullptr) {
      ifs_.read(reinterpret_cast<char *>(buf), size);
    } else {
      ifs_.ignore(size);
    }
    size_t count = ifs_.gcount();
    count_ += count;
    return count;
  }
 private:
  std::ifstream ifs_;
  size_t count_;
};

inline std::string FromU32ToString(uint32_t n) {
  char buf[16];
  sprintf(buf, "%" PRIu32, n);
  return buf;
}

inline std::string FromU32ToStringHex(uint32_t n) {
  char buf[9];
  sprintf(buf, "%08" PRIx32, n);
  return buf;
}

inline std::string FromU16ToString(uint16_t n) {
  char buf[8];
  sprintf(buf, "%" PRIu16, n);
  return buf;
}

inline std::string FromU16ToStringHex(uint16_t n) {
  char buf[5];
  sprintf(buf, "%04" PRIx16, n);
  return buf;
}

inline std::string FromU8ToString(uint8_t n) {
  char buf[4];
  sprintf(buf, "%" PRIu8, n);
  return buf;
}

inline std::string FromU8ToStringHex(uint8_t n) {
  char buf[3];
  sprintf(buf, "%02" PRIx8, n);
  return buf;
}

inline uint32_t FromBytesToU32(const uint8_t bytes[]) {
  return (bytes[0] << 030) | (bytes[1] << 020) | (bytes[2] << 010) | bytes[3];
}

inline uint16_t FromBytesToU16(const uint8_t bytes[]) {
  return (bytes[0] << 010) | bytes[1];
}

inline uint8_t FromBytesToU8(const uint8_t bytes[]) {
  return bytes[0];
}

inline std::string FromBytesToStringHex(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    ss << FromU8ToStringHex(bytes[i]);
  }
  std::string s;
  ss >> s;
  return std::move(s);
}

inline std::string FromBytesToString(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    uint8_t byte = bytes[i];
    if (isgraph(byte)) {
      ss << static_cast<char>(byte);
    } else {
      ss << "\\" << std::oct << std::right << std::setw(3) << std::setfill('0') << static_cast<unsigned int>(byte);
    }
  }
  std::string s;
  ss >> s;
  return std::move(s);
}

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
