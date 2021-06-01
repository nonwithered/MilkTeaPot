#include "launcher.h"

namespace {

constexpr char kUsage[] =
"Usage: milk play\n"
;

} // namespace

namespace Milk {

void Launcher::LaunchPlay(std::list<std::string_view> &args) {
}

std::string_view Launcher::UsagePlay() {
  return kUsage;
}

} // namespace Milk
