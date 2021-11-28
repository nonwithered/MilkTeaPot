#ifndef LIB_MILKPOWDER_MIDI_H_
#define LIB_MILKPOWDER_MIDI_H_

#ifdef __cplusplus

#include <milkpowder/track.h>

namespace MilkPowder {

class MidiConstInterface : virtual public TypeConstInterface<TypeSet::Midi> {
 public:
  uint16_t GetFormat() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetFormat(Self(), &result));
    return result;
  }
  uint16_t GetNtrks() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetNtrks(Self(), &result));
    return result;
  }
  uint16_t GetDivision() const {
    uint16_t result = 0;
    MilkTea_panic(MilkPowder_Midi_GetDivision(Self(), &result));
    return result;
  }
  TrackConstWrapper GetTrack(uint16_t index) const {
    const RawTypeMap<TypeSet::Track>::target *result = nullptr;
    MilkTea_panic(MilkPowder_Midi_GetTrack(Self(), index, &result));
    return TrackConstWrapper(result);
  }
};

class MidiConstWrapper final : virtual public MidiConstInterface {
  TypeConstWrapper(Midi, MidiConstWrapper)
};

class MidiInterface : virtual public TypeInterface<TypeSet::Midi> {
};

class MidiWrapper final : virtual public MidiConstInterface, virtual public MidiInterface {
  TypeWrapper(Midi, MidiWrapper)
 public:
  explicit MidiWrapper(std::function<bool(uint8_t *)> callback) : MidiWrapper() {
    MilkTea_panic(MilkPowder_Midi_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  MidiWrapper(uint16_t format, uint16_t ntrks, uint16_t division, TrackWrapper items[]) : MidiWrapper() {
    size_t size = static_cast<size_t>(ntrks);
    std::vector<RawTypeMap<TypeSet::Track>::target *> vec(size);
    for (size_t i = 0; i != size; ++i) {
      vec[i] = std::forward<TrackWrapper>(items[i]).release();
    }
    MilkTea_panic(MilkPowder_Midi_Create(addr(), format, ntrks, division, vec.data()));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MIDI_H_
