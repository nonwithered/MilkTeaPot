#include <milktea_command.h>

namespace {

constexpr char TAG[] = "MilkTea_Command";

} // namespace

extern "C" {

MilkTea_noreturn
void MilkTea_Command_Stub() {
  MilkTea_throw(Unsupported, "Stub");
}

} // extern "C"
