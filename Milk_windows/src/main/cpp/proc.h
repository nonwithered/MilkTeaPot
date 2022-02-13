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
    std::array<std::function<void()>, 3> sweep{ []() {}, []() {}, []() {}, };
    MilkTea_defer({
      for (size_t i = 0; i != 3; ++i) {
        sweep[i]();
      }
    });
    std::array<Proxy_HANDLE, 3> hStd;
    sweep[0] = Inherit(hStd[0], pipes[0], &Proxy_Pipe_t::reader_, Proxy_StdHandle_t::Proxy_STD_INPUT_HANDLE);
    sweep[1] = Inherit(hStd[1], pipes[1], &Proxy_Pipe_t::writer_, Proxy_StdHandle_t::Proxy_STD_OUTPUT_HANDLE);
    sweep[2] = Inherit(hStd[2], pipes[2], &Proxy_Pipe_t::writer_, Proxy_StdHandle_t::Proxy_STD_ERROR_HANDLE);
    Proxy_PROCESS_INFORMATION proc{};
    bool b = Milk_Windows_Proxy_invoke(Proxy_CreateProcess, proc, std::string(line), std::move(hStd));
    if (b) {
      reset(proc);
      CloseInfo();
    }
  }
  ProcessWrapper(ProcessWrapper &&another) : obj_(another.release()) {}
  ~ProcessWrapper() {
    CloseInfo();
  }
 private:
  static std::function<void()> Inherit(Proxy_HANDLE &hFile, PipeWrapper *pipe, Proxy_HANDLE Proxy_Pipe_t::*member, Proxy_StdHandle_t nStdHandle) {
    if (pipe == nullptr) {
      hFile = Proxy_GetStdHandle(nStdHandle);
      return []() {};
    }
    hFile = pipe->get(member);
    return [pipe, member]() {
      pipe->Close(member);
    };
  }
  void CloseInfo() {
    Close(&Proxy_PROCESS_INFORMATION::hThread);
    Close(&Proxy_PROCESS_INFORMATION::hProcess);
  }
  void Close(Proxy_HANDLE Proxy_PROCESS_INFORMATION::*member) {
    auto h = release(member);
    if (h == nullptr) {
      return;
    }
    Milk_Windows_Proxy_invoke(Proxy_CloseHandle, h);
  }
  Proxy_HANDLE release(Proxy_HANDLE Proxy_PROCESS_INFORMATION::*member) {
    return reset(member, nullptr);
  }
  Proxy_HANDLE reset(Proxy_HANDLE Proxy_PROCESS_INFORMATION::*member, Proxy_HANDLE another) {
    std::swap(another, obj_.*member);
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
