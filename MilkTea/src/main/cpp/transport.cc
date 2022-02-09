#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea#transport";

MilkTea::BaseReader &BaseReader_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<MilkTea::BaseReader *>(obj);
}

MilkTea::BaseWriter &BaseWriter_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<MilkTea::BaseWriter *>(obj);
}

MilkTea_Exception_t MilkTea_call MilkTea_BaseReader_Interface_Deleter(void *obj) MilkTea_with_except({
  std::move(BaseReader_cast(obj)).Destroy();
})

MilkTea_Exception_t MilkTea_call MilkTea_FileReader_Interface_Read(void *obj, uint8_t bytes[], size_t *len) MilkTea_with_except({
  MilkTea_nonnull(len);
  *len = BaseReader_cast(obj).Read(bytes, *len);
})

MilkTea_Exception_t MilkTea_call Milk_BaseWriter_Interface_Deleter(void *obj) MilkTea_with_except({
  std::move(BaseWriter_cast(obj)).Destroy();
})

MilkTea_Exception_t MilkTea_call Milk_BaseWriter_Interface_Write(void *obj, const uint8_t bytes[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(bytes);
  BaseWriter_cast(obj).Write(bytes, len);
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

MilkTea_extern(MilkTea_Reader_Destroy, (MilkTea_Reader_t obj), {
    return obj.interface_->Deleter(obj.obj_);
})

MilkTea_extern(MilkTea_Reader_Read, (MilkTea_Reader_t obj, uint8_t bytes[], size_t *len), {
    return obj.interface_->Read(obj.obj_, bytes, len);
})

MilkTea_extern(MilkTea_Writer_Destroy, (MilkTea_Writer_t obj), {
    return obj.interface_->Deleter(obj.obj_);
})

MilkTea_extern(MilkTea_Writer_Write, (MilkTea_Writer_t obj, const uint8_t bytes[], size_t len), {
    return obj.interface_->Write(obj.obj_, bytes, len);
})
