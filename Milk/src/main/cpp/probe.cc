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
"  -v, --version\n"
"    print version code\n"
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
    callbacks_["-v"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
      return true;
    };
    callbacks_["--version"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
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
      std::cerr << "milk probe: no input files" << std::endl;
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
  void ShowVersion() {
    help_ = true;
    std::cout << "version: " << kVersion << std::endl;
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
    switch (detail_) {
      case 0:
        PreviewHeader(reader);
        break;
      case 1:
        PreviewEvents(reader);
        break;
      case 2:
        PreviewVerbose(reader);
        break;
      default:
        break;
    }
  }
  void PreviewHeader(InputReader &reader) {
    uint8_t buf[5];
    size_t count;
    // header type
    count = reader.Read(buf, 4);
    if (count < 4) {
      std::cerr << "Failed to read header type 0x" << FromBytesToStringHex(buf, count) << std::endl;
      return;
    }
    buf[5] = '\0';
    std::string type(reinterpret_cast<char *>(buf));
    if (type != "MThd") {
      std::cerr << "Error header type " << FromStringToStr(type.data(), 4) << std::endl;
      return;
    }
    // header length
    count = reader.Read(buf, 4);
    if (count < 4) {
      std::cerr << "Failed to read header length 0x" << FromBytesToStringHex(buf, count) << std::endl;
      return;
    }
    uint32_t length = FromBytesToU32(buf);
    if (length != 6) {
      std::cerr << "Error header length " << FromU32ToStr(length) << std::endl;
      return;
    }
    // header format
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header format 0x" << FromBytesToStringHex(buf, count) << std::endl;
      return;
    }
    uint16_t format = FromBytesToU16(buf);
    // header ntrks
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header ntrks 0x" << FromBytesToStringHex(buf, count) << std::endl;
      return;
    }
    uint16_t ntrks = FromBytesToU16(buf);
    // header division
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header division 0x" << FromBytesToStringHex(buf, count) << std::endl;
      return;
    }
    uint16_t division = FromBytesToU16(buf);
    // MThd chunk
    std::cout << "[HEADER]" << std::endl;
    std::cout << "type=" << FromStringToStr(type.data(), 4) << std::endl;
    std::cout << "length=" << FromU32ToStr(length) << std::endl;
    std::cout << "format=" << FromU16ToStr(format) << std::endl;
    std::cout << "ntrks=" << FromU16ToStr(ntrks) << std::endl;
    std::cout << "division=" << FromU16ToStr(division) << std::endl;
    std::cout << "[/HEADER]" << std::endl;
  }
  void PreviewEvents(InputReader &reader) {
  }
  void PreviewVerbose(InputReader &reader) {
  }
  std::string FromStringToStr(const char *bytes, size_t length) {
    return hex_
    ? FromBytesToStringHex(reinterpret_cast<const uint8_t *>(bytes), length)
    : FromBytesToString(reinterpret_cast<const uint8_t *>(bytes), length);
  }
  std::string FromU32ToStr(uint32_t n) {
    return hex_
    ? FromU32ToStringHex(n)
    : FromU32ToString(n);
  }
  std::string FromU16ToStr(uint16_t n) {
    return hex_
    ? FromU16ToStringHex(n)
    : FromU16ToString(n);
  }
  std::string FromU8ToStr(uint8_t n) {
    return hex_
    ? FromU8ToStringHex(n)
    : FromU8ToString(n);
  }
};

void Launcher::LaunchProbe(std::list<std::string_view> &args) {
  Probe().Launch(args);
}

std::string_view Launcher::UsageProbe() {
  return Probe::kUsage;
}

} // namespace Milk
