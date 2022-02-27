#include <tea.h>

#include "midi.h"
#include "track.h"
#include "message.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

using namespace tea::meta;
using namespace MilkPowder::internal;

namespace {

auto Midi_Create = Class<Midi>::create<uint16_t, uint16_t, uint16_t, MilkPowder::Track **>;
auto Midi_Destroy = Class<Midi>::destroy;

auto Midi_Parse = function_handle<file::input>::invoke<&Midi::Parse>;
auto Midi_Dump = method_handle<file::output>::invoke<&Midi::Dump>;
auto Midi_GetFormat = field_handle::getter<&Midi::format_>;
auto Midi_GetDivision = field_handle::getter<&Midi::division_>;
auto Midi_SetFormat = field_handle::setter<&Midi::format_>;
auto Midi_SetDivision = field_handle::setter<&Midi::division_>;
auto Midi_GetCount = method_handle<>::invoke<&Midi::size>;
auto Midi_GetTrack = method_handle<uint16_t>::invoke<&Midi::operator[]>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Midi_Create);
TEA_FUNC(MilkPowder, Midi_Destroy);

TEA_FUNC(MilkPowder, Midi_Parse);
TEA_FUNC(MilkPowder, Midi_Dump);
TEA_FUNC(MilkPowder, Midi_GetFormat);
TEA_FUNC(MilkPowder, Midi_GetDivision);
TEA_FUNC(MilkPowder, Midi_SetFormat);
TEA_FUNC(MilkPowder, Midi_SetDivision);
TEA_FUNC(MilkPowder, Midi_GetCount);
TEA_FUNC(MilkPowder, Midi_GetTrack);

} // extern "C"

namespace {

auto Track_Create = Class<Track>::create<MilkPowder::Message **, uint32_t>;
auto Track_Destroy = Class<Track>::destroy;

auto Track_GetCount = method_handle<>::invoke<&Track::size>;
auto Track_GetMessage = method_handle<uint32_t>::invoke<&Track::operator[]>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Track_Create);
TEA_FUNC(MilkPowder, Track_Destroy);

TEA_FUNC(MilkPowder, Track_GetCount);
TEA_FUNC(MilkPowder, Track_GetMessage);

} // extern "C"

namespace {

auto Message_GetType = field_handle::getter<&Message::type_>;
auto Message_GetDelta = field_handle::getter<&Message::delta_>;
auto Message_SetDelta = field_handle::setter<&Message::delta_>;

auto Message_FromEvent = method_handle<>::invoke<&Event::FromEvent>;
auto Message_ToEvent = function_handle<const MilkPowder::Message *>::invoke<&Event::ToEvent>;
auto Message_FromMeta = method_handle<>::invoke<&Meta::FromMeta>;
auto Message_ToMeta = function_handle<const MilkPowder::Message *>::invoke<&Meta::ToMeta>;
auto Message_FromSysex = method_handle<>::invoke<&Sysex::FromSysex>;
auto Message_ToSysex = function_handle<const MilkPowder::Message *>::invoke<&Sysex::ToSysex>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Message_GetType);
TEA_FUNC(MilkPowder, Message_GetDelta);
TEA_FUNC(MilkPowder, Message_SetDelta);

TEA_FUNC(MilkPowder, Message_FromEvent);
TEA_FUNC(MilkPowder, Message_ToEvent);
TEA_FUNC(MilkPowder, Message_FromMeta);
TEA_FUNC(MilkPowder, Message_ToMeta);
TEA_FUNC(MilkPowder, Message_FromSysex);
TEA_FUNC(MilkPowder, Message_ToSysex);

} // extern "C"

namespace {

auto Event_Create = Class<Event>::create<uint32_t, uint8_t, uint8_t, uint8_t>;
auto Event_Destroy = Class<Event>::destroy;

auto Event_GetType = field_handle::getter<&Event::type_>;
auto Event_GetArgs = method_handle<uint8_t *>::invoke<&Event::args>;

auto Event_FromMessage = function_handle<MilkPowder::Message *>::invoke<&Event::FromMessage>;
auto Event_ToMessage = method_handle<>::invoke<&Event::ToMessage>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Event_Create);
TEA_FUNC(MilkPowder, Event_Destroy);

TEA_FUNC(MilkPowder, Event_GetType);
TEA_FUNC(MilkPowder, Event_GetArgs);

TEA_FUNC(MilkPowder, Event_FromMessage);
TEA_FUNC(MilkPowder, Event_ToMessage);

} // extern "C"

namespace {

auto Meta_Create = Class<Meta>::create<uint32_t, uint8_t, const uint8_t *, uint32_t>;
auto Meta_Destroy = Class<Meta>::destroy;

auto Meta_GetType = field_handle::getter<&Meta::type_>;
auto Meta_GetArgs = method_handle<const uint8_t **>::invoke<&Meta::args>;

auto Meta_FromMessage = function_handle<MilkPowder::Message *>::invoke<&Meta::FromMessage>;
auto Meta_ToMessage = method_handle<>::invoke<&Meta::ToMessage>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Meta_Create);
TEA_FUNC(MilkPowder, Meta_Destroy);

TEA_FUNC(MilkPowder, Meta_GetType);
TEA_FUNC(MilkPowder, Meta_GetArgs);

TEA_FUNC(MilkPowder, Meta_FromMessage);
TEA_FUNC(MilkPowder, Meta_ToMessage);

} // extern "C"

namespace {

auto Sysex_Create = Class<Sysex>::create<const uint32_t *, const uint8_t *const *, const uint32_t *, uint32_t>;
auto Sysex_Destroy = Class<Sysex>::destroy;

auto Sysex_GetCount = method_handle<>::invoke<&Sysex::size>;
auto Sysex_GetArgs = method_handle<uint32_t, const uint8_t **>::invoke<&Sysex::args>;
auto Sysex_GetDelta = method_handle<uint32_t>::invoke<&Sysex::delta>;

auto Sysex_FromMessage = function_handle<MilkPowder::Message *>::invoke<&Sysex::FromMessage>;
auto Sysex_ToMessage = method_handle<>::invoke<&Sysex::ToMessage>;

} // namespace

extern "C" {

TEA_FUNC(MilkPowder, Sysex_Create);
TEA_FUNC(MilkPowder, Sysex_Destroy);

TEA_FUNC(MilkPowder, Sysex_GetCount);
TEA_FUNC(MilkPowder, Sysex_GetArgs);
TEA_FUNC(MilkPowder, Sysex_GetDelta);

TEA_FUNC(MilkPowder, Sysex_FromMessage);
TEA_FUNC(MilkPowder, Sysex_ToMessage);

} // extern "C"
