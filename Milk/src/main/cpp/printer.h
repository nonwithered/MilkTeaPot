#ifndef MILK_PRINTER_H_
#define MILK_PRINTER_H_

#include <milk/common.h>

namespace Milk {

struct StringWrapper final {
 public:
  StringWrapper(std::string s) : s_(std::move(s)) {}
  StringWrapper(std::string_view s) : s_(s) {}
  StringWrapper(const char *s) : s_(s) {}
  operator const std::string &() const {
    return s_;
  }
 private:
  const std::string s_;
};

class PrinterImpl final {
 public:
  explicit PrinterImpl(MilkTea::BaseWriter &writer) : writer_(writer), buf_() {}
  PrinterImpl &operator()(std::initializer_list<StringWrapper> s) {
    for (auto &it : s) {
      buf_ += it;
    }
    return *this;
  }
  template<typename T>
  PrinterImpl &operator<<(T t) {
    buf_ += MilkTea::ToString::From(t);
    return *this;
  }
  PrinterImpl &operator<<(std::string s) {
    buf_ += s;
    return *this;
  }
  PrinterImpl &operator<<(std::string_view s) {
    buf_ += s;
    return *this;
  }
  PrinterImpl &operator<<(const char s[]) {
    buf_ += s;
    return *this;
  }
  PrinterImpl &operator<<(std::nullptr_t) {
    buf_ += '\n';
    writer_.Write(reinterpret_cast<const uint8_t *>(buf_.data()), buf_.size());
    buf_.clear();
    return *this;
  }
  ~PrinterImpl() {
    if (!buf_.empty()) {
      *this << nullptr;
    }
  }
 private:
  MilkTea::BaseWriter &writer_;
  std::string buf_;
  MilkTea_NonCopy(PrinterImpl)
  MilkTea_NonMove(PrinterImpl)
};

} // namespace Milk

#endif // ifndef MILK_PRINTER_H_
