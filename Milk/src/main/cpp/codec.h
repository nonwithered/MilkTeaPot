#ifndef MILK_CODEC_H_
#define MILK_CODEC_H_

#include <map>
#include <memory>
#include <functional>
#include <vector>

#include "launch.h"

namespace Milk {

class Codec final : public Command {
 public:
  Codec() : Command(),
      callbacks_(),
      help_(false),
      version_(false),
      target_(""),
      format_(-1) {
    Callback("-h", &Codec::ShowHelp);
    Callback("--help", &Codec::ShowHelp);
    Callback("--log", &Codec::InitLog);
    Callback("-v", &Codec::ShowVersion);
    Callback("--version", &Codec::ShowVersion);
    Callback("-o", &Codec::InitTarget);
    Callback("-fmt", &Codec::SetFormat);
    Callback("--format", &Codec::SetFormat);
  }
 protected:
  void Launch(std::list<std::string_view> &args) final {
    if (target_ == "") {
      TornApart(args);
    } else {
      GenTarget(args, target_);
    }
  }
  std::string_view Usage() const final {
    return
"Usage: milk [OPTIONS] [FILES]\n"
"  -h, --help\n"
"    print this help message\n"
"  --log {d, i, w, e, debug, info, warn, error}\n"
"    init log level, or no log\n"
"  -v, --version\n"
"    print version code\n"
"  -o\n"
"    name of target file\n"
"    or torn apart all files if this option is not set\n"
"  -fmt {0, 1, 2}\n"
"  --format {0, 1, 2}\n"
"    header format of output files\n"
    ;
  }
  std::string_view Name() const final {
    return "";
  }
 private:
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  bool help_;
  bool version_;
  std::string_view target_;
  int32_t format_;
  bool ShowHelp(std::list<std::string_view>::iterator &, std::list<std::string_view> &) {
    if (help_) {
      return true;
    } else {
      help_ = true;
    }
    Command::ShowHelp();
    return true;
  }
  bool ShowVersion(std::list<std::string_view>::iterator &, std::list<std::string_view> &) {
    if (version_) {
      return true;
    } else {
      version_ = true;
    }
    Command::ShowVersion();
    return true;
  }
  bool SetFormat(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    if (itr == args.end()) {
      std::cerr << "milk --format: need format value" << std::endl;
      return false;
    } else if (*itr == "0") {
      format_ = 0;
    } else if (*itr == "1") {
      format_ = 1;
    } else if (*itr == "2") {
      format_ = 2;
    } else {
      std::cerr << "milk --format: invalid format value: " << *itr << std::endl;
      return false;
    }
    itr = args.erase(itr);
    return true;
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
      MilkPowder::MidiWrapper midi;
      {
        MilkPowder::FileReader reader(filename);
        if (reader.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        midi = MilkPowder::MidiWrapper(reader);
      }
      uint16_t format = midi.GetFormat();
      uint16_t ntrks = midi.GetNtrks();
      uint16_t division = midi.GetDivision();
      if (format_ != -1) {
        format = static_cast<uint16_t>(format_);
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        MilkPowder::TrackWrapper tracks[1] = { MilkPowder::TrackWrapper(midi.GetTrack(idx)) };
        MilkPowder::MidiWrapper target(format, 1, division, tracks);
        MilkPowder::FileWriter writer(std::string(filename) + "." + MilkTea::ToStringHexFromU16(idx) + ".mid");
        if (writer.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        target.Dump(writer);
      }
    }
  }
  void GenTarget(const std::list<std::string_view> &filenames, std::string_view name) {
    int32_t f = format_;
    int32_t d = -1;
    std::vector<MilkPowder::TrackWrapper> tracks;
    for (auto filename : filenames) {
      MilkPowder::MidiWrapper midi;
      {
        MilkPowder::FileReader reader(filename);
        if (reader.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        midi = MilkPowder::MidiWrapper(reader);
      }
      uint16_t format = midi.GetFormat();
      uint16_t ntrks = midi.GetNtrks();
      uint16_t division = midi.GetDivision();
      if (f == -1) {
        f = static_cast<int32_t>(format);
      }
      if (d == -1) {
        d = static_cast<int32_t>(division);
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        tracks.emplace_back(midi.GetTrack(idx));
      }
    }
    if (f == 0) {
      MergeTracks(tracks);
    }
    MilkPowder::MidiWrapper target = MilkPowder::MidiWrapper(static_cast<uint16_t>(f), static_cast<uint16_t>(tracks.size()), static_cast<uint16_t>(d), tracks.data());
    MilkPowder::FileWriter writer(name);
    if (writer.NonOpen()) {
      std::cerr << "Failed to open: " << name << std::endl;
      return;
    }
    target.Dump(writer);
  }
  static void MergeTracks(std::vector<MilkPowder::TrackWrapper> &tracks) {
    std::vector<MilkPowder::MessageWrapper> messages = MergeMsgs(GetMsgs(tracks));
    tracks.clear();
    tracks.emplace_back(messages.data(), static_cast<uint32_t>(messages.size()));
  }
  static std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>> GetMsgs(const std::vector<MilkPowder::TrackWrapper> &tracks) {
    const size_t size = tracks.size();
    std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>> message_vec(size);
    std::vector<uint64_t> delta_vec(size);
    for (size_t i = 0; i != size; ++i) {
      uint64_t &delta = delta_vec[i];
      std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>> &messages = message_vec[i];
      std::function<void(MilkPowder::MessageConstWrapper)> callback = [&delta, &messages](MilkPowder::MessageConstWrapper item) -> void {
        delta += item.GetDelta();
        messages.push_back(std::make_tuple(delta, item));
      };
      tracks[i].GetMessages([&delta, &messages](MilkPowder::MessageConstWrapper item) -> void {
        delta += item.GetDelta();
        messages.push_back(std::make_tuple(delta, item));
      });
    }
    return message_vec;
  }
  static std::vector<MilkPowder::MessageWrapper> MergeMsgs(const std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>> &messages) {
    const size_t size = messages.size();
    std::vector<MilkPowder::MessageWrapper> messages_vec;
    std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>::const_iterator> itrs_vec(size);
    for (size_t i = 0; i != size; ++i) {
      itrs_vec[i] = messages[i].begin();
    }
    uint64_t delta = 0;
    while (true) {
      size_t idx = 0;
      for (size_t i = 0; i != size; ++i) {
        auto itr = itrs_vec[i];
        if (itr == messages[i].end()) {
          continue;
        }
        if (idx == 0) {
          idx = i + 1;
          continue;
        }
        if (std::get<0>(*itr) < std::get<0>(*itrs_vec[idx - 1])) {
          idx = i + 1;
          continue;
        }
      }
      if (idx == 0) {
        break;
      }
      const std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>::const_iterator itr = itrs_vec[idx - 1]++;
      MilkPowder::MessageWrapper message(std::get<1>(*itr));
      message.SetDelta(static_cast<uint32_t>(std::get<0>(*itr) - delta));
      messages_vec.emplace_back(std::move(message));
      delta = std::get<0>(*itr);
    }
    return messages_vec;
  }
  static constexpr char TAG[] = "Milk#Codec";
};

} // namespace Milk

#endif // ifndef MILK_CODEC_H_
