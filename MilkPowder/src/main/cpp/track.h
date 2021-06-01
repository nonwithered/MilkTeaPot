#ifndef MILKPOWDER_TRACK_H_
#define MILKPOWDER_TRACK_H_

#include <vector>

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Track final {
 public:
  static std::unique_ptr<Track> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const;
  Track(std::vector<std::unique_ptr<Message>> items) : items_(std::move(items)) {}
  Track(const Track &another) : items_(Clone(another.items_)) {}
  const std::vector<std::unique_ptr<Message>> &items() const { return items_; }
 private:
  const std::vector<std::unique_ptr<Message>> items_;
  static std::vector<std::unique_ptr<Message>> Clone(const std::vector<std::unique_ptr<Message>> &another) {
    std::vector<std::unique_ptr<Message>> items(another.size());
    for (const auto &it : another) {
      items.emplace_back(it->Clone());
    }
    return std::move(items);
  }
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_TRACK_H_
