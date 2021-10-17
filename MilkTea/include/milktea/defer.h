#ifndef LIB_MILKTEA_DEFER_H_
#define LIB_MILKTEA_DEFER_H_

#include <milktea/util.h>
#include <functional>

#ifdef __cplusplus
namespace MilkTea {

class Defer final {
 public:
  ~Defer() {
    defer_();
  }
  explicit Defer(std::function<void()> defer) : defer_(defer) {
  }
 private:
  const std::function<void()> defer_;
  MilkTea_NonCopy(Defer)
  MilkTea_NonMove(Defer)
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_DEFER_H_
