#ifndef SOYMILK_LISTENER_H_
#define SOYMILK_LISTENER_H_

#include <cstring>

#include <tea.h>

#include <soymilk.h>

namespace SoyMilk {

namespace internal {

class ListenerImpl final : tea::remove_copy {
 public:
  ListenerImpl(SoyMilk_Player_OnStateChange_recv_t obj) : obj_(obj) {}
  ListenerImpl(ListenerImpl &&another) : ListenerImpl(another.obj_) {
    std::memset(&another.obj_, 0, sizeof(SoyMilk_Player_OnStateChange_recv_t));
  }
  ~ListenerImpl() {
    if (obj_.close == nullptr) {
      return;
    }
    obj_.close(obj_.ctx);
  }
  auto OnPrepare(duration_type time) -> void {
    obj_.OnPrepare(obj_.ctx, time.count());
  }
  auto OnStart() -> void {
    obj_.OnStart(obj_.ctx);
  }
  auto OnResume() -> void {
    obj_.OnResume(obj_.ctx);
  }
  auto OnPause(duration_type time) -> void {
    obj_.OnPause(obj_.ctx, time.count());
  }
  auto OnStop() -> void {
    obj_.OnStop(obj_.ctx);
  }
  auto OnReset() -> void {
    obj_.OnReset(obj_.ctx);
  }
  auto OnComplete() -> void {
    obj_.OnComplete(obj_.ctx);
  }
  auto OnSeekBegin() -> void {
    obj_.OnSeekBegin(obj_.ctx);
  }
  auto OnSeekEnd(duration_type time) -> void {
    obj_.OnSeekEnd(obj_.ctx, time.count());
  }
 private:
  SoyMilk_Player_OnStateChange_recv_t obj_;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_LISTENER_H_
