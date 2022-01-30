#ifndef LIB_MILKTEA_CORE_TRANSPORT_H_
#define LIB_MILKTEA_CORE_TRANSPORT_H_

#include <milktea/core/except.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MilkTea_Reader_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *self);
  MilkTea_Exception_t (MilkTea_call *Read)(void *self, uint8_t bytes[], size_t *len);
};

#ifndef __cplusplus
typedef struct MilkTea_Reader_Interface_t MilkTea_Reader_Interface_t;
#endif

struct MilkTea_Reader_t {
  void *self_;
  const MilkTea_Reader_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct MilkTea_Reader_t MilkTea_Reader_t;
#endif

MilkTea_api
MilkTea_Reader_Destroy(MilkTea_Reader_t self);

MilkTea_api
MilkTea_Reader_Read(MilkTea_Reader_t self, uint8_t bytes[], size_t *len);

struct MilkTea_Writer_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *self);
  MilkTea_Exception_t (MilkTea_call *Write)(void *self, const uint8_t bytes[], size_t len);
};

#ifndef __cplusplus
typedef struct MilkTea_Writer_Interface_t MilkTea_Writer_Interface_t;
#endif

struct MilkTea_Writer_t {
  void *self_;
  const MilkTea_Writer_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct MilkTea_Writer_t MilkTea_Writer_t;
#endif

MilkTea_api
MilkTea_Writer_Destroy(MilkTea_Writer_t self);

MilkTea_api
MilkTea_Writer_Write(MilkTea_Writer_t self, const uint8_t bytes[], size_t len);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_CORE_TRANSPORT_H_
