#ifndef MILK_PRINTER_H_
#define MILK_PRINTER_H_

#include <milk/common.h>

namespace Milk {

class PrinterWripper final : public BasePrinter {
  static constexpr char TAG[] = "Milk::PrinterWripper";
  using raw_type = Milk_Printer_t;
  using interface_type = Milk_Printer_Interface_t;
 public:
  PrinterWripper(raw_type another = {}) : self_(another) {}
  raw_type ToRawType() final {
    return self_;
  }
  void Print(const char msg[], size_t len) final {
    MilkTea_invoke_panic(interface().Print, self(), msg, len);
  }
 private:
  void *self() const {
    return get().self_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  const raw_type &get() const {
    return self_;
  }
  raw_type self_;
};

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

class BufferPrinter final {
 public:
  explicit BufferPrinter(BasePrinter &printer) : printer_(printer), buf_() {}
  BufferPrinter &operator()(std::initializer_list<StringWrapper> s) {
    for (const auto &it : s) {
      buf_ += it;
    }
    return *this;
  }
  template<typename T>
  BufferPrinter &operator<<(T t) {
    std::stringstream ss;
    ss << t;
    std::string s;
    ss >> s;
    buf_ += s;
    return *this;
  }
  BufferPrinter &operator<<(std::string s) {
    buf_ += s;
    return *this;
  }
  BufferPrinter &operator<<(std::string_view s) {
    buf_ += s;
    return *this;
  }
  BufferPrinter &operator<<(const char s[]) {
    buf_ += s;
    return *this;
  }
  BufferPrinter &operator<<(std::nullptr_t) {
    printer_.Print(buf_.data(), buf_.size());
    buf_.clear();
    return *this;
  }
  ~BufferPrinter() {
    if (!buf_.empty()) {
      *this << nullptr;
    }
  }
 private:
  BasePrinter &printer_;
  std::string buf_;
  MilkTea_NonCopy(BufferPrinter)
  MilkTea_NonMove(BufferPrinter)
};

} // namespace Milk

#endif // ifndef MILK_PRINTER_H_
