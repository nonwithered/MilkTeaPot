#ifndef LIB_MILKPOWDER_WRAPPER_SYSEX_H_
#define LIB_MILKPOWDER_WRAPPER_SYSEX_H_

#include <functional>
#include <tuple>

#include <milkpowder/wrapper/message.h>

namespace MilkPowder {

using SysexItem = Mapping::Sysex::raw_item_type;

template<>
class ConstInterface<Mapping::Sysex> {
  using mapping = Mapping::Sysex;
  using raw_type = mapping::raw_type;
 public:
  virtual const raw_type *get() const = 0;
 public:
  static ConstWrapper<mapping> From(ConstWrapper<Mapping::Message> another) {
    const raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_message_to, another.get(), &obj);
    return obj;
  }
  uint32_t GetCount() const {
    uint32_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_count, get(), &result);
    return result;
  }
  SysexItem GetItem(uint32_t index) const {
    mapping::raw_item_type result{};
    MilkTea_invoke_panic(MilkPowder_Sysex_GetItem, get(), index, &result);
    return SysexItem{
      .delta_ = result.delta_,
      .length_ = static_cast<uint32_t>(result.length_),
      .args_ = result.args_,
    };
  }
};

using SysexConstWrapper = ConstWrapper<Mapping::Sysex>;

struct SysexItem_mut {
  uint32_t &delta_;
  uint32_t length_;
  uint8_t *args_;
};

template<>
class MutableInterface<Mapping::Sysex> {
  using mapping = Mapping::Sysex;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<mapping> Parse(std::function<size_t(uint8_t [], size_t)> reader) {
    MilkPowder::Mapping::ByteReader reader_ = reader;
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &obj, reader_);
    return obj;
  }
  static MutableWrapper<mapping> Make(const std::vector<SysexItem> &vec) {
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_create, &obj, static_cast<uint32_t>(vec.size()), vec.data());
    return obj;
  }
  static MutableWrapper<mapping> From(MutableWrapper<Mapping::Message> &&another) {
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_from_message, &obj, another.get());
    another.release();
    return obj;
  }
  void AllItem(std::function<void(SysexItem_mut)> consumer) {
    std::function<void(mapping::raw_item_mut_type)> consumer_ = [&consumer](mapping::raw_item_mut_type it) {
      SysexItem_mut it_{
        .delta_ = *it.delta_,
        .length_ = it.length_,
        .args_ = it.args_,
      };
      consumer(it_);
    };
    MilkTea_invoke_panic(mapping::raw_all_item, get(), MilkTea::FunctionAdapter::ToRawType<mapping::raw_consumer_type>(consumer_));
  }
};

using SysexMutableWrapper = MutableWrapper<Mapping::Sysex>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_SYSEX_H_
