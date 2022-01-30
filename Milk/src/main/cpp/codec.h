#ifndef MILK_CODEC_H_
#define MILK_CODEC_H_

#include <map>
#include <memory>
#include <functional>
#include <vector>

#include <milkpowder.h>

#include "control.h"

namespace Milk {

class CodecController final : public BaseController<CodecController> {
  static constexpr char TAG[] = "Milk::CodecController";
  enum class FormatType {
    UNCONFINED,
    SINGLE,
    SIMULTANEOUS,
    INDEPENDENT,
  };
 public:
  static constexpr auto kName = "";
  static constexpr auto kUsage = R"(
Usage: milk [OPTIONS] [FILES]
  -v, --version
    print version code
  -h, --help
    print this help message
  --log {d, i, w, e, debug, info, warn, error}
    init log level, or no log
  -o
    name of target file
    or torn apart all files if this option is not set
  -f {0, 1, 2}
  --format {0, 1, 2}
    header format of output files
  -t
  --tick
    reset division by a 16 bit unsigned integer described in hexadecimal
    only the header will be changed by default
  -r
  --reset
    reset the delta of all messages if the division of target is different from original type
)";
  CodecController(ContextWrapper &context) : BaseController(context) {}
 protected:
  void Main(args_type &args) final {
    if (args.empty()) {
      Err() << Tip() << "no input files" << End();
      return;
    }
    if (update_delta_ && (division_ & 0x8000) != 0) {
      accuracy_ = GetAccuracy(division_);
    }
    if (target_ == "") {
      TornApart(args);
    } else {
      GenTarget(args, target_);
    }
  }
  pipeline_type Config(pipeline_type &&pipeline) final {
    return super_type::Config(std::move(pipeline))
      .Append({
        "-o",
      }, &self_type::InitTarget)
      .Append({
        "-t",
        "--tick",
      }, &self_type::SetTick)
      .Append({
        "-r",
        "--reset",
      }, &self_type::update_delta_, true)
      .Append({
        "-f",
        "--format",
      }, &self_type::format_,
      [this]() {
        Err() << Tip() << "need format value" << End();
      },
      [this](auto &it) {
        Err() << Tip() << "invalid format value: " << it << End();
      }, {
        { FormatType::SINGLE, { "0" } },
        { FormatType::SIMULTANEOUS, { "1" } },
        { FormatType::INDEPENDENT, { "2" } },
      });
  }
 private:
  bool InitTarget(cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip() << "-o: need target name" << End();
      return false;
    }
    if (target_ != "") {
      Err() << Tip() << "-o: target name has been set to " << target_ << End();
      return false;
    }
    target_ = *cursor;
    ++cursor;
    return true;
  }
  bool SetTick(cursor_type &cursor) {
    if (!cursor) {
      Err() << Tip() << "--tick: need division value" << End();
      return false;
    }
    auto s = *cursor;
    for (size_t i = 0, n = s.size(); i != n; ++i) {
      auto c = s[i];
      if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')) {
        continue;
      }
      Err() << Tip() << "--tick: invalid division value -- " << s << End();
      return false;
    }
    if (s.size() > 4) {
      Err() << Tip() << "--tick: the division value is too large -- " << s << End();
      return false;
    }
    uint16_t division = 0;
    for (size_t i = 0, n = s.size(); i != n; ++i) {
      uint16_t b = 0;
      auto c = s[i];
      if ('0' <= c && c <= '9') {
        b = c - '0';
      }
      if ('a' <= c && c <= 'f') {
        b = c - 'a' + 10;
      }
      if ('A' <= c && c <= 'F') {
        b = c - 'A' + 10;
      }
      division <<= 04;
      division |= b;
    }
    division_ = division;
    ++cursor;
    return true;
  }
  uint32_t UpdateDelta(uint32_t delta, uint16_t division) const {
    if (division == 0) {
      MilkTea_logW("UpdateDelta but the division is zero");
      return delta;
    }
    if ((division & 0x8000) == 0 && (division_ & 0x8000) == 0) {
      uint64_t d = delta;
      d *= division_;
      d /= division;
      uint32_t u = d & 0x0f'ff'ff'ff;
      if (d != u) {
        MilkTea_logW("UpdateDelta but the result is too large -- %" PRIu64, d);
      }
      return u;
    }
    if ((division & 0x8000) != 0 && (division_ & 0x8000) != 0) {
      uint32_t accuracy = GetAccuracy(division);
      uint64_t d = delta;
      d *= accuracy_;
      d /= accuracy;
      uint32_t u = d & 0x0f'ff'ff'ff;
      if (d != u) {
        MilkTea_logW("UpdateDelta but the result is too large -- %" PRIu64, d);
      }
      return u;
    }
    MilkTea_logD("UpdateDelta but the type of division is difference -- from %" PRIu32 " to %" PRIu32, division, division_);
    return false;
  }
  void UpdateDelta(MilkPowder::TrackMutableWrapper &track, uint16_t division) {
    if (update_delta_ && division_ != 0 && division != 0) {
      if (((division ^ division_) & 0x8000) != 0) {
        MilkTea_logW("UpdateDelta but the type of division is difference -- from %" PRIu32 " to %" PRIu32, division, division_);
        return;
      }
      track.AllMessage([this, division](auto &it) {
        it.SetDelta(UpdateDelta(it.GetDelta(), division));
        if (it.IsSysex()) {
          auto sysex = MilkPowder::MutableInterface<MilkPowder::Mapping::Sysex>::From(std::move(it));
          sysex.AllItem([this, division](auto item) {
            item.delta_ = this->UpdateDelta(item.delta_, division);
          });
          it = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(std::move(sysex));
        }
      });
    }
  }
  void TornApart(const args_type &filenames) {
    for (auto filename : filenames) {
      auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
        auto reader = Context().GetFileReader(filename);
        return MilkPowder::MidiMutableWrapper::Parse([&reader](auto bytes[], auto len) -> auto {
          return reader.Read(bytes, len);
        });
      }();
      uint16_t format = midi.GetFormat();
      uint16_t ntrks = midi.GetNtrks();
      uint16_t division = midi.GetDivision();
      switch (format_) {
        case FormatType::SINGLE:
          format = 0;
          break;
        case FormatType::SIMULTANEOUS:
          format = 1;
          break;
        case FormatType::INDEPENDENT:
          format = 2;
          break;
        default:
          break;
      }
      for (uint16_t idx = 0; idx != ntrks; ++idx) {
        std::vector<MilkPowder::TrackMutableWrapper> tracks = { midi.GetTrack(idx), };
        UpdateDelta(tracks[0], division);
        if (division_ != 0) {
          division = division_;
        }
        auto target = MilkPowder::MidiMutableWrapper::Make(format, division, std::move(tracks));
        {
          std::string name = std::string() + filename.data() + "." + MilkTea::ToStringHex::FromU16(idx) + ".mid";
          auto writer = Context().GetFileWriter(name);
          target.Dump([&writer](auto bytes[], auto len) {
            writer.Write(bytes, len);
          });
        }
      }
    }
  }
  void GenTarget(const args_type &filenames, std::string_view name) {
    uint16_t f = [this]() -> auto {
      switch (format_) {
        case FormatType::SINGLE:
          return 0;
        case FormatType::SIMULTANEOUS:
          return 1;
        case FormatType::INDEPENDENT:
          return 2;
        default:
          return 0;
      }
    }();
    uint16_t d = division_;
    std::vector<MilkPowder::TrackMutableWrapper> tracks;
    std::vector<uint16_t> divisions;
    for (auto filename : filenames) {
      auto midi = [&]() -> MilkPowder::MidiMutableWrapper {
        auto reader = Context().GetFileReader(filename);
        return MilkPowder::MidiMutableWrapper::Parse([&reader](auto bytes[], auto len) -> auto {
          return reader.Read(bytes, len);
        });
      }();
      uint16_t format = midi.GetFormat();
      uint16_t ntrks = midi.GetNtrks();
      uint16_t division = midi.GetDivision();
      if (f == 0) {
        f = format;
      }
      if (d == 0) {
        d = division;
      }
      for (auto idx = 0; idx != ntrks; ++idx) {
        divisions.push_back(division);
        tracks.emplace_back(midi.GetTrack(idx));
      }
    }
    if (f == 0 && tracks.size() > 1) {
      Err() << Tip() << "try to generate target with one more tracks but the expected format is single mode" << End();
      return;
    } else if (tracks.size() == 1 && format_ == FormatType::SINGLE) {
      f = 0;
    }
    division_ = d;
    for (size_t i = 0, n = divisions.size(); i != n; ++i) {
      UpdateDelta(tracks[i], divisions[i]);
    }
    auto target = MilkPowder::MidiMutableWrapper::Make(f, d, std::move(tracks));
    {
      auto writer = Context().GetFileWriter(name);
      target.Dump([&writer](auto bytes[], auto len) {
        writer.Write(bytes, len);
      });
    }
  }
  static uint32_t GetAccuracy(uint16_t division) {
    uint32_t result = 1;
    result *= division & 0x00ff;
    result *= -(*reinterpret_cast<const int16_t *>(&division) >> 010);
    return result;
  }
  std::string_view target_ = "";
  FormatType format_ = FormatType::UNCONFINED;
  uint16_t division_ = 0;
  bool update_delta_ = false;
  uint32_t accuracy_ = 0;
};

} // namespace Milk

#endif // ifndef MILK_CODEC_H_
