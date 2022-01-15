#ifndef LIB_MILKPOWDER_WRAPPER_READER_H_
#define LIB_MILKPOWDER_WRAPPER_READER_H_

#include <fstream>
#include <string_view>

#include <milktea.h>

namespace MilkPowder {

class ByteReader final {
  using call_type = std::function<MilkTea_Exception_t(uint8_t[], size_t &)>;
 public:
  ByteReader(call_type call) : call_(call) {}
  size_t operator()(uint8_t bytes[], size_t len) {
    MilkTea_invoke_panic(call_, bytes, len);
    return len;
  }
 private:
  call_type call_;
  MilkTea_NonCopy(ByteReader)
  MilkTea_NonMove(ByteReader)
};

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_READER_H_
