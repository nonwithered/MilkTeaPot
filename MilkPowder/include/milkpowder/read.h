#ifndef LIB_MILKPOWDER_READ_H_
#define LIB_MILKPOWDER_READ_H_

#ifdef __cplusplus
#include <fstream>
#include <string_view>
#include <milktea.h>
namespace MilkPowder {

class FileReader final {
 public:
  static bool Callback(void *obj, uint8_t *byte) {
    FileReader &self = *reinterpret_cast<FileReader *>(obj);
    return self.Read(byte, 1) != 0;
  }
  FileReader(std::string_view filename)
    : ifs_(filename.data(), std::ios::binary) {
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
    return ifs_.gcount();
  }
 private:
  std::ifstream ifs_;
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_READ_H_