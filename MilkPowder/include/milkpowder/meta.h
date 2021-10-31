#ifndef LIB_MILKPOWDER_META_H_
#define LIB_MILKPOWDER_META_H_

#ifdef __cplusplus
#include <milkpowder/holder.h>
#include <functional>
namespace MilkPowder {
class MetaRef final : public MilkPowder_HolderRef(Meta) {
 public:
  explicit MetaRef(const MilkPowder_Meta_t *ref) : MilkPowder_HolderRef(Meta)(ref) {}
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Meta_GetType(get(), &res));
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_panic(MilkPowder_Meta_GetArgs(get(), &ptr, &res));
    if (args != nullptr) {
      *args = ptr;
    }
    return res;
  }
};
class Meta final : public MilkPowder_Holder(Meta) {
 public:
  explicit Meta(MilkPowder_Meta_t *ptr = nullptr) : MilkPowder_Holder(Meta)(ptr) {}
  explicit Meta(const MetaRef &ref) : MilkPowder_Holder(Meta)(ref) {}
  explicit Meta(std::function<bool(uint8_t *)> callback) : Meta() {
    MilkTea_panic(MilkPowder_Meta_Parse(reset(), &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
  Meta(uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length) : Meta() {
    MilkTea_panic(MilkPowder_Meta_Create(reset(), delta, type, args, length));
  }
  uint8_t GetType() const {
    return MetaRef(get()).GetType();
  }
  uint32_t GetArgs(const uint8_t **args) const {
    return MetaRef(get()).GetArgs(args);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_META_H_
