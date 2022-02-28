#ifndef SOYMILK_PERFORM_H_
#define SOYMILK_PERFORM_H_

#include <soymilk.h>
#include <tea.h>

#include "frame.h"
#include "buffer.h"

namespace SoyMilk {

namespace internal {

class PerformImpl final : public std::enable_shared_from_this<PerformImpl>, tea::remove_move {
  static constexpr char TAG[] = "SoyMilk::PerformImpl";
  using command_type = std::function<void(PerformImpl &)>;
  using frame_type = std::function<void(const FrameBufferImpl &)>;
 public:
  void scheduled(scheduled_type f) {
    scheduled_ = std::move(f);
  }
  void complete(action_type f) {
    complete_ = std::move(f);
  }
  void frame(frame_type f) {
    frame_ = std::move(f);
  }
  PerformImpl()
  : queue_(),
    cursor_(queue_.Cursor()),
    action_(nullptr),
    tag_(),
    idle_(),
    position_(duration_type(-1)) {}
  ~PerformImpl() = default;
  duration_type Prepare(std::vector<const MilkPowder::Midi *> vec) {
    return queue_fill(std::move(vec));
  }
  void Start() {
    auto time = Now();
    tag(time);
    idle(time);
    position_reset();
    cursor_reset();
  }
  void Resume() {
    tag_delay(idle_diff(now()));
  }
  std::function<duration_type(PerformImpl &)> Pause() {
    future_cancel();
    return [](PerformImpl &obj) -> duration_type {
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
  duration_type Seek(duration_type time) {
    if (time < duration_type(-1)) {
      time = duration_type(-1);
    }
    if (time < position()) {
      position_reset();
      cursor_reset();
    }
    while (true) {
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
    if (!action_) {
      Post();
      return;
    }
    action_.reset();
    while (true) {
      auto *fbo = *cursor();
      if (fbo == nullptr) {
        Complete();
        return;
      }
      auto delay = tag() + fbo->time() - Now();
      if (delay > duration_type::zero()) {
        Post(delay);
        return;
      }
      Frame(*fbo);
      ++cursor();
    }
  }
 private:
  void Post(duration_type delay = duration_type::zero()) {
    action_ = std::make_shared<action_type>([](PerformImpl &obj) {
      obj.Loop();
    });
    scheduled_([weak = std::weak_ptr(action_)]() {
      auto action = weak.lock();
      if (action) {
        (*action)();
      }
    }, delay);
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
  void tag_delay(duration_type time) {
    tag_ += time;
  }
  duration_type idle_diff(time_point_type time) const {
    return time - idle();
  }
  void idle(time_point_type time) {
    idle_ = time;
  }
  time_point_type idle() const {
    return idle_;
  }
  void position(duration_type time) {
    position_ = time;
  }
  duration_type position() const {
    return position_;
  }
  void position_reset() {
    position(duration_type(-1));
  }
  void cursor_reset() {
    cursor_ = queue_.Cursor();
  }
  FrameBufferCursorImpl &cursor() {
    return cursor_;
  }
  void future_cancel() {
    if (!action_) {
      return;
    }
    action_ = nullptr;
  }
  duration_type queue_fill(std::vector<const MilkPowder::Midi *> vec) {
    auto time = queue_.Fill(std::move(vec));
    cursor_reset();
    return time;
  }
  void queue_sweep() {
    queue_.Sweep();
    cursor_reset();
  }
  static time_point_type now() {
    return Now();
  }
  FrameBufferQueueImpl queue_;
  FrameBufferCursorImpl cursor_;
  std::shared_ptr<action_type> action_;
  time_point_type tag_;
  time_point_type idle_;
  duration_type position_;
  scheduled_type scheduled_;
  action_type complete_;
  frame_type frame_;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_PERFORM_H_
