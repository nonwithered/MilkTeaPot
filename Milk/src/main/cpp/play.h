#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <thread>
#include <chrono>

#include <soymilk.h>

#include "launch.h"

namespace Milk {

class RendererImpl final : public SoyMilk::BaseRenderer {
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  RendererImpl(uint16_t format, uint16_t ntrks)
  : format_(format),
    ntrks_(ntrks),
    handle_(),
    seek_(nullptr) {
    if (format == 0x02) {
      for (uint16_t i = 0; i != ntrks; ++i) {
        handle_.push_back(Make());
      }
    } else {
      handle_.push_back(Make());
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
    return new RendererImpl(std::forward<RendererImpl>(*this));
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
    std::cerr << "OnPrepareListener" << " " << time.count() << std::endl;
    if (OnPrepareListener) {
      OnPrepareListener(time);
    }
  }
  std::function<void()> OnStartListener;
  void OnStart() final {
    std::cerr << "OnStartListener" << std::endl;
    if (OnStartListener) {
      OnStartListener();
    }
  }
  std::function<void(duration_type)> OnPauseListener;
  void OnPause(duration_type time) final {
    std::cerr << "OnPauseListener" << std::endl;
    if (OnPauseListener) {
      OnPauseListener(time);
    }
  }
  std::function<void()> OnSeekBeginListener;
  void OnSeekBegin() final {
    std::cerr << "OnSeekBeginListener" << std::endl;
    if (OnSeekBeginListener) {
      OnSeekBeginListener();
    }
  }
  std::function<void(duration_type)> OnSeekEndListener;
  void OnSeekEnd(duration_type time) final {
    std::cerr << "OnSeekEndListener" << " " << time.count() << std::endl;
    if (OnSeekEndListener) {
      OnSeekEndListener(time);
    }
  }
  std::function<void()> OnResumeListener;
  void OnResume() final {
    std::cerr << "OnResumeListener" << std::endl;
    if (OnResumeListener) {
      OnResumeListener();
    }
  }
  std::function<void()> OnStopListener;
  void OnStop() final {
    std::cerr << "OnStopListener" << std::endl;
    if (OnStopListener) {
      OnStopListener();
    }
  }
  std::function<void()> OnResetListener;
  void OnReset() final {
    std::cerr << "OnResetListener" << std::endl;
    if (OnResetListener) {
      OnResetListener();
    }
  }
  std::function<void()> OnCompleteListener;
  void OnComplete() final {
    std::cerr << "OnCompleteListener" << std::endl;
    if (OnCompleteListener) {
      OnCompleteListener();
    }
  }
  bool *seek_;
 private:
  static SoyBean::HandleWrapper Make() {
    return ConfigWrapper::Instance().make_soybean_factory().make_handle();
  }
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

class Play final : public Command {
  static constexpr char TAG[] = "Milk::Play";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  Play() : Command() {
    Callback("-h", &Play::ShowHelp);
    Callback("--help", &Play::ShowHelp);
    Callback("--log", &Play::InitLog);
  }
 protected:
  void Launch(std::list<std::string_view> &args) final {
    MilkPowder::MidiMutableWrapper midi(nullptr);
    {
      auto filename = args.front();
      MilkPowder::FileReader reader(filename);
      if (reader.NonOpen()) {
        std::cerr << "Failed to open: " << filename << std::endl;
        return;
      }
      midi = MilkPowder::MidiMutableWrapper::Parse(reader);
    }
    TeaPot::TimerWorkerWrapper timer([](auto type, auto what) -> bool {
      if (type == MilkTea::Exception::Type::Nil) {
        return false;
      }
      std::cerr << MilkTea::Exception::TypeName(type) << ": " << what << std::endl;
      return false;
    });
    std::unique_ptr<SoyMilk::PlayerWrapper> player(nullptr);
    RendererImpl renderer(midi.GetFormat(), midi.GetNtrks());
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
  }
  std::string_view Usage() const final {
    return
"Usage: milk play\n"
"  -h, --help\n"
"    print this help message\n"
"  --log {d, i, w, e, debug, info, warn, error}\n"
"    init log level, or no log\n"
    ;
  }
  std::string_view Name() const final {
    return "play";
  }
 private:
  bool help_;
  bool ShowHelp(std::list<std::string_view>::iterator &, std::list<std::string_view> &) {
    if (help_) {
      return true;
    } else {
      help_ = true;
    }
    std::cerr << Usage() << std::endl;
    return true;
  }
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
