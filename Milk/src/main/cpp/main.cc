#include <string_view>
#include <list>

void LaunchCodec(const std::list<std::string_view> &);
void LaunchProbe(const std::list<std::string_view> &);
void LaunchPlay(const std::list<std::string_view> &);
void LaunchDefault(const std::list<std::string_view> &);

int main(int argc, char *argv[0]) {
  std::list<std::string_view> args(argc - 1);
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  if (args.front() == "codec") {
    args.pop_back();
    LaunchCodec(args);
  } else if (args.front() == "probe") {
    args.pop_back();
    LaunchProbe(args);
  } else if (args.front() == "play") {
    args.pop_back();
    LaunchPlay(args);
  } else {
    LaunchDefault(args);
  }
  return 0;
}