#ifndef MILK_FILEIO_H_
#define MILK_FILEIO_H_

#include <milk/common.h>

namespace Milk {

class FileReaderWrapper final : public BaseFileReader {
  static constexpr char TAG[] = "Milk::FileReaderWrapper";
  using raw_type = Milk_FileReader_t;
  using interface_type = Milk_FileReader_Interface_t;
 public:
  FileReaderWrapper(raw_type another = {}) : self_(another) {}
  FileReaderWrapper(FileReaderWrapper &&another) : FileReaderWrapper(another.release()) {}
  raw_type ToRawType() && final {
    return release();
  }
  ~FileReaderWrapper() final {
    raw_type self = release();
    if (self.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(self.interface_->Destroy, self.self_);
  }
  BaseFileReader &Move() && final {
    return *new FileReaderWrapper(release());
  }
  void Destroy() && final {
    delete this;
  }
  size_t Read(uint8_t bytes[], size_t len) final {
    MilkTea_invoke_panic(interface().Read, self(), bytes, &len);
    return len;
  }
 private:
  raw_type get() const {
    return self_;
  }
  raw_type release() {
    raw_type result = {};
    std::swap(self_, result);
    return result;
  }
  void *self() const {
    return get().self_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  raw_type self_;
  MilkTea_NonCopy(FileReaderWrapper)
  MilkTea_NonMoveAssign(FileReaderWrapper)
};

class FileWriterWrapper final : public BaseFileWriter {
  static constexpr char TAG[] = "Milk::FileWriterWrapper";
  using raw_type = Milk_FileWriter_t;
  using interface_type = Milk_FileWriter_Interface_t;
 public:
  FileWriterWrapper(raw_type another = {}) : self_(another) {}
  FileWriterWrapper(FileWriterWrapper &&another) : FileWriterWrapper(another.release()) {}
  raw_type ToRawType() && final {
    return release();
  }
  ~FileWriterWrapper() final {
    raw_type self = release();
    if (self.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(self.interface_->Destroy, self.self_);
  }
  BaseFileWriter &Move() && final {
    return *new FileWriterWrapper(release());
  }
  void Destroy() && final {
    delete this;
  }
  void Write(const uint8_t bytes[], size_t len) final {
    MilkTea_invoke_panic(interface().Write, self(), bytes, len);
  }
 private:
  raw_type get() const {
    return self_;
  }
  raw_type release() {
    raw_type result = {};
    std::swap(self_, result);
    return result;
  }
  void *self() const {
    return get().self_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  raw_type self_;
  MilkTea_NonCopy(FileWriterWrapper)
  MilkTea_NonMoveAssign(FileWriterWrapper)
};

} // namespace Milk

#endif // ifndef MILK_FILEIO_H_
