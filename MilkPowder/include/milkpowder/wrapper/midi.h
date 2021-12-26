#ifndef LIB_MILKPOWDER_WRAPPER_MIDI_H_
#define LIB_MILKPOWDER_WRAPPER_MIDI_H_

#include <milkpowder/wrapper/track.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Midi> {
  using mapping = Mapping::Midi;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  uint16_t GetFormat() const {
    uint16_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_format, get(), &result);
    return result;
  }
  uint16_t GetNtrks() const {
    uint16_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_ntrks, get(), &result);
    return result;
  }
  uint16_t GetDivision() const {
    uint16_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_division, get(), &result);
    return result;
  }
  ConstWrapper<Mapping::Track> GetTrack(uint16_t index) const {
    const Mapping::Track::raw_type *result = nullptr;
    MilkTea_invoke_panic(mapping::raw_get_track, get(), index, &result);
    return result;
  }
};

using MidiConstWrapper = ConstWrapper<Mapping::Midi>;

template<>
class MutableInterface<Mapping::Midi> {
  using mapping = Mapping::Midi;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<Mapping::Midi> Parse(std::function<bool(uint8_t *)> reader) {
    raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &self, Mapping::Reader(reader));
    return self;
  }
  static MutableWrapper<Mapping::Midi> Make(uint16_t format, uint16_t division, std::vector<MutableWrapper<Mapping::Track>> tracks) {
    raw_type *self = nullptr;
    uint16_t ntrks = tracks.size();
    std::vector<Mapping::Track::raw_type *> vec(ntrks);
    for (uint16_t i = 0; i != ntrks; ++i) {
      vec[i] = tracks[i].release();
    }
    MilkTea_invoke_panic(mapping::raw_create, &self, format, ntrks, division, vec.data());
    return self;
  }
  void AllTrack(std::function<void(MutableWrapper<Mapping::Track> &)> consumer) {
    std::function<void(Mapping::Track::raw_type *)> consumer_ = [&consumer](Mapping::Track::raw_type *it) {
      MutableWrapper<Mapping::Track> it_ = it;
      MilkTea::Defer defer([&it_]() {
        it_.release();
      });
      consumer(it_);
    };
    MilkTea_invoke_panic(mapping::raw_all_track, get(), MilkTea::FunctionFactory<decltype(consumer_)>::ToRawType<Mapping::Track::raw_consumer_type>(consumer_));
  }
  void SetFormat(uint16_t format) {
    MilkTea_invoke_panic(mapping::raw_set_format, get(), format);
  }
  void SetDivision(uint16_t division) {
    MilkTea_invoke_panic(mapping::raw_set_division, get(), division);
  }
};

using MidiMutableWrapper = MutableWrapper<Mapping::Midi>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_MIDI_H_
