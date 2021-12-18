#ifndef SOYMILK_PLAYER_H_
#define SOYMILK_PLAYER_H_

#include <atomic>
#include <memory>

#include <soymilk/common.h>

#include "decode.h"

namespace SoyMilk {

class PlayerImpl final : public std::enable_shared_from_this<PlayerImpl> {
  static constexpr char TAG[] = "SoyMilk::PlayerImpl";
  using player_type = std::shared_ptr<PlayerImpl>;
  using player_weak = std::weak_ptr<PlayerImpl>;
  using State = Player::State;
  using duration_type = TeaPot::TimerUnit::duration_type;
  using time_point_type = TeaPot::TimerUnit::time_point_type;
 public:
  explicit PlayerImpl(RendererWrapper &&renderer, TeaPot::Executor::executor_type executor, TeaPot::TimerWorkerWeakWrapper &&timer)
  : state_(State::INIT),
    renderer_(std::forward<RendererWrapper>(renderer)),
    executor_(executor),
    timer_(std::forward<TeaPot::TimerWorkerWeakWrapper>(timer)) {}
  ~PlayerImpl() = default;
  State state() const {
    return state_;
  }
  TeaPot::TimerFutureWrapper Prepare(MilkPowder::MidiMutableWrapper midi) {
    ChangeState(State::INIT, State::PREPARING);
    return Post([weak = Weak(), midi_ = std::make_shared<decltype(midi)>(std::move(midi))]() {
      auto self = Lock(weak);
      auto time = self->PerformPrepare(midi_->get());
      self->Execute([self]() {
        self->ChangeState(State::PREPARING, State::PREPARED);
      });
      self->Renderer().OnPrepare(time);
    });
  }
  void Start() {
    ChangeState(State::PREPARED, State::STARTED);
    PerformStart([weak = Weak()]() {
      auto self = Lock(weak);
      self->Execute([self]() {
        self->ChangeState(State::STARTED, State::PLAYING);
      });
      self->Renderer().OnStart();
    });
  }
  void Pause() {
    ChangeState(State::PLAYING, State::PAUSED);
    PerformPause([weak = Weak()](duration_type time) {
      auto self = Lock(weak);
      self->Execute([self]() {
        self->ChangeState(State::PAUSED, State::SUSPEND);
      });
      self->Renderer().OnPause(time);
    });
  }
  TeaPot::TimerFutureWrapper Seek(duration_type time) {
    ChangeState(State::SUSPEND, State::SEEKING);
    return Post([weak = Weak(), time]() {
      auto self = Lock(weak);
      self->Renderer().OnSeekBegin();
      self->PerformSeek(time);
      self->Execute([self]() {
        self->ChangeState(State::SEEKING, State::SUSPEND);
      });
      self->Renderer().OnSeekEnd();
    });
  }
  void Resume() {
    ChangeState(State::SUSPEND, State::STARTED);
    PerformResume([weak = Weak()]() {
      auto self = Lock(weak);
      self->Execute([self]() {
        self->ChangeState(State::STARTED, State::PLAYING);
      });
      self->Renderer().OnResume();
    });
  }
  void Stop() {
    ChangeState(State::SUSPEND, State::PREPARED);
    PerformStop([weak = Weak()]() {
      auto self = Lock(weak);
      self->Renderer().OnStop();
    });
  }
  void Reset() {
    ChangeState(State::PREPARED, State::INIT);
    PerformReset([weak = Weak()]() {
      auto self = Lock(weak);
      self->Renderer().OnReset();
    });
  }
 private:
  void CompleteInternal() {
    const time_point_type tag = time_tag_;
    Execute([weak = Weak(), tag]() {
      auto self = Lock(weak);
      const State state_ = self->state();
      const time_point_type tag_ = self->time_tag_;
      if (state_ != State::PLAYING || tag_ != tag) {
        MilkTea_logI("complete try but fail -- state: %s, time: %, tag: " PRIi64, StateName(state_), tag_.time_since_epoch().count(), tag.time_since_epoch().count());
        return;
      }
      self->ChangeState(State::PLAYING, State::PREPARED);
      self->Post([self]() {
        self->Renderer().OnComplete();
      });
    });
  }
  duration_type PerformPrepare(MilkPowder::MidiConstWrapper midi) {
    // todo
    return duration_type::zero();
  }
  void PerformStart(std::function<void()> action) {
    // todo
  }
  void PerformPause(std::function<void(duration_type)> action) {
    // todo
  }
  void PerformSeek(duration_type time) {
    // todo
  }
  void PerformResume(std::function<void()> action) {
    // todo
  }
  void PerformStop(std::function<void()> action) {
    // todo
  }
  void PerformReset(std::function<void()> action) {
    // todo
  }
  TeaPot::TimerFutureWrapper Post(TeaPot::Action::action_type action, duration_type delay = duration_type::zero()) {
    auto timer = timer_.lock();
    if (!timer) {
      MilkTea_throw(LogicError, "execute but timer is null");
    }
    return timer.Post(delay, action);
  }
  void Execute(TeaPot::Action::action_type action) {
    executor_(action);
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
  player_weak Weak() {
    return weak_from_this();
  }
  static player_type Lock(player_weak weak) {
    auto self = weak.lock();
    if (!self) {
      MilkTea_throw(LogicError, "lock but self is nullptr");
    }
    return self;
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
  std::atomic<time_point_type> time_tag_;
  MilkTea_NonCopy(PlayerImpl)
  MilkTea_NonMove(PlayerImpl)
};

} // namespace SoyBean_Windows

#endif // ifndef SOYMILK_PLAYER_H_
