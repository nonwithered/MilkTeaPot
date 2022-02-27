#include <sstream>
#include <array>
#include <cstring>

#include <tea.h>
#include <milktea.h>

#include "midi.h"
#include "track.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace MilkTea::ToString;

using reader_type = std::function<size_t(uint8_t [], size_t)>;

using namespace MilkPowder::internal;

inline
auto ParseU8(reader_type &reader) -> uint8_t {
  uint8_t b = 0;
  if (size_t n = reader(&b, 1); n < 1) {
    err::raise<err_enum::unexpected_eof>("parse u8");
    return 0;
  }
  return b;
}

inline
auto ParseU16(reader_type &reader) -> uint16_t {
  std::array<uint8_t, 2> arr{};
  if (size_t n = reader(arr.data(), 2); n < 2) {
    err::raise<err_enum::unexpected_eof>("parse u16");
    return 0;
  }
  return static_cast<uint16_t>((arr[0] << 010) | arr[1]);
}

inline
auto ParseU32(reader_type &reader) -> uint32_t {
  std::array<uint8_t, 4> arr{};
  if (size_t n = reader(arr.data(), 4); n < 4) {
    err::raise<err_enum::unexpected_eof>("parse u32");
    return 0;
  }
  return static_cast<uint32_t>((arr[0] << 030) | arr[1] << 020 | arr[2] << 010 | arr[3]);
}

inline
auto ParseUsize(reader_type &reader) -> uint32_t {
  std::array<uint8_t, 4> arr{};
  uint8_t last = 0xff;
  size_t len = 0;
  while (len < 4 && last >= 0x80) {
    arr[len++] = last = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::unexpected_eof>("parse usize", e);
      return 0;
    }
  }
  if (len == 4 && last >= 0x80) {
    err::raise<err_enum::undefined_format>(FromFormat("parse usize but it is too large: "
    "[ %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 " ]"));
    return 0;
  }
  uint32_t ret = arr[0] & 0x7f;
  for (size_t i = 1; i != len; ++i) {
    ret <<= 07;
    ret |= arr[i] & 0x7f;
  }
  return ret;
}

inline
auto ParseVecN(reader_type &reader, size_t len) -> std::vector<uint8_t> {
  std::vector<uint8_t> vec(len);
  if (size_t n = reader(vec.data(), len); n < len) {
    err::raise<err_enum::unexpected_eof>(FromFormat(
    "parse bytes, expected length is %" PRIu32 " but only get %" PRIu32 " bytes", (uint32_t) len, (uint32_t) n));
    return {};
  }
  return vec;
}

inline
auto ParseArgs(reader_type &reader) -> std::vector<uint8_t> {
  uint32_t capacity = ParseUsize(reader);
  return ParseVecN(reader, static_cast<size_t>(capacity));
}

inline
auto ParseEvent(reader_type &reader, uint32_t delta, uint8_t type) -> Event * {
  if (type < 0x80 || type >= 0xf0) {
    type = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse event type", e);
      return nullptr;
    }
    if (type < 0x80 || type >= 0xf0) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse event but type is invalid: %" PRIx8, type));
      return nullptr;
    }
  }
  std::array<uint8_t, 2> args{};
  args[0] = ParseU8(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse event arg0", e);
    return nullptr;
  }
  if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
    args[1] = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse event arg1", e);
      return nullptr;
    }
  }
  return new Event(delta, type, args);
}

inline
auto ParseMeta(reader_type &reader, uint32_t delta, uint8_t type) -> Meta * {
  if (type != 0xff) {
    type = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse meta type", e);
      return nullptr;
    }
    if (type != 0xff) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse meta but type is not equals 0xff: %" PRIx8, type));
      return nullptr;
    }
  }
  type = ParseU8(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse meta", e);
    return nullptr;
  }
  if (type >= 0x80) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse meta but type is invalid: %" PRIx8, type));
      return nullptr;
  }
  auto args = ParseArgs(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse meta args", e);
    return nullptr;
  }
  return new Meta(delta, type, std::move(args));
}

inline
auto ParseSysex(reader_type &reader, uint32_t delta, uint8_t type) -> Sysex * {
  if (type != 0xf0) {
    type = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse sysex type", e);
      return nullptr;
    }
    if (type != 0xf0) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse sysex but type is not equals 0xf0: %" PRIx8, type));
      return nullptr;
    }
  }
  auto args = ParseArgs(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse sysex args", e);
    return nullptr;
  }
  if (args.empty()) {
    err::raise<err_enum::undefined_format>("parse sysex but args is empty");
    return nullptr;
  }
  auto last = args.back();
  std::vector<Sysex::item_type> items;
  items.emplace_back(delta, std::move(args));
  while (last != 0xf7) {
    delta = ParseUsize(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse sysex delta", e);
      return nullptr;
    }
    type = ParseU8(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse sysex types", e);
      return nullptr;
    }
    if (type != 0xf7) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse sysex but type is not equals 0xf7: %" PRIx8, type));
      return nullptr;
    }
    args = ParseArgs(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse sysex args in loop", e);
      return nullptr;
    }
    if (args.empty()) {
      err::raise<err_enum::undefined_format>("parse sysex in loop but args is empty");
      return nullptr;
    }
    last = args.back();
    items.emplace_back(delta, std::move(args));
  }
  return new Sysex(std::move(items));
}

