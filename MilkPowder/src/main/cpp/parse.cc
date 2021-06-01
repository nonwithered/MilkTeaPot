#include "midi.h"
#include "track.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

inline uint8_t ParseU8(std::function<std::tuple<uint8_t, bool>()> callback) {
  auto ret = callback();
  if (!std::get<1>(ret)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::EndOfFile);
  }
  return std::get<0>(ret);
}

inline uint16_t ParseU16(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint16_t h = ParseU8(callback);
  uint16_t l = ParseU8(callback);
  return static_cast<uint16_t>((h << 010) | l);
}

inline uint32_t ParseU32(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint32_t h = ParseU16(callback);
  uint32_t l = ParseU16(callback);
  return static_cast<uint32_t>((h << 020) | l);
}

uint32_t ParseUsize(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint8_t arr[4], last;
  size_t len = 0;
  arr[len++] = last = ParseU8(callback);
  while (len < 4 && last >= 0x80) {
    arr[len++] = last = ParseU8(callback);
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

std::vector<uint8_t> ParseArgs(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint32_t capacity = ParseUsize(callback);
  std::vector<uint8_t> args(capacity);
  for (size_t i = 0; i != capacity; ++i) {
    args.emplace_back(ParseU8(callback));
  }
  return args;
}

std::unique_ptr<MilkPowder::Event> ParseEvent(std::function<std::tuple<uint8_t, bool>()> callback, uint32_t delta, uint8_t type) {
  if (type < 0x80 || type >= 0xf0) {
    type = ParseU8(callback);
    if (type < 0x80 || type >= 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  std::tuple<uint8_t, uint8_t> args = std::make_tuple(ParseU8(callback), 0);
  if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
    std::get<1>(args) = ParseU8(callback);
  }
  return std::unique_ptr<MilkPowder::Event>(new MilkPowder::Event(delta, type, args));
}

std::unique_ptr<MilkPowder::Meta> ParseMeta(std::function<std::tuple<uint8_t, bool>()> callback, uint32_t delta, uint8_t type) {
  if (type != 0xff) {
    type = ParseU8(callback);
    if (type != 0xff) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  type = ParseU8(callback);
  if (type >= 0x80) {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
  std::vector<uint8_t> args = ParseArgs(callback);
  return std::unique_ptr<MilkPowder::Meta>(new MilkPowder::Meta(delta, type, std::move(args)));
}

std::unique_ptr<MilkPowder::Sysex> ParseSysex(std::function<std::tuple<uint8_t, bool>()> callback, uint32_t delta, uint8_t type) {
  if (type != 0xf0) {
    type = ParseU8(callback);
    if (type != 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
  }
  std::vector<uint8_t> args = ParseArgs(callback);
  uint8_t last = args.back();
  std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items;
  items.emplace_back(delta, std::move(args));
  while (last != 0xf7) {
    delta = ParseUsize(callback);
    type = ParseU8(callback);
    if (type != 0xf7) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
    args = ParseArgs(callback);
    last = args.back();
    items.emplace_back(delta, std::move(args));
  }
  return std::unique_ptr<MilkPowder::Sysex>(new MilkPowder::Sysex(std::move(items)));
}

} // namespace

namespace MilkPowder {

std::unique_ptr<Midi> Midi::Parse(std::function<std::tuple<uint8_t, bool>()> callback) {
  if (static_cast<uint8_t>('M') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('T') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('h') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('d') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint32_t>(6) != ParseU32(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  uint16_t format = ParseU16(callback);
  uint16_t ntrks = ParseU16(callback);
  uint16_t division = ParseU16(callback);
  std::vector<std::unique_ptr<Track>> items;
  for (uint16_t i = 0; i != ntrks; ++i) {
    items.emplace_back(Track::Parse(callback));
  }
  return std::unique_ptr<MilkPowder::Midi>(new MilkPowder::Midi(format, division, std::move(items)));
}
  
std::unique_ptr<Track> Track::Parse(std::function<std::tuple<uint8_t, bool>()> callback) {
  if (static_cast<uint8_t>('M') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('T') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('r') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  if (static_cast<uint8_t>('k') != ParseU8(callback)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::Unsupported);
  }
  uint32_t capacity = ParseU32(callback);
  std::function<std::tuple<uint8_t, bool>()> func = [callback, &capacity]() -> std::tuple<uint8_t, bool> {
    if (capacity-- == 0) {
      return std::make_tuple(0, false);
    }
    return callback();
  };
  std::vector<std::unique_ptr<Message>> items;
  uint8_t last = 0;
  while (capacity != 0) {
    std::unique_ptr<Message> it = Message::Parse(func, last);
    last = it->type();
    items.emplace_back(std::move(it));
  }
  return std::unique_ptr<MilkPowder::Track>(new MilkPowder::Track(std::move(items)));
}

std::unique_ptr<Message> Message::Parse(std::function<std::tuple<uint8_t, bool>()> callback, uint8_t last) {
  uint32_t delta = ParseUsize(callback);
  uint8_t type = ParseU8(callback);
  if (type >= 0x80 && type < 0xf0) {
    return ParseEvent(callback, delta, type);
  } else if (type == 0xff) {
    return ParseMeta(callback, delta, 0xff);
  } else if (type == 0xf0) {
    return ParseSysex(callback, delta, 0xf0);
  } else if (type < 0x80) {
    if (last < 0x80 || last >= 0xf0) {
      throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
    }
    std::tuple<uint8_t, uint8_t> args = std::make_tuple(type, 0);
    type = last;
    if ((type & 0xf0) != 0xc0 && (type & 0xf0) != 0xd0) {
      std::get<1>(args) = ParseU8(callback);
    }
    return std::unique_ptr<MilkPowder::Event>(new MilkPowder::Event(delta, type, args));
  } else {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
}
  
std::unique_ptr<Event> Event::Parse(std::function<std::tuple<uint8_t, bool>()> callback, uint8_t last) {
  uint32_t delta = ParseUsize(callback);
  return ParseEvent(callback, delta, last);
}
  
std::unique_ptr<Meta> Meta::Parse(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint32_t delta = ParseUsize(callback);
  return ParseMeta(callback, delta, 0);
}
  
std::unique_ptr<Sysex> Sysex::Parse(std::function<std::tuple<uint8_t, bool>()> callback) {
  uint32_t delta = ParseUsize(callback);
  return ParseSysex(callback, delta, 0);
}

} // namespace MilkPowder
