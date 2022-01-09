#ifndef SOYMILK_PLAYER_H_
#define SOYMILK_PLAYER_H_

#include <atomic>
#include <memory>

#include <soymilk/common.h>

#include "perform.h"

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
  using executor_type = TeaPot::Executor::executor_type;
  using renderer_type = RendererWrapper;
  using timer_weak = TeaPot::TimerWorkerWeakWrapper;
  using future_type = TeaPot::TimerFutureWrapper;
  using command_type = std::function<void(PlayerImpl &)>;
  using action_type = TeaPot::Action::action_type;
 public:
  explicit PlayerImpl(RendererWrapper &&renderer, TeaPot::Executor::executor_type executor, TeaPot::TimerWorkerWeakWrapper &&timer)
  : state_(State::INIT),
    renderer_(std::forward<RendererWrapper>(renderer)),
    executor_(executor),
    timer_(std::forward<TeaPot::TimerWorkerWeakWrapper>(timer)),
    perform_() {}
  ~PlayerImpl() = default;
  void Init() {
    Perform().post([weak = weak_from_this()](auto action, auto delay) -> auto {
      auto self = Lock(weak);
      if (!self) {
        return future_type();
      }
      return self->Post([action](auto &self) {
        action(self.Perform());
      }, delay);
    });
    Perform().complete(Command([](auto &self) {
      self.Execute([](auto &self) {
        auto state = self.state();
        if (state != State::PLAYING) {
          MilkTea_logI("complete try but fail -- state: %s" , StateName(state));
          return;
        }
        self.ChangeState(State::PLAYING, State::PREPARED);
        self.Post([](self_type &self) {
          self.Renderer().OnComplete();
        });
      });
    }));
    Perform().frame([weak = weak_from_this()](auto fbo) {
      auto self = Lock(weak);
      if (!self) {
        return;
      }
      self->Renderer().OnFrame(std::move(fbo));
    });
  }
  State state() const {
    return state_;
  }
  TeaPot::TimerFutureWrapper Prepare(MilkPowder::MidiConstWrapper midi) {
    ChangeState(State::INIT, State::PREPARING);
    return Post([midi_ = std::make_shared<MilkPowder::MidiMutableWrapper>(midi)](auto &self) {
      auto length = self.Perform().Prepare(midi_->get());
      self.ChangeStateAsync(State::PREPARING, State::PREPARED);
      self.Renderer().OnPrepare(length);
    });
  }
  void Start() {
    ChangeState(State::PREPARED, State::STARTED);
    Post([](auto &self) {
      self.Perform().Start();
      self.ChangeStateAsync(State::STARTED, State::SUSPEND);
      self.Renderer().OnStart();
    });
  }
  void Resume() {
    ChangeState(State::SUSPEND, State::RESUMED);
    Post([](auto &self) {
      self.Perform().Resume();
      self.ChangeStateAsync(State::RESUMED, State::PLAYING);
      self.Renderer().OnResume();
      self.Perform().Loop();
    });
  }
  void Pause() {
    ChangeState(State::PLAYING, State::PAUSED);
    Post([](auto &self) {
      self.Post([command = self.Perform().Pause()](auto &self) {
        auto pos = command(self.Perform());
        self.ChangeStateAsync(State::PAUSED, State::PAUSED);
        self.Renderer().OnPause(pos);
      });
    });
  }
  TeaPot::TimerFutureWrapper Seek(duration_type time) {
    ChangeState(State::SUSPEND, State::SEEKING);
    return Post([time](auto &self) {
      self.Renderer().OnSeekBegin();
      auto pos = self.Perform().Seek(time);
      self.ChangeStateAsync(State::SEEKING, State::SUSPEND);
      self.Renderer().OnSeekEnd(pos);
    });
  }
  void Stop() {
    ChangeState(State::SUSPEND, State::STOPPED);
    Post([](auto &self) {
      self.Perform().Stop();
      self.ChangeStateAsync(State::STOPPED, State::PREPARED);
      self.Renderer().OnStop();
    });
  }
  void Reset() {
    ChangeState(State::PREPARED, State::RESET);
    Post([](auto &self) {
      self.Perform().Reset();
      self.ChangeStateAsync(State::RESET, State::INIT);
      self.Renderer().OnReset();
    });
  }
 private:
  TeaPot::TimerFutureWrapper Post(command_type action, duration_type delay = duration_type::zero()) {
    auto timer = timer_.lock();
    if (!timer) {
      MilkTea_throw(LogicError, "execute but timer is null");
    }
    return timer.Post(Command(action), delay);
  }
  void Execute(command_type action) {
    executor_(Command(action));
  }
  action_type Command(command_type action) {
    return [weak = weak_from_this(), action]() {
      auto self = Lock(weak);
      if (!self) {
        return;
      }
      action(*self);
    };
  }
  void ChangeState(const State expect, const State target) {
    State current = expect;
    if (state_.compare_exchange_strong(current, target)) {
      MilkTea_logI("ChangeState from %s to %s", StateName(expect), StateName(target));
      return;
    }
    MilkTea_throwf(LogicError, "ChangeState fail -- expect: %s, target: %s, current: %s",
      StateName(expect),
      StateName(target),
      StateName(current)
    );
  }
  renderer_type &Renderer() {
    return renderer_;
  }
  PerformImpl &Perform() {
    return perform_;
  }
  void ChangeStateAsync(State expect, State target) {
    Execute([expect, target](self_type &self) {
      self.ChangeState(expect, target);
    });
  }
  static player_type Lock(player_weak weak) {
    auto self = weak.lock();
    if (!self) {
      MilkTea_logW("try lock weak but nullptr");
    }
    return self;
  }
  static const char *StateName(State state) {
    return Player::StateName(state).data();
  }
  std::atomic<State> state_;
  renderer_type renderer_;
  executor_type executor_;
  timer_weak timer_;
  PerformImpl perform_;
  MilkTea_NonCopy(PlayerImpl)
  MilkTea_NonMove(PlayerImpl)
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_PLAYER_H_
