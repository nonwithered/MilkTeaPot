#ifndef MILK_WINDOWS_PIPE_H_
#define MILK_WINDOWS_PIPE_H_

#include "proxy.h"

namespace Milk_Windows {

class PipeWrapper final : public MilkTea::BaseReader, public MilkTea::BaseWriter {
  static constexpr char TAG[] = "Milk_Windows::PipeWrapper";
 public:
  explicit PipeWrapper(std::array<bool, 2> bInherit = { false, false, }) : obj_{} {
    Milk_Windows_Proxy_invoke(Proxy_CreatePipe, obj_, bInherit);
  }
  PipeWrapper(PipeWrapper &&another)
  : obj_{
      .reader_ = another.release(&Proxy_Pipe_t::reader_),
      .writer_ = another.release(&Proxy_Pipe_t::writer_),
  } {}
  ~PipeWrapper() final {
    CloseReader();
    CloseWriter();
  }
  operator bool() const {
    return get(&Proxy_Pipe_t::reader_) != nullptr && get(&Proxy_Pipe_t::writer_) != nullptr;
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
      } else {
        n += sz;
      }
    }
  }
  void CloseReader() {
    Close(&Proxy_Pipe_t::reader_);
  }
  void CloseWriter() {
    Close(&Proxy_Pipe_t::writer_);
  }
  void Close(Proxy_HANDLE Proxy_Pipe_t::*member) {
    auto h = release(member);
    if (h == nullptr) {
      return;
    }
    Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
  }
  Proxy_HANDLE get(Proxy_HANDLE Proxy_Pipe_t::*member) const {
    return obj_.*member;
  }
 private:
  Proxy_HANDLE release(Proxy_HANDLE Proxy_Pipe_t::*member) {
    return reset(member, nullptr);
  }
  Proxy_HANDLE reset(Proxy_HANDLE Proxy_Pipe_t::*member, Proxy_HANDLE another) {
    std::swap(another, obj_.*member);
    return another;
  }
  Proxy_Pipe_t obj_{};
  MilkTea_NonCopy(PipeWrapper)
  MilkTea_NonMoveAssign(PipeWrapper)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_PIPE_H_
