#ifndef MILKPOWDER_TRACK_H_
#define MILKPOWDER_TRACK_H_

#include <vector>

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Track final {
 public:
  static std::unique_ptr<Track> Parse(const uint8_t *&begin, const uint8_t *const end);
  void Dump(std::vector<uint8_t> &) const;
  Track(std::vector<std::unique_ptr<Message>> items) : items_(std::move(items)) {}
  const std::vector<std::unique_ptr<Message>> &items() const { return items_; }
 private:
  const std::vector<std::unique_ptr<Message>> items_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_TRACK_H_
