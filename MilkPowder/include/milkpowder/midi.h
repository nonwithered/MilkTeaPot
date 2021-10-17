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
    MilkTea_panic(MilkPowder_Midi_GetFormat(*this, &res));
    return res;
  }
  uint16_t GetNtrks() const {
    uint16_t res = 0;
    MilkTea_panic(MilkPowder_Midi_GetNtrks(*this, &res));
    return res;
  }
  uint16_t GetDivision() const {
    uint16_t res = 0;
    MilkTea_panic(MilkPowder_Midi_GetDivision(*this, &res));
    return res;
  }
  TrackRef GetTrack(uint16_t index) const {
    const MilkPowder_Track_t *res = nullptr;
    MilkTea_panic(MilkPowder_Midi_GetTrack(*this, index, &res));
    return TrackRef(res);
  }
};
class Midi final : public MilkPowder_Holder(Midi) {
 public:
  Midi() {}
  explicit Midi(const MidiRef &ref) : MilkPowder_Holder(Midi)(ref) {}
  explicit Midi(std::function<bool(uint8_t *)> callback) {
    MilkTea_panic(MilkPowder_Midi_Parse(&*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
  Midi(uint16_t format, uint16_t ntrks, uint16_t division, Track items[]) {
    auto size = static_cast<size_t>(ntrks);
    std::vector<MilkPowder_Track_t *> vec(size);
    for (auto i = 0; i != size; ++i) {
      vec[i] = items[i].release();
    }
    MilkTea_panic(MilkPowder_Midi_Create(&*this, format, ntrks, division, vec.data()));
  }
  uint16_t GetFormat() const {
    return MidiRef(*this).GetFormat();
  }
  uint16_t GetNtrks() const {
    return MidiRef(*this).GetNtrks();
  }
  uint16_t GetDivision() const {
    return MidiRef(*this).GetDivision();
  }
  TrackRef GetTrack(uint16_t index) const {
    return MidiRef(*this).GetTrack(index);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MIDI_H_
