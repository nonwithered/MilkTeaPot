#ifndef MILKPOWDER_EXCEPT_H_
#define MILKPOWDER_EXCEPT_H_

#include <milktea.h>

MilkTea_Exception_What_DECL(MilkPowder)

#define THROW MILKTEA_EXCEPTION_THROW
#define THROW_FORMAT MILKTEA_EXCEPTION_THROW_FORMAT
#define THROW_NULL MILKTEA_EXCEPTION_THROW_NULL
#define WITH_THROW(...) MILKTEA_EXCEPTION_WITH(MilkPowder, ##__VA_ARGS__)

#endif // ifndef MILKPOWDER_EXCEPT_H_
