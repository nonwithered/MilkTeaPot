#ifndef MILK_RENDERER_H_
#define MILK_RENDERER_H_

#include <soybean.h>
#include <soymilk.h>

namespace Milk {

class PlayRenderer final : public SoyMilk::BaseRenderer {
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  PlayRenderer(SoyMilk::BaseRenderer &reporter, SoyBean::BaseFactory &factory, uint16_t format, uint16_t ntrks)
  : reporter_(reporter),
    format_(format),
    ntrks_(ntrks),
    handle_() {
    if (!IsMulti()) {
      handle_.emplace_back(factory.Create());
      return;
    }
    for (uint16_t i = 0; i != ntrks; ++i) {
      handle_.emplace_back(factory.Create());
    }
  }
  PlayRenderer *Move() && final {
    return new PlayRenderer(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void OnFrame(SoyMilk::Codec::FrameBufferWrapper fbo) final {
    for (size_t i = 0, n = fbo.GetLen(); i != n; ++i) {
      auto item = fbo.GetItem(i);
      auto idx = 0;
      if (IsMulti()) {
        idx = item.GetNtrk();
      }
      for (size_t j = 0, m = item.GetLen(); j != m; ++j) {
        auto message = item.GetMsg(j);
        if (!message.IsEvent()) {
          continue;
        }
        auto event = MilkPowder::EventConstWrapper::From(message);
        OnFrame(idx, event);
      }
    }
  }
  void OnPrepare(duration_type time) final {
    Reporter().OnPrepare(time);
  }
  void OnStart() final {
    Reporter().OnStart();
  }
  void OnPause(duration_type time) final {
    Reporter().OnPause(time);
  }
  void OnSeekBegin() final {
    Reporter().OnSeekBegin();
  }
  void OnSeekEnd(duration_type time) final {
    Reporter().OnSeekEnd(time);
  }
  void OnResume() final {
    Reporter().OnResume();
  }
  void OnStop() final {
    Reporter().OnStop();
  }
  void OnReset() final {
    Reporter().OnReset();
  }
  void OnComplete() final {
    Reporter().OnComplete();
  }
 private:
  PlayRenderer(PlayRenderer &&another)
  : reporter_(another.reporter_),
    format_(another.format_),
    ntrks_(another.ntrks_),
    handle_(std::move(another.handle_)) {}
  void OnFrame(uint16_t index, MilkPowder::EventConstWrapper event) {
    Frame(handle_[index], event.GetType(), event.GetArgs());
  }
  bool IsMulti() const {
    return format_ == 0x02;
  }
  SoyMilk::BaseRenderer &Reporter() {
    return reporter_;
  }
  static void Frame(SoyBean::BaseHandle &handle, uint8_t type, std::array<uint8_t, 2> args) {
    uint8_t channel = type & 0x0f;
    type &= 0xf0;
    switch (type) {
      case 0x80:
        handle.NoteOff(channel, args[0], args[1]);
        break;
      case 0x90:
        handle.NoteOn(channel, args[0], args[1]);
        break;
      case 0xa0:
        handle.AfterTouch(channel, args[0], args[1]);
        break;
      case 0xb0:
        handle.ControlChange(channel, args[0], args[1]);
        break;
      case 0xc0:
        handle.ProgramChange(channel, args[0]);
        break;
      case 0xd0:
        handle.ChannelPressure(channel, args[0]);
        break;
      case 0xe0:
        handle.PitchBend(channel, args[0], args[1]);
        break;
    }
  }
  SoyMilk::BaseRenderer &reporter_;
  const uint16_t format_;
  const uint16_t ntrks_;
  std::vector<SoyBean::HandleWrapper> handle_;
  MilkTea_NonCopy(PlayRenderer)
  MilkTea_NonMoveAssign(PlayRenderer)
};


} // namespace Milk

#endif // ifndef MILK_RENDERER_H_
