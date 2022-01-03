#ifndef SOYMILK_PLAYER_H_
#define SOYMILK_PLAYER_H_

#include <atomic>
#include <memory>

#include <soymilk/common.h>

#include "frame.h"
#include "decode.h"

namespace SoyMilk {

class PlayerImpl final : public std::enable_shared_from_this<PlayerImpl> {
  static constexpr char TAG[] = "SoyMilk::PlayerImpl";
  using self_type = PlayerImpl;
  using player_type = std::shared_ptr<PlayerImpl>;
  using player_weak = std::weak_ptr<PlayerImpl>;
  using State = Player::State;
  using duration_type = TeaPot::TimerUnit::duration_type;
  using time_point_type = TeaPot::TimerUnit::time_point_type;
  using clock_type = TeaPot::TimerUnit::clock_type;
  using command_type = std::function<void(PlayerImpl &)>;
 public:
  explicit PlayerImpl(RendererWrapper &&renderer, TeaPot::Executor::executor_type executor, TeaPot::TimerWorkerWeakWrapper &&timer)
  : state_(State::INIT),
    renderer_(std::forward<RendererWrapper>(renderer)),
    executor_(executor),
    timer_(std::forward<TeaPot::TimerWorkerWeakWrapper>(timer)),
    queue_(),
    cursor_(queue_.Cursor()),
    fbo_(nullptr),
    future_(nullptr),
    tag_(),
    position_(duration_type(-1)),
    idle_() {}
  ~PlayerImpl() = default;
  State state() const {
    return state_;
  }
  TeaPot::TimerFutureWrapper Prepare(MilkPowder::MidiMutableWrapper midi) {
    ChangeState(State::INIT, State::PREPARING);
    return Post([midi_ = std::make_shared<decltype(midi)>(std::move(midi))](self_type &self) {
      auto length = self.queue_.Fill(midi_->get());
      self.cursor_ = self.queue_.Cursor();
      self.Execute([](self_type &self) {
        self.ChangeState(State::PREPARING, State::PREPARED);
      });
      self.Renderer().OnPrepare(length);
    });
  }
  void Start() {
    ChangeState(State::PREPARED, State::STARTED);
    Post([](self_type &self) {
      self.tag(TeaPot::TimerUnit::Now());
      self.Execute([](self_type &self) {
        self.ChangeState(State::STARTED, State::PLAYING);
      });
      self.Renderer().OnStart();
      self.PostLoop();
    });
  }
  void Pause() {
    ChangeState(State::PLAYING, State::PAUSED);
    Post([](self_type &self) {
      auto &future = self.future_;
      if (future) {
        future->Cancel();
        future.release();
      }
      self.Post([](self_type &self) {
        self.idle_now();
        self.Execute([](self_type &self) {
          self.ChangeState(State::PAUSED, State::SUSPEND);
        });
        auto duration = self.position();
        self.Renderer().OnPause(duration);
      });
    });
  }
  TeaPot::TimerFutureWrapper Seek(duration_type time) {
    ChangeState(State::SUSPEND, State::SEEKING);
    return Post([time](self_type &self) {
      self.Renderer().OnSeekBegin();
      auto position = self.position();
      self.PerformSeek(time);
      auto duration = self.position();
      self.tag((self.tag() + position) - duration);
      self.Execute([](self_type &self) {
        self.ChangeState(State::SEEKING, State::SUSPEND);
      });
      self.Renderer().OnSeekEnd(duration);
    });
  }
  void Resume() {
    ChangeState(State::SUSPEND, State::STARTED);
    Post([](self_type &self) {
      self.tag(self.tag() + self.idle_diff());
      self.Execute([](self_type &self) {
        self.ChangeState(State::STARTED, State::PLAYING);
      });
      self.Renderer().OnResume();
      self.PostLoop();
    });
  }
  void Stop() {
    ChangeState(State::SUSPEND, State::PREPARED);
    Post([](self_type &self) {
      self.cursor_ = self.queue_.Cursor();
      self.position(duration_type(-1));
      self.Renderer().OnStop();
    });
  }
  void Reset() {
    ChangeState(State::PREPARED, State::INIT);
    Post([](self_type &self) {
      self.queue_.Sweep();
      self.cursor_ = self.queue_.Cursor();
      self.Renderer().OnReset();
    });
  }
 private:
  void PerformComplete() {
    auto state_ = state();
    if (state_ != State::PLAYING) {
      MilkTea_logI("complete try but fail -- state: %s" , StateName(state_));
      return;
    }
    ChangeState(State::PLAYING, State::PREPARED);
    Post([](self_type &self) {
      self.Renderer().OnComplete();
    });
  }
  void PerformLoop() {
    future_.release();
    MilkTea_loop {
      auto *fbo = TakeFrame();
      if (fbo == nullptr) {
        Execute([](self_type &self) {
          self.PerformComplete();
        });
        return;
      }
      auto delay = tag() + fbo->time() - TeaPot::TimerUnit::Now();
      if (delay > duration_type::zero()) {
        PostLoop(delay);
        return;
      }
      PerformFrame(*fbo);
      fbo_ = nullptr;
    }
  }
  void PostLoop(duration_type delay = duration_type::zero()) {
    auto future = Post([](self_type &self) {
      self.PerformLoop();
    }, delay);
    future_ = std::make_unique<decltype(future)>(std::move(future));
  }
  void PerformSeek(duration_type time) {
    if (time < duration_type(-1)) {
      time = duration_type(-1);
    }
    if (time < position()) {
      position(duration_type(-1));
      cursor_ = queue_.Cursor();
      fbo_ = nullptr;
    }
    MilkTea_loop {
      auto *fbo = TakeFrame();
      if (fbo == nullptr) {
        return;
      }
      if (fbo->time() >= time) {
        return;
      }
      PerformFrame(*fbo);
      fbo_ = nullptr;
    }
  }
  void PerformFrame(const Codec::FrameBufferImpl &fbo) {
    position(fbo.time());
    Renderer().OnFrame(fbo.operator Codec::FrameBufferWrapper());
  }
  const Codec::FrameBufferImpl *TakeFrame() {
    const Codec::FrameBufferImpl *fbo = fbo_;
    if (fbo == nullptr) {
      fbo = cursor_.next();
      fbo_ = fbo;
    }
    return fbo;
  }
  TeaPot::TimerFutureWrapper Post(command_type action, duration_type delay = duration_type::zero()) {
    auto timer = timer_.lock();
    if (!timer) {
      MilkTea_throw(LogicError, "execute but timer is null");
    }
    return timer.Post(delay, Command(action));
  }
  void Execute(command_type action) {
    executor_(Command(action));
  }
  TeaPot::Action::action_type Command(command_type action) {
    return [weak = weak_from_this(), action]() {
      auto self = weak.lock();
      if (!self) {
        MilkTea_logW("try lock weak but nullptr");
        return []() {};
      }
      action(*self);
    };
  }
  void ChangeState(State expect, State target) {
    if (state_.compare_exchange_strong(expect, target)) {
      return;
    }
    MilkTea_throwf(LogicError, "ChangeState fail -- expect: %s, target: %s, current: %s",
      StateName(expect),
      StateName(target),
      StateName(state())
    );
  }
  RendererWrapper &Renderer() {
    return renderer_;
  }
  duration_type position() const {
    return position_;
  }
  void position(duration_type duration) {
    position_ = duration;
  }
  duration_type idle_diff() const {
    return TeaPot::TimerUnit::Now() - idle_;
  }
  void idle_now() {
    idle_ = TeaPot::TimerUnit::Now();
  }
  time_point_type tag() const {
    return tag_;
  }
  void tag(time_point_type time) {
    tag_ = time;
  }
  static const char *StateName(State state) {
    switch (state) {
      case State::INIT: return "INIT";
      case State::PREPARING: return "PREPARING";
      case State::PREPARED: return "PREPARED";
      case State::STARTED: return "STARTED";
      case State::PLAYING: return "PLAYING";
      case State::PAUSED: return "PAUSED";
      case State::SUSPEND: return "SUSPEND";
      case State::SEEKING: return "SEEKING";
      default: MilkTea_assert("StateName assert");
    }
  }
  std::atomic<State> state_;
  RendererWrapper renderer_;
  TeaPot::Executor::executor_type executor_;
  TeaPot::TimerWorkerWeakWrapper timer_;
  Codec::FrameBufferQueueImpl queue_;
  Codec::FrameBufferCursorImpl cursor_;
  const Codec::FrameBufferImpl *fbo_;
  std::unique_ptr<TeaPot::TimerFutureWrapper> future_;
  time_point_type tag_;
  duration_type position_;
  time_point_type idle_;
  MilkTea_NonCopy(PlayerImpl)
  MilkTea_NonMove(PlayerImpl)
};

} // namespace SoyBean_Windows

#endif // ifndef SOYMILK_PLAYER_H_
