#ifndef LIB_MILKPOWDER_TRACK_H_
#define LIB_MILKPOWDER_TRACK_H_

#ifdef __cplusplus
#include <milkpowder/message.h>
#include <vector>
namespace MilkPowder {
class TrackRef final : public MilkPowder_HolderRef(Track) {
 public:
  explicit TrackRef(const MilkPowder_Track_t *ref) : MilkPowder_HolderRef(Track)(ref) {}
  void GetMessages(std::function<void(MessageRef)> callback) const {
    std::function<void(const MilkPowder_Message_t *)> callback_ = [callback](const MilkPowder_Message_t *obj) -> void {
      callback(MessageRef(obj));
    };
    MilkTea_panic(MilkPowder_Track_GetMessages(get(), &callback_, MilkTea::CallbackToken<decltype(callback_)>::Invoke));
  }
};
class Track final : public MilkPowder_Holder(Track) {
 public:
  explicit Track(MilkPowder_Track_t *ptr = nullptr) : MilkPowder_Holder(Track)(ptr) {}
  explicit Track(const TrackRef &ref) : MilkPowder_Holder(Track)(ref) {}
  explicit Track(std::function<bool(uint8_t *)> callback) : Track() {
    MilkTea_panic(MilkPowder_Track_Parse(reset(), &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
  Track(Message items[], uint32_t length) : Track() {
    size_t size = static_cast<size_t>(length);
    std::vector<MilkPowder_Message_t *> vec(size);
    for (size_t i = 0; i != size; ++i) {
      vec[i] = items[i].release();
    }
    MilkTea_panic(MilkPowder_Track_Create(reset(), vec.data(), length));
  }
  void GetMessages(std::function<void(MessageRef)> callback) const {
    TrackRef(get()).GetMessages(callback);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_TRACK_H_
