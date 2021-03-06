#include <milk/common.h>

#include "codec.h"
#include "dump.h"
#include "play.h"

namespace {

constexpr char TAG[] = "Milk";

using container_type = Milk::args_type;

auto Args(int argc, char *argv[]) {
  container_type args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  return args;
}

} // namespace

MilkTea_extern(Milk_Main, (int argc, char *argv[], Milk_Context_t context), {
  if (argc <= 0) {
    MilkTea_assertf("invalid argc: %d", argc);
  }
  auto args = Args(argc, argv);
  auto context_wrapper = Milk::ContextWrapper(argv[0], context);
  MilkTea_Command::Dispatcher(args, context_wrapper).Start(
    Milk::Mode<Milk::CodecController>(),
    Milk::Mode<Milk::DumpController>(),
    Milk::Mode<Milk::PlayController>()
  );
})

namespace Milk {

} // namespace Milk
