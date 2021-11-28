#ifndef LIB_MILKPOWDER_TRACK_H_
#define LIB_MILKPOWDER_TRACK_H_

#ifdef __cplusplus

#include <milkpowder/message.h>
#include <vector>

namespace MilkPowder {

class TrackConstInterface : virtual public TypeConstInterface<TypeSet::Track> {
 public:
  void GetMessages(std::function<void(MessageConstWrapper)> callback) const {
    std::function<void(const RawTypeMap<TypeSet::Message>::target *)> callback_ = [callback](const RawTypeMap<TypeSet::Message>::target *message) -> void {
      callback(MessageConstWrapper(message));
    };
    MilkTea_panic(MilkPowder_Track_GetMessages(Self(), &callback_, MilkTea::ClosureToken<decltype(callback_)>::Invoke));
  }
};

class TrackConstWrapper final : virtual public TrackConstInterface {
  friend class MidiConstInterface;
  TypeConstWrapper(Track, TrackConstWrapper)
};

class TrackInterface : virtual public TypeInterface<TypeSet::Track> {
 public:
  RawType *release() && {
    return Move(std::forward<TypeInterface<TypeSet::Track>>(*this));
  }
};

class TrackWrapper final : virtual public TrackConstInterface, virtual public TrackInterface {
  TypeWrapper(Track, TrackWrapper)
 public:
  explicit TrackWrapper(std::function<bool(uint8_t *)> callback) : TrackWrapper() {
    MilkTea_panic(MilkPowder_Track_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  TrackWrapper(MessageWrapper items[], uint32_t length) : TrackWrapper() {
    size_t size = static_cast<size_t>(length);
    std::vector<RawTypeMap<TypeSet::Message>::target *> vec(size);
    for (size_t i = 0; i != size; ++i) {
      vec[i] = std::forward<MessageWrapper>(items[i]).release();
    }
    MilkTea_panic(MilkPowder_Track_Create(addr(), vec.data(), length));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_TRACK_H_
