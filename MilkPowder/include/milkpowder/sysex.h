#ifndef LIB_MILKPOWDER_SYSEX_H_
#define LIB_MILKPOWDER_SYSEX_H_

#ifdef __cplusplus
#include <milkpowder/holder.h>
#include <functional>
namespace MilkPowder {
class SysexRef final : public MilkPowder_HolderRef(Sysex) {
 public:
  explicit SysexRef(const MilkPowder_Sysex_t *ref) : MilkPowder_HolderRef(Sysex)(ref) {}
  void GetArgs(std::function<void(uint32_t, const uint8_t *, uint32_t)> callback) const {
    MilkTea_panic(MilkPowder_Sysex_GetArgs(*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
};
class Sysex final : public MilkPowder_Holder(Sysex) {
 public:
  Sysex() {}
  explicit Sysex(const SysexRef &ref) : MilkPowder_Holder(Sysex)(ref) {}
  explicit Sysex(std::function<bool(uint8_t *)> callback) {
    MilkTea_panic(MilkPowder_Sysex_Parse(&*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
  Sysex(uint32_t delta[], const uint8_t *const args[], uint32_t const length[], uint32_t size) {
    MilkTea_panic(MilkPowder_Sysex_Create(&*this, delta, args, length, size));
  }
  void GetArgs(std::function<void(uint32_t, const uint8_t *, uint32_t)> callback) const {
    SysexRef(*this).GetArgs(callback);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_SYSEX_H_
