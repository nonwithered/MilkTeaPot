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
  auto &factory_ = BaseContext_cast(self).GetSoyBeanFactory();
  MilkTea::Defer defer([&factory_]() {
    std::move(factory_).Destroy();
  });
  *factory = std::move(factory_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetPrinterOut(void *self, MilkTea_Writer_t *writer) MilkTea_with_except({
  MilkTea_nonnull(writer);
  auto &writer_ = BaseContext_cast(self).GetPrinterOut();
  MilkTea::Defer defer([&writer_]() {
    std::move(writer_).Destroy();
  });
  *writer = std::move(writer_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetPrinterErr(void *self, MilkTea_Writer_t *writer) MilkTea_with_except({
  MilkTea_nonnull(writer);
  auto &writer_ = BaseContext_cast(self).GetPrinterErr();
  MilkTea::Defer defer([&writer_]() {
    std::move(writer_).Destroy();
  });
  *writer = std::move(writer_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetFileReader(void *self, MilkTea_Reader_t *reader, const char name[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(name);
  std::string s;
  std::string_view sv;
  if (name[len] == '\0') {
    sv = std::string_view(name, len);
  } else {
    s = std::string(name, len);
    sv = s;
  }
  auto &reader_ = BaseContext_cast(self).GetFileReader(sv);
  MilkTea::Defer defer([&reader_]() {
    std::move(reader_).Destroy();
  });
  *reader = std::move(reader_).ToRawType();
})

MilkTea_Exception_t MilkTea_call Milk_Context_Interface_GetFileWriter(void *self, MilkTea_Writer_t *writer, const char name[], size_t len) MilkTea_with_except({
  MilkTea_nonnull(name);
  std::string s;
  std::string_view sv;
  if (name[len] == '\0') {
    sv = std::string_view(name, len);
  } else {
    s = std::string(name, len);
    sv = s;
  }
  auto &writer_ = BaseContext_cast(self).GetFileWriter(sv);
  MilkTea::Defer defer([&writer_]() {
    std::move(writer_).Destroy();
  });
  *writer = std::move(writer_).ToRawType();
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
