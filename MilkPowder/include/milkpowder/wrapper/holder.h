#ifndef LIB_MILKPOWDER_WRAPPER_HOLDER_H_
#define LIB_MILKPOWDER_WRAPPER_HOLDER_H_

#include <utility>

#include <milkpowder/common.h>

namespace MilkPowder {

template<typename T>
class ConstInterface {
  using raw_type = typename T::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  virtual ~ConstInterface() = default;
};

template<typename T>
class ConstWrapper final : public ConstInterface<T> {
  using raw_type = typename T::raw_type;
 private:
  const raw_type *obj_;
 public:
  const raw_type *get() const final {
    return obj_;
  }
 public:
  ConstWrapper(const raw_type *obj = nullptr) : obj_(obj) {}
  ~ConstWrapper() {
    obj_ = nullptr;
  }
  ConstWrapper(const ConstWrapper<T> &another) : ConstWrapper(another.get()) {}
  void Dump(std::function<void(const uint8_t [], size_t)> writer) const {
    MilkPowder::Mapping::ByteWriter writer_ = writer;
    MilkTea_invoke_panic(T::raw_dump, get(), writer_);
  }
  const raw_type *release() {
    return reset(nullptr);
  }
  const raw_type *reset(const raw_type *obj) {
    std::swap(obj_, obj);
    return obj;
  }
  void operator=(const ConstWrapper<T> &another) {
    this->~ConstWrapper<T>();
    new (this) ConstWrapper<T>(another);
  }
};

template<typename T>
class MutableInterface {
  using raw_type = typename T::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  virtual ~MutableInterface() = default;
};

template<typename T>
class MutableWrapper final : public ConstInterface<T>, public MutableInterface<T> {
  using raw_type = typename T::raw_type;
 private:
  raw_type *obj_;
 public:
  const raw_type *get() const final {
    return obj_;
  }
  raw_type *get() final {
    return obj_;
  }
 public:
  MutableWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  ~MutableWrapper() {
    auto *obj = release();
    if (obj == nullptr) {
      return;
    }
    MilkTea_invoke_panic(T::raw_destroy, obj);
  }
  MutableWrapper(const ConstWrapper<T> &another) : MutableWrapper(nullptr) {
    const raw_type *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(T::raw_clone, another_, &obj);
    std::swap(obj_, obj);
  }
  MutableWrapper(const MutableWrapper<T> &another) : MutableWrapper(nullptr) {
    const raw_type *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(T::raw_clone, another_, &obj);
    std::swap(obj_, obj);
  }
  MutableWrapper(MutableWrapper<T> &&another) : MutableWrapper(nullptr) {
    std::swap(obj_, another.obj_);
  }
  void operator=(const ConstWrapper<T> &another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(another);
  }
  void operator=(const MutableWrapper<T> &another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(another);
  }
  void operator=(MutableWrapper<T> &&another) {
    this->~MutableWrapper<T>();
    new (this) MutableWrapper<T>(std::move(another));
  }
  raw_type *release() {
    return reset(nullptr);
  }
  raw_type *reset(raw_type *obj) {
    std::swap(obj_, obj);
    return obj;
  }
  void Dump(std::function<void(const uint8_t [], size_t)> writer) const {
    MilkPowder::Mapping::ByteWriter writer_ = writer;
    MilkTea_invoke_panic(T::raw_dump, get(), writer_);
  }
};

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_HOLDER_H_
