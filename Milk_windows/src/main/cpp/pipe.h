#ifndef MILK_WINDOWS_PIPE_H_
#define MILK_WINDOWS_PIPE_H_

#include "proxy.h"

namespace Milk_Windows {

class PipeWrapper final : public MilkTea::BaseReader, public MilkTea::BaseWriter {
  static constexpr char TAG[] = "Milk_Windows::PipeWrapper";
 public:
  enum class Type {
    READER,
    WRITER,
  };
  explicit PipeWrapper(std::array<bool, 2> bInherit = { false, false, }) : obj_{} {
    bool b = Milk_Windows_Proxy_invoke(Proxy_CreatePipe, obj_, bInherit);
    if (b) {
      MilkTea_logI("ctor success, bInherit: { %d, %d, }, hPipe: { %p, %p, }",
        bInherit[0], bInherit[1], obj_.reader_, obj_.writer_);
    } else {
      MilkTea_logI("ctor success, bInherit: { %d, %d, }",
        bInherit[0], bInherit[1]);
    }
  }
  PipeWrapper(PipeWrapper &&another)
  : obj_{
      .reader_ = another.release(Type::READER),
      .writer_ = another.release(Type::WRITER),
  } {}
  ~PipeWrapper() final {
    CloseReader();
    CloseWriter();
  }
  operator bool() const {
    return get(Type::READER) != nullptr && get(Type::WRITER) != nullptr;
  }
  PipeWrapper &Move() && final {
    return *new PipeWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  size_t Read(uint8_t bytes[], size_t len) final {
    if (len == 0) {
      return 0;
    }
    size_t n = 0;
    bool b = true;
    while (b && n != len) {
      size_t sz = 0;
      b = Milk_Windows_Proxy_invoke(Proxy_ReadFile, obj_.reader_, bytes + n, len - n, sz);
      if (sz > len - n) {
        n = len;
        MilkTea_logW("Read -- sz: %" PRIu32 ", len: %" PRIu32 ", n: %" PRIu32 ", hPipe: { %p, %p, } ",
          (uint32_t) sz, (uint32_t) len, (uint32_t) n, obj_.reader_, obj_.writer_);
      } else {
        n += sz;
      }
    }
    return n;
  }
  void Write(const uint8_t bytes[], size_t len) final {
    if (len == 0) {
      return;
    }
    size_t n = 0;
    bool b = true;
    while (b && n != len) {
      size_t sz = 0;
      b = Milk_Windows_Proxy_invoke(Proxy_WriteFile, obj_.writer_, bytes + n, len - n, sz);
      if (sz > len - n) {
        n = len;
        MilkTea_logW("Write -- sz: %" PRIu32 ", len: %" PRIu32 ", n: %" PRIu32 ", hPipe: { %p, %p, } ",
          (uint32_t) sz, (uint32_t) len, (uint32_t) n, obj_.reader_, obj_.writer_);
      } else {
        n += sz;
      }
    }
  }
  void CloseReader() {
    auto h = release(Type::READER);
    if (h == nullptr) {
      return;
    }
    bool b = Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
    MilkTea_logI("CloseReader -- success: %d, hPipe: %p", b, h);
  }
  void CloseWriter() {
    auto h = release(Type::WRITER);
    if (h == nullptr) {
      return;
    }
    bool b = Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
    MilkTea_logI("CloseWriter -- success: %d, hPipe: %p", b, h);
  }
  void Close(Proxy_HANDLE Proxy_Pipe_t::*member) {
    if (member == &Proxy_Pipe_t::reader_) {
      CloseReader();
      return;
    }
    if (member == &Proxy_Pipe_t::writer_) {
      CloseWriter();
      return;
    }
  }
  Proxy_HANDLE get(Type type) const {
    switch (type) {
      case Type::READER: return obj_.reader_;
      case Type::WRITER: return obj_.writer_;
      default: return nullptr;
    }
  }
 private:
  Proxy_HANDLE release(Type type) {
    return reset(type, nullptr);
  }
  Proxy_HANDLE reset(Type type, Proxy_HANDLE another) {
    switch (type) {
      case Type::READER: std::swap(another, obj_.reader_); break;
      case Type::WRITER: std::swap(another, obj_.writer_); break;
      default: another = nullptr; break;
    }
    return another;
  }
  Proxy_Pipe_t obj_{};
  MilkTea_NonCopy(PipeWrapper)
  MilkTea_NonMoveAssign(PipeWrapper)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_PIPE_H_
