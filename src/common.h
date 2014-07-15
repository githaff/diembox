#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>


typedef int8_t  s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;
typedef int64_t s64_t;

#define BIT(VAL, POS) ((VAL >> POS) & 0x1)
#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

#define COLOR_BOLD "\033[4;97m"
#define COLOR_NONE "\033[0m"
#define COLOR_TEXT(STR) (COLOR_BOLD STR COLOR_NONE)

#define err_msg(...) fprintf(stderr, "Error: " __VA_ARGS__)
#define dbg_msg(...) fprintf(stderr, __VA_ARGS__)


#endif /* __COMMON_H */
