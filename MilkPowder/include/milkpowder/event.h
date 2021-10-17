#ifndef LIB_MILKPOWDER_EVENT_H_
#define LIB_MILKPOWDER_EVENT_H_

#ifdef __cplusplus
#include <milkpowder/holder.h>
#include <functional>
#include <tuple>
namespace MilkPowder {
class EventRef final : public MilkPowder_HolderRef(Event) {
 public:
  explicit EventRef(const MilkPowder_Event_t *ref) : MilkPowder_HolderRef(Event)(ref) {}
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Event_GetType(*this, &res));
    return res;
  }
  std::tuple<uint8_t, uint8_t> GetArgs() const {
    uint8_t res[2] = {0};
    MilkTea_panic(MilkPowder_Event_GetArgs(*this, res));
    return std::make_tuple(res[0], res[1]);
  }
};
class Event final : public MilkPowder_Holder(Event) {
 public:
  Event() {}
  explicit Event(const EventRef &ref) : MilkPowder_Holder(Event)(ref) {}
  explicit Event(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) {
    MilkTea_panic(MilkPowder_Event_Parse(&*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke, last));
  }
  Event(uint32_t delta, uint8_t type, std::tuple<uint8_t, uint8_t> args) {
    MilkTea_panic(MilkPowder_Event_Create(&*this, delta, type, std::get<0>(args), std::get<1>(args)));
  }
  uint8_t GetType() const {
    return EventRef(*this).GetType();
  }
  std::tuple<uint8_t, uint8_t> GetArgs() const {
    return EventRef(*this).GetArgs();
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_EVENT_H_
