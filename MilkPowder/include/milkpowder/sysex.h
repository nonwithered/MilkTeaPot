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
    MilkTea_panic(MilkPowder_Sysex_GetArgs(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
};
class Sysex final : public MilkPowder_Holder(Sysex) {
 public:
  explicit Sysex(MilkPowder_Sysex_t *ptr = nullptr) : MilkPowder_Holder(Sysex)(ptr) {}
  explicit Sysex(const SysexRef &ref) : MilkPowder_Holder(Sysex)(ref) {}
  explicit Sysex(std::function<bool(uint8_t *)> callback) : Sysex() {
    MilkTea_panic(MilkPowder_Sysex_Parse(reset(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  Sysex(uint32_t delta[], const uint8_t *const args[], uint32_t const length[], uint32_t size) : Sysex() {
    MilkTea_panic(MilkPowder_Sysex_Create(reset(), delta, args, length, size));
  }
  void GetArgs(std::function<void(uint32_t, const uint8_t *, uint32_t)> callback) const {
    SysexRef(get()).GetArgs(callback);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_SYSEX_H_
