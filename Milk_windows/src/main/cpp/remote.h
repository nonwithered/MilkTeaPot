#ifndef MILK_WINDOWS_REMOTE_H_
#define MILK_WINDOWS_REMOTE_H_

#include <memory>
#include <iostream>

#include <soybean_windows.h>

#include "proc.h"

namespace Milk_Windows {

class RemoteImpl final {
  static constexpr size_t msg_size = 4;
  using msg_type = std::array<uint8_t, msg_size>;
 public:
  static constexpr char TAG[] = "Milk_Windows::RemoteImpl";
  static void Loop() {
    auto midi = []() -> auto {
      SoyBean::FactoryWrapper factory = SoyBean_Windows::make_factory(0, nullptr, nullptr, 0);
      SoyBean::HandleWrapper handle = factory.Create();
      return handle;
    }();
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
      RecvMsg(midi, msg[0], msg[1], msg[2], msg[3]);
    }
  }
  static void RecvMsg(SoyBean::BaseHandle &midi, uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1) {
    type &= 0xf0;
    channel &= 0x0f;
    switch (type) {
      case 0x80: midi.NoteOff(channel, arg0, arg1); return;
      case 0x90: midi.NoteOn(channel, arg0, arg1); return;
      case 0xa0: midi.AfterTouch(channel, arg0, arg1); return;
      case 0xb0: midi.ControlChange(channel, arg0, arg1); return;
      case 0xc0: midi.ProgramChange(channel, arg0); return;
      case 0xd0: midi.ChannelPressure(channel, arg0); return;
      case 0xe0: midi.PitchBend(channel, arg0, arg1); return;
    }
  }
  static std::shared_ptr<RemoteImpl> Make(std::string_view work_path) {
    PipeWrapper pipe({ true, false, });
    if (!pipe) {
      return nullptr;
    }
    ProcessWrapper proc(MilkTea::ToString::From(" ")(work_path, TAG), { &pipe, nullptr, nullptr, });
    if (!proc) {
      return nullptr;
    }
    pipe.CloseReader();
    return std::make_shared<RemoteImpl>(std::make_unique<PipeWrapper>(std::move(pipe)));
  }
  explicit RemoteImpl(std::unique_ptr<MilkTea::BaseWriter> writer) : writer_(std::move(writer)) {}
  RemoteImpl(RemoteImpl &&another) : RemoteImpl(std::move(another.writer_)) {}
  void SendMsg(uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1) {
    msg_type msg = { type, channel, arg0, arg1, };
    writer_->Write(msg.data(), msg_size);
  }
 private:
  std::unique_ptr<MilkTea::BaseWriter> writer_;
  MilkTea_NonCopy(RemoteImpl)
  MilkTea_NonMoveAssign(RemoteImpl)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_REMOTE_H_
