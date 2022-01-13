#ifndef MILK_LAUNCH_H_
#define MILK_LAUNCH_H_

#include <vector>

#include "control.h"

namespace Milk {

class ControllerInfo final {
  using factory_type = std::function<BaseController *(std::string)>;
 public:
  template<typename Controller>
  static ControllerInfo Make() {
    return ControllerInfo(Controller::kName, Controller::kUsage, [](auto s) -> Controller * {
      return new Controller(s);
    });
  }
  std::string_view name() const {
    return name_;
  }
  std::string_view usage() const {
    return usage_;
  }
  std::unique_ptr<BaseController> Create(std::string help_text) const {
    return std::unique_ptr<BaseController>(factory_(help_text));
  }
 private:
  ControllerInfo(std::string_view name, std::string_view usage, factory_type factory)
  : name_(name),
    usage_(usage),
    factory_(std::move(factory)) {}
  const std::string_view name_;
  const std::string_view usage_;
  factory_type factory_;
};

class Launch final {
 public:
  Launch(ControllerInfo info, std::initializer_list<ControllerInfo> infos)
  : info_(std::move(info)),
    infos_(std::move(infos)) {}
  void Main(int argc, char *argv[]) {
    auto args = Args(argc, argv);
    if (!args.empty()) {
      auto mode = args.front();
      for (const auto &it : infos_) {
        if (it.name() == mode) {
          args.pop_front();
          it.Create(std::string(it.usage()))->Launch(args);
          return;
        }
      }
    }
    std::string help_text = std::string(info_.usage());
    for (const auto &it : infos_) {
      help_text += it.usage();
    }
    info_.Create(std::move(help_text))->Launch(args);
  }
 private:
  static std::list<std::string_view> Args(int argc, char *argv[]) {
    std::list<std::string_view> args;
    for (int i = 1; i < argc; ++i) {
      args.emplace_back(argv[i]);
    }
    return args;
  }
  ControllerInfo info_;
  std::vector<ControllerInfo> infos_;
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
