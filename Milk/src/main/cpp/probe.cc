#define check_err(s) \
do { \
  if (err != MilkPowder_Errno_t::Nil) { \
    std::cerr << "Failed to " s " because of " << ErrMsg(err) << std::endl; \
    return; \
  } \
} while (false)

#include <map>
#include <memory>
#include <functional>

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
"  -L {h, d, e, v, header, data, events, verbose}\n"
"  --level {h, d, e, v, header, data, events, verbose}\n"
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
    } else if (*itr == "d" || *itr == "data") {
      detail_ = 1;
    } else if (*itr == "e" || *itr == "events") {
      detail_ = 2;
    } else if (*itr == "v" || *itr == "verbose") {
      detail_ = 3;
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
      case 1:
        PreviewHead(reader);
        break;
      case 2:
      case 3:
        PreviewBody(reader);
        break;
      default:
        break;
    }
  }
  void PreviewHead(InputReader &reader) {
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
      std::cerr << "Error header type " << FromBytesToStr(reinterpret_cast<const uint8_t *>(type.data()), 4) << std::endl;
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
    // header chunk
    std::cout << "[HEADER]" << std::endl;
    std::cout << "type=" << FromBytesToStr(reinterpret_cast<const uint8_t *>(type.data()), 4) << std::endl;
    std::cout << "length=" << FromU32ToStr(length) << std::endl;
    std::cout << "format=" << FromU16ToStr(format) << std::endl;
    std::cout << "ntrks=" << FromU16ToStr(ntrks) << std::endl;
    std::cout << "division=" << FromU16ToStr(division) << std::endl;
    std::cout << "[/HEADER]" << std::endl;
    // foreach chunk
    for (uint16_t i = 0; i < ntrks; ++i) {
      // chunk type
      count = reader.Read(buf, 4);
      if (count < 4) {
        std::cerr << "Failed to read chunk type 0x" << FromBytesToStringHex(buf, count) << std::endl;
        return;
      }
      buf[5] = '\0';
      std::string type(reinterpret_cast<char *>(buf));
      // chunk length
      count = reader.Read(buf, 4);
      if (count < 4) {
        std::cerr << "Failed to read chunk length 0x" << FromBytesToStringHex(buf, count) << std::endl;
        return;
      }
      uint32_t length = FromBytesToU32(buf);
      // each chunk
      std::cout << "[CHUNK]" << std::endl;
      std::cout << "type=" << FromBytesToStr(reinterpret_cast<const uint8_t *>(type.data()), 4) << std::endl;
      std::cout << "length=" << FromU32ToStr(length) << std::endl;
      if (!ShowData(reader, length)) {
        std::cerr << "Failed to read data of chunk because of unexpected EOF" << std::endl;
        return;
      }
      std::cout << "[/CHUNK]" << std::endl;
    }
  }
  bool ShowData(InputReader &reader, const uint32_t length) {
    size_t count;
    if (detail_ != 1) {
      count = reader.Read(nullptr, static_cast<size_t>(length));
      if (count < static_cast<size_t>(length)) {
        return false;
      }
    } else {
      count = 0;
      std::cout << "  Offset: 0001 0203 0405 0607 0809 0A0B 0C0D 0E0F" << std::endl;
      uint32_t line = 0;
      while (count < static_cast<size_t>(length)) {
        uint8_t buf[16];
        size_t c = static_cast<size_t>(length) - count;
        if (c >= 16) {
          c = reader.Read(buf, 16);
        } else {
          c = reader.Read(buf, c);
        }
        if (c == 0) {
          return false;
        } else {
          count += c;
        }
        std::cout << FromU32ToStringHex(line) << ":";
        line += 16;
        for (size_t i = 0; i < 8; ++i) {
          std::cout << " ";
          for (size_t j = 0; j < 2; ++j) {
            size_t k = i * 2 + j;
            if (k < c) {
              std::cout << FromU8ToStringHex(buf[k]);
            } else {
              std::cout << "  ";
            }
          }
        }
        std::cout << "  " << FromBytesToStringDot(buf, c) << std::endl;
      }
    }
    return true;
  }
  void PreviewBody(InputReader &reader) {
    MilkPowder_Errno_t err;
    MilkPowderHolder<MilkPowder_Midi_t> midi;
    err = MilkPowder_Midi_Parse(&midi, &reader, InputReader::Reader);
    check_err("read midi file");
    // header chunk
    std::cout << "[HEADER]" << std::endl;
    std::cout << "type=MThd" << std::endl;
    std::cout << "length=" << FromU32ToStr(6) << std::endl;
    // format
    uint16_t format;
    err = MilkPowder_Midi_GetFormat(midi, &format);
    check_err("get format");
    std::cout << "format=" << FromU16ToStr(format) << std::endl;
    // ntrks
    uint16_t ntrks;
    err = MilkPowder_Midi_GetNtrks(midi, &ntrks);
    check_err("get ntrks");
    std::cout << "ntrks=" << FromU16ToStr(ntrks) << std::endl;
    // division
    uint16_t division;
    err = MilkPowder_Midi_GetDivision(midi, &division);
    check_err("get division");
    std::cout << "division=" << FromU16ToStr(division) << std::endl;
    std::cout << "[/HEADER]" << std::endl;
    // foreach chunk
    for (uint16_t i = 0; i != ntrks; ++i) {
      const MilkPowder_Track_t *track;
      MilkPowder_Midi_GetTrack(midi, i, &track);
      // each chunk
      std::cout << "[CHUNK]" << std::endl;
      std::cout << "type=MTrk" << std::endl;
      std::function<void(const MilkPowder_Message_t *)> callback;
      if (detail_ == 2) {
        callback = [this](const MilkPowder_Message_t *item) -> void {
          ShowMessages(item);
        };
      } else {
        callback = [this](const MilkPowder_Message_t *item) -> void {
          ShowMessagesVerbose(item);
        };
      }
      err = MilkPowder_Track_GetMessages(track, &callback, [](void *obj, const MilkPowder_Message_t *item) -> void {
        std::function<void(const MilkPowder_Message_t *)> &callback = *reinterpret_cast<std::function<void(const MilkPowder_Message_t *)> *>(obj);
        callback(item);
      });
      check_err("get messages");
      std::cout << "[/CHUNK]" << std::endl;
    }
  }
  void ShowMessages(const MilkPowder_Message_t *message) {
    MilkPowder_Errno_t err;
    std::cout << "[EVENT]" << std::endl;
    // delta
    uint32_t delta;
    err = MilkPowder_Message_GetDelta(message, &delta);
    check_err("get delta");
    std::cout << "delta=" << FromUsizeToStr(delta) << std::endl;
    // type
    uint8_t type;
    err = MilkPowder_Message_GetType(message, &type);
    check_err("get type");
    std::cout << "type=" << FromU8ToStringHex(type) << std::endl;
    std::cout << "[/EVENT]" << std::endl;
  }
  void ShowMessagesVerbose(const MilkPowder_Message_t *message) {
    MilkPowder_Errno_t err;
    std::cout << "[EVENT]" << std::endl;
    // delta
    uint32_t delta;
    err = MilkPowder_Message_GetDelta(message, &delta);
    check_err("get delta");
    std::cout << "delta=" << FromUsizeToStr(delta) << std::endl;
    // type
    uint8_t type;
    err = MilkPowder_Message_GetType(message, &type);
    check_err("get type");
    std::cout << "type=" << FromU8ToStringHex(type);
    do {
      bool b;
      // event
      err = MilkPowder_Message_IsEvent(message, &b);
      check_err("check event");
      if (b) {
        const MilkPowder_Event_t *event;
        err = MilkPowder_Message_ToEvent(message, &event);
        check_err("cast event");
        std::cout << std::endl;
        uint8_t args[2];
        err = MilkPowder_Event_GetArgs(event, args);
        check_err("get args");
        std::cout << "args=" << FromU8ToStringHex(args[0]);
        if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
          std::cout << FromU8ToStringHex(args[1]);
        }
        std::cout << std::endl;
        break;
      }
      // meta
      err = MilkPowder_Message_IsMeta(message, &b);
      check_err("check meta");
      if (b) {
        const MilkPowder_Meta_t *meta;
        err = MilkPowder_Message_ToMeta(message, &meta);
        check_err("cast meta");
        err = MilkPowder_Meta_GetType(meta, &type);
        check_err("get type");
        std::cout << FromU8ToStringHex(type) << std::endl;
        uint32_t len;
        const uint8_t *args;
        err = MilkPowder_Meta_GetArgs(meta, &args, &len);
        check_err("get args");
        std::cout << "args=" << FromBytesToStr(args, len) << std::endl;
        break;
      }
      // sysex
      err = MilkPowder_Message_IsSysex(message, &b);
      check_err("check sysex");
      if (b) {
        const MilkPowder_Sysex_t *sysex;
        err = MilkPowder_Message_ToSysex(message, &sysex);
        check_err("cast sysex");
        std::cout << std::endl;
        uint32_t idx = 0;
        std::function<void(uint32_t, const uint8_t *, uint32_t)> callback = [this, &idx](uint32_t delta, const uint8_t *args, uint32_t length) -> void {
          if (idx == 0) {
            std::cout << "args[" << idx++ << "]=" << FromBytesToStr(args, length) << std::endl;
            return;
          } else {
            std::cout << "args[" << idx++ << "]=" << FromBytesToStr(args, length) << ", delta=" << FromUsizeToStringHex(delta) << std::endl;
          }
        };
        err = MilkPowder_Sysex_GetArgs(sysex, &callback, [](void *obj, uint32_t delta, const uint8_t *args, uint32_t length) -> void {
          std::function<void(uint32_t, const uint8_t *, uint32_t)> &callback = *reinterpret_cast<std::function<void(uint32_t, const uint8_t *, uint32_t)> *>(obj);
          callback(delta, args, length);
        });
        check_err("get args");
        break;
      }
    } while (false);
    std::cout << "[/EVENT]" << std::endl;
  }
  std::string FromBytesToStr(const uint8_t *bytes, size_t length) {
    return hex_
    ? FromBytesToStringHex(bytes, length)
    : FromBytesToString(bytes, length);
  }
  std::string FromUsizeToStr(uint32_t n) {
    return hex_
    ? FromUsizeToStringHex(n)
    : FromU32ToString(n);
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
