#ifndef LIB_MILK_POWDER_SYSEX_H_
#define LIB_MILK_POWDER_SYSEX_H_

#ifdef __cplusplus
#include <tuple>
#include <vector>
#include <tea/slice.h>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#include <milkpowder/message.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Sysex_t;

extern
TEA_API struct MilkPowder_Sysex_t * (TEA_CALL *
MilkPowder_Sysex_Create)(const uint32_t delta[], const uint8_t *const args[], const uint32_t len[], uint32_t count);

extern
TEA_API void (TEA_CALL *
MilkPowder_Sysex_Destroy)(struct MilkPowder_Sysex_t *);

extern
TEA_API struct MilkPowder_Sysex_t * (TEA_CALL *
MilkPowder_Sysex_Clone)(const struct MilkPowder_Sysex_t *);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Sysex_GetCount)(const struct MilkPowder_Sysex_t *);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Sysex_GetArgs)(const struct MilkPowder_Sysex_t *, uint32_t index, const uint8_t **);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Sysex_GetDelta)(const struct MilkPowder_Sysex_t *, uint32_t index);

extern
TEA_API struct MilkPowder_Sysex_t * (TEA_CALL *
MilkPowder_Sysex_FromMessage)(struct MilkPowder_Message_t *);

extern
TEA_API const struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Sysex_ToMessage)(const struct MilkPowder_Sysex_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Sysex = MilkPowder_Sysex_t;

} // namespace MilkPowder

struct MilkPowder_Sysex_t : tea::mask_type<MilkPowder::Sysex> {
  static
  auto init(tea::slice<const std::tuple<uint32_t, tea::slice<const uint8_t>>> items) -> MilkPowder::Sysex * {
    size_t count = items.size();
    auto delta = std::vector<uint32_t>(count);
    auto args = std::vector<const uint8_t *>(count);
    auto len = std::vector<uint32_t>(count);
    for (size_t i = 0; i != count; ++i) {
      delta[i] = std::get<0>(items[i]);
      args[i] = std::get<1>(items[i]).data();
      len[i] = std::get<1>(items[i]).size();
    }
    return MilkPowder_Sysex_Create(delta.data(), args.data(), len.data(), count);
  }
  auto drop() && -> void {
    MilkPowder_Sysex_Destroy(get());
  }
  auto clone() const -> MilkPowder::Sysex * {
    return MilkPowder_Sysex_Clone(get());
  }
  auto size() const -> uint32_t {
    return MilkPowder_Sysex_GetCount(get());
  }
  auto args(uint32_t index) const -> tea::slice<const uint8_t> {
    const uint8_t *ptr = nullptr;
    auto len = MilkPowder_Sysex_GetArgs(get(), index, &ptr);
    return tea::slice<const uint8_t>(ptr, len);
  }
  auto operator[](uint32_t index) const -> tea::slice<const uint8_t> {
    return args(index);
  }
  static
  auto from_message(MilkPowder::Message *&&it) -> MilkPowder::Sysex * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Sysex_FromMessage(it);
  }
  auto to_message() const-> const MilkPowder::Message * {
    return MilkPowder_Sysex_ToMessage(get());
  }
  auto delta() const -> uint32_t {
    return to_message()->delta();
  }
  auto delta(uint32_t d) -> void {
    auto *message = MilkPowder::Message::from_sysex(get());
    if (message == nullptr) {
      return;
    }
    message->delta(d);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_SYSEX_H_
