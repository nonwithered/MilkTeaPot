#ifndef MILK_WINDOWS_PROXY_H_
#define MILK_WINDOWS_PROXY_H_

#include <array>
#include <string>
#include <cstdint>
#include <cinttypes>

#include <milktea.h>

#define Milk_Windows_Proxy_invoke(f, ...) \
Proxy_CheckLastError(TAG, #f, f(__VA_ARGS__))

namespace Milk_Windows {

size_t Proxy_FormatMessage(std::string &buffer, uint32_t &dwMessageId, size_t nSize);

inline
bool Proxy_CheckLastError(std::string_view tag, std::string_view s, bool b) {
  if (!b) {
    std::string buf;
    uint32_t e = 0;
    auto len = Proxy_FormatMessage(buf, e, 0);
    auto TAG = tag.data();
    if (len == 0) {
      MilkTea_logE("%s %" PRIu32 " -- FormatMessage error", s.data(), e);
    } else {
      MilkTea_logE("%s %" PRIu32 " %s", s.data(), e, buf.data());
    }
  }
  return b;
}

struct Proxy_HANDLE_t;
using Proxy_HANDLE = Proxy_HANDLE_t *;
bool Proxy_CloseHandle(Proxy_HANDLE hObject);

enum class Proxy_StdHandle_t {
  Proxy_STD_INPUT_HANDLE,
  Proxy_STD_OUTPUT_HANDLE,
  Proxy_STD_ERROR_HANDLE,
};

template<Proxy_StdHandle_t nStdHandle>
Proxy_HANDLE Proxy_GetStdHandle();

bool Proxy_ReadFile(Proxy_HANDLE hFile, uint8_t buffer[], size_t nNumberOfBytesToRead, size_t &nNumberOfBytesRead);
bool Proxy_WriteFile(Proxy_HANDLE hFile, const uint8_t buffer[], size_t nNumberOfBytesToWrite, size_t &nNumberOfBytesWritten);

struct Proxy_Pipe_t {
  Proxy_HANDLE reader_;
  Proxy_HANDLE writer_;
};

bool Proxy_CreatePipe(Proxy_Pipe_t &p, std::array<bool, 2> bInheritHandle_rw);

struct Proxy_PROCESS_INFORMATION_t {
    Proxy_HANDLE hProcess;
    Proxy_HANDLE hThread;
};

using Proxy_PROCESS_INFORMATION = Proxy_PROCESS_INFORMATION_t *;

bool Proxy_CreateProcess(Proxy_PROCESS_INFORMATION &proc, std::string commandLine, std::array<Proxy_HANDLE, 3> hStd);

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_PROXY_H_
