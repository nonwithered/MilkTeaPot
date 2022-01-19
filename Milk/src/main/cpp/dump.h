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
  enum class DetailLevel {
    HEADER,
    DATA,
    EVENTS,
    VERBOSE,
  };
 public:
  static constexpr auto kName = "dump";
  static constexpr auto kUsage = R"(
Usage: milk dump [OPTIONS] [FILES]
  -v, --version
    print version code
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
  -x
    show all numbers in hex
  -L {h, d, e, v, header, data, events, verbose}
  --level {h, d, e, v, header, data, events, verbose}
    detail level
    only headers by default
)";
  DumpController(BaseContext &context)
  : BaseController(context, kUsage) {}
 public:
  void Main(std::list<std::string_view> &args) final {
    if (!BaseController::Config(Cocoa::Pipeline(*this, args))
        .Append({
            "-x",
        }, &self_type::hex_, true)
        .Append({
            "-L",
            "--level",
        }, &self_type::detail_,
        [this](auto &cmd) {
          Err() << "milk " << cmd << ": need detail level" << End();
        },
        [this](auto &cmd, auto &it) {
          Err() << "milk " << cmd << ": invalid detail level: " << it << End();
        }, {
          { DetailLevel::HEADER, { "h", "header" } },
          { DetailLevel::DATA, { "d", "data" } },
          { DetailLevel::EVENTS, { "e", "events" } },
          { DetailLevel::VERBOSE, { "v", "verbose" } },
        })
        .Launch(kName)) {
      return;
    }
    BaseController::Main(args);
    if (args.empty()) {
      Err() << "milk dump: no input files" << End();
      return;
    }
    for (auto it : args) {
      Preview(it);
    }
  }
 private:
  void Preview(std::string_view filename) {
    auto reader = Context().GetFileReader(filename.data(), filename.size());
    switch (detail_) {
      case DetailLevel::HEADER:
      case DetailLevel::DATA:
        PreviewHead(*reader);
        break;
      case DetailLevel::EVENTS:
      case DetailLevel::VERBOSE:
        PreviewBody(*reader);
        break;
      default:
        MilkTea_assert("Preview");
    }
  }
  void PreviewHead(BaseFileReader &reader) {
    uint8_t buf[5];
    size_t count;
    // header type
    count = reader.Read(buf, 4);
    if (count < 4) {
      Err() << "Failed to read header type 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
      return;
    }
    buf[4] = '\0';
    auto type = std::string(reinterpret_cast<char *>(buf));
    if (type != "MThd") {
      Err() << "Invalid header type " << ToStringFromBytes(buf, 4) << End();
      return;
    }
    // header length
    count = reader.Read(buf, 4);
    if (count < 4) {
      Err() << "Failed to read header length 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
      return;
    }
    auto length = FromBytesToU32(buf);
    if (length != 6) {
      Err() << "Invalid header length " << ToStringFromU32(length) << End();
      return;
    }
    // header format
    count = reader.Read(buf, 2);
    if (count < 2) {
      Err() << "Failed to read header format 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
      return;
    }
    auto format = FromBytesToU16(buf);
    // header ntrks
    count = reader.Read(buf, 2);
    if (count < 2) {
      Err() << "Failed to read header ntrks 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
      return;
    }
    auto ntrks = FromBytesToU16(buf);
    // header division
    count = reader.Read(buf, 2);
    if (count < 2) {
      Err() << "Failed to read header division 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
      return;
    }
    auto division = FromBytesToU16(buf);
    // header chunk
    Out() <<
    "[HEADER]" << Line() <<
    "type=" << ToStringFromBytes(reinterpret_cast<const uint8_t *>(type.data()), 4) << Line() <<
    "length=" << ToStringFromU32(length) << Line() <<
    "format=" << ToStringFromU16(format) << Line() <<
    "ntrks=" << ToStringFromU16(ntrks) << Line() <<
    "division=" << ToStringFromU16(division) << Line() <<
    "[/HEADER]" << End();
    // foreach chunk
    for (uint16_t i = 0; i < ntrks; ++i) {
      // chunk type
      count = reader.Read(buf, 4);
      if (count < 4) {
        Err() << "Failed to read chunk type 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
        return;
      }
      buf[4] = '\0';
      auto type = std::string(reinterpret_cast<char *>(buf));
      // chunk length
      count = reader.Read(buf, 4);
      if (count < 4) {
        Err() << "Failed to read chunk length 0x" << MilkTea::ToStringHex::FromBytes(buf, count) << End();
        return;
      }
      auto length = FromBytesToU32(buf);
      // each chunk
      Out() <<
      "[CHUNK]" << Line() <<
      "type=" << ToStringFromBytes(reinterpret_cast<const uint8_t *>(type.data()), 4) << Line() <<
      "length=" << ToStringFromU32(length) << Line();
      if (!ShowData(reader, length)) {
        Err() << "Failed to read data of chunk because of unexpected EOF" << End();
        return;
      }
      Out() <<
      "[/CHUNK]" << End();
    }
  }
  bool ShowData(BaseFileReader &reader, const uint32_t length) {
    if (detail_ != DetailLevel::DATA) {
      uint32_t count = reader.Read(nullptr, length);
      if (count < length) {
        MilkTea_logW("ShowData -- length: %" PRIu32 ", count: %" PRIu32, length, count);
        return false;
      }
      return true;
    }
    Out() << "  Offset: 0001 0203 0405 0607 0809 0A0B 0C0D 0E0F" << End();
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
      Out() << MilkTea::ToStringHex::FromU32(line) << ":";
      line += 16;
      for (auto i = 0; i < 8; ++i) {
        Out() << " ";
        for (auto j = 0; j < 2; ++j) {
          size_t k = i * 2 + j;
          if (k < c) {
            Out() << MilkTea::ToStringHex::FromU8(buf[k]);
          } else {
            Out() << "  ";
          }
        }
      }
      Out() << "  " << MilkTea::ToString::FromChars(buf, c) << End();
    }
    return true;
  }
  void PreviewBody(BaseFileReader &reader) {
    auto midi = MilkPowder::MidiMutableWrapper::Parse(reader);
    auto format = midi.GetFormat();
    auto ntrks = midi.GetNtrks();
    auto division = midi.GetDivision();
    Out() <<
    "[HEADER]" << Line() <<
    "type=MThd" << Line() <<
    "length=" << ToStringFromU32(6) << Line() <<
    "format=" << ToStringFromU16(format) << Line() <<
    "ntrks=" << ToStringFromU16(ntrks) << Line() <<
    "division=" << ToStringFromU16(division) << Line() <<
    "[/HEADER]" << End();
    for (uint16_t i = 0; i != ntrks; ++i) {
      auto track = midi.GetTrack(i);
      Out() <<
      "[CHUNK]" << Line() <<
      "type=MTrk" << Line();
      auto show_msg = [this]() -> std::function<void(MilkPowder::MessageConstWrapper)> {
        switch (detail_) {
          case DetailLevel::EVENTS:
            return [this](auto it) {
              ShowMessages(it);
            };
          case DetailLevel::VERBOSE:
            return [this](auto it) {
              ShowMessagesVerbose(it);
            };
          default:
            MilkTea_assert("PreviewBody");
        }
      }();
      for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
        auto item = track.GetMessage(i);
        show_msg(item);
      }
      Out() <<
      "[/CHUNK]" << End();
    }
  }
  void ShowMessages(MilkPowder::MessageConstWrapper message) {
    auto delta = message.GetDelta();
    auto type = message.GetType();
    Out() <<
    "[EVENT]" << Line() <<
    "delta=" << ToStringFromVarLen(delta) << Line() <<
    "type=" << MilkTea::ToStringHex::FromU8(type) << Line() <<
    "[/EVENT]" << End();
  }
  void ShowMessagesVerbose(MilkPowder::MessageConstWrapper message) {
    auto delta = message.GetDelta();
    auto type = message.GetType();
    Out() <<
    "[EVENT]" << Line() <<
    "delta=" << ToStringFromVarLen(delta) << Line() <<
    "type=" << MilkTea::ToStringHex::FromU8(type);
    do {
      if (message.IsEvent()) {
        Out() << Line();
        auto event = MilkPowder::EventConstWrapper::From(message);
        auto [arg0, arg1] = event.GetArgs();
        Out() <<
        "args=" << MilkTea::ToStringHex::FromU8(arg0);
        if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
          Out() << MilkTea::ToStringHex::FromU8(arg1);
        }
        Out() << Line();
        break;
      }
      if (message.IsMeta()) {
        auto meta = MilkPowder::MetaConstWrapper::From(message);
        type = meta.GetType();
        const uint8_t *args = nullptr;
        auto len = meta.GetArgs(&args);
        Out() << MilkTea::ToStringHex::FromU8(type) << Line() <<
        "args=" << ToStringFromBytes(args, len) << Line();
        break;
      }
      if (message.IsSysex()) {
        Out() << Line();
        auto sysex = MilkPowder::SysexConstWrapper::From(message);
        for (uint32_t i = 0, n = sysex.GetCount(); i != n; ++i) {
          auto it = sysex.GetItem(i);
          auto length = it.length_;
          auto *args = it.args_;
          if (i == 0) {
            Out() <<
            "args[" << i << "]=" << ToStringFromBytes(args, length) << Line();
          } else {
            auto delta = it.delta_;
            Out() <<
            "args[" << i << "]=" << ToStringFromBytes(args, length) << ", delta=" << MilkTea::ToStringHex::FromVarLen(delta) << Line();
          }
        }
        break;
      }
      // assert
      MilkTea_assertf("ShowMessagesVerbose -- type: %02" PRIx8, message.GetType());
    } while (false);
    Out() <<
    "[/EVENT]" << End();
  }
  std::string ToStringFromBytes(const uint8_t *bytes, size_t length) {
    return hex_
    ? MilkTea::ToStringHex::FromBytes(bytes, length)
    : MilkTea::ToString::FromBytes(bytes, length);
  }
  std::string ToStringFromVarLen(uint32_t n) {
    return hex_
    ? MilkTea::ToStringHex::FromVarLen(n)
    : MilkTea::ToString::FromU32(n);
  }
  std::string ToStringFromU32(uint32_t n) {
    return hex_
    ? MilkTea::ToStringHex::FromU32(n)
    : MilkTea::ToString::FromU32(n);
  }
  std::string ToStringFromU16(uint16_t n) {
    return hex_
    ? MilkTea::ToStringHex::FromU16(n)
    : MilkTea::ToString::FromU16(n);
  }
  std::string ToStringFromU8(uint8_t n) {
    return hex_
    ? MilkTea::ToStringHex::FromU8(n)
    : MilkTea::ToString::FromU8(n);
  }
  static uint32_t FromBytesToU32(const uint8_t bytes[]) {
    return (bytes[0] << 030) | (bytes[1] << 020) | (bytes[2] << 010) | bytes[3];
  }
  static uint16_t FromBytesToU16(const uint8_t bytes[]) {
    return (bytes[0] << 010) | bytes[1];
  }
  bool hex_ = false;
  DetailLevel detail_ = DetailLevel::HEADER;
};

} // namespace Milk

#endif // ifndef  MILK_DUMP_H_
