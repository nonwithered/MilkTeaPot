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
  static MutableWrapper<Mapping::Midi> Parse(std::function<size_t(uint8_t [], size_t)> reader) {
    MilkPowder::Mapping::ByteReader reader_ = reader;
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &obj, reader_);
    return obj;
  }
  template<typename iterator_type>
  static MutableWrapper<Mapping::Midi> Make(uint16_t format, uint16_t ntrk, uint16_t division, iterator_type iterator) {
    raw_type *obj = nullptr;
    std::vector<Mapping::Track::raw_type *> vec(ntrk);
    for (uint16_t i = 0; i != ntrk; ++i) {
      vec[i] = iterator->release();
      ++iterator;
    }
    MilkTea_invoke_panic(mapping::raw_create, &obj, format, ntrk, division, vec.data());
    return obj;
  }
  void AllTrack(std::function<void(MutableWrapper<Mapping::Track> &)> consumer) {
    std::function<void(Mapping::Track::raw_type *)> consumer_ = [&consumer](Mapping::Track::raw_type *it) {
      MutableWrapper<Mapping::Track> it_ = it;
      MilkTea::Defer defer([&it_]() {
        it_.release();
      });
      consumer(it_);
    };
    MilkTea_invoke_panic(mapping::raw_all_track, get(), MilkTea::FunctionAdapter::ToRawType<Mapping::Track::raw_consumer_type>(consumer_));
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
