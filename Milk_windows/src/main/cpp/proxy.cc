#include "proxy.h"

#include <windows.h>

namespace Milk_Windows {

size_t Proxy_FormatMessage(std::string &buffer, uint32_t &dwMessageId, size_t nSize) {
  auto e = GetLastError();
  dwMessageId = e;
  char (*lpBuffer)[] = nullptr;
  auto n = FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    nullptr,
    e,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (char *) &lpBuffer,
    nSize,
    nullptr
  );
  buffer = *lpBuffer;
  if (lpBuffer != nullptr) {
    LocalFree(lpBuffer);
  }
  return n;
}

bool Proxy_CloseHandle(Proxy_HANDLE hObject) {
  return CloseHandle(hObject);
}

Proxy_HANDLE Proxy_GetStdHandle(Proxy_StdHandle_t nStdHandle) {
  switch (nStdHandle) {
    case Proxy_StdHandle_t::Proxy_STD_INPUT_HANDLE: return (Proxy_HANDLE) GetStdHandle(STD_INPUT_HANDLE);
    case Proxy_StdHandle_t::Proxy_STD_OUTPUT_HANDLE: return (Proxy_HANDLE) GetStdHandle(STD_OUTPUT_HANDLE);
    case Proxy_StdHandle_t::Proxy_STD_ERROR_HANDLE: return (Proxy_HANDLE) GetStdHandle(STD_ERROR_HANDLE);
    default: return nullptr;
  }
}

bool Proxy_ReadFile(Proxy_HANDLE hFile, uint8_t buffer[], size_t nNumberOfBytesToRead, size_t &nNumberOfBytesRead) {
  DWORD lpNumberOfBytesRead = 0;
  bool b = ReadFile(hFile, buffer, nNumberOfBytesToRead, &lpNumberOfBytesRead, nullptr);
  if (b) {
    nNumberOfBytesRead = lpNumberOfBytesRead;
  }
  return b;
}

bool Proxy_WriteFile(Proxy_HANDLE hFile, const uint8_t buffer[], size_t nNumberOfBytesToWrite, size_t &nNumberOfBytesWritten) {
  DWORD lpNumberOfBytesWritten = 0;
  bool b = WriteFile(hFile, buffer, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, nullptr);
  if (b) {
    nNumberOfBytesWritten = lpNumberOfBytesWritten;
  }
  return b;
}

bool Proxy_CreatePipe(Proxy_Pipe_t &p, std::array<bool, 2> bInheritHandle_rw) {
  SECURITY_ATTRIBUTES attr{};
  bool bInheritHandle = bInheritHandle_rw[0] || bInheritHandle_rw[1];
  if (bInheritHandle) {
    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.bInheritHandle = true;
    attr.lpSecurityDescriptor = nullptr;
  }
  HANDLE reader_ = nullptr;
  HANDLE writer_ = nullptr;
  bool b = CreatePipe(&reader_, &writer_, &attr, 0);
  if (b) {
    p.reader_ = (Proxy_HANDLE) reader_;
    p.writer_ = (Proxy_HANDLE) writer_;
    if (bInheritHandle) {
      if (!bInheritHandle_rw[0]) {
        b = SetHandleInformation(reader_, HANDLE_FLAG_INHERIT, 0);
      }
      if (!bInheritHandle_rw[1]) {
        b = SetHandleInformation(writer_, HANDLE_FLAG_INHERIT, 0);
      }
    }
  }
  return b;
}

bool Proxy_CreateProcess(Proxy_PROCESS_INFORMATION &proc, std::string commandLine, std::array<Proxy_HANDLE, 3> hStd) {
  bool bInheritHandles = hStd[0] != nullptr || hStd[1] != nullptr || hStd[1] != nullptr;
  STARTUPINFO lpStartupInfo{};
  if (bInheritHandles) {
    lpStartupInfo.cb = sizeof(STARTUPINFO);
    lpStartupInfo.hStdInput = hStd[0];
    lpStartupInfo.hStdOutput = hStd[1];
    lpStartupInfo.hStdError = hStd[2];
    lpStartupInfo.dwFlags |= STARTF_USESTDHANDLES;
  }
  PROCESS_INFORMATION lpProcessInformation{};
  bool b = CreateProcess(nullptr, commandLine.data(), nullptr, nullptr, bInheritHandles, 0, nullptr, nullptr, &lpStartupInfo, &lpProcessInformation);
  if (b) {
    proc.hProcess = (Proxy_HANDLE) lpProcessInformation.hProcess;
    proc.hThread = (Proxy_HANDLE) lpProcessInformation.hThread;
    proc.dwProcessId = lpProcessInformation.dwProcessId;
    proc.dwThreadId = lpProcessInformation.dwThreadId;
  }
  return b;
}

} // namespace Milk_Windows
