#ifndef LIB_MILKPOWDER_MIDI_H_
#define LIB_MILKPOWDER_MIDI_H_

#ifdef __cplusplus

#include <milkpowder/track.h>

namespace MilkPowder {

template<>
class ConstInterface<TypeSet::Midi> {
  using RawType = typename RawTypeMap<TypeSet::Midi>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  uint16_t GetFormat() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetFormat(get(), &result));
    return result;
  }
  uint16_t GetNtrks() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetNtrks(get(), &result));
    return result;
  }
  uint16_t GetDivision() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetDivision(get(), &result));
    return result;
  }
  ConstWrapper<TypeSet::Track> GetTrack(uint16_t index) const {
    const RawTypeMap<TypeSet::Track>::target *result = nullptr;
    MilkTea_panic(MilkPowder_Midi_GetTrack(get(), index, &result));
    return result;
  }
};

using MidiConstWrapper = ConstWrapper<TypeSet::Midi>;

template<>
class MutableInterface<TypeSet::Midi> {
  using RawType = typename RawTypeMap<TypeSet::Midi>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  static MutableWrapper<TypeSet::Midi> Parse(std::function<bool(uint8_t *)> callback) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Midi>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<TypeSet::Midi> Make(uint16_t format, uint16_t division, std::vector<MutableWrapper<TypeSet::Track>> tracks) {
    RawType *self = nullptr;
    uint16_t ntrks = tracks.size();
    std::vector<RawTypeMap<TypeSet::Track>::target *> vec(ntrks);
    for (uint16_t i = 0; i != ntrks; ++i) {
      vec[i] = tracks[i].release();
    }
    MilkTea_panic(RawCreateMap<TypeSet::Midi>::target(&self, format, ntrks, division, vec.data()));
    return self;
  }
};

using MidiMutableWrapper = MutableWrapper<TypeSet::Midi>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MIDI_H_
