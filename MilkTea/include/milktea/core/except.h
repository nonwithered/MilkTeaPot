#ifndef LIB_MILKTEA_CORE_EXCEPT_H_
#define LIB_MILKTEA_CORE_EXCEPT_H_

#include <milktea/core/macro.h>

#define MilkTea_error MilkTea_Exception_t

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_Exception_t {
  MilkTea_Exception_Nil,
  MilkTea_Exception_Unknown,
  MilkTea_Exception_Assertion,
  MilkTea_Exception_NullPointer,
  MilkTea_Exception_Unsupported,
  MilkTea_Exception_InvalidParam,
  MilkTea_Exception_LogicError,
  MilkTea_Exception_EndOfFile
};

#ifndef __cplusplus
typedef enum MilkTea_Exception_t MilkTea_Exception_t;
#endif

MilkTea_decl(const char *)
MilkTea_Exception_What();

MilkTea_noreturn
MilkTea_decl(void)
MilkTea_Exception_Throw(MilkTea_Exception_t, const char *);

#define MilkTea_throw(type, what) MilkTea_block({ \
  MilkTea_Exception_Throw(MilkTea_Exception_##type, what); \
})

#define MilkTea_throwf(type, ...) MilkTea_block({ \
  const size_t size_ = snprintf(MilkTea_null, 0, ##__VA_ARGS__) + 1; \
  char what_[size_]; \
  snprintf(what_, size_, ##__VA_ARGS__); \
  MilkTea_throw(type, what_); \
})

#define MilkTea_assert(what) \
MilkTea_throw(Assertion, what)

#define MilkTea_assertf(...) \
MilkTea_throwf(Assertion, ##__VA_ARGS__)

#define MilkTea_nonnull(sym) MilkTea_block({ \
  if (sym == MilkTea_null) { \
    MilkTea_throw(NullPointer, #sym " is null"); \
  } \
})

#define MilkTea_panic(e) MilkTea_block({ \
  if (e != MilkTea_Exception_Nil) { \
    MilkTea_Exception_Throw(e, MilkTea_Exception_What()); \
  } \
})

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_CORE_EXCEPT_H_
