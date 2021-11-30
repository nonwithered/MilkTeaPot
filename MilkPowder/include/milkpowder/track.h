#ifndef LIB_MILKPOWDER_TRACK_H_
#define LIB_MILKPOWDER_TRACK_H_

#ifdef __cplusplus

#include <milkpowder/message.h>
#include <vector>

namespace MilkPowder {

template<>
class ConstInterface<TypeSet::Track> {
  using RawType = typename RawTypeMap<TypeSet::Track>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  std::vector<ConstWrapper<TypeSet::Message>> GetMessages() const {
    std::vector<ConstWrapper<TypeSet::Message>> result;
    std::function<void(const RawTypeMap<TypeSet::Message>::target *)> callback = [&result](const RawTypeMap<TypeSet::Message>::target *message) -> void {
      result.emplace_back(message);
    };
    MilkTea_panic(MilkPowder_Track_GetMessages(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return result;
  }
};

using TrackConstWrapper = ConstWrapper<TypeSet::Track>;

template<>
class MutableInterface<TypeSet::Track> {
  using RawType = typename RawTypeMap<TypeSet::Track>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  static MutableWrapper<TypeSet::Track> Parse(std::function<bool(uint8_t *)> callback) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Track>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<TypeSet::Track> Make(std::vector<MutableWrapper<TypeSet::Message>> messages) {
    RawType *self = nullptr;
    uint32_t length = messages.size();
    std::vector<RawTypeMap<TypeSet::Message>::target *> vec(length);
    for (uint32_t i = 0; i != length; ++i) {
      vec[i] = messages[i].release();
    }
    MilkTea_panic(RawCreateMap<TypeSet::Track>::target(&self, vec.data(), length));
    return self;
  }
};

using TrackMutableWrapper = MutableWrapper<TypeSet::Track>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_TRACK_H_
