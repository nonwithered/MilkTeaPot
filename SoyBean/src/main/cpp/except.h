#ifndef SOYBEAN_EXCEPT_H_
#define SOYBEAN_EXCEPT_H_

#include <milktea.h>

#define THROW MILKTEA_EXCEPTION_THROW
#define THROW_FORMAT MILKTEA_EXCEPTION_THROW_FORMAT
#define THROW_NULL MILKTEA_EXCEPTION_THROW_NULL
#define WITH_THROW(...) MILKTEA_EXCEPTION_WITH(SoyBean, ##__VA_ARGS__)

#endif // ifndef SOYBEAN_EXCEPT_H_
