#ifndef SOYMILK_FRAME_H_
#define SOYMILK_FRAME_H_

#include <vector>
#include <map>

#include <tea.h>

#include <soymilk.h>

namespace SoyMilk {

namespace internal {

class FrameTrackImpl final {
  using items_type = std::vector<tea::owner_ptr<MilkPowder::Message>>;
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
  void Append(tea::owner_ptr<MilkPowder::Message> item) {
    items_.emplace_back(std::move(item));
  }
  void GetItems(std::function<void(const MilkPowder::Message *)> f) const {
    std::for_each(items_.begin(), items_.end(), [f](auto &it) {
      f(it);
    });
  }
 private:
  explicit FrameTrackImpl(items_type items) : items_(std::move(items)) {}
  items_type items_;
};

class FrameMidiImpl final {
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
    auto it = items_.find(index);
    if (it == items_.end()) {
      it = items_.insert(it, { index, FrameTrackImpl() });
    }
    return it->second;
  }
  void GetItems(std::function<void(uint16_t, const MilkPowder::Message *)> f) const {
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
    auto it = items_.find(time);
    if (it == items_.end()) {
      it = items_.insert(it, { time, FrameMidiImpl() });
    }
    return it->second;
  }
  void GetItems(std::function<void(size_t, uint16_t, const MilkPowder::Message *)> f) const {
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
 public:
  FrameBufferImpl() = default;
  FrameBufferImpl(duration_type time, FramePackageImpl items)
  : time_(std::move(time)),
    items_(std::move(items)) {}
  FrameBufferImpl(const FrameBufferImpl &another)
  : FrameBufferImpl(another.time_, another.items_) {}
  FrameBufferImpl(FrameBufferImpl &&another)
  : FrameBufferImpl(std::move(another.time_), std::move(another.items_)) {}
  duration_type time() const {
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
  void GetItems(std::function<void(size_t, uint16_t, const MilkPowder::Message *)> f) const {
    items_.GetItems(f);
  }
 private:
  duration_type time_;
  FramePackageImpl items_;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_FRAME_H_
