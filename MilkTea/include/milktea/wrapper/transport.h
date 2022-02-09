#ifndef LIB_MILKTEA_WRAPPER_TRANSPORT_H_
#define LIB_MILKTEA_WRAPPER_TRANSPORT_H_

#include <milktea/common.h>

namespace MilkTea {

class ReaderWrapper final : public BaseReader {
  static constexpr char TAG[] = "MilkTea::ReaderWrapper";
  using raw_type = MilkTea_Reader_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  ReaderWrapper(raw_type another = {}) : obj_(another) {}
  ReaderWrapper(ReaderWrapper &&another) : ReaderWrapper(another.release()) {}
  ~ReaderWrapper() final {
    auto obj = release();
    if (obj.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(MilkTea_Reader_Destroy, obj);
  }
  BaseReader &Move() && final {
    return *new ReaderWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  size_t Read(uint8_t bytes[], size_t len) final {
    MilkTea_invoke_panic(MilkTea_Reader_Read, get(), bytes, &len);
    return len;
  }
  raw_type release() {
    raw_type result = {};
    std::swap(obj_, result);
    return result;
  }
  raw_type get() {
    return obj_;
  }
private:
  raw_type obj_;
  MilkTea_NonCopy(ReaderWrapper)
  MilkTea_NonMoveAssign(ReaderWrapper)
};

class WriterWrapper final : public BaseWriter {
  static constexpr char TAG[] = "MilkTea::WriterWrapper";
  using raw_type = MilkTea_Writer_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  WriterWrapper(raw_type another = {}) : obj_(another) {}
  WriterWrapper(WriterWrapper &&another) : WriterWrapper(another.release()) {}
  ~WriterWrapper() final {
    auto obj = release();
    if (obj.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(MilkTea_Writer_Destroy, obj);
  }
  BaseWriter &Move() && final {
    return *new WriterWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Write(const uint8_t bytes[], size_t len) final {
    MilkTea_invoke_panic(MilkTea_Writer_Write, get(), bytes, len);
  }
  raw_type release() {
    raw_type result = {};
    std::swap(obj_, result);
    return result;
  }
  raw_type get() {
    return obj_;
  }
private:
  raw_type obj_;
  MilkTea_NonCopy(WriterWrapper)
  MilkTea_NonMoveAssign(WriterWrapper)
};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_TRANSPORT_H_
