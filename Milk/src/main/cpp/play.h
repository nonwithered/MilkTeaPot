#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <thread>
#include <chrono>

#include <milkpowder.h>
#include <soymilk.h>

#include "control.h"

namespace Milk {

class RendererImpl final : public SoyMilk::BaseRenderer {
  using tempo_type = SoyMilk::tempo_type;
 public:
  RendererImpl(SoyBean::FactoryWrapper factory, std::vector<uint16_t> format, std::vector<uint16_t> ntrks)
  : format_(format),
    handle_(format.size()),
    seek_(nullptr) {
    for (size_t i = 0, n = format.size(); i != n; ++i) {
      if (format_[i] == 0x02) {
        for (uint16_t j = 0; j != ntrks[i]; ++j) {
          handle_[i].emplace_back(factory.Create());
        }
      } else {
        handle_[i].emplace_back(factory.Create());
      }
    }
  }
  RendererImpl(RendererImpl &&another)
  : format_(another.format_),
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
  void OnFrame(const SoyMilk::FrameBufferWrapper &fbo) final {
    if (*seek_) {
      return;
    }
    fbo.AllMessage([this](size_t index, uint16_t ntrk, auto msg) {
      if (!msg.IsEvent()) {
        return;
      }
      auto event = MilkPowder::EventConstWrapper::From(msg);
      if (format_[index] == 0x02) {
        OnFrame(handle_[index][ntrk], event);
      } else {
        OnFrame(handle_[index][0], event);
      }
    });
  }
  std::function<void(tempo_type)> OnPrepareListener;
  void OnPrepare(tempo_type time) final {
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
  std::function<void(tempo_type)> OnPauseListener;
  void OnPause(tempo_type time) final {
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
  std::function<void(tempo_type)> OnSeekEndListener;
  void OnSeekEnd(tempo_type time) final {
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
  void OnFrame(SoyBean::HandleWrapper &handle, MilkPowder::EventConstWrapper event) {
    uint8_t type = event.GetType();
    auto args = event.GetArgs();
    handle.SendMessage(type, args[0], args[1]);
  }
  const std::vector<uint16_t> format_;
  std::vector<std::vector<SoyBean::HandleWrapper>> handle_;
};

class PlayController final : public BaseController<PlayController> {
  static constexpr char TAG[] = "Milk::PlayController";
  using tempo_type = SoyMilk::tempo_type;
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
  --listen
    only listen to music, no need to do play together
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
    std::vector<MilkPowder::MidiMutableWrapper> vec = [this, &args]() -> auto {
      auto n = args.size();
      std::vector<MilkPowder::MidiMutableWrapper> v(n);
      for (size_t i = 0; i != n; ++i) {
        auto reader = Context().GetFileReader(args[i]);
        v[i] = MilkPowder::MidiMutableWrapper::Parse([&reader](auto bytes[], auto len) -> auto {
          return reader.Read(bytes, len);
        });
      }
      return v;
    }();
    TeaPot::TimerWorkerWrapper timer([this](auto type, auto what) -> bool {
      if (type == MilkTea::Exception::Type::Nil) {
        return false;
      }
      Err() << MilkTea::Exception::TypeName(type) << ": " << what << End();
      return false;
    });
    std::unique_ptr<SoyMilk::PlayerWrapper> player(nullptr);
    std::vector<std::uint16_t> format_vec(vec.size());
    std::vector<std::uint16_t> ntrk_vec(vec.size());
    for (size_t i = 0, n = vec.size(); i != n; ++i) {
      format_vec[i] = vec[i].GetFormat();
      ntrk_vec[i] = vec[i].GetNtrks();
    }
    RendererImpl renderer(Context().GetSoyBeanFactory(), std::move(format_vec), std::move(ntrk_vec));
    tempo_type pos = tempo_type::zero();
    bool seek = false;
    renderer.seek_ = &seek;
    renderer.OnPrepareListener = [&](auto time) {
      pos = time - tempo_type(10'000'000);
      pos = tempo_type::zero();
      timer.Post([&]() {
        player->Start();
      });
    };
    renderer.OnStartListener = [&]() {
      timer.Post([&]() {
        player->Resume();
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
      player->Prepare(vec.begin(), vec.size());
      vec.clear();
    });
    timer.Start();
    timer.AwaitTermination();
    timer.Close();
    Out() << "return" << End();
  }
  pipeline_type Config(pipeline_type &&pipeline) final {
    return super_type::Config(std::move(pipeline))
      .Append({
          "-r",
          "--range",
        }, &self_type::SetRange)
      .Append({
          "--listen",
        }, &self_type::listen_, true)
      .Append({
          "-o",
        }, &self_type::target_,
        [this](auto &option) {
          Err() << Tip(option) << "need target name" << End();
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
  bool SetRange(const value_type &option, cursor_type &cursor) {
    auto f = [this, &option, &cursor](int64_t &position_, std::string_view name) -> bool {
      if (!cursor) {
        Err() << Tip(option) << "need " << name << " position value" << End();
        return false;
      }
      auto sv = *cursor;
      bool negative = false;
      int64_t position = 0;
      size_t n = sv.size();
      size_t len = 0;
      bool l = false;
      MilkTea_block({
        auto s = sv.data();
        while (n >= 1 && s[0] == '-') {
          negative = !negative;
          s += 1;
          n -= 1;
        }
        if (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
          MilkTea_logD("SetRange %s dec", name.data());
          s += 2;
          n -= 2;
          if (n > 60 / 4) {
            l = true;
            break;
          }
          len = MilkTea::FromStringHex::ToInt<int64_t>(s, n, &position);
          break;
        }
        if (n > 1 && s[0] == '0') {
          MilkTea_logD("SetRange %s oct", name.data());
          s += 1;
          n -= 1;
          if (n > 60 / 3) {
            l = true;
            break;
          }
          len = MilkTea::FromStringOct::ToInt<int64_t>(s, n, &position);
          break;
        }
        {
          MilkTea_logD("SetRange %s dec", name.data());
          if (n > 3 * 6) {
            l = true;
            break;
          }
          len = MilkTea::FromStringDec::ToInt<int64_t>(s, n, &position);
        }
      });
      if (l) {
        Err() << Tip(option) << "the " << name << " position value is too long -- " << sv << End();
        return false;
      }
      if (len != n) {
        Err() << Tip(option) << "invalid " << name << " position value -- " << sv << End();
        return false;
      }
      if (negative) {
        position *= -1;
      }
      position_ = position;
      ++cursor;
      MilkTea_logD("SetRange %s %" PRId64, name.data(), position);
      return true;
    };
    if (!f(begin_, "begin")) {
      return false;
    }
    if (!f(end_, "end")) {
      return false;
    }
    return true;
  }
  bool SetTick(const value_type &option, cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip(option) << "need division value" << End();
      return false;
    }
    auto sv = *cursor;
    uint16_t division = 0;
    size_t n = sv.size();
    size_t len = 0;
    {
      auto s = sv.data();
      if (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        n -= 2;
      }
      if (n > 4) {
        Err() << Tip(option) << "the division value is too long -- " << sv << End();
        return false;
      }
      len = MilkTea::FromStringHex::ToInt<uint16_t>(s, n, &division);
    }
    if (len != n) {
      Err() << Tip(option) << "invalid division value -- " << sv << End();
      return false;
    }
    if (division == 0) {
      Err() << Tip(option) << "division value can not be zero" << End();
      return false;
    }
    division_ = division;
    ++cursor;
    return true;
  }
  bool SetTempo(const value_type &option, cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip(option) << "need tempo value" << End();
      return false;
    }
    auto sv = *cursor;
    uint32_t tempo = 0;
    size_t n = sv.size();
    size_t len = 0;
    bool l = false;
    MilkTea_block({
      auto s = sv.data();
      if (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        MilkTea_logD("SetTempo hex");
        s += 2;
        n -= 2;
        if (n > 24 / 4) {
          l = true;
          break;
        }
        len = MilkTea::FromStringHex::ToInt<uint32_t>(s, n, &tempo);
        break;
      }
      if (n > 1 && s[0] == '0') {
        MilkTea_logD("SetTempo oct");
        s += 1;
        n -= 1;
        if (n > 24 / 3) {
          l = true;
          break;
        }
        len = MilkTea::FromStringOct::ToInt<uint32_t>(s, n, &tempo);
        break;
      }
      {
        MilkTea_logD("SetTempo dec");
        if (n > 8) {
          l = true;
          break;
        }
        len = MilkTea::FromStringDec::ToInt<uint32_t>(s, n, &tempo);
        if (tempo > 0xff'ff'ff) {
          l = true;
          break;
        }
      }
    });
    if (l) {
      Err() << Tip(option) << "the tempo value is too long -- " << sv << End();
      return false;
    }
    if (len != n) {
      Err() << Tip(option) << "invalid tempo value -- " << sv << End();
      return false;
    }
    if (tempo == 0) {
      Err() << Tip(option) << "tempo value can not be zero" << End();
      return false;
    }
    tempo_ = tempo;
    ++cursor;
      MilkTea_logD("SetTempo %" PRId64, tempo);
    return true;
  }
  std::string_view target_ = "";
  bool background_ = false;
  uint16_t division_ = 0x0078;
  uint32_t tempo_ = 60'000'000 / 120;
  int64_t begin_ = 0;
  int64_t end_ = 0;
  bool listen_ = false;
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
