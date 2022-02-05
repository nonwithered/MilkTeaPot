#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <thread>
#include <chrono>

#include <milkpowder.h>
#include <soymilk.h>

#include "control.h"

namespace Milk {

class RendererImpl final : public SoyMilk::BaseRenderer {
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  RendererImpl(SoyBean::FactoryWrapper factory, uint16_t format, uint16_t ntrks)
  : format_(format),
    ntrks_(ntrks),
    handle_(),
    seek_(nullptr) {
    if (format == 0x02) {
      for (uint16_t i = 0; i != ntrks; ++i) {
        handle_.emplace_back(factory.Create());
      }
    } else {
      handle_.emplace_back(factory.Create());
    }
  }
  RendererImpl(RendererImpl &&another)
  : format_(another.format_),
    ntrks_(another.ntrks_),
    handle_(std::move(another.handle_)),
    OnPrepareListener(another.OnPrepareListener),
    OnStartListener(another.OnStartListener),
    OnPauseListener(another.OnPauseListener),
    OnSeekBeginListener(another.OnSeekBeginListener),
    OnSeekEndListener(another.OnSeekEndListener),
    OnResumeListener(another.OnResumeListener),
    OnStopListener(another.OnStopListener),
    OnResetListener(another.OnResetListener),
    OnCompleteListener(another.OnCompleteListener),
    seek_(another.seek_) {}
  ~RendererImpl() final = default;
  RendererImpl *Move() && final {
    return new RendererImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void OnFrame(SoyMilk::Codec::FrameBufferWrapper fbo) final {
    if (*seek_) {
      return;
    }
    for (size_t i = 0, n = fbo.GetLen(); i != n; ++i) {
      auto item = fbo.GetItem(i);
      for (size_t j = 0, m = item.GetLen(); j != m; ++j) {
        auto message = item.GetMsg(j);
        if (!message.IsEvent()) {
          continue;
        }
        auto event = MilkPowder::EventConstWrapper::From(message);
        if (format_ == 0x02) {
          OnFrame(item.GetNtrk(), event);
        } else {
          OnFrame(0, event);
        }
      }
    }
  }
  std::function<void(duration_type)> OnPrepareListener;
  void OnPrepare(duration_type time) final {
//    Err() << "OnPrepareListener" << " " << time.count() << End();
    if (OnPrepareListener) {
      OnPrepareListener(time);
    }
  }
  std::function<void()> OnStartListener;
  void OnStart() final {
//    Err() << "OnStartListener" << End();
    if (OnStartListener) {
      OnStartListener();
    }
  }
  std::function<void(duration_type)> OnPauseListener;
  void OnPause(duration_type time) final {
//    Err() << "OnPauseListener" << End();
    if (OnPauseListener) {
      OnPauseListener(time);
    }
  }
  std::function<void()> OnSeekBeginListener;
  void OnSeekBegin() final {
//    Err() << "OnSeekBeginListener" << End();
    if (OnSeekBeginListener) {
      OnSeekBeginListener();
    }
  }
  std::function<void(duration_type)> OnSeekEndListener;
  void OnSeekEnd(duration_type time) final {
//    Err() << "OnSeekEndListener" << " " << time.count() << End();
    if (OnSeekEndListener) {
      OnSeekEndListener(time);
    }
  }
  std::function<void()> OnResumeListener;
  void OnResume() final {
//    Err() << "OnResumeListener" << End();
    if (OnResumeListener) {
      OnResumeListener();
    }
  }
  std::function<void()> OnStopListener;
  void OnStop() final {
//    Err() << "OnStopListener" << End();
    if (OnStopListener) {
      OnStopListener();
    }
  }
  std::function<void()> OnResetListener;
  void OnReset() final {
//    Err() << "OnResetListener" << End();
    if (OnResetListener) {
      OnResetListener();
    }
  }
  std::function<void()> OnCompleteListener;
  void OnComplete() final {
//    Err() << "OnCompleteListener" << End();
    if (OnCompleteListener) {
      OnCompleteListener();
    }
  }
  bool *seek_;
 private:
  void OnFrame(uint16_t index, MilkPowder::EventConstWrapper event) {
    auto &handle = handle_[index];
    uint8_t type = event.GetType();
    uint8_t channel = type & 0x0f;
    type &= 0xf0;
    auto args = event.GetArgs();
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
  const uint16_t format_;
  const uint16_t ntrks_;
  std::vector<SoyBean::HandleWrapper> handle_;
};

class PlayController final : public BaseController<PlayController> {
  static constexpr char TAG[] = "Milk::PlayController";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  static constexpr auto kName = "play";
  static constexpr auto kUsage = R"(
Usage: milk play [OPTIONS] [FILES]
  -v, --version
    print version code
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
  -r <begin> <end>
  --range <begin> <end>
    set the range of playing time
    the parameters can be signed integer described in decimal, hexadecimal, octal
    the absolute value cannot be greater than 10 ^ 18 - 1 in decimal
                                           or  2 ^ 60 - 1 in hexadecimal and octal
  -o <filename>
    name of target file
    the input events will be recorded
  -b
  --background
    the background will be recorded together
  -t <division>
  --tick <division>
    set division for the target file by a 16 bit unsigned integer described in hexadecimal
    the default value is 0x0078
  -m <microseconds>
  --tempo <microseconds>
    set tempo for the target file in microseconds
    it should be an unsigned integer in decimal, hexadecimal, octal
    it will not take effect if the target is timecode based
    it cannot be greater than 2 ^ 24 - 1
    the default value is 500000
)";
  PlayController(ContextWrapper &context) : BaseController(context) {}
 protected:
  void Main(args_type &args) final {
    if (args.empty()) {
      Err() << Tip() << "no input files" << End();
      return;
    }
    auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
      auto filename = args.front();
      auto reader = Context().GetFileReader(filename);
      return MilkPowder::MidiMutableWrapper::Parse([&reader](auto bytes[], auto len) -> auto {
        return reader.Read(bytes, len);
      });
    }();
    TeaPot::TimerWorkerWrapper timer([this](auto type, auto what) -> bool {
      if (type == MilkTea::Exception::Type::Nil) {
        return false;
      }
      Err() << MilkTea::Exception::TypeName(type) << ": " << what << End();
      return false;
    });
    std::unique_ptr<SoyMilk::PlayerWrapper> player(nullptr);
    RendererImpl renderer(Context().GetSoyBeanFactory(), midi.GetFormat(), midi.GetNtrks());
    duration_type pos = duration_type::zero();
    bool seek = false;
    renderer.seek_ = &seek;
    renderer.OnPrepareListener = [&](auto time) {
      pos = time - duration_type(10'000'000);
      timer.Post([&]() {
        player->Start();
      });
    };
    renderer.OnStartListener = [&]() {
      timer.Post([&]() {
        player->Seek(pos);
      });
    };
    renderer.OnSeekBeginListener = [&]() {
      seek = true;
    };
    renderer.OnSeekEndListener = [&](auto time) {
      seek = false;
      timer.Post([&]() {
        player->Resume();
      });
    };
    renderer.OnCompleteListener = [&]() {
      timer.Post([&]() {
        player->Reset();
        timer.Shutdown();
      });
    };
    renderer.OnResetListener = [&]() {
      timer.Post([&]() {
        timer.Shutdown();
      });
    };
    player.reset(new SoyMilk::PlayerWrapper(std::move(renderer), [&timer](auto action) {
      timer.Post(action);
    }, timer));
    timer.Post([&]() {
      player->Prepare(midi.get());
    });
    timer.Start();
    timer.AwaitTermination();
    timer.Close();
    Out() << "return" << End();
  }
  pipeline_type Config(pipeline_type &&pipeline) final {
    return super_type::Config(std::move(pipeline))
      .Append({
          "-o",
        }, &self_type::target_,
        [this]() {
          Err() << Tip() << "-o: need target name" << End();
        })
      .Append({
          "-b",
          "--background",
        }, &self_type::background_, true)
      .Append({
          "-t",
          "--tick",
        }, &self_type::SetTick)
      .Append({
          "-m",
          "--tempo",
        }, &self_type::SetTempo)
      ;
  }
 private:
  bool SetTick(cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip() << "-t: need division value" << End();
      return false;
    }
    auto s = *cursor;
    const size_t n = s.size();
    size_t j = 0;
    if (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
      j = 2;
    }
    uint16_t division = 0;
    for (size_t i = j; i != n; ++i) {
      if (i - j >= 4) {
        Err() << Tip() << "-t: the division value is too long -- " << s << End();
        return false;
      }
      auto c = s[i];
      uint8_t b = 0;
      if ('0' <= c && c <= '9') {
        b = c - '0';
      } else if ('a' <= c && c <= 'f') {
        b = c - 'a' + 10;
      } else if ('A' <= c && c <= 'F') {
        b = c - 'A' + 10;
      } else {
        Err() << Tip() << "-t: invalid division value -- " << s << End();
        return false;
      }
      division <<= 04;
      division |= b;
    }
    if (division == 0) {
      Err() << Tip() << "-t: division value can not be zero" << End();
      return false;
    }
    division_ = division;
    ++cursor;
    return true;
  }
  bool SetTempo(cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip() << "-m: need tempo value" << End();
      return false;
    }
    auto s = *cursor;
    const size_t n = s.size();
    uint32_t tempo = 0;
    if (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
      for (size_t i = 2; i != n; ++i) {
        if (i >= 24 / 4 + 2) {
          Err() << Tip() << "-m: the tempo value is too long -- " << s << End();
          return false;
        }
        auto c = s[i];
        uint8_t b = 0;
        if ('0' <= c && c <= '9') {
          b = c - '0';
        } else if ('a' <= c && c <= 'f') {
          b = c - 'a' + 10;
        } else if ('A' <= c && c <= 'F') {
          b = c - 'A' + 10;
        } else {
          Err() << Tip() << "-m: invalid tempo value -- " << s << End();
          return false;
        }
        tempo <<= 04;
        tempo |= b;
      }
    } else if (n > 1 && s[0] == '0') {
      for (size_t i = 1; i != n; ++i) {
        if (i >= 24 / 3 + 1) {
          Err() << Tip() << "-m: the tempo value is too long -- " << s << End();
          return false;
        }
        auto c = s[i];
        uint8_t b = 0;
        if ('0' <= c && c <= '7') {
          b = c - '0';
        } else {
          Err() << Tip() << "-m: invalid tempo value -- " << s << End();
          return false;
        }
        tempo <<= 03;
        tempo |= b;
      }
    } else {
      for (size_t i = 1; i != n; ++i) {
        auto c = s[i];
        uint8_t b = 0;
        if ('0' <= c && c <= '9') {
          b = c - '0';
        } else {
          Err() << Tip() << "-m: invalid tempo value -- " << s << End();
          return false;
        }
        tempo *= 10;
        tempo += b;
        if (tempo > 0xff'ff'ff) {
          Err() << Tip() << "-m: the tempo value is too long -- " << s << End();
          return false;
        }
      }
    }
    if (tempo == 0) {
      Err() << Tip() << "-m: tempo value can not be zero" << End();
      return false;
    }
    tempo_ = tempo;
    ++cursor;
    return true;
  }
  std::string_view target_ = "";
  bool background_ = false;
  uint16_t division_ = 0x0078;
  uint32_t tempo_ = 60'000'000 / 120;
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
