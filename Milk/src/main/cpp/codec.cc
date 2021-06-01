#include "launcher.h"

namespace {

constexpr char kUsage[] =
"Usage: milk codec\n"
;

} // namespace

namespace Milk {

void Launcher::LaunchCodec(std::list<std::string_view> &args) {
}

std::string_view Launcher::UsageCodec() {
  return kUsage;
}

} // namespace Milk
