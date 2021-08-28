#include <map>
#include <memory>
#include <vector>

#include "launch.h"
#include "codec.h"
#include "dump.h"
#include "play.h"

int main(int argc, char *argv[]) {
  std::list<std::string_view> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  std::vector<std::unique_ptr<Milk::Command>> arr;
  arr.emplace_back(new Milk::Codec());
  arr.emplace_back(new Milk::Dump());
  arr.emplace_back(new Milk::Play());
  Milk::Command::LaunchMain(args, arr);
  return 0;
}
