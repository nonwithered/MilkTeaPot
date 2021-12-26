#ifndef LIB_MILKPOWDER_WRAPPER_TRACK_H_
#define LIB_MILKPOWDER_WRAPPER_TRACK_H_

#include <vector>

#include <milkpowder/wrapper/message.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Track> {
  using mapping = Mapping::Track;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  uint32_t GetCount() const {
    uint32_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_count, get(), &result);
    return result;
  }
  ConstWrapper<Mapping::Message> GetMessage(uint32_t index) const {
    const Mapping::Message::raw_type *result = nullptr;
    MilkTea_invoke_panic(MilkPowder_Track_GetMessage, get(), index, &result);
    return result;
  }
};

using TrackConstWrapper = ConstWrapper<Mapping::Track>;

template<>
class MutableInterface<Mapping::Track> {
  using mapping = Mapping::Track;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<Mapping::Track> Parse(std::function<bool(uint8_t *)> reader) {
    raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &self, Mapping::Reader(reader));
    return self;
  }
  static MutableWrapper<Mapping::Track> Make(std::vector<MutableWrapper<Mapping::Message>> messages) {
    raw_type *self = nullptr;
    uint32_t length = messages.size();
    std::vector<Mapping::Message::raw_type *> vec(length);
    for (uint32_t i = 0; i != length; ++i) {
      vec[i] = messages[i].release();
    }
    MilkTea_invoke_panic(mapping::raw_create, &self, vec.data(), length);
    return self;
  }
  void AllMessage(std::function<void(MutableWrapper<Mapping::Message> &)> consumer) {
    std::function<void(Mapping::Message::raw_type *)> consumer_ = [&consumer](Mapping::Message::raw_type *it) {
      MutableWrapper<Mapping::Message> it_ = it;
      MilkTea::Defer defer([&it_]() {
        it_.release();
      });
      consumer(it_);
    };
    MilkTea_invoke_panic(mapping::raw_all_message, get(), MilkTea::FunctionFactory<decltype(consumer_)>::ToRawType<Mapping::Message::raw_consumer_type>(consumer_));
  }
};

using TrackMutableWrapper = MutableWrapper<Mapping::Track>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_TRACK_H_
