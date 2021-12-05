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
  std::vector<ConstWrapper<Mapping::Message>> GetMessages() const {
    std::vector<ConstWrapper<Mapping::Message>> result;
    std::function<void(const Mapping::Message::raw_type *)> callback = [&result](const Mapping::Message::raw_type *message) -> void {
      result.emplace_back(message);
    };
    MilkTea_panic(mapping::raw_get_messages(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
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
  static MutableWrapper<Mapping::Track> Parse(std::function<bool(uint8_t *)> callback) {
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_parse(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<Mapping::Track> Make(std::vector<MutableWrapper<Mapping::Message>> messages) {
    raw_type *self = nullptr;
    uint32_t length = messages.size();
    std::vector<Mapping::Message::raw_type *> vec(length);
    for (uint32_t i = 0; i != length; ++i) {
      vec[i] = messages[i].release();
    }
    MilkTea_panic(mapping::raw_create(&self, vec.data(), length));
    return self;
  }
};

using TrackMutableWrapper = MutableWrapper<Mapping::Track>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_TRACK_H_
