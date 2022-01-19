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
        handle_.emplace_back(&factory.Create());
      }
    } else {
      handle_.emplace_back(&factory.Create());
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
    auto &handle = *handle_[index].get();
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
  std::vector<std::unique_ptr<SoyBean::BaseHandle>> handle_;
};

class PlayController final : public BaseController {
  static constexpr char TAG[] = "Milk::PlayController";
  using self_type = PlayController;
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  static constexpr auto kName = "play";
  static constexpr auto kUsage = R"(
Usage: milk play
  -v, --version
    print version code
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
)";
  PlayController(BaseContext &context)
  : BaseController(context, kUsage) {}
 public:
  void Main(std::list<std::string_view> &args) final {
    if (!BaseController::Config(Cocoa::Pipeline(*this, args))
        .Launch(kName)) {
      return;
    }
    BaseController::Main(args);
    if (args.empty()) {
      Err() << "milk " << kName << ": no input files" << End();
      return;
    }
    auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
      auto filename = args.front();
      auto reader = Context().GetFileReader(filename.data(), filename.size());
      return MilkPowder::MidiMutableWrapper::Parse(*reader);
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
 private:
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
