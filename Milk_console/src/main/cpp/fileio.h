#ifndef MILK_CONSOLE_FILEIO_H_
#define MILK_CONSOLE_FILEIO_H_

#include <fstream>

#include <milk/common.h>

namespace Milk_Console {

class FileReaderImpl final : public Milk::BaseFileReader {
  static constexpr char TAG[] = "Milk_Console::FileReaderImpl";
 public:
  FileReaderImpl(std::ifstream ifs) : ifs_(std::move(ifs)) {}
  FileReaderImpl(FileReaderImpl &&another) : FileReaderImpl(std::move(another.ifs_)) {}
  ~FileReaderImpl() final  = default;
  BaseFileReader &Move() && final {
    return *new FileReaderImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  size_t Read(uint8_t bytes[], size_t len) final {
   if (ifs_.eof()) {
      return 0;
    }
    if (bytes != nullptr) {
      ifs_.read(reinterpret_cast<char *>(bytes), len);
    } else {
      ifs_.ignore(len);
    }
    return ifs_.gcount();
  }
 private:
  std::ifstream ifs_;
};

class FileWriterImpl final : public Milk::BaseFileWriter {
  static constexpr char TAG[] = "Milk_Console::FileWriterImpl";
 public:
  FileWriterImpl(std::ofstream ofs) : ofs_(std::move(ofs)) {}
  FileWriterImpl(FileWriterImpl &&another) : FileWriterImpl(std::move(another.ofs_)) {}
  ~FileWriterImpl() final  = default;
  BaseFileWriter &Move() && final {
    return *new FileWriterImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Write(const uint8_t bytes[], size_t len) final {
    ofs_.write(reinterpret_cast<const char *>(bytes), len);
  }
 private:
  std::ofstream ofs_;
};

} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_FILEIO_H_
