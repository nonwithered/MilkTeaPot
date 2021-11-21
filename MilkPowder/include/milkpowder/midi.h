#ifndef LIB_MILKPOWDER_MIDI_H_
#define LIB_MILKPOWDER_MIDI_H_

#ifdef __cplusplus
#include <milkpowder/track.h>
namespace MilkPowder {
class MidiRef final : public MilkPowder_HolderRef(Midi) {
 public:
  explicit MidiRef(const MilkPowder_Midi_t *ref) : MilkPowder_HolderRef(Midi)(ref) {}
  uint16_t GetFormat() const {
    uint16_t res = 0;
    MilkTea_panic(MilkPowder_Midi_GetFormat(get(), &res));
    return res;
  }
  uint16_t GetNtrks() const {
    uint16_t res = 0;
    MilkTea_panic(MilkPowder_Midi_GetNtrks(get(), &res));
    return res;
  }
  uint16_t GetDivision() const {
    uint16_t res = 0;
    MilkTea_panic(MilkPowder_Midi_GetDivision(get(), &res));
    return res;
  }
  TrackRef GetTrack(uint16_t index) const {
    const MilkPowder_Track_t *res = nullptr;
    MilkTea_panic(MilkPowder_Midi_GetTrack(get(), index, &res));
    return TrackRef(res);
  }
};
class Midi final : public MilkPowder_Holder(Midi) {
 public:
  explicit Midi(MilkPowder_Midi_t *ptr = nullptr) : MilkPowder_Holder(Midi)(ptr) {}
  explicit Midi(const MidiRef &ref) : MilkPowder_Holder(Midi)(ref) {}
  explicit Midi(std::function<bool(uint8_t *)> callback) : Midi() {
    MilkTea_panic(MilkPowder_Midi_Parse(reset(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  Midi(uint16_t format, uint16_t ntrks, uint16_t division, Track items[]) : Midi() {
    size_t size = static_cast<size_t>(ntrks);
    std::vector<MilkPowder_Track_t *> vec(size);
    for (size_t i = 0; i != size; ++i) {
      vec[i] = items[i].release();
    }
    MilkTea_panic(MilkPowder_Midi_Create(reset(), format, ntrks, division, vec.data()));
  }
  uint16_t GetFormat() const {
    return MidiRef(get()).GetFormat();
  }
  uint16_t GetNtrks() const {
    return MidiRef(get()).GetNtrks();
  }
  uint16_t GetDivision() const {
    return MidiRef(get()).GetDivision();
  }
  TrackRef GetTrack(uint16_t index) const {
    return MidiRef(get()).GetTrack(index);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MIDI_H_
