#ifndef LIB_MILKPOWDER_WRAPPER_WRITER_H_
#define LIB_MILKPOWDER_WRAPPER_WRITER_H_

#include <functional>

#include <milktea.h>

namespace MilkPowder {

class ByteWriter final {
  using call_type = std::function<MilkTea_Exception_t(const uint8_t[], size_t)>;
 public:
  ByteWriter(call_type call) : call_(call) {}
  void operator()(const uint8_t bytes[], size_t len) {
    MilkTea_invoke_panic(call_, bytes, len);
  }
 private:
  call_type call_;
  MilkTea_NonCopy(ByteWriter)
  MilkTea_NonMove(ByteWriter)
};

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_WRITER_H_
