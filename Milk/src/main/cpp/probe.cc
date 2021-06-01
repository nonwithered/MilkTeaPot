#include <map>

#include "launcher.h"

namespace {

constexpr char kUsage[] =
"Usage: milk probe [OPTIONS] [FILES]\n"
"  -h, --help\n"
"    print this help message\n"
"  -L {d, i, w, e}\n"
"  -L {debug, info, warn, error}\n"
"  --log {d, i, w, e}\n"
"  --log {debug, info, warn, error}\n"
"    init log level, or no log\n"
"  -x\n"
"    show all numbers in hex\n"
"  -c {h, e, d}\n"
"  --content {header, event, detail}\n"
"    only headers, or show events with delta-time, or all args\n"
"    only headers by default\n"
;

class Probe final {
 public:
  bool ShowHelp(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    std::cerr << kUsage << std::endl;
    return true;
  }
  bool InitLog(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    MilkPowder_Log_Level_t level = MilkPowder_Log_Level_t::ASSERT;
    if (itr == args.end()) {
      std::cerr << "milk probe -L: need log level" << std::endl;
      return false;
    } else if (*itr == "d" || *itr == "debug") {
      level = MilkPowder_Log_Level_t::DEBUG;
    } else if (*itr == "i" || *itr == "info") {
      level = MilkPowder_Log_Level_t::INFO;
    } else if (*itr == "w" || *itr == "warn") {
      level = MilkPowder_Log_Level_t::WARN;
    } else if (*itr == "e" || *itr == "error") {
      level = MilkPowder_Log_Level_t::ERROR;
    }
    if (level == MilkPowder_Log_Level_t::ASSERT) {
      std::cerr << "milk probe -L: invalid level: " << *itr << std::endl;
      return false;
    }
    LogInitLevel(level);
    itr = args.erase(itr);
    return true;
  }
  Probe() {
    callbacks_["-h"] = [this](auto &itr, auto &args) -> bool { return ShowHelp(itr, args); };
    callbacks_["--help"] = [this](auto &itr, auto &args) -> bool { return ShowHelp(itr, args); };
    callbacks_["-L"] = [this](auto &itr, auto &args) -> bool { return InitLog(itr, args); };
    callbacks_["--log"] = [this](auto &itr, auto &args) -> bool { return InitLog(itr, args); };
  }
  void Launch(std::list<std::string_view> &args) {
    for (auto itr = args.begin(); itr != args.end(); ) {
      auto it = callbacks_.find(*itr);
      if (it != callbacks_.end()) {
        itr = args.erase(itr);
        if (!it->second(itr, args)) {
          return;
        }
      } else {
        ++itr;
      }
    }
    for (auto it : args) {
      std::cout << it << std::endl;
    }
  }
 private:
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
};

} // namespace

namespace Milk {

void Launcher::LaunchProbe(std::list<std::string_view> &args) {
  Probe().Launch(args);
}

std::string_view Launcher::UsageProbe() {
  return kUsage;
}

} // namespace Milk
