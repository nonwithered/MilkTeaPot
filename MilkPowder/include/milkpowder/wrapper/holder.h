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
  const raw_type *const self_;
 public:
  const raw_type *get() const final {
    return self_;
  }
 public:
  ConstWrapper(const typename T::raw_type *self) : self_(self) {}
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(T::raw_dump(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
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
  raw_type *self_;
 public:
  const raw_type *get() const final {
    return self_;
  }
  raw_type *get() final {
    return self_;
  }
 public:
  MutableWrapper(raw_type *self) : self_(self) {}
  ~MutableWrapper() {
    raw_type *self = nullptr;
    std::swap(self_, self);
    if (self == nullptr) {
      return;
    }
    MilkTea_panic(T::raw_destroy(self));
  }
  MutableWrapper(const ConstWrapper<T> &another) : MutableWrapper(nullptr) {
    const raw_type *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    raw_type *self = nullptr;
    MilkTea_panic(T::raw_clone(another_, &self));
    std::swap(self_, self);
  }
  MutableWrapper(const MutableWrapper<T> &another) : MutableWrapper(nullptr) {
    const raw_type *another_ = another.get();
    if (another_ == nullptr) {
      return;
    }
    raw_type *self = nullptr;
    MilkTea_panic(T::raw_clone(another_, &self));
    std::swap(self_, self);
  }
  MutableWrapper(MutableWrapper<T> &&another) : MutableWrapper(nullptr) {
    std::swap(self_, another.self_);
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
    new (this) MutableWrapper<T>(std::forward<MutableWrapper<T>>(another));
  }
  raw_type *release() {
    raw_type *self = nullptr;
    std::swap(self_, self);
    return self;
  }
  void Dump(std::function<void(const uint8_t *, size_t)> callback) const {
    MilkTea_panic(T::raw_dump(get(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
};

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_HOLDER_H_
