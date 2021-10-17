#ifndef LIB_MILKPOWDER_WRITE_H_
#define LIB_MILKPOWDER_WRITE_H_

#ifdef __cplusplus
#include <fstream>
#include <string_view>
#include <milktea.h>
namespace MilkPowder {

class FileWriter final {
 public:
  static void Callback(void *obj, const uint8_t *bytes, size_t len) {
    FileWriter &self = *reinterpret_cast<FileWriter *>(obj);
    self.Write(bytes, len);
  }
  explicit FileWriter(std::string_view filename)
    : ofs_(filename.data(), std::ios::binary) {
  }
  void Write(const uint8_t *buf, size_t size) {
    ofs_.write(reinterpret_cast<const char *>(buf), size);
  }
  bool NonOpen() const {
    return !ofs_.is_open();
  }
 private:
  std::ofstream ofs_;
  MilkTea_NonCopy(FileWriter)
  MilkTea_NonMove(FileWriter)
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_WRITE_H_
