#ifndef MILK_DUMP_H_
#define MILK_DUMP_H_

#include <map>
#include <memory>
#include <functional>

#include <milkpowder.h>

#include "control.h"

namespace Milk {

class DumpController final : public BaseController {
  static constexpr char TAG[] = "Milk::DumpController";
  using self_type = DumpController;
  static constexpr auto endl = Printer::endl;
 public:
  static constexpr auto kName = "dump";
  static constexpr auto kUsage = R"(
Usage: milk dump [OPTIONS] [FILES]
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
  -v, --version
    print version code
  -x
    show all numbers in hex
  -L {h, d, e, v, header, data, events, verbose}
  --level {h, d, e, v, header, data, events, verbose}
    detail level
    only headers by default
)";
  DumpController(std::string help_text)
  : BaseController(std::move(help_text)),
    printer_(),
    hex_(false),
    detail_(0) {
    Config(&self_type::EnableHex, {
      "-x",
    });
    Config(&self_type::DetailLevel, {
      "-L",
      "--level",
    });
  }
 protected:
  void Main(std::list<std::string_view> &args) final {
    if (args.empty()) {
      printer() << "milk dump: no input files" << Printer::endl;
      return;
    }
    for (auto it : args) {
      Preview(it);
    }
  }
 private:
  void EnableHex() {
    hex_ = true;
  }
  bool DetailLevel(ArgsCursor &cursor) {
    if (!cursor) {
      std::cerr << "milk dump --level: need detail level" << std::endl;
      return false;
    } else if (*cursor == "h" || *cursor == "header") {
      detail_ = 0;
    } else if (*cursor == "d" || *cursor == "data") {
      detail_ = 1;
    } else if (*cursor == "e" || *cursor == "events") {
      detail_ = 2;
    } else if (*cursor == "v" || *cursor == "verbose") {
      detail_ = 3;
    } else {
      std::cerr << "milk dump --level: invalid detail level: " << *cursor << std::endl;
      return false;
    }
    ++cursor;
    return true;
  }
  void Preview(std::string_view filename) {
    MilkPowder::FileReader reader(filename);
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
  void PreviewHead(MilkPowder::FileReader &reader) {
    uint8_t buf[5];
    size_t count;
    // header type
    count = reader.Read(buf, 4);
    if (count < 4) {
      std::cerr << "Failed to read header type 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
      return;
    }
    buf[4] = '\0';
    std::string type(reinterpret_cast<char *>(buf));
    if (type != "MThd") {
      std::cerr << "Error header type " << InternalToStringFromBytes(reinterpret_cast<const uint8_t *>(type.data()), 4) << std::endl;
      return;
    }
    // header length
    count = reader.Read(buf, 4);
    if (count < 4) {
      std::cerr << "Failed to read header length 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
      return;
    }
    uint32_t length = InternalFromBytesToU32(buf);
    if (length != 6) {
      std::cerr << "Error header length " << InternalToStringFromU32(length) << std::endl;
      return;
    }
    // header format
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header format 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
      return;
    }
    uint16_t format = InternalFromBytesToU16(buf);
    // header ntrks
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header ntrks 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
      return;
    }
    uint16_t ntrks = InternalFromBytesToU16(buf);
    // header division
    count = reader.Read(buf, 2);
    if (count < 2) {
      std::cerr << "Failed to read header division 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
      return;
    }
    uint16_t division = InternalFromBytesToU16(buf);
    // header chunk
    printer() << "[HEADER]" << endl;
    printer() << "type=" << InternalToStringFromBytes(reinterpret_cast<const uint8_t *>(type.data()), 4) << endl;
    printer() << "length=" << InternalToStringFromU32(length) << endl;
    printer() << "format=" << InternalToStringFromU16(format) << endl;
    printer() << "ntrks=" << InternalToStringFromU16(ntrks) << endl;
    printer() << "division=" << InternalToStringFromU16(division) << endl;
    printer() << "[/HEADER]" << endl;
    // foreach chunk
    for (uint16_t i = 0; i < ntrks; ++i) {
      // chunk type
      count = reader.Read(buf, 4);
      if (count < 4) {
        std::cerr << "Failed to read chunk type 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
        return;
      }
      buf[4] = '\0';
      std::string type(reinterpret_cast<char *>(buf));
      // chunk length
      count = reader.Read(buf, 4);
      if (count < 4) {
        std::cerr << "Failed to read chunk length 0x" << MilkTea::ToStringHexFromBytes(buf, count) << std::endl;
        return;
      }
      uint32_t length = InternalFromBytesToU32(buf);
      // each chunk
      printer() << "[CHUNK]" << endl;
      printer() << "type=" << InternalToStringFromBytes(reinterpret_cast<const uint8_t *>(type.data()), 4) << endl;
      printer() << "length=" << InternalToStringFromU32(length) << endl;
      if (!ShowData(reader, length)) {
        std::cerr << "Failed to read data of chunk because of unexpected EOF" << std::endl;
        return;
      }
      printer() << "[/CHUNK]" << endl;
    }
  }
  bool ShowData(MilkPowder::FileReader &reader, const uint32_t length) {
    if (detail_ != 1) {
      size_t count = reader.Read(nullptr, static_cast<size_t>(length));
      if (count < static_cast<size_t>(length)) {
        return false;
      }
    } else {
      printer() << "  Offset: 0001 0203 0405 0607 0809 0A0B 0C0D 0E0F" << endl;
      uint32_t line = 0;
      size_t count = 0;
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
        printer() << MilkTea::ToStringHexFromU32(line) << ":";
        line += 16;
        for (size_t i = 0; i < 8; ++i) {
          printer() << " ";
          for (size_t j = 0; j < 2; ++j) {
            size_t k = i * 2 + j;
            if (k < c) {
              printer() << MilkTea::ToStringHexFromU8(buf[k]);
            } else {
              printer() << "  ";
            }
          }
        }
        printer() << "  " << MilkTea::ToStringFromChars(buf, c) << endl;
      }
    }
    return true;
  }
  void PreviewBody(MilkPowder::FileReader &reader) {
    auto midi = MilkPowder::MidiMutableWrapper::Parse(reader);
    // header chunk
    printer() << "[HEADER]" << endl;
    printer() << "type=MThd" << endl;
    printer() << "length=" << InternalToStringFromU32(6) << endl;
    // format
    uint16_t format = midi.GetFormat();
    printer() << "format=" << InternalToStringFromU16(format) << endl;
    // ntrks
    uint16_t ntrks = midi.GetNtrks();
    printer() << "ntrks=" << InternalToStringFromU16(ntrks) << endl;
    // division
    uint16_t division = midi.GetDivision();
    printer() << "division=" << InternalToStringFromU16(division) << endl;
    printer() << "[/HEADER]" << endl;
    // foreach chunk
    for (uint16_t i = 0; i != ntrks; ++i) {
      MilkPowder::TrackConstWrapper track = midi.GetTrack(i);
      // each chunk
      printer() << "[CHUNK]" << endl;
      printer() << "type=MTrk" << endl;
      std::function<void(MilkPowder::MessageConstWrapper)> callback;
      if (detail_ == 2) {
        callback = [this](MilkPowder::MessageConstWrapper item) -> void {
          ShowMessages(item);
        };
      } else {
        callback = [this](MilkPowder::MessageConstWrapper item) -> void {
          ShowMessagesVerbose(item);
        };
      }
      for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
        auto item = track.GetMessage(i);
        callback(item);
      }
      printer() << "[/CHUNK]" << endl;
    }
  }
  void ShowMessages(MilkPowder::MessageConstWrapper message) {
    printer() << "[EVENT]" << endl;
    // delta
    uint32_t delta = message.GetDelta();
    printer() << "delta=" << InternalToStringFromVarLen(delta) << endl;
    // type
    uint32_t type = message.GetType();
    printer() << "type=" << MilkTea::ToStringHexFromU8(type) << endl;
    printer() << "[/EVENT]" << endl;
  }
  void ShowMessagesVerbose(MilkPowder::MessageConstWrapper message) {
    printer() << "[EVENT]" << endl;
    // delta
    uint32_t delta = message.GetDelta();
    printer() << "delta=" << InternalToStringFromVarLen(delta) << endl;
    // type
    uint32_t type = message.GetType();
    printer() << "type=" << MilkTea::ToStringHexFromU8(type);
    do {
      // event
      if (message.IsEvent()) {
        printer() << endl;
        auto event = MilkPowder::EventConstWrapper::From(message);
        auto [arg0, arg1] = event.GetArgs();
        printer() << "args=" << MilkTea::ToStringHexFromU8(arg0);
        if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
          printer() << MilkTea::ToStringHexFromU8(arg1);
        }
        printer() << endl;
        break;
      }
      // meta
      if (message.IsMeta()) {
        auto meta = MilkPowder::MetaConstWrapper::From(message);
        type = meta.GetType();
        printer() << MilkTea::ToStringHexFromU8(type) << endl;
        const uint8_t *args = nullptr;
        uint32_t len = meta.GetArgs(&args);
        printer() << "args=" << InternalToStringFromBytes(args, len) << endl;
        break;
      }
      // sysex
      if (message.IsSysex()) {
        printer() << endl;
        auto sysex = MilkPowder::SysexConstWrapper::From(message);
        uint32_t idx = 0;
        std::function<void(const MilkPowder::SysexItem &)> callback = [this, &idx](const MilkPowder::SysexItem &it) -> void {
          uint32_t delta = it.delta_;
          uint32_t length = it.length_;
          const uint8_t *args = it.args_;
          if (idx == 0) {
            printer() << "args[" << idx++ << "]=" << InternalToStringFromBytes(args, length) << endl;
            return;
          } else {
            printer() << "args[" << idx++ << "]=" << InternalToStringFromBytes(args, length) << ", delta=" << MilkTea::ToStringHexFromVarLen(delta) << endl;
          }
        };
        for (uint32_t i = 0, n = sysex.GetCount(); i != n; ++i) {
          callback(sysex.GetItem(i));
        }
        break;
      }
      // assert
      MilkTea_assertf("ShowMessagesVerbose -- type: %02" PRIx8, message.GetType());
    } while (false);
    printer() << "[/EVENT]" << endl;
  }
  std::string InternalToStringFromBytes(const uint8_t *bytes, size_t length) {
    return hex_
    ? MilkTea::ToStringHexFromBytes(bytes, length)
    : MilkTea::ToStringFromBytes(bytes, length);
  }
  std::string InternalToStringFromVarLen(uint32_t n) {
    return hex_
    ? MilkTea::ToStringHexFromVarLen(n)
    : MilkTea::ToStringFromU32(n);
  }
  std::string InternalToStringFromU32(uint32_t n) {
    return hex_
    ? MilkTea::ToStringHexFromU32(n)
    : MilkTea::ToStringFromU32(n);
  }
  std::string InternalToStringFromU16(uint16_t n) {
    return hex_
    ? MilkTea::ToStringHexFromU16(n)
    : MilkTea::ToStringFromU16(n);
  }
  std::string InternalToStringFromU8(uint8_t n) {
    return hex_
    ? MilkTea::ToStringHexFromU8(n)
    : MilkTea::ToStringFromU8(n);
  }
  static uint32_t InternalFromBytesToU32(const uint8_t bytes[]) {
    return (bytes[0] << 030) | (bytes[1] << 020) | (bytes[2] << 010) | bytes[3];
  }
  static uint16_t InternalFromBytesToU16(const uint8_t bytes[]) {
    return (bytes[0] << 010) | bytes[1];
  }
  Printer &printer() {
    return printer_;
  }
  Printer printer_;
  bool hex_;
  uint8_t detail_;
};

} // namespace Milk

#endif // ifndef  MILK_DUMP_H_
