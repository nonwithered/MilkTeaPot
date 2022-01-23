//#ifndef MILK_LAUNCH_H_
//#define MILK_LAUNCH_H_

//#include <vector>

//#include "context.h"
//#include "control.h"
//#include "codec.h"
//#include "dump.h"
//#include "play.h"

//namespace Milk {

//class ControllerInfo final {
//  using factory_type = std::function<BaseController *(BaseContext &)>;
// public:
//  template<typename Controller>
//  static ControllerInfo Make() {
//    return ControllerInfo(Controller::kName, [](auto &context) -> Controller * {
//      return new Controller(context);
//    });
//  }
//  std::string_view name() const {
//    return name_;
//  }
//  std::unique_ptr<BaseController> Create(BaseContext &context) const {
//    return std::unique_ptr<BaseController>(factory_(context));
//  }
// private:
//  ControllerInfo(std::string_view name, factory_type factory)
//  : name_(name),
//    factory_(std::move(factory)) {}
//  const std::string_view name_;
//  factory_type factory_;
//};

//class Launcher final {
//  static constexpr char TAG[] = "Milk::Launcher";
//  template<typename Controller>
//  static ControllerInfo Info() {
//    return ControllerInfo::Make<Controller>();
//  }
// public:
//  static void Launch(int argc, char *argv[], ContextWrapper context) {
//    Launcher(context, {
//      Info<CodecController>(),
//      Info<DumpController>(),
//      Info<PlayController>(),
//    }).Main(argc, argv);
//  }
// private:
//  Launcher(BaseContext &context, std::initializer_list<ControllerInfo> infos)
//  : context_(context),
//    infos_(std::move(infos)) {}
//  void Main(int argc, char *argv[]) {
//    auto args = Args(argc, argv);
//    if (infos_.empty()) {
//      MilkTea_assert("no command to launch");
//    }
//    if (!args.empty()) {
//      auto mode = args.front();
//      for (const auto &it : infos_) {
//        if (it.name() == mode) {
//          args.pop_front();
//          it.Create(context_)->Main(args);
//          return;
//        }
//      }
//    }
//    infos_[0].Create(context_)->Main(args);
//  }
//  static std::list<std::string_view> Args(int argc, char *argv[]) {
//    std::list<std::string_view> args;
//    for (int i = 1; i < argc; ++i) {
//      args.emplace_back(argv[i]);
//    }
//    return args;
//  }
//  BaseContext &context_;
//  std::vector<ControllerInfo> infos_;

//};

//} // namespace Milk

//#endif // ifndef MILK_LAUNCH_H_
