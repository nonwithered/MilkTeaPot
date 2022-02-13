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
    renderer_(std::move(renderer)),
    executor_(executor),
    timer_(std::move(timer)),
    perform_() {}
  ~PlayerImpl() = default;
  void Init() {
    Perform().post([weak = weak_from_this()](auto action, auto delay) -> auto {
      auto obj = Lock(weak);
      if (!obj) {
        return future_type();
      }
      return obj->Post([action](auto &obj) {
        action(obj.Perform());
      }, delay);
    });
    Perform().complete(Command([](auto &obj) {
      obj.Execute([](auto &obj) {
        auto state = obj.state();
        if (state != State::PLAYING) {
          MilkTea_logI("complete try but fail -- state: %s" , StateName(state));
          return;
        }
        obj.ChangeState(State::PLAYING, State::PREPARED);
        obj.Post([](self_type &obj) {
          obj.Renderer().OnComplete();
        });
      });
    }));
    Perform().frame([weak = weak_from_this()](auto &fbo) {
      auto obj = Lock(weak);
      if (!obj) {
        return;
      }
      FrameBufferWrapper fbo_ = reinterpret_cast<FrameBufferWrapper::raw_type *>(const_cast<FrameBufferImpl *>(&fbo));
      MilkTea_defer({
        fbo_.release();
      });
      obj->Renderer().OnFrame(fbo_);
    });
  }
  State state() const {
    return state_;
  }
  TeaPot::TimerFutureWrapper Prepare(std::vector<MilkPowder::MidiMutableWrapper> vec) {
    ChangeState(State::INIT, State::PREPARING);
    return Post([vec_ = std::make_shared<std::vector<MilkPowder::MidiMutableWrapper>>(std::move(vec))](auto &obj) {
      std::vector<MilkPowder::MidiConstWrapper> v;
      std::for_each(vec_->begin(), vec_->end(), [&v](auto &it) {
        v.push_back(it.get());
      });
      auto length = obj.Perform().Prepare(std::move(v));
      obj.ChangeStateAsync(State::PREPARING, State::PREPARED);
      obj.Renderer().OnPrepare(length);
    });
  }
  void Start() {
    ChangeState(State::PREPARED, State::STARTED);
    Post([](auto &obj) {
      obj.Perform().Start();
      obj.ChangeStateAsync(State::STARTED, State::SUSPEND);
      obj.Renderer().OnStart();
    });
  }
  void Resume() {
    ChangeState(State::SUSPEND, State::RESUMED);
    Post([](auto &obj) {
      obj.Perform().Resume();
      obj.ChangeStateAsync(State::RESUMED, State::PLAYING);
      obj.Renderer().OnResume();
      obj.Perform().Loop();
    });
  }
  void Pause() {
    ChangeState(State::PLAYING, State::PAUSED);
    Post([](auto &obj) {
      obj.Post([command = obj.Perform().Pause()](auto &obj) {
        auto pos = command(obj.Perform());
        obj.ChangeStateAsync(State::PAUSED, State::PAUSED);
        obj.Renderer().OnPause(pos);
      });
    });
  }
  TeaPot::TimerFutureWrapper Seek(tempo_type time) {
    ChangeState(State::SUSPEND, State::SEEKING);
    return Post([time](auto &obj) {
      obj.Renderer().OnSeekBegin();
      auto pos = obj.Perform().Seek(time);
      obj.ChangeStateAsync(State::SEEKING, State::SUSPEND);
      obj.Renderer().OnSeekEnd(pos);
    });
  }
  void Stop() {
    ChangeState(State::SUSPEND, State::STOPPED);
    Post([](auto &obj) {
      obj.Perform().Stop();
      obj.ChangeStateAsync(State::STOPPED, State::PREPARED);
      obj.Renderer().OnStop();
    });
  }
  void Reset() {
    ChangeState(State::PREPARED, State::RESET);
    Post([](auto &obj) {
      obj.Perform().Reset();
      obj.ChangeStateAsync(State::RESET, State::INIT);
      obj.Renderer().OnReset();
    });
  }
 private:
  TeaPot::TimerFutureWrapper Post(command_type action, tempo_type delay = tempo_type::zero()) {
    auto timer = timer_.lock();
    if (!timer) {
      MilkTea_throw(LogicError, "execute but timer is null");
    }
    return timer.Post(Command(action), std::chrono::duration_cast<TeaPot::TimerUnit::duration_type>(delay));
  }
  void Execute(command_type action) {
    executor_(Command(action));
  }
  action_type Command(command_type action) {
    return [weak = weak_from_this(), action]() {
      auto obj = Lock(weak);
      if (!obj) {
        return;
      }
      action(*obj);
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
    Execute([expect, target](self_type &obj) {
      obj.ChangeState(expect, target);
    });
  }
  static player_type Lock(player_weak weak) {
    auto obj = weak.lock();
    if (!obj) {
      MilkTea_logW("try lock weak but nullptr");
    }
    return obj;
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
