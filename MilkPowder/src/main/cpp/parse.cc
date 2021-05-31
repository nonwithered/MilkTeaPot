#include "midi.h"
#include "track.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

inline uint8_t ParseU8(const uint8_t *&begin, const uint8_t *const end) {
  if (begin >= end) {
    throw MilkPowder::Except(MilkPowder::Except::Type::EndOfFile);
  }
  return *begin++;
}

inline uint16_t ParseU16(const uint8_t *&begin, const uint8_t *const end) {
  uint16_t h = ParseU8(begin, end);
  uint16_t l = ParseU8(begin, end);
  return static_cast<uint16_t>((h << 010) | l);
}

inline uint32_t ParseU32(const uint8_t *&begin, const uint8_t *const end) {
  uint32_t h = ParseU16(begin, end);
  uint32_t l = ParseU16(begin, end);
  return static_cast<uint32_t>((h << 020) | l);
}

uint32_t ParseUsize(const uint8_t *&begin, const uint8_t *const end) {
  uint8_t arr[4], last;
  size_t len = 0;
  arr[len++] = last = ParseU8(begin, end);
  while (len < 4 && last >= 0x80) {
    arr[len++] = last = ParseU8(begin, end);
  }
  if (len == 4 && last >= 0x80) {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
  uint32_t ret = arr[0] & 0x7f;
  for (size_t i = 1; i != len; ++i) {
    ret <<= 07;
    ret |= arr[i] & 0x7f;
  }
  return ret;
}

std::vector<uint8_t> ParseArgs(const uint8_t *&begin, const uint8_t *const end) {
  uint32_t capacity = ParseUsize(begin, end);
  const uint8_t *position = begin + capacity;
  if (end < position) {
    throw MilkPowder::Except(MilkPowder::Except::Type::EndOfFile);
  }
  std::vector<uint8_t> args(capacity);
  for (size_t i = 0; i != capacity; ++i) {
    args.emplace_back(ParseU8(begin, position));
  }
  return args;
}

std::unique_ptr<MilkPowder::Event> ParseEvent(const uint8_t *&begin, const uint8_t *const end, uint32_t delta, uint8_t type) {
  if (type < 0x80 || type >= 0xf0) {
    type = ParseU8(begin, end);
    if (type < 0x80 || type >= 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  std::tuple<uint8_t, uint8_t> args = std::make_tuple(ParseU8(begin, end), 0);
  if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
    std::get<1>(args) = ParseU8(begin, end);
  }
  return std::unique_ptr<MilkPowder::Event>(new MilkPowder::Event(delta, type, args));
}

std::unique_ptr<MilkPowder::Meta> ParseMeta(const uint8_t *&begin, const uint8_t *const end, uint32_t delta, uint8_t type) {
  if (type != 0xff) {
    type = ParseU8(begin, end);
    if (type != 0xff) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  type = ParseU8(begin, end);
  if (type >= 0x80) {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
  std::vector<uint8_t> args = ParseArgs(begin, end);
  return std::unique_ptr<MilkPowder::Meta>(new MilkPowder::Meta(delta, type, std::move(args)));
}

std::unique_ptr<MilkPowder::Sysex> ParseSysex(const uint8_t *&begin, const uint8_t *const end, uint32_t delta, uint8_t type) {
  if (type != 0xf0) {
    type = ParseU8(begin, end);
    if (type != 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  std::vector<uint8_t> args = ParseArgs(begin, end);
  uint8_t last = args.back();
  std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items;
  items.emplace_back(delta, std::move(args));
  while (last != 0xf7) {
    delta = ParseUsize(begin, end);
    type = ParseU8(begin, end);
    if (type != 0xf7) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
    args = ParseArgs(begin, end);
    last = args.back();
    items.emplace_back(delta, std::move(args));
  }
  return std::unique_ptr<MilkPowder::Sysex>(new MilkPowder::Sysex(std::move(items)));
}

} // namespace

namespace MilkPowder {

std::unique_ptr<Midi> Midi::Parse(const uint8_t *&begin, const uint8_t *const end) {
  if (static_cast<uint8_t>('M') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('T') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('h') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('d') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint32_t>(6) != ParseU32(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  uint16_t format = ParseU16(begin, end);
  uint16_t ntrks = ParseU16(begin, end);
  uint16_t division = ParseU16(begin, end);
  std::vector<std::unique_ptr<Track>> items;
  for (uint16_t i = 0; i != ntrks; ++i) {
    items.emplace_back(Track::Parse(begin, end));
  }
  return std::unique_ptr<MilkPowder::Midi>(new MilkPowder::Midi(format, division, std::move(items)));
}
  
std::unique_ptr<Track> Track::Parse(const uint8_t *&begin, const uint8_t *const end) {
  if (static_cast<uint8_t>('M') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('T') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('r') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('k') != ParseU8(begin, end)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  uint32_t capacity = ParseU32(begin, end);
  const uint8_t *position = begin + capacity;
  if (end < position) {
    throw MilkPowder::Except(MilkPowder::Except::Type::EndOfFile);
  }
  std::vector<std::unique_ptr<Message>> items;
  uint8_t last = 0;
  while (begin < position) {
    std::unique_ptr<Message> it = Message::Parse(begin, position, last);
    last = it->type();
    items.emplace_back(std::move(it));
  }
  return std::unique_ptr<MilkPowder::Track>(new MilkPowder::Track(std::move(items)));
}

std::unique_ptr<Message> Message::Parse(const uint8_t *&begin, const uint8_t *const end, uint8_t last) {
  uint32_t delta = ParseUsize(begin, end);
  uint8_t type = ParseU8(begin, end);
  if (type >= 0x80 && type < 0xf0) {
    return ParseEvent(begin, end, delta, type);
  } else if (type == 0xff) {
    return ParseMeta(begin, end, delta, 0xff);
  } else if (type == 0xf0) {
    return ParseSysex(begin, end, delta, 0xf0);
  } else if (type < 0x80) {
    if (last < 0x80 || last >= 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
    std::tuple<uint8_t, uint8_t> args = std::make_tuple(type, 0);
    type = last;
    if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
      std::get<1>(args) = ParseU8(begin, end);
    }
    return std::unique_ptr<MilkPowder::Event>(new MilkPowder::Event(delta, type, args));
  } else {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
}
  
std::unique_ptr<Event> Event::Parse(const uint8_t *&begin, const uint8_t *const end, uint8_t last) {
  uint32_t delta = ParseUsize(begin, end);
  return ParseEvent(begin, end, delta, last);
}
  
std::unique_ptr<Meta> Meta::Parse(const uint8_t *&begin, const uint8_t *const end) {
  uint32_t delta = ParseUsize(begin, end);
  return ParseMeta(begin, end, delta, 0);
}
  
std::unique_ptr<Sysex> Sysex::Parse(const uint8_t *&begin, const uint8_t *const end) {
  uint32_t delta = ParseUsize(begin, end);
  return ParseSysex(begin, end, delta, 0);
}

} // namespace MilkPowder
