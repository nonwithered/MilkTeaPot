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
  bool ShowHelp(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    if (help_) {
      return true;
    } else {
      help_ = true;
    }
    Command::ShowHelp();
    return true;
  }
  bool ShowVersion(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
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
    MilkPowder_Exception_t err;
    for (auto filename : filenames) {
      MilkPowder_Holder(Midi) midi;
      {
        MilkPowder::FileReader reader(filename);
        if (reader.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        err = MilkPowder_Midi_Parse(&midi, &reader, MilkPowder::FileReader::Callback);
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
      if (format_ != -1) {
        format = static_cast<uint16_t>(format_);
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        MilkPowder_Track_t *tracks[1];
        const MilkPowder_Track_t *trk;
        err = MilkPowder_Midi_GetTrack(midi, idx, &trk);
        check_err("get track");
        err = MilkPowder_Track_Clone(trk, &tracks[0]);
        check_err("clone track");
        MilkPowder_Holder(Midi) target;
        err = MilkPowder_Midi_Create(&target, format, 1, division, tracks);
        check_err("create midi");
        MilkPowder::FileWriter writer(std::string(filename) + "." + MilkTea::ToStringHexFromU16(idx) + ".mid");
        if (writer.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        err = MilkPowder_Midi_Dump(target, &writer, MilkPowder::FileWriter::Callback);
        check_err("dump midi");
      }
    }
  }
  void GenTarget(const std::list<std::string_view> &filenames, std::string_view name) {
    MilkPowder_Exception_t err;
    int32_t f = -1;
    int32_t d = -1;
    std::vector<MilkPowder_Track_t *> tracks;
    if (format_ != -1) {
      f = format_;
    }
    for (auto filename : filenames) {
      MilkPowder_Holder(Midi) midi;
      {
        MilkPowder::FileReader reader(filename);
        if (reader.NonOpen()) {
          std::cerr << "Failed to open: " << filename << std::endl;
          return;
        }
        err = MilkPowder_Midi_Parse(&midi, &reader, MilkPowder::FileReader::Callback);
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
      if (f == -1) {
        f = static_cast<int32_t>(format);
      }
      if (d == -1) {
        d = static_cast<int32_t>(division);
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        const MilkPowder_Track_t *trk;
        err = MilkPowder_Midi_GetTrack(midi, idx, &trk);
        check_err("get track");
        MilkPowder_Holder(Track) track;
        err = MilkPowder_Track_Clone(trk, &track);
        check_err("clone track");
        tracks.push_back(track.release());
      }
    }
    if (f == 0) {
      MergeTracks(tracks);
    }
    MilkPowder_Holder(Midi) target;
    err = MilkPowder_Midi_Create(&target, static_cast<uint16_t>(f), static_cast<uint16_t>(tracks.size()), static_cast<uint16_t>(d), tracks.data());
    check_err("create midi");
    MilkPowder::FileWriter writer(name);
    if (writer.NonOpen()) {
      std::cerr << "Failed to open: " << name << std::endl;
      return;
    }
    err = MilkPowder_Midi_Dump(target, &writer, MilkPowder::FileWriter::Callback);
    check_err("dump midi");
  }
  static void MergeTracks(std::vector<MilkPowder_Track_t *> &tracks) {
    MilkPowder_Exception_t err;
    std::vector<MilkPowder_Message_t *> messages = MergeMsgs(GetMsgs(tracks));
    for (auto track : tracks) {
      err = MilkPowder_Track_Destroy(track);
      check_err("destroy track");
    }
    tracks.clear();
    MilkPowder_Holder(Track) track;
    err = MilkPowder_Track_Create(&track, messages.data(), static_cast<uint32_t>(messages.size()));
    check_err("create track");
    tracks.push_back(track.release());
  }
  static std::vector<std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>>> GetMsgs(const std::vector<MilkPowder_Track_t *> &tracks) {
    MilkPowder_Exception_t err;
    const size_t size = tracks.size();
    std::vector<std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>>> message_vec(size);
    std::vector<uint64_t> delta_vec(size);
    for (size_t i = 0; i != size; ++i) {
      uint64_t &delta = delta_vec[i];
      std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>> &messages = message_vec[i];
      std::function<void(const MilkPowder_Message_t *)> callback = [&delta, &messages](const MilkPowder_Message_t *item) -> void {
        uint32_t d = 0;
        MilkPowder_Exception_t err = MilkPowder_Message_GetDelta(item, &d);
        check_err("get delta");
        delta += d;
        messages.push_back(std::make_tuple(delta, item));
      };
      err = MilkPowder_Track_GetMessages(tracks[i], &callback, MilkTea::CallbackOf(callback));
      check_err("get messages");
    }
    return message_vec;
  }
  static std::vector<MilkPowder_Message_t *> MergeMsgs(const std::vector<std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>>> &messages) {
    MilkPowder_Exception_t err;
    const size_t size = messages.size();
    std::vector<MilkPowder_Message_t *> messages_vec;
    std::vector<std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>>::const_iterator> itrs_vec(size);
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
      MilkPowder_Holder(Message) message;
      const std::vector<std::tuple<uint64_t, const MilkPowder_Message_t *>>::const_iterator itr = itrs_vec[idx - 1]++;
      err = MilkPowder_Message_Clone(std::get<1>(*itr), &message);
      check_err("clone message");
      err = MilkPowder_Message_SetDelta(message, static_cast<uint32_t>(std::get<0>(*itr) - delta));
      check_err("set message delta");
      messages_vec.push_back(message.release());
      delta = std::get<0>(*itr);
    }
    return messages_vec;
  }
};

} // namespace Milk

#endif // ifndef MILK_CODEC_H_
