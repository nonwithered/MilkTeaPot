#ifndef LIB_MILKTEA_CORE_EXCEPT_H_
#define LIB_MILKTEA_CORE_EXCEPT_H_

#include <milktea/core/macro.h>
#include <milktea/core/closure.h>

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
  MilkTea_Exception_IOError
};

#ifndef __cplusplus
typedef enum MilkTea_Exception_t MilkTea_Exception_t;
#endif

MilkTea_decl(const char *)
MilkTea_Exception_What();

MilkTea_noreturn
MilkTea_decl(void)
MilkTea_Exception_Throw(MilkTea_Exception_t, const char *);

MilkTea_Function_t(MilkTea_Exception_Block_t, void)

#ifndef __cplusplus
typedef struct MilkTea_Exception_Block_t MilkTea_Exception_Block_t;
#endif

MilkTea_api
MilkTea_Exception_Catch(MilkTea_Exception_Block_t block);

#ifndef MilkTea_throw_what_size
#define MilkTea_throw_what_size 1024
#endif // ifndef MilkTea_throw_what_size

#ifdef MilkTea_ignore_throw
#define MilkTea_throwf(...) MilkTea_block({})
#else // ifdef MilkTea_ignore_throw
#define MilkTea_throwf(type, format, ...) MilkTea_block({ \
  char what_[MilkTea_throw_what_size]; \
  snprintf(what_, MilkTea_throw_what_size, "%s(%d): " format, TAG, __LINE__, ##__VA_ARGS__); \
  MilkTea_Exception_Throw(MilkTea_Exception_##type, what_); \
})
#endif // ifdef MilkTea_ignore_throw

#define MilkTea_throw(type, what) MilkTea_block({ \
  MilkTea_throwf(type, "%s", what); \
})

#define MilkTea_stub(sym) \
MilkTea_throw(Unsupported, #sym ": Stub!")

#ifdef NDEBUG
#define MilkTea_ignore_assert
#endif // ifdef NDEBUG

#ifdef MilkTea_ignore_assert
#define MilkTea_assert(what) MilkTea_block({})
#else // ifdef MilkTea_ignore_assert
#define MilkTea_assert(what) \
MilkTea_throw(Assertion, what)
#endif // ifdef MilkTea_ignore_assert

#ifdef MilkTea_ignore_assert
#define MilkTea_assertf(...) MilkTea_block({})
#else // ifdef MilkTea_ignore_assert
#define MilkTea_assertf(...) \
MilkTea_throwf(Assertion, ##__VA_ARGS__)
#endif // ifdef MilkTea_ignore_assert

#define MilkTea_nonnull(sym) MilkTea_block({ \
  if (sym == MilkTea_null) { \
    MilkTea_throw(NullPointer, #sym " is null"); \
  } \
})

#define MilkTea_panic(e) MilkTea_block({ \
  MilkTea_Exception_t e_ = e; \
  if (e_ != MilkTea_Exception_Nil) { \
    MilkTea_Exception_Throw(e_, MilkTea_Exception_What()); \
  } \
})

#define MilkTea_invoke_panic(f, ...) \
MilkTea_panic(f(__VA_ARGS__))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_CORE_EXCEPT_H_
