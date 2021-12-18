#ifndef LIB_TEAPOT_COMMON_ACTION_H_
#define LIB_TEAPOT_COMMON_ACTION_H_

#include <teapot/core.h>

namespace TeaPot {

struct Action {
  using raw_type = TeaPot_Action_t;
  using action_type = std::function<void()>;
  static action_type FromRawType(raw_type action) {
    MilkTea_nonnull(action.run_);
    return MilkTea::ClosureToken<void()>::FromRawType(action.self_, action.run_);
  }
  static raw_type ToRawType(action_type action) {
    return {
      .self_ = MilkTea::ClosureToken<decltype(action)>::ToRawType(action),
      .run_ = MilkTea::ClosureToken<decltype(action)>::Invoke,
    };
  }
};

struct Executor {
  using raw_type = TeaPot_Executor_t;
};

} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_ACTION_H_
