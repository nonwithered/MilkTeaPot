#include <milk/common.h>

#include "context.h"
#include "launch.h"

namespace {

constexpr char TAG[] = "Milk";

} // namespace

MilkTea_decl(int)
Milk_Main(int argc, char *argv[], Milk_Context_t context) {
  return Milk::Launcher::Launch(argc, argv, context) ? EXIT_SUCCESS : EXIT_FAILURE;
}

namespace Milk {

} // namespace Milk
