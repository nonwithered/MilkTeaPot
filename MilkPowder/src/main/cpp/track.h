#ifndef MILKPOWDER_TRACK_H_
#define MILKPOWDER_TRACK_H_

#include <vector>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Track {
  Track(std::vector<std::unique_ptr<Message>> items)
  : items_(std::move(items)) {}
  const std::vector<std::unique_ptr<Message>> items_;
};

} // namespace internal

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_TRACK_H_
