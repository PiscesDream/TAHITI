#ifndef _STDINT_H_
#define _STDINT_H_

// define as ISO C99: 7.18 Integer types <stdint.h>
typedef signed char                             int8_t;
typedef short                                   int16_t;
typedef long int                                int32_t;
typedef long long int                           int64_t;

typedef unsigned char                           uint8_t;
typedef unsigned short                          uint16_t;
typedef unsigned long int                       uint32_t;
typedef unsigned long long int                  uint64_t;

/* Maximum of signed integral types.  */
#define INT8_MAX                127
#define INT16_MAX               32767
#define INT32_MAX               2147483647L
#define INT64_MAX               9223372036854775807LL

/* Minimum of signed integral types.  */
#define INT8_MIN                (-INT8_MAX - 1)
#define INT16_MIN               (-INT16_MAX - 1)
#define INT32_MIN               (-INT32_MAX - 1)
#define INT64_MIN               (-INT64_MAX - 1)

/* Maximum of unsigned integral types.  */
#define UINT8_MAX               255U
#define UINT16_MAX              65535U
#define UINT32_MAX              4294967295UL
#define UINT64_MAX              18446744073709551615ULL

#define INTMAX_MIN              INT64_MIN
#define INTMAX_MAX              INT64_MAX
#define UINTMAX_MAX             UINT64_MAX

#endif

