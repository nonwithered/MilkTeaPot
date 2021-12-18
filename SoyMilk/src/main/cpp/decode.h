#ifndef SOYMILK_DECODE_H_
#define SOYMILK_DECODE_H_

#include <vector>

#include <soymilk/common.h>

namespace SoyMilk {

namespace Codec {

namespace Decode {

namespace {

using duration_type = TeaPot::TimerUnit::duration_type;

} // namespace

struct FrameBuffer {
  struct Item {
    uint16_t ntrk_;
    std::vector<MilkPowder::MessageMutableWrapper> msg_;
  };
  duration_type time_;
  std::vector<Item> items_;
};

} // namespace Decode
} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_DECODE_H_
