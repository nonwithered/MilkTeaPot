#ifndef LIB_TEAPOT_COMMON_ACTION_H_
#define LIB_TEAPOT_COMMON_ACTION_H_

#include <teapot/core.h>

namespace TeaPot {

struct Action {
  using raw_type = TeaPot_Action_t;
  using action_type = std::function<void()>;
  static action_type FromRawType(raw_type action) {
    MilkTea_nonnull(action.run_);
    return MilkTea::ClosureToken<action_type>::FromRawType(action.self_, action.run_);
  }
  static raw_type ToRawType(action_type action) {
    return {
      .self_ = MilkTea::ClosureToken<action_type>::ToRawType(action),
      .run_ = MilkTea::ClosureToken<action_type>::Invoke,
    };
  }
};

struct Executor {
  using raw_type = TeaPot_Executor_t;
  using wrapper_type = std::function<void(Action::raw_type)>;
  using executor_type = std::function<void(Action::action_type)>;
  static executor_type FromRawType(raw_type executor) {
    static auto wrap_ = [](raw_type executor_) -> wrapper_type {
      return MilkTea::ClosureToken<wrapper_type>::FromRawType(executor_.self_, executor_.submit_);
    };
    MilkTea_nonnull(executor.submit_);
    return [executor_ = wrap_(executor)](Action::action_type action) {
      executor_(Action::ToRawType(action));
    };
  }
  static raw_type ToRawType(executor_type executor) {
    static auto unwrap_ = [](executor_type executor_) -> wrapper_type {
      return [executor_](Action::raw_type action) {
        executor_(Action::FromRawType(action));
      };
    };
    return {
      .self_ = MilkTea::ClosureToken<wrapper_type>::ToRawType(unwrap_(executor)),
      .submit_ = MilkTea::ClosureToken<wrapper_type>::Invoke,
    };
  }
};

} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_ACTION_H_
