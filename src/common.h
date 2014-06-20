#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>


typedef int8_t   s8_t;
typedef uint8_t  u8_t;
typedef int16_t  s16_t;
typedef uint16_t u16_t;
typedef int32_t  s32_t;
typedef uint32_t u32_t;
typedef int64_t  s64_t;
typedef uint64_t u64_t;

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

#define err_msg(...) fprintf(stderr, "Error: " __VA_ARGS__)
#define dbg_msg(...) fprintf(stderr, __VA_ARGS__)


#endif /* __COMMON_H */
