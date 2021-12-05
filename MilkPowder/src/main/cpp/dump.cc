#include <milktea.h>

#include "midi.h"
#include "track.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace MilkPowder {

constexpr char TAG[] = "MilkPowder::Dump";

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

void DumpEvent(const EventImpl &self, std::vector<uint8_t> &vec) {
  DumpUsize(self.delta(), vec);
  auto type = self.type();
  DumpU8(type, vec);
  const auto &args = self.args();
  DumpU8(std::get<0>(args), vec);
  type &= 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    DumpU8(std::get<1>(args), vec);
  }
}

void DumpMeta(const MetaImpl &self, std::vector<uint8_t> &vec) {
  DumpUsize(self.delta(), vec);
  DumpU8(0xff, vec);
  DumpU8(self.type(), vec);
  DumpArgs(self.args(), vec);
}

void DumpSysex(const SysexImpl &self, std::vector<uint8_t> &vec) {
  DumpUsize(self.delta(), vec);
  DumpU8(0xf0, vec);
  const auto &items = self.items();
  DumpArgs(std::get<1>(items[0]), vec);
  if (items.size() != 1) {
    std::for_each(items.begin() + 1, items.end(), [&vec](const auto &it) {
      DumpUsize(static_cast<uint32_t>(std::get<0>(it)), vec);
      DumpU8(0xf7, vec);
      DumpArgs(std::get<1>(it), vec);
    });
  }
}

void DumpTrack(const TrackImpl &self, std::vector<uint8_t> &vec) {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('r'), vec);
  DumpU8(static_cast<uint8_t>('k'), vec);
  size_t index = vec.size();
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  for (const auto &it : self.items()) {
    if (it->IsEvent()) {
      DumpEvent(dynamic_cast<const EventImpl &>(*it), vec);
    } else if (it->IsMeta()) {
      DumpMeta(dynamic_cast<const MetaImpl &>(*it), vec);
    } else if (it->IsSysex()) {
      DumpSysex(dynamic_cast<const SysexImpl &>(*it), vec);
    } else {
      MilkTea_assertf("DumpTrack type -- %" PRIx8, it->type());
    }
  }
  std::vector<uint8_t> len;
  DumpU32(static_cast<uint32_t>(vec.size() - index - 4), len);
  for (size_t i = 0; i != 4; ++i) {
    vec[index + i] = len[i];
  }
}

void DumpMidi(const MidiImpl &self, std::vector<uint8_t> &vec) {
  DumpU8(static_cast<uint8_t>('M'), vec);
  DumpU8(static_cast<uint8_t>('T'), vec);
  DumpU8(static_cast<uint8_t>('h'), vec);
  DumpU8(static_cast<uint8_t>('d'), vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(0, vec);
  DumpU8(6, vec);
  DumpU16(self.format(), vec);
  DumpU16(static_cast<uint16_t>(self.items().size()), vec);
  DumpU16(self.division(), vec);
  for (const std::unique_ptr<TrackImpl> &it : self.items()) {
    DumpTrack(*it, vec);
  }
}
    
std::vector<uint8_t> MidiImpl::Dump() const {
  std::vector<uint8_t> vec;
  DumpMidi(*this, vec);
  return vec;
}

std::vector<uint8_t> TrackImpl::Dump() const {
  std::vector<uint8_t> vec;
  DumpTrack(*this, vec);
  return vec;
}

std::vector<uint8_t> EventImpl::Dump() const {
  std::vector<uint8_t> vec;
  DumpEvent(*this, vec);
  return vec;
}

std::vector<uint8_t> MetaImpl::Dump() const {
  std::vector<uint8_t> vec;
  DumpMeta(*this, vec);
  return vec;
}

std::vector<uint8_t> SysexImpl::Dump() const {
  std::vector<uint8_t> vec;
  DumpSysex(*this, vec);
  return vec;
}

} // namespace MilkPowder
