#ifndef LIB_MILKTEA_COMMON_TRANSPORT_H_
#define LIB_MILKTEA_COMMON_TRANSPORT_H_

#include <tuple>
#include <functional>

#include <milktea/core.h>

namespace MilkTea {

class BaseReader {
  using raw_type = MilkTea_Reader_t;
  using interface_type = MilkTea_Reader_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .obj_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseReader() = default;
  virtual BaseReader &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual size_t Read(uint8_t [], size_t) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

class BaseWriter {
  using raw_type = MilkTea_Writer_t;
  using interface_type = MilkTea_Writer_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .obj_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseWriter() = default;
  virtual BaseWriter &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void Write(const uint8_t [], size_t) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_TRANSPORT_H_
