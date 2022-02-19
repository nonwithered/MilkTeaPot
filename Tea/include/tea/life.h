#ifndef LIB_TEA_LIFE_H_
#define LIB_TEA_LIFE_H_

#ifdef __cplusplus

namespace tea {

class remove_constructor {
  remove_constructor() = delete;
};

class remove_destructor {
  ~remove_destructor() = delete;
};

template<typename T>
class remove_copy {
  remove_copy(const T &) = delete;
  auto operator=(const T &) -> void = delete;
};

template<typename T>
class remove_move {
  remove_move(T &&) = delete;
  auto operator=(T &&) -> void = delete;
};

template<typename T>
class remove_assign {
  auto operator=(const T &) -> void = delete;
  auto operator=(T &&) -> void = delete;
};

class empty_class : remove_constructor, remove_destructor {
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_LIFE_H_
