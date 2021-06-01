#include <map>
#include <memory>

#include "launcher.h"

namespace Milk {

class Probe final {
 public:
  static constexpr char kUsage[] =
"Usage: milk probe [OPTIONS] [FILES]\n"
"  -h, --help\n"
"    print this help message\n"
"  --log {d, i, w, e, debug, info, warn, error}\n"
"    init log level, or no log\n"
"  -x\n"
"    show all numbers in hex\n"
"  -L {h, e, v, header, events, verbose}\n"
"  --level {h, e, v, header, events, verbose}\n"
"    detail level\n"
"    only headers by default\n"
;
  Probe()
    : callbacks_(),
      help_(false),
      hex_(false) {
    callbacks_["-h"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["--help"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["--log"] = [this](auto &itr, auto &args) -> bool {
      return InitLog(itr, args);
    };
    callbacks_["-x"] = [this](auto &itr, auto &args) -> bool {
      EnableHex();
      return true;
    };
    callbacks_["-L"] = [this](auto &itr, auto &args) -> bool {
      return DetailLevel(itr, args);
    };
    callbacks_["--level"] = [this](auto &itr, auto &args) -> bool {
      return DetailLevel(itr, args);
    };
  }
  void Launch(std::list<std::string_view> &args) {
    for (auto itr = args.begin(); itr != args.end(); ) {
      auto it = callbacks_.find(*itr);
      if (it != callbacks_.end()) {
        itr = args.erase(itr);
        if (!it->second(itr, args)) {
          ShowHelp();
          return;
        }
      } else {
        ++itr;
      }
    }
    if (args.size() == 0) {
      ShowHelp();
      return;
    }
    for (auto it : args) {
      Preview(it);
    }
  }
 private:
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  bool help_;
  bool hex_;
  uint8_t detail_ = 0;
  void ShowHelp() {
    if (help_) {
      return;
    } else {
      help_ = true;
    }
    std::cerr << kUsage << std::endl;
  }
  bool InitLog(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    MilkPowder_Log_Level_t level = MilkPowder_Log_Level_t::ASSERT;
    if (itr == args.end()) {
      std::cerr << "milk probe --log: need log level" << std::endl;
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
      std::cerr << "milk probe --log: invalid log level: " << *itr << std::endl;
      return false;
    }
    LogInitLevel(level);
    itr = args.erase(itr);
    return true;
  }
  void EnableHex() {
    hex_ = true;
  }
  bool DetailLevel(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    if (itr == args.end()) {
      std::cerr << "milk probe --level: need detail level" << std::endl;
      return false;
    } else if (*itr == "h" || *itr == "header") {
      detail_ = 0;
    } else if (*itr == "e" || *itr == "events") {
      detail_ = 1;
    } else if (*itr == "v" || *itr == "verbose") {
      detail_ = 2;
    } else {
      std::cerr << "milk probe --level: invalid detail level: " << *itr << std::endl;
      return false;
    }
    itr = args.erase(itr);
    return true;
  }
  void Preview(std::string_view filename) {
    InputReader reader(filename);
    if (reader.NonOpen()) {
      std::cerr << "Failed to open: " << filename << std::endl;
      return;
    }
  }
};

void Launcher::LaunchProbe(std::list<std::string_view> &args) {
  Probe().Launch(args);
}

std::string_view Launcher::UsageProbe() {
  return Probe::kUsage;
}

} // namespace Milk
