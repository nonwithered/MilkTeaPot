#ifndef LIB_TEA_FILE_H_
#define LIB_TEA_FILE_H_

#ifdef __cplusplus
#include <tea/func.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct tea_file_input_t {
  struct tea_file_input_capture_t *capture;
  size_t (TEA_CALL *invoke)(struct tea_file_input_capture_t *, uint8_t [], size_t);
  void (TEA_CALL *close)(struct tea_file_input_capture_t *);
};

struct tea_file_output_t {
  struct tea_file_output_capture_t *capture;
  size_t (TEA_CALL *invoke)(struct tea_file_output_capture_t *, const uint8_t [], size_t);
  void (TEA_CALL *close)(struct tea_file_output_capture_t *);
};

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace tea {
namespace file {

using input = tea_file_input_t;
using output = tea_file_output_t;

} // namespace file

template<>
struct meta::func_sign<file::input> {
  using type = size_t(uint8_t *, size_t);
};

template<>
struct meta::func_sign<file::output> {
  using type = size_t(const uint8_t *, size_t);
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_FILE_H_
