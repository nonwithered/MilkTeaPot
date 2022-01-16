#include <milk/common.h>

#include "context.h"
#include "launch.h"

namespace {

constexpr char TAG[] = "Milk";

} // namespace

MilkTea_extern(Milk_Main, (int argc, char *argv[], Milk_Context_t context), {
  Milk::Launcher::Launch(argc, argv, context);
})

namespace Milk {

} // namespace Milk
