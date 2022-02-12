#ifndef SOYMILK_PERFORM_H_
#define SOYMILK_PERFORM_H_

#include <soymilk/common.h>

#include "frame.h"
#include "buffer.h"

namespace SoyMilk {

class PerformImpl final : public std::enable_shared_from_this<PerformImpl> {
  static constexpr char TAG[] = "SoyMilk::PerformImpl";
  using time_point_type = TeaPot::TimerUnit::time_point_type;
  using future_type = TeaPot::TimerFutureWrapper;
  using action_type = TeaPot::Action::action_type;
  using command_type = std::function<void(PerformImpl &)>;
  using post_type = std::function<future_type(command_type, tempo_type)>;
  using frame_type = std::function<void(const FrameBufferImpl &)>;
 public:
  void post(post_type f) {
    post_ = f;
  }
  void complete(action_type f) {
    complete_ = f;
  }
  void frame(frame_type f) {
    frame_ = f;
  }
  PerformImpl()
  : queue_(),
    cursor_(queue_.Cursor()),
    future_(nullptr),
    tag_(),
    idle_(),
    position_(tempo_type(-1)) {}
  ~PerformImpl() = default;
  tempo_type Prepare(std::vector<MilkPowder::MidiConstWrapper> vec) {
    return queue_fill(std::move(vec));
  }
  void Start() {
    auto time = TeaPot::TimerUnit::Now();
    tag(time);
    idle(time);
    position_reset();
    cursor_reset();
  }
  void Resume() {
    tag_delay(idle_diff(now()));
  }
  std::function<tempo_type(PerformImpl &)> Pause() {
    future_cancel();
    return [](PerformImpl &obj) -> tempo_type {
      auto time = obj.now();
      obj.idle(time);
      return time - obj.tag();
    };
  }
  void Stop() {
  }
  void Reset() {
    queue_sweep();
  }
  tempo_type Seek(tempo_type time) {
    if (time < tempo_type(-1)) {
      time = tempo_type(-1);
    }
    if (time < position()) {
      position_reset();
      cursor_reset();
    }
    MilkTea_loop {
      auto *fbo = *cursor();
      if (fbo == nullptr || fbo->time() >= time) {
        break;
      }
      Frame(*fbo);
      ++cursor();
    }
    auto duration = position();
    tag(idle() - duration);
    return duration;
  }
  void Loop() {
    if (!future()) {
      Post();
      return;
    }
    future().release();
    MilkTea_loop {
      auto *fbo = *cursor();
      if (fbo == nullptr) {
        Complete();
        return;
      }
      auto delay = tag() + fbo->time() - TeaPot::TimerUnit::Now();
      if (delay > tempo_type::zero()) {
        Post(delay);
        return;
      }
      Frame(*fbo);
      ++cursor();
    }
  }
 private:
  void Post(tempo_type delay = tempo_type::zero()) {
    auto future = post_([](auto &obj) {
      obj.Loop();
    }, delay);
    if (future.get() != nullptr) {
      future_ = std::make_unique<future_type>(std::move(future));
    }
  }
  void Frame(const FrameBufferImpl &fbo) {
    position(fbo.time());
    frame_(fbo);
  }
  void Complete() {
    complete_();
  }
  void tag(time_point_type time) {
    tag_ = time;
  }
  time_point_type tag() const {
    return tag_;
  }
  void tag_delay(tempo_type time) {
    tag_ += time;
  }
  tempo_type idle_diff(time_point_type time) const {
    return time - idle();
  }
  void idle(time_point_type time) {
    idle_ = time;
  }
  time_point_type idle() const {
    return idle_;
  }
  void position(tempo_type time) {
    position_ = time;
  }
  tempo_type position() const {
    return position_;
  }
  void position_reset() {
    position(tempo_type(-1));
  }
  void cursor_reset() {
    cursor_ = queue_.Cursor();
  }
  FrameBufferCursorImpl &cursor() {
    return cursor_;
  }
  void future_cancel() {
    if (!future_) {
      return;
    }
    future_->Cancel();
    future_.release();
  }
  std::unique_ptr<future_type> &future() {
    return future_;
  }
  tempo_type queue_fill(std::vector<MilkPowder::MidiConstWrapper> vec) {
    auto time = queue_.Fill(std::move(vec));
    cursor_reset();
    return time;
  }
  void queue_sweep() {
    queue_.Sweep();
    cursor_reset();
  }
  static time_point_type now() {
    return TeaPot::TimerUnit::Now();
  }
  FrameBufferQueueImpl queue_;
  FrameBufferCursorImpl cursor_;
  std::unique_ptr<future_type> future_;
  time_point_type tag_;
  time_point_type idle_;
  tempo_type position_;
  post_type post_;
  action_type complete_;
  frame_type frame_;
  MilkTea_NonCopy(PerformImpl)
  MilkTea_NonMove(PerformImpl)
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_PERFORM_H_
