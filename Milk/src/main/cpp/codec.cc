#include <map>
#include <memory>
#include <functional>

#include "launcher.h"

namespace Milk {

class Codec final {
 public:
  static constexpr char kUsage[] =
"Usage: milk [OPTIONS] [FILES]\n"
"  -h, --help\n"
"    print this help message\n"
"  -v, --version\n"
"    print version code\n"
"  -o\n"
"    name of target file\n"
"    or torn apart all files if this option is not set\n"
;
  Codec()
    : callbacks_(),
      help_(false),
      version_(false),
      target_("") {
    callbacks_["-h"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["--help"] = [this](auto &itr, auto &args) -> bool {
      ShowHelp();
      return true;
    };
    callbacks_["-v"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
      return true;
    };
    callbacks_["--version"] = [this](auto &itr, auto &args) -> bool {
      ShowVersion();
      return true;
    };
    callbacks_["-o"] = [this](auto &itr, auto &args) -> bool {
      return InitTarget(itr, args);
    };
  }
  void Launch(std::list<std::string_view> &args) {
    for (auto itr = args.begin(); itr != args.end(); ) {
      auto it = callbacks_.find(*itr);
      if (it != callbacks_.end()) {
        itr = args.erase(itr);
        if (!it->second(itr, args)) {
          ShowHelp();
          return;
        }
      } else {
        ++itr;
      }
    }
    if (args.size() == 0) {
      std::cerr << "milk: no input files" << std::endl;
      return;
    }
    if (target_ == "") {
      TornApart(args);
    } else {
      GenTarget(args, target_);
    }
  }
 private:
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  bool help_;
  bool version_;
  std::string_view target_;
  void ShowHelp() {
    if (help_) {
      return;
    } else {
      help_ = true;
    }
    Launcher::ShowHelp();
  }
  void ShowVersion() {
    if (version_) {
      return;
    } else {
      version_ = true;
    }
    Launcher::ShowVersion();
  }
  bool InitTarget(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    if (itr == args.end()) {
      std::cerr << "milk -o: need target name" << std::endl;
      return false;
    }
    if (target_ != "") {
      std::cerr << "milk -o: target name has been set to " << target_ << std::endl;
      return false;
    }
    target_ = *itr;
    itr = args.erase(itr);
    return true;
  }
  void TornApart(const std::list<std::string_view> &filenames) {
    for (auto filename : filenames) {
      MilkPowder_Errno_t err;
      MilkPowderHolder<MilkPowder_Midi_t> midi;
      {
        InputReader reader(filename);
        err = MilkPowder_Midi_Parse(&midi, &reader, InputReader::Reader);
        check_err("read midi file");
      }
      uint16_t format;
      err = MilkPowder_Midi_GetFormat(midi, &format);
      check_err("get format");
      uint16_t ntrks;
      err = MilkPowder_Midi_GetNtrks(midi, &ntrks);
      check_err("get ntrks");
      uint16_t division;
      err = MilkPowder_Midi_GetDivision(midi, &division);
      check_err("get division");
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        MilkPowder_Track_t *tracks[1];
        const MilkPowder_Track_t *trk;
        err = MilkPowder_Midi_GetTrack(midi, idx, &trk);
        check_err("get track");
        err = MilkPowder_Track_Clone(trk, &tracks[0]);
        check_err("clone track");
        MilkPowderHolder<MilkPowder_Midi_t> target;
        err = MilkPowder_Midi_Create(&target, format, 1, division, tracks);
        check_err("create midi");
        OutputWriter writer(std::string(filename) + "." + FromU16ToStringHex(idx) + ".mid");
        if (writer.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        err = MilkPowder_Midi_Dump(target, &writer, OutputWriter::Writer);
        check_err("dump midi");
      }
    }
  }
  void GenTarget(const std::list<std::string_view> &filenames, std::string_view target) {
  }
};

void Launcher::LaunchCodec(std::list<std::string_view> &args) {
  Codec().Launch(args);
}

std::string_view Launcher::UsageCodec() {
  return Codec::kUsage;
}

} // namespace Milk
