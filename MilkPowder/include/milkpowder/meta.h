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
    MilkTea_panic(MilkPowder_Meta_GetType(*this, &res));
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_panic(MilkPowder_Meta_GetArgs(*this, &ptr, &res));
    if (args != nullptr) {
      *args = ptr;
    }
    return res;
  }
};
class Meta final : public MilkPowder_Holder(Meta) {
 public:
  Meta() {}
  explicit Meta(const MetaRef &ref) : MilkPowder_Holder(Meta)(ref) {}
  explicit Meta(std::function<bool(uint8_t *)> callback) {
    MilkTea_panic(MilkPowder_Meta_Parse(&*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke));
  }
  Meta(uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length) {
    MilkTea_panic(MilkPowder_Meta_Create(&*this, delta, type, args, length));
  }
  uint8_t GetType() const {
    return MetaRef(*this).GetType();
  }
  uint32_t GetArgs(const uint8_t **args) const {
    return MetaRef(*this).GetArgs(args);
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_META_H_
