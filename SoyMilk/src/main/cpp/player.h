#ifndef SOYMILK_PLAYER_H_
#define SOYMILK_PLAYER_H_

#include <atomic>
#include <memory>

#include <tea.h>
#include <milkpowder.h>

#include <soymilk.h>

#include "perform.h"
#include "listener.h"

namespace SoyMilk {

namespace internal {

class PlayerImpl final : public std::enable_shared_from_this<PlayerImpl>, tea::remove_move, tea::with_log<PlayerImpl> {
  using self_type = PlayerImpl;
  using player_type = std::shared_ptr<PlayerImpl>;
  using player_weak = std::weak_ptr<PlayerImpl>;
  using State = Player::State;
  using command_type = std::function<void(PlayerImpl &)>;
  using renderer_type = std::function<void(const FrameBufferImpl &)>;
 public:
  static constexpr char TAG[] = "Player";
  PlayerImpl(
    ListenerImpl listener,
    renderer_type renderer,
    executor_type executor,
    scheduled_type scheduled)
  : state_(State::INIT),
    perform_(),
    listener_(std::move(listener)),
    renderer_(std::move(renderer)),
    executor_(std::move(executor)),
    scheduled_(std::move(scheduled)) {
    Perform().scheduled([weak = weak_from_this()](auto action, auto delay) {
      auto obj = Lock(weak);
      if (!obj) {
        return;
      }
      obj->scheduled_(action, delay);
    });
    Perform().complete(Command([](auto &obj) {
      obj.Execute([](auto &obj) {
        auto state = obj.state();
        if (state != State::PLAYING) {
          log<I>("complete try but fail -- state: %s" , state);
          return;
        }
        obj.ChangeState(State::PLAYING, State::PREPARED);
        obj.Execute([](self_type &obj) {
          obj.Listener().OnComplete();
        });
      });
    }));
    Perform().frame([weak = weak_from_this()](auto &fbo) {
      auto obj = Lock(weak);
      if (!obj) {
        return;
      }
      obj->Renderer()(fbo);
    });
    }
  ~PlayerImpl() = default;
  State_t state() const {
    return state_;
  }
  void Prepare(std::vector<tea::owner_ptr<MilkPowder::Midi>> vec) {
    ChangeState(State::INIT, State::PREPARING);
    return Scheduled([vec_ = std::move(vec)](auto &obj) {
      std::vector<const MilkPowder::Midi *> v;
      std::for_each(vec_.begin(), vec_.end(), [&v](auto &it) {
        v.push_back(it.get());
      });
      auto length = obj.Perform().Prepare(std::move(v));
      obj.ChangeStateAsync(State::PREPARING, State::PREPARED);
      obj.Listener().OnPrepare(length);
    });
  }
  void Start() {
    ChangeState(State::PREPARED, State::STARTED);
    Scheduled([](auto &obj) {
      obj.Perform().Start();
      obj.ChangeStateAsync(State::STARTED, State::SUSPEND);
      obj.Listener().OnStart();
    });
  }
  void Resume() {
    ChangeState(State::SUSPEND, State::RESUMED);
    Scheduled([](auto &obj) {
      obj.Perform().Resume();
      obj.ChangeStateAsync(State::RESUMED, State::PLAYING);
      obj.Listener().OnResume();
      obj.Perform().Loop();
    });
  }
  void Pause() {
    ChangeState(State::PLAYING, State::PAUSED);
    Scheduled([](auto &obj) {
      obj.Scheduled([command = obj.Perform().Pause()](auto &obj) {
        auto pos = command(obj.Perform());
        obj.ChangeStateAsync(State::PAUSED, State::SUSPEND);
        obj.Listener().OnPause(pos);
      });
    });
  }
  void Seek(duration_type time) {
    ChangeState(State::SUSPEND, State::SEEKING);
    return Scheduled([time](auto &obj) {
      obj.Listener().OnSeekBegin();
      auto pos = obj.Perform().Seek(time);
      obj.ChangeStateAsync(State::SEEKING, State::SUSPEND);
      obj.Listener().OnSeekEnd(pos);
    });
  }
  void Stop() {
    ChangeState(State::SUSPEND, State::STOPPED);
    Scheduled([](auto &obj) {
      obj.Perform().Stop();
      obj.ChangeStateAsync(State::STOPPED, State::PREPARED);
      obj.Listener().OnStop();
    });
  }
  void Reset() {
    ChangeState(State::PREPARED, State::RESET);
    Scheduled([](auto &obj) {
      obj.Perform().Reset();
      obj.ChangeStateAsync(State::RESET, State::INIT);
      obj.Listener().OnReset();
    });
  }
 private:
  void Scheduled(command_type action, duration_type delay = duration_type::zero()) {
    return scheduled_(Command(action), delay);
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
  void ChangeState(const State_t expect, const State_t target) {
    State_t current = expect;
    if (state_.compare_exchange_strong(current, target)) {
      log<I>("ChangeState from %d to %d", expect, target);
      return;
    }
    tea::err::raise<tea::err_enum::invalid_state>(MilkTea::ToString::FromFormat(
    "ChangeState fail -- expect: %s, target: %s, current: %s", expect, target, current));
  }
  renderer_type &Renderer() {
    return renderer_;
  }
  ListenerImpl &Listener() {
    return listener_;
  }
  PerformImpl &Perform() {
    return perform_;
  }
  void ChangeStateAsync(State_t expect, State_t target) {
    Execute([expect, target](self_type &obj) {
      obj.ChangeState(expect, target);
    });
  }
  static player_type Lock(player_weak weak) {
    auto obj = weak.lock();
    if (!obj) {
      log<W>("try lock weak but nullptr");
    }
    return obj;
  }
  std::atomic<State_t> state_;
  PerformImpl perform_;
  ListenerImpl listener_;
  renderer_type renderer_;
  executor_type executor_;
  scheduled_type scheduled_;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_PLAYER_H_
