#ifndef MILK_CODEC_H_
#define MILK_CODEC_H_

#include <map>
#include <memory>
#include <functional>
#include <vector>

#include <milkpowder.h>

#include "control.h"

namespace Milk {

class CodecController final : public BaseController {
  static constexpr char TAG[] = "Milk::CodecController";
  using self_type = CodecController;
 public:
  static constexpr auto kName = "";
  static constexpr auto kUsage = R"(
Usage: milk [OPTIONS] [FILES]
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
  -v, --version
    print version code
  -o
    name of target file
    or torn apart all files if this option is not set
  -fmt {0, 1, 2}
  --format {0, 1, 2}
    header format of output files
)";
  CodecController(BaseContext &context, std::string help_text)
  : BaseController(context, std::move(help_text)),
    target_(""),
    format_(-1) {
    Config(&self_type::InitTarget, {
      "-o",
    });
    Config(&self_type::SetFormat, {
      "-fmt",
      "--format",
    });
  }
 protected:
  void Main(std::list<std::string_view> &args) final {
    if (args.empty()) {
      Err() << "milk: no input files" << End();
      return;
    }
    if (target_ == "") {
      TornApart(args);
    } else {
      GenTarget(args, target_);
    }
  }
 private:
  bool SetFormat(ArgsCursor &cursor) {
    if (!cursor) {
      Err() << "milk --format: need format value" << End();
      return false;
    } else if (*cursor == "0") {
      format_ = 0;
    } else if (*cursor == "1") {
      format_ = 1;
    } else if (*cursor == "2") {
      format_ = 2;
    } else {
      Err() << "milk --format: invalid format value: " << *cursor << End();
      return false;
    }
    ++cursor;
    return true;
  }
  bool InitTarget(ArgsCursor &cursor) {
    if (!cursor) {
      Err() << "milk -o: need target name" << End();
      return false;
    }
    if (target_ != "") {
      Err() << "milk -o: target name has been set to " << target_ << End();
      return false;
    }
    target_ = *cursor;
    ++cursor;
    return true;
  }
  void TornApart(const std::list<std::string_view> &filenames) {
    for (auto filename : filenames) {
      auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
        auto reader = Context().GetFileReader(filename.data(), filename.size());
        return MilkPowder::MidiMutableWrapper::Parse(*reader);
      }();
      uint16_t format = midi.GetFormat();
      uint16_t ntrks = midi.GetNtrks();
      uint16_t division = midi.GetDivision();
      if (format_ != -1) {
        format = static_cast<uint16_t>(format_);
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        std::vector<MilkPowder::TrackMutableWrapper> tracks = { midi.GetTrack(idx) };
        auto target = MilkPowder::MidiMutableWrapper::Make(format, division, std::move(tracks));
        {
          auto writer = Context().GetFileWriter(filename.data(), filename.size());
          target.Dump(*writer);
        }
      }
    }
  }
  void GenTarget(const std::list<std::string_view> &filenames, std::string_view name) {
    int32_t f = format_;
    int32_t d = -1;
    std::vector<MilkPowder::TrackMutableWrapper> tracks;
    for (auto filename : filenames) {
      auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
        auto reader = Context().GetFileReader(filename.data(), filename.size());
        return MilkPowder::MidiMutableWrapper::Parse(*reader);
      }();
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
    auto target = MilkPowder::MidiMutableWrapper::Make(static_cast<uint16_t>(f), static_cast<uint16_t>(d), std::move(tracks));
    {
      auto writer = Context().GetFileWriter(name.data(), name.size());
      target.Dump(*writer);
    }
  }
  static void MergeTracks(std::vector<MilkPowder::TrackMutableWrapper> &tracks) {
    tracks = { MilkPowder::TrackMutableWrapper::Make(MergeMsgs(GetMsgs(tracks))) };
  }
  static std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>> GetMsgs(const std::vector<MilkPowder::TrackMutableWrapper> &tracks) {
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
      for (uint32_t i = 0, n = tracks[i].GetCount(); i != n; ++i) {
        auto item = tracks[i].GetMessage(i);
        delta += item.GetDelta();
        messages.push_back(std::make_tuple(delta, item));
      }
    }
    return message_vec;
  }
  static std::vector<MilkPowder::MessageMutableWrapper> MergeMsgs(const std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>> &messages) {
    const size_t size = messages.size();
    std::vector<MilkPowder::MessageMutableWrapper> messages_vec;
    std::vector<std::vector<std::tuple<uint64_t, MilkPowder::MessageConstWrapper>>::const_iterator> itrs_vec(size);
    for (size_t i = 0; i != size; ++i) {
      itrs_vec[i] = messages[i].begin();
    }
    uint64_t delta = 0;
    MilkTea_loop {
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
      MilkPowder::MessageMutableWrapper message(std::get<1>(*itr));
      message.SetDelta(static_cast<uint32_t>(std::get<0>(*itr) - delta));
      messages_vec.emplace_back(std::move(message));
      delta = std::get<0>(*itr);
    }
    return messages_vec;
  }
  std::string_view target_;
  int32_t format_;
};

} // namespace Milk

#endif // ifndef MILK_CODEC_H_
