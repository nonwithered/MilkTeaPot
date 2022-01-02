#ifndef LIB_TEAPOT_COMMON_ACTION_H_
#define LIB_TEAPOT_COMMON_ACTION_H_

#include <teapot/core.h>

namespace TeaPot {

struct Action {
  using raw_type = TeaPot_Action_t;
  using action_type = std::function<void()>;
  static action_type FromRawType(raw_type action) {
    return action;
  }
  static raw_type ToRawType(action_type action) {
    return action;
  }
};

struct Executor {
  using raw_type = TeaPot_Executor_t;
  using wrapper_type = std::function<void(Action::raw_type)>;
  using executor_type = std::function<void(Action::action_type)>;
  static executor_type FromRawType(raw_type executor) {
    return [executor_ = executor.release()](Action::action_type action) {
      executor_(Action::ToRawType(action));
    };
  }
  static raw_type ToRawType(executor_type executor) {
    return raw_type([executor](Action::raw_type action) {
      executor(Action::FromRawType(action));
    });
  }
};

} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_ACTION_H_
