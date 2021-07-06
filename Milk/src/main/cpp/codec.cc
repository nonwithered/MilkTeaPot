#include <map>
#include <memory>
#include <functional>

#include "launcher.h"

namespace Milk {

class Codec final {
 public:
  static constexpr char kUsage[] =
"Usage: milk [OPTIONS] [FILES]\n"
"  -h, --help\n"
"    print this help message\n"
"  -v, --version\n"
"    print version code\n"
;
  Codec()
    : callbacks_(),
      help_(false),
      version_(false) {
    callbacks_["-h"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["--help"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["-v"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
      return true;
    };
    callbacks_["--version"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
      return true;
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
      std::cerr << "milk: no input files" << std::endl;
      return;
    }
    // todo
  }
 private:
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  bool help_;
  bool version_;
  void ShowHelp() {
    if (help_) {
      return;
    } else {
      help_ = true;
    }
    Launcher::ShowHelp();
  }
  void ShowVersion() {
    if (version_) {
      return;
    } else {
      version_ = true;
    }
    Launcher::ShowVersion();
  }
};

void Launcher::LaunchCodec(std::list<std::string_view> &args) {
  Codec().Launch(args);
}

std::string_view Launcher::UsageCodec() {
  return Codec::kUsage;
}

} // namespace Milk
