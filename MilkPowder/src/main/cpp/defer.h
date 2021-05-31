#ifndef MILKPOWDER_DEFER_H_
#define MILKPOWDER_DEFER_H_

#include <functional>

namespace MilkPowder {

class Defer final {
 public:
  ~Defer() {
    defer_();
  }
  Defer(std::function<void()> defer) : defer_(defer) {
  }
 private:
  const std::function<void()> defer_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_DEFER_H_
