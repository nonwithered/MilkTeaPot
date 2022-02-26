#ifndef LIB_MILK_POWDER_MIDI_H_
#define LIB_MILK_POWDER_MIDI_H_

#ifdef __cplusplus
#include <tea/mask.h>
#include <tea/slice.h>
#endif // ifdef __cplusplus

#include <tea/def.h>
#include <tea/file.h>

#include <milkpowder/track.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Midi_t;

extern
TEA_API struct MilkPowder_Midi_t * (TEA_CALL *
MilkPowder_Midi_Create)(uint16_t format, uint16_t division, uint16_t count, struct MilkPowder_Track_t *[]);

extern
TEA_API void (TEA_CALL *
MilkPowder_Midi_Destroy)(struct MilkPowder_Midi_t *);

extern
TEA_API struct MilkPowder_Midi_t * (TEA_CALL *
MilkPowder_Midi_Parse)(tea_file_input_t);

extern
TEA_API uint16_t (TEA_CALL *
MilkPowder_Midi_GetFormat)(const struct MilkPowder_Midi_t *);

extern
TEA_API uint16_t (TEA_CALL *
MilkPowder_Midi_GetDivision)(const struct MilkPowder_Midi_t *);

extern
TEA_API void (TEA_CALL *
MilkPowder_Midi_SetFormat)(struct MilkPowder_Midi_t *, uint16_t);

extern
TEA_API void (TEA_CALL *
MilkPowder_Midi_SetDivision)(struct MilkPowder_Midi_t *, uint16_t);

extern
TEA_API uint16_t (TEA_CALL *
MilkPowder_Midi_GetCount)(const struct MilkPowder_Midi_t *);

extern
TEA_API struct MilkPowder_Track_t * (TEA_CALL *
MilkPowder_Midi_GetTrack)(struct MilkPowder_Midi_t *, uint16_t);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Midi = MilkPowder_Midi_t;

} // namespace MilkPowder

struct MilkPowder_Midi_t : tea::mask_type<MilkPowder::Midi> {
  static
  auto init(std::function<size_t(uint8_t *, size_t)> func) -> MilkPowder::Midi * {
    return MilkPowder_Midi_Parse(tea::meta::closure_cast<tea_file_input_t>(std::move(func)));
  }
  static
  auto init(uint16_t format, uint16_t division, tea::slice<MilkPowder::Track *> trks) -> MilkPowder::Midi * {
    return MilkPowder_Midi_Create(format, division, static_cast<uint16_t>(0xff & trks.size()), trks.data());
  }
  auto drop() && -> void {
    MilkPowder_Midi_Destroy(get());
  }
  auto format() const -> uint16_t {
    return MilkPowder_Midi_GetFormat(get());
  }
  auto format(uint16_t f) -> void {
    MilkPowder_Midi_SetFormat(get(), f);
  }
  auto division() const -> uint16_t {
    return MilkPowder_Midi_GetDivision(get());
  }
  auto division(uint16_t d) -> void {
    MilkPowder_Midi_SetDivision(get(), d);
  }
  auto size() const -> uint16_t {
    return MilkPowder_Midi_GetCount(get());
  }
  auto item(uint16_t index) -> MilkPowder::Track * {
    return MilkPowder_Midi_GetTrack(get(), index);
  }
  auto item(uint32_t index) const -> const MilkPowder::Track * {
    return MilkPowder_Midi_GetTrack(const_cast<MilkPowder::Midi *>(get()), index);
  }
  auto operator[](uint16_t index) -> MilkPowder::Track * {
    return item(index);
  }
  auto operator[](uint32_t index) const -> const MilkPowder::Track * {
    return item(index);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_MIDI_H_
