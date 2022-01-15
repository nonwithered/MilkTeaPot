#include <milk/common.h>

namespace {

constexpr char TAG[] = "Milk#Context";

Milk::BaseContext &BaseContext_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk::BaseContext *>(self);
}

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_SetLogLevel(void *self, MilkTea_Logger_Level_t level) MilkTea_with_except({
  BaseContext_cast(self).SetLogLevel(MilkTea::Logger::FromRawType(level));
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetSoyBeanFactory(void *self, SoyBean_Factory_t *factory) MilkTea_with_except({
  MilkTea_nonnull(factory);
  auto *factory_ = BaseContext_cast(self).GetSoyBeanFactory().release();
  MilkTea::Defer defer([factory_]() {
    std::move(*factory_).Destroy();
  });
  *factory = std::move(*factory_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetPrinterOut(void *self, Milk_Printer_t *printer) MilkTea_with_except({
  MilkTea_nonnull(printer);
  *printer = BaseContext_cast(self).GetPrinterOut().ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetPrinterErr(void *self, Milk_Printer_t *printer) MilkTea_with_except({
  MilkTea_nonnull(printer);
  *printer = BaseContext_cast(self).GetPrinterErr().ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetFileReader(void *self, Milk_FileReader_t *reader, const char name[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(name);
  auto *reader_ = BaseContext_cast(self).GetFileReader(name, len).release();
  MilkTea::Defer defer([reader_]() {
    std::move(*reader_).Destroy();
  });
  *reader = std::move(*reader_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetFileWriter(void *self, Milk_FileWriter_t *writer, const char name[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(name);
  auto *writer_ = BaseContext_cast(self).GetFileWriter(name, len).release();
  MilkTea::Defer defer([writer_]() {
    std::move(*writer_).Destroy();
  });
  *writer = std::move(*writer_).ToRawType();
})

} // namespace

namespace Milk {

const BaseContext::interface_type &BaseContext::Interface() {
  static const interface_type instance_{
    .SetLogLevel = Milk_Context_Interface_SetLogLevel,
    .GetSoyBeanFactory = Milk_Context_Interface_GetSoyBeanFactory,
    .GetPrinterOut = Milk_Context_Interface_GetPrinterOut,
    .GetPrinterErr = Milk_Context_Interface_GetPrinterErr,
    .GetFileReader = Milk_Context_Interface_GetFileReader,
    .GetFileWriter = Milk_Context_Interface_GetFileWriter,
  };
  return instance_;
}

} // namespace Milk
