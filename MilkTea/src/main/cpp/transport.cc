#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea#transport";

MilkTea::BaseReader &BaseReader_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<MilkTea::BaseReader *>(self);
}

MilkTea::BaseWriter &BaseWriter_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<MilkTea::BaseWriter *>(self);
}

MilkTea_Exception_t MilkTea_call MilkTea_BaseReader_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseReader_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call MilkTea_FileReader_Interface_Read(void *self, uint8_t bytes[], size_t *len) MilkTea_with_except({
  MilkTea_nonnull(len);
  *len = BaseReader_cast(self).Read(bytes, *len);
})

MilkTea_Exception_t MilkTea_call Milk_BaseWriter_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseWriter_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call Milk_BaseWriter_Interface_Write(void *self, const uint8_t bytes[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(bytes);
  BaseWriter_cast(self).Write(bytes, len);
})

} // namespace

namespace MilkTea {

const BaseReader::interface_type &BaseReader::Interface() {
  static const interface_type instance_{
    .Deleter = MilkTea_BaseReader_Interface_Deleter,
    .Read = MilkTea_FileReader_Interface_Read,
  };
  return instance_;
}

const BaseWriter::interface_type &BaseWriter::Interface() {
  static const interface_type instance_{
    .Deleter = Milk_BaseWriter_Interface_Deleter,
    .Write = Milk_BaseWriter_Interface_Write,
  };
  return instance_;
}

} // namespace Milk

MilkTea_extern(MilkTea_Reader_Destroy, (MilkTea_Reader_t self), {
    return self.interface_->Deleter(self.self_);
})

MilkTea_extern(MilkTea_Reader_Read, (MilkTea_Reader_t self, uint8_t bytes[], size_t *len), {
    return self.interface_->Read(self.self_, bytes, len);
})

MilkTea_extern(MilkTea_Writer_Destroy, (MilkTea_Writer_t self), {
    return self.interface_->Deleter(self.self_);
})

MilkTea_extern(MilkTea_Writer_Write, (MilkTea_Writer_t self, const uint8_t bytes[], size_t len), {
    return self.interface_->Write(self.self_, bytes, len);
})
