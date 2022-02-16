#ifndef MILK_WINDOWS_REMOTE_H_
#define MILK_WINDOWS_REMOTE_H_

#include <memory>
#include <iostream>

#include "proc.h"
#include "proxy.h"

namespace Milk_Windows {

class RemoteImpl final {
  static constexpr size_t msg_size = 4;
  using msg_type = std::array<uint8_t, msg_size>;
 public:
  static constexpr char TAG[] = "Milk_Windows::RemoteImpl";
  static void Loop() {
    Proxy_CloseHandle(Proxy_GetStdHandle(Proxy_StdHandle_t::Proxy_STD_OUTPUT_HANDLE));
    Proxy_MMRESULT r = Proxy_MMSYSERR_NOERROR;
    Proxy_HMIDIOUT midi = nullptr;
    r = Proxy_midiOutOpen(midi, 0, nullptr, nullptr, 0);
    std::cerr << r << std::endl;
    MilkTea_defer({
      r = Proxy_midiOutClose(midi);
      std::cerr << r << std::endl;
    });
    msg_type msg{};
    for (size_t c = 0; !std::cin.eof(); ) {
      {
        uint8_t *pos = msg.data() + c;
        size_t n = msg_size - c;
        std::cin.read(reinterpret_cast<char *>(pos), n);
      }
      {
        size_t g = std::cin.gcount();
        c += g;
      }
      if (c < msg_size) {
        continue;
      }
      c = 0;
      r = Proxy_midiOutShortMsg(midi, *reinterpret_cast<uint32_t *>(msg.data()));
      std::cerr << r << std::endl;
    }
  }
  static std::shared_ptr<RemoteImpl> Make(std::string_view work_path) {
    PipeWrapper pipe_in({ true, false, });
    if (!pipe_in) {
      return nullptr;
    }
    PipeWrapper pipe_out({ false, true, });
    if (!pipe_out) {
      return nullptr;
    }
    ProcessWrapper proc(MilkTea::ToString::From(" ")(work_path, TAG), { &pipe_in, &pipe_out, nullptr, });
    if (!proc) {
      return nullptr;
    }
    pipe_in.CloseReader();
    pipe_out.CloseWriter();
    uint8_t b[4] = { 0, 0, 0, 0, };
    pipe_out.Read(b, 1);
    pipe_in.Write(b, 4);
    return std::make_shared<RemoteImpl>(std::make_unique<PipeWrapper>(std::move(pipe_in)));
  }
  explicit RemoteImpl(std::unique_ptr<MilkTea::BaseWriter> writer) : writer_(std::move(writer)) {}
  RemoteImpl(RemoteImpl &&another) : RemoteImpl(std::move(another.writer_)) {}
  void SendMsg(uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1) {
    uint32_t msg = arg1 << 020 | arg0 << 010 | type | channel;
    writer_->Write(reinterpret_cast<const uint8_t *>(&msg), msg_size);
  }
 private:
  std::unique_ptr<MilkTea::BaseWriter> writer_;
  MilkTea_NonCopy(RemoteImpl)
  MilkTea_NonMoveAssign(RemoteImpl)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_REMOTE_H_
