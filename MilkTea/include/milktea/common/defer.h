#ifndef LIB_MILKTEA_COMMON_DEFER_H_
#define LIB_MILKTEA_COMMON_DEFER_H_

#include <functional>

#include <milktea/common/macro.h>

#define MilkTea_defer_sym(s, block) \
MilkTea::Defer s([&]() { \
  block \
})

#define MilkTea_defer(block) \
MilkTea_defer_sym(defer_, block)

namespace MilkTea {

class Defer final {
 public:
  ~Defer() {
    defer_();
  }
  Defer(std::function<void()> defer) : defer_(defer) {
  }
 private:
  const std::function<void()> defer_;
  MilkTea_NonCopy(Defer)
  MilkTea_NonMove(Defer)
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_DEFER_H_
