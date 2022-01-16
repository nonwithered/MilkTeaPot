#include <milk/common.h>

namespace {

constexpr char TAG[] = "Milk#FileIO";

Milk::BaseFileReader &BaseFileReader_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk::BaseFileReader *>(self);
}

Milk::BaseFileWriter &BaseFileWriter_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk::BaseFileWriter *>(self);
}

MilkTea_Exception_t MilkTea_call Milk_BaseFileReader_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseFileReader_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call Milk_BaseFileReader_Interface_Read(void *self, uint8_t bytes[], size_t *len) MilkTea_with_except({
  MilkTea_nonnull(len);
  *len = BaseFileReader_cast(self).Read(bytes, *len);
})

MilkTea_Exception_t MilkTea_call Milk_BaseFileWriter_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseFileWriter_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call Milk_BaseFileWriter_Interface_Write(void *self, const uint8_t bytes[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(bytes);
  BaseFileWriter_cast(self).Write(bytes, len);
})

} // namespace

namespace Milk {

const BaseFileReader::interface_type &BaseFileReader::Interface() {
  static const interface_type instance_{
    .Deleter = Milk_BaseFileReader_Interface_Deleter,
    .Read = Milk_BaseFileReader_Interface_Read,
  };
  return instance_;
}

const BaseFileWriter::interface_type &BaseFileWriter::Interface() {
  static const interface_type instance_{
    .Deleter = Milk_BaseFileWriter_Interface_Deleter,
    .Write = Milk_BaseFileWriter_Interface_Write,
  };
  return instance_;
}

} // namespace Milk
