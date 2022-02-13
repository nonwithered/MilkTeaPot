#ifndef MILK_WINDOWS_PROC_H_
#define MILK_WINDOWS_PROC_H_

#include <string>
#include <string_view>
#include <functional>

#include "pipe.h"

namespace Milk_Windows {

class ProcessWrapper final {
  static constexpr char TAG[] = "Milk_Windows::ProcessWrapper";
 public:
  explicit ProcessWrapper(std::string_view line, std::array<PipeWrapper *, 3> pipes) : obj_{} {
    std::array<Proxy_HANDLE, 3> hStd = {
      Inherit(pipes[0], PipeWrapper::Type::READER, Proxy_StdHandle_t::Proxy_STD_INPUT_HANDLE),
      Inherit(pipes[1], PipeWrapper::Type::WRITER, Proxy_StdHandle_t::Proxy_STD_OUTPUT_HANDLE),
      Inherit(pipes[2], PipeWrapper::Type::WRITER, Proxy_StdHandle_t::Proxy_STD_ERROR_HANDLE),
    };
    Proxy_PROCESS_INFORMATION proc{};
    bool b = Milk_Windows_Proxy_invoke(Proxy_CreateProcess, proc, std::string(line), std::move(hStd));
    MilkTea_logI("ctor -- success: %d, this: %p, pipes: { %p, %p, %p, }", b, this, pipes[0], pipes[1], pipes[2]);
    if (b) {
      reset(proc);
      CloseInfo();
    }
  }
  ProcessWrapper(ProcessWrapper &&another) : obj_(another.release()) {}
  ~ProcessWrapper() {
    CloseInfo();
    obj_.dwProcessId = 0;
    obj_.dwThreadId = 0;
  }
  operator bool() const {
    return obj_.dwProcessId != 0 && obj_.dwThreadId != 0;
  }
 private:
  enum class InfoType {
    PROCESS,
    THREAD,
  };
  static Proxy_HANDLE Inherit(PipeWrapper *pipe, PipeWrapper::Type type, Proxy_StdHandle_t nStdHandle) {
    if (pipe == nullptr) {
      return Proxy_GetStdHandle(nStdHandle);
    }
    return pipe->get(type);
  }
  void CloseInfo() {
    CloseProcess();
    CloseThread();
  }
  void CloseProcess() {
    auto h = release(InfoType::PROCESS);
    if (h == nullptr) {
      return;
    }
    bool b = Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
    MilkTea_logI("CloseProcess -- success: %d, hProcess: %p", b, h);
  }
  void CloseThread() {
    auto h = release(InfoType::THREAD);
    if (h == nullptr) {
      return;
    }
    bool b = Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
    MilkTea_logI("CloseThread -- success: %d, hThread: %p", b, h);
  }
  Proxy_HANDLE release(InfoType type) {
    return reset(type, nullptr);
  }
  Proxy_HANDLE reset(InfoType type, Proxy_HANDLE another) {
    switch (type) {
      case InfoType::PROCESS: std::swap(another, obj_.hProcess); break;
      case InfoType::THREAD: std::swap(another, obj_.hThread); break;
      default: another = nullptr; break;
    }
    return another;
  }
  Proxy_PROCESS_INFORMATION release() {
    return reset({});
  }
  Proxy_PROCESS_INFORMATION reset(Proxy_PROCESS_INFORMATION another) {
    std::swap(another, obj_);
    return another;
  }
  Proxy_PROCESS_INFORMATION obj_{};
  MilkTea_NonCopy(ProcessWrapper)
  MilkTea_NonMoveAssign(ProcessWrapper)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_PROC_H_
