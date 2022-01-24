#ifndef LIB_MILK_COMMON_FILEIO_H_
#define LIB_MILK_COMMON_FILEIO_H_

#include <tuple>
#include <functional>

#include <milk/core.h>

namespace Milk {

class BaseFileReader {
  using raw_type = Milk_FileReader_t;
  using interface_type = Milk_FileReader_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .self_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseFileReader() = default;
  virtual BaseFileReader &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual size_t Read(uint8_t [], size_t) = 0;
  operator std::function<size_t(uint8_t [], size_t)>() {
    return [this](auto bytes[], auto len) -> auto {
      return Read(bytes, len);
    };
  }
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

class BaseFileWriter {
  using raw_type = Milk_FileWriter_t;
  using interface_type = Milk_FileWriter_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .self_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseFileWriter() = default;
  virtual BaseFileWriter &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void Write(const uint8_t [], size_t) = 0;
  operator std::function<void(const uint8_t [], size_t)>() {
    return [this](const auto bytes[], auto len) {
      Write(bytes, len);
    };
  }
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace Milk

#endif // ifndef LIB_MILK_COMMON_FILEIO_H_
