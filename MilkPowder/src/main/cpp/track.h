#ifndef MILKPOWDER_TRACK_H_
#define MILKPOWDER_TRACK_H_

#include <vector>

#include "message.h"

namespace MilkPowder {

class TrackImpl final {
 public:
  static std::unique_ptr<TrackImpl> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const;
  TrackImpl(std::vector<std::unique_ptr<MessageImpl>> items) : items_(std::move(items)) {
    MilkTea_LogPrint(INFO, TAG, "ctor size=%" PRIu32, static_cast<uint32_t>(items_.size()));
  }
  TrackImpl(const TrackImpl &another) : items_(Clone(another.items_)) {
    MilkTea_LogPrint(INFO, TAG, "copy size=%" PRIu32, static_cast<uint32_t>(items_.size()));
  }
  const std::vector<std::unique_ptr<MessageImpl>> &items() const { return items_; }
 private:
  const std::vector<std::unique_ptr<MessageImpl>> items_;
  static std::vector<std::unique_ptr<MessageImpl>> Clone(const std::vector<std::unique_ptr<MessageImpl>> &another) {
    std::vector<std::unique_ptr<MessageImpl>> items;
    for (const auto &it : another) {
      items.emplace_back(it->Clone());
    }
    return std::move(items);
  }
  static constexpr char TAG[] = "track";
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_TRACK_H_
