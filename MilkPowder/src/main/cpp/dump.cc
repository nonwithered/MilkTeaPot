#include <sstream>

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

inline
auto DumpU8(uint8_t n, std::vector<uint8_t> &vec) -> void {
  vec.push_back(n);
}

inline
auto DumpU16(uint16_t n, std::vector<uint8_t> &vec) -> void {
  DumpU8(static_cast<uint8_t>(n >> 010), vec);
  DumpU8(static_cast<uint8_t>(n & 0xff), vec);
}

inline
auto DumpU32(uint32_t n, std::vector<uint8_t> &vec) -> void {
  DumpU16(static_cast<uint16_t>(n >> 020), vec);
  DumpU16(static_cast<uint16_t>(n & 0xffff), vec);
}

inline
auto DumpUsize(uint32_t n, std::vector<uint8_t> &vec) -> void {
  uint8_t bytes[4];
  size_t size = 0;
  bytes[size++] = static_cast<uint8_t>(n & 0x7f);
  while (n > 0x7f) {
    n >>= 07;
    bytes[size++] = static_cast<uint8_t>((n & 0x7f) | 0x80);
  }
  for (size_t i = 0; i != size; ++i) {
    DumpU8(bytes[size - 1 -i], vec);
  }
}

inline
auto DumpArgs(const std::vector<uint8_t> &args, std::vector<uint8_t> &vec) -> void {
  DumpUsize(args.size(), vec);
  for (uint8_t b : args) {
    DumpU8(b, vec);
  }
}

inline
auto DumpEvent(const Event &obj, std::vector<uint8_t> &vec) -> void {
  DumpUsize(obj.delta_, vec);
  auto type = obj.type_;
  DumpU8(type, vec);
  const auto &args = obj.args_;
  DumpU8(args[0], vec);
  type &= 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    DumpU8(args[1], vec);
  }
}

inline
auto DumpMeta(const Meta &obj, std::vector<uint8_t> &vec) -> void {
  DumpUsize(obj.delta_, vec);
  DumpU8(0xff, vec);
  DumpU8(obj.type_, vec);
  DumpArgs(obj.args_, vec);
}

inline
auto DumpSysex(const Sysex &obj, std::vector<uint8_t> &vec) -> void {
  DumpUsize(obj.delta_, vec);
  DumpU8(0xf0, vec);
  const auto &items = obj.items_;
  DumpArgs(std::get<1>(items[0]), vec);
  if (items.size() != 1) {
    std::for_each(items.begin() + 1, items.end(), [&vec](const auto &it) {
      DumpUsize(std::get<0>(it), vec);
      DumpU8(0xf7, vec);
      DumpArgs(std::get<1>(it), vec);
    });
  }
}

inline
auto DumpTrack(const Track &obj, std::vector<uint8_t> &vec) -> void {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('r'), vec);
  DumpU8(static_cast<uint8_t>('k'), vec);
  size_t index = vec.size();
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  for (const auto &it : obj.items_) {
    if (auto event = dynamic_cast<const Event *>(it.get()); event != nullptr) {
      DumpEvent(*event, vec);
      continue;
    }
    if (auto meta = dynamic_cast<const Meta *>(it.get()); meta != nullptr) {
      DumpMeta(*meta, vec);
      continue;
    }
    if (auto sysex = dynamic_cast<const Sysex *>(it.get()); sysex != nullptr) {
      DumpSysex(*sysex, vec);
      continue;
    }
    err::raise<err_enum::cast_failure>(FromFormat("dump track but unknown type of message: %" PRIx8, it->type_));
    return;
  }
  std::vector<uint8_t> len;
  DumpU32(vec.size() - index - 4, len);
  for (size_t i = 0; i != 4; ++i) {
    vec[index + i] = len[i];
  }
}

inline
auto DumpMidi(const Midi &obj, std::vector<uint8_t> &vec) -> void {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('h'), vec);
  DumpU8(static_cast<uint8_t>('d'), vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(6, vec);
  DumpU16(obj.format_, vec);
  DumpU16(static_cast<uint16_t>(obj.items_.size()), vec);
  DumpU16(obj.division_, vec);
  for (const auto &it : obj.items_) {
    DumpTrack(*it, vec);
    if (auto e = err::init(); e != nullptr) {
      err::raise<err_enum::invalid_param>("dump midi failure", e);
      return;
    }
  }
}

auto Midi::Dump(file::output func) const -> void {
  auto invoke = func.invoke;
  if (invoke == nullptr) {
    err::raise<err_enum::null_obj>("dump midi but the invoke param is nullptr");
    return;
  }
  defer d = [func]() {
    if (auto close = func.close; close != nullptr) {
      close(func.capture);
    }
  };
  std::vector<uint8_t> vec;
  DumpMidi(*this, vec);
  if (auto e = err::init(); e != nullptr) {
    err::raise<err_enum::io_failure>("dump midi", e);
    return;
  }
  invoke(func.capture, vec.data(), vec.size());
}

} // namespace internal

} // namespace MilkPowder