inline
auto ParseMessage(reader_type &reader, uint8_t last) -> Message * {
  auto delta = ParseUsize(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse message delta", e);
    return nullptr;
  }
  auto type = ParseU8(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse message type", e);
    return nullptr;
  }
  if (type >= 0x80 && type < 0xf0) {
    return ParseEvent(reader, delta, type);
  }
  if (type == 0xff) {
    return ParseMeta(reader, delta, 0xff);
  }
  if (type == 0xf0) {
    return ParseSysex(reader, delta, 0xf0);
  }
  if (type < 0x80) {
    if (last < 0x80 || last >= 0xf0) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse message event but last type is invalid: %" PRIx8, last));
      return nullptr;
    }
    std::array<uint8_t, 2> args{};
    args[0] = type;
    type = last;
    if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
      args[1] = ParseU8(reader);
      if (auto e = err::init(); e != nullptr) {
        err::raise<err_enum::io_failure>("parse message event args", e);
        return nullptr;
      }
    }
    return new Event(delta, type, args);
  }
  err::raise<err_enum::undefined_format>(FromFormat(
  "parse message but type is invalid: %" PRIx8, type));
  return nullptr;
}

inline
auto ParseTrack(reader_type &reader) -> Track * {
  {
    auto head = ParseVecN(reader, 4);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse track head", e);
      return nullptr;
    }
    if (std::memcmp(head.data(), "MTrk", 4) != 0) {
      auto head_str = FromBytes(head.data(), 4);
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse track but head is invalid: %s", head_str.data()));
      return nullptr;
    }
  }
  auto capacity = ParseU32(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse track capacity", e);
    return nullptr;
  }
  auto len = capacity;
  reader_type func = [reader, capacity, &len](uint8_t bytes[], size_t n) -> size_t {
    if (len < n) {
      err::raise<err_enum::unexpected_eof>(FromFormat(
      "parse track but capacity is not enough: %" PRIu32, (uint32_t) capacity));
      return 0;
    }
    len -= n;
    return reader(bytes, n);
  };
  std::vector<Track::item_type> items;
  uint8_t last = 0;
  while (len != 0) {
    auto it = ParseMessage(func, last);
    if (it == nullptr) {
      auto e = err::init();
      err::raise<err_enum::io_failure>("parse track messages", e);
      return nullptr;
    }
    last = it->type_;
    bool end = last == 0xff && dynamic_cast<Meta &>(*it).type_ == 0x2f;
    items.emplace_back(it);
    if (end) {
      auto n = func(nullptr, len);
      if (n < len) {
        auto e = err::init();
        err::raise<err_enum::io_failure>("parse track when clear buffer", e);
        return nullptr;
      }
      break;
    }
  }
  return new Track(std::move(items));
}

inline
auto ParseMidi(reader_type &reader) -> Midi * {
  {
    auto head = ParseVecN(reader, 4);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse midi head", e);
      return nullptr;
    }
    if (std::memcmp(head.data(), "MThd", 4) != 0) {
      auto head_str = FromBytes(head.data(), 4);
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse midi but head is invalid: %s", head_str.data()));
      return nullptr;
    }
  }
  {
    auto len = ParseU32(reader);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::io_failure>("parse midi head length", e);
      return nullptr;
    }
    if (6 != len) {
      err::raise<err_enum::undefined_format>(FromFormat(
      "parse midi but length of head is invalid: %" PRIu32, len));
      return nullptr;
    }
  }
  auto format = ParseU16(reader);
  auto count = ParseU16(reader);
  auto division = ParseU16(reader);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("parse midi head info", e);
    return nullptr;
  }
  std::vector<Midi::item_type> items;
  for (uint16_t i = 0; i != count; ++i) {
    auto track = ParseTrack(reader);
    if (track == nullptr) {
      auto e = err::init();
      err::raise<err_enum::io_failure>(FromFormat(
      "parse midi track: index of track is %" PRIu16 " and head of midi is "
      "[ %" PRIx8 ", %" PRIx8 ", %" PRIx8 " ]", i, format, count, division), e);
      return nullptr;
    }
    items.emplace_back(track);
  }
  return new Midi(format, division, std::move(items));
}

auto Midi::Parse(file::input func) -> MilkPowder::Midi * {
  auto invoke = func.invoke;
  if (invoke == nullptr) {
    err::raise<err_enum::null_obj>("parse midi but the invoke param is nullptr");
    return nullptr;
  }
  defer d = [func]() {
    if (auto close = func.close; close != nullptr) {
      close(func.capture);
    }
  };
  reader_type reader = [invoke, capture = func.capture](uint8_t bytes[], size_t len) -> size_t {
    return invoke(capture, bytes, len);
  };
  return unwrap_cast(*ParseMidi(reader));
}

} // namespace internal

} // namespace MilkPowder
