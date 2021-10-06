#include "midi.h"
#include "track.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

void DumpU8(uint8_t n, std::vector<uint8_t> &vec) {
  vec.push_back(n);
}

void DumpU16(uint16_t n, std::vector<uint8_t> &vec) {
  DumpU8(static_cast<uint8_t>(n >> 010), vec);
  DumpU8(static_cast<uint8_t>(n & 0xff), vec);
}

void DumpU32(uint32_t n, std::vector<uint8_t> &vec) {
  DumpU16(static_cast<uint16_t>(n >> 020), vec);
  DumpU16(static_cast<uint16_t>(n & 0xffff), vec);
}

void DumpUsize(uint32_t n, std::vector<uint8_t> &vec) {
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

void DumpArgs(const std::vector<uint8_t> &args, std::vector<uint8_t> &vec) {
  DumpUsize(static_cast<uint32_t>(args.size()), vec);
  for (uint8_t b : args) {
    DumpU8(b, vec);
  }
}

} // namespace

namespace MilkPowder {
    
void Midi::Dump(std::vector<uint8_t> &vec) const {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('h'), vec);
  DumpU8(static_cast<uint8_t>('d'), vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(6, vec);
  DumpU16(format_, vec);
  DumpU16(static_cast<uint16_t>(items_.size()), vec);
  DumpU16(division_, vec);
  for (const std::unique_ptr<Track> &it : items_) {
    it->Dump(vec);
  }
}

void Track::Dump(std::vector<uint8_t> &vec) const {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('r'), vec);
  DumpU8(static_cast<uint8_t>('k'), vec);
  size_t index = vec.size();
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  for (const std::unique_ptr<Message> &it : items_) {
    it->Dump(vec);
  }
  std::vector<uint8_t> len;
  DumpU32(static_cast<uint32_t>(vec.size() - index - 4), len);
  for (size_t i = 0; i != 4; ++i) {
    vec[index + i] = len[i];
  }
}

void Event::Dump(std::vector<uint8_t> &vec) const {
  DumpUsize(Message::delta(), vec);
  uint8_t type = Message::type();
  DumpU8(type, vec);
  DumpU8(std::get<0>(args_), vec);
  type &= 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    DumpU8(std::get<1>(args_), vec);
  }
}

void Meta::Dump(std::vector<uint8_t> &vec) const {
  DumpUsize(Message::delta(), vec);
  DumpU8(0xff, vec);
  DumpU8(type_, vec);
  DumpArgs(args_, vec);
}

void Sysex::Dump(std::vector<uint8_t> &vec) const {
  DumpUsize(Message::delta(), vec);
  DumpU8(0xf0, vec);
  std::vector<std::tuple<uint32_t, std::vector<uint8_t>>>::const_iterator itr = items_.begin();
  DumpArgs(std::get<1>(*itr++), vec);
  for (; itr != items_.end(); ++itr) {
    DumpUsize(static_cast<uint32_t>(std::get<0>(*itr)), vec);
    DumpU8(0xf7, vec);
    DumpArgs(std::get<1>(*itr), vec);
  }
}

} // namespace MilkPowder
