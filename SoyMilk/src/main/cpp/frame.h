#ifndef SOYMILK_FRAME_H_
#define SOYMILK_FRAME_H_

#include <list>
#include <map>

#include <soymilk/common.h>

namespace SoyMilk {

class FrameTrackImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameTrackImpl";
  using items_type = std::list<MilkPowder::MessageMutableWrapper>;
 public:
  FrameTrackImpl() : FrameTrackImpl(items_type{}) {}
  FrameTrackImpl(const FrameTrackImpl &another) : FrameTrackImpl(another.items_) {}
  FrameTrackImpl(FrameTrackImpl &&another) : FrameTrackImpl(std::move(another.items_)) {}
  void operator=(const FrameTrackImpl &another) {
    items_ = another.items_;
  }
  void operator=(FrameTrackImpl &&another) {
    items_ = std::move(another.items_);
  }
  void Append(MilkPowder::MessageMutableWrapper item) {
    items_.emplace_back(std::move(item));
  }
  void GetItems(std::function<void(MilkPowder::MessageConstWrapper)> f) const {
    std::for_each(items_.begin(), items_.end(), [f](auto &it) {
      f(it.get());
    });
  }
 private:
  explicit FrameTrackImpl(items_type items) : items_(std::move(items)) {}
  items_type items_;
};

class FrameMidiImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameMidiImpl";
  using key_type = uint16_t;
  using items_type = std::map<key_type, FrameTrackImpl>;
 public:
  FrameMidiImpl() : FrameMidiImpl(items_type{}) {}
  FrameMidiImpl(const FrameMidiImpl &another) : FrameMidiImpl(another.items_) {}
  FrameMidiImpl(FrameMidiImpl &&another) : FrameMidiImpl(std::move(another.items_)) {}
  void operator=(const FrameMidiImpl &another) {
    items_ = another.items_;
  }
  void operator=(FrameMidiImpl &&another) {
    items_ = std::move(another.items_);
  }
  FrameTrackImpl &Obtain(key_type index) {
    return items_.try_emplace(index).first->second;
  }
  void GetItems(std::function<void(uint16_t, MilkPowder::MessageConstWrapper)> f) const {
    std::for_each(items_.begin(), items_.end(), [f](auto &trk) {
      auto ntrk = trk.first;
      trk.second.GetItems([f, ntrk](auto msg) {
        f(ntrk, msg);
      });
    });
  }
 private:
  explicit FrameMidiImpl(items_type items) : items_(std::move(items)) {}
  items_type items_;
};

class FramePackageImpl final {
  static constexpr char TAG[] = "SoyMilk::FramePackageImpl";
  using key_type = size_t;
  using items_type = std::map<key_type, FrameMidiImpl>;
 public:
  FramePackageImpl() : FramePackageImpl(items_type{}) {}
  FramePackageImpl(const FramePackageImpl &another) : FramePackageImpl(another.items_) {}
  FramePackageImpl(FramePackageImpl &&another) : FramePackageImpl(std::move(another.items_)) {}
  void operator=(const FramePackageImpl &another) {
    items_ = another.items_;
  }
  void operator=(FramePackageImpl &&another) {
    items_ = std::move(another.items_);
  }
  FrameMidiImpl &Obtain(key_type time) {
    return items_.try_emplace(time).first->second;
  }
  void GetItems(std::function<void(size_t, uint16_t, MilkPowder::MessageConstWrapper)> f) const {
    std::for_each(items_.begin(), items_.end(), [f](auto &midi) {
      auto index = midi.first;
      midi.second.GetItems([f, index](auto ntrk, auto msg) {
        f(index, ntrk, msg);
      });
    });
  }
 private:
  explicit FramePackageImpl(items_type items) : items_(std::move(items)) {}
  items_type items_;
};

class FrameBufferImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameBufferImpl";
  using tempo_type = MilkPowder::Mapping::Message::tempo_type;
 public:
  FrameBufferImpl() = default;
  FrameBufferImpl(tempo_type time, FramePackageImpl items)
  : time_(std::move(time)),
    items_(std::move(items)) {}
  FrameBufferImpl(const FrameBufferImpl &another)
  : FrameBufferImpl(another.time_, another.items_) {}
  FrameBufferImpl(FrameBufferImpl &&another)
  : FrameBufferImpl(std::move(another.time_), std::move(another.items_)) {}
  tempo_type time() const {
    return time_;
  }
  void operator=(const FrameBufferImpl &another) {
    time_ = another.time_;
    items_ = another.items_;
  }
  void operator=(FrameBufferImpl &&another) {
    time_ = std::move(another.time_);
    items_ = std::move(another.items_);
  }
  void GetItems(std::function<void(size_t, uint16_t, MilkPowder::MessageConstWrapper)> f) const {
    items_.GetItems(f);
  }
 private:
  tempo_type time_;
  FramePackageImpl items_;
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_FRAME_H_
