#ifndef __EXTOPTS_H
#define __EXTOPTS_H

/*
 * Extended Options management.
 *
 * This is implementation of extended options based on getopt_long
 * from getopt.h functionality.
 *
 * Major advantage is self-documenting ability - pretty options usage
 * can be generated and printed through extopts_usage() function.
 *
 * Usage is changed into a little more simple. Full copy of original
 * getopt functionality can be implemented through _orig structures
 * and functions (in this case extopt_orig will be only wrapping
 * getopt_long implementing identical behaviour so it will be fully
 * compatible).
 */

#include <getopt.h>


/*
 * Types of option argument.
 * Defines the way argument will be written into @arg field of extopt
 * structure.
 */
enum extopt_argtype {
    /* Field 'arg' will be used as 'flag' pointing to flag for whether
     * parameter was met in command line or not */
    EXTOPT_ARGTYPE_NO_ARG,
    /* Field 'arg' will be used as 'addr' pointing to the variable of
     * corresponding size where parsed parameter argument value will
     * be stored */
    EXTOPT_ARGTYPE_STR,
    EXTOPT_ARGTYPE_INT,
    EXTOPT_ARGTYPE_CHAR,
    /* Field 'arg' will be used as 'setter' handler which will be
     * called for argument parsing */
    EXTOPT_ARGTYPE_SPECIAL,
};

/*
 * Extended option description structure.
 */
struct extopt {
    char *name_long;
    char name_short;
    char *desc;
    
    /* Option argument */
    int has_arg;
    char *arg_name;
    enum extopt_argtype arg_type;
    union {
        void *addr;
        int *flag_addr;
        int (*setter)(struct extopt *opt, char *arg);
    } arg;
};

/* Should be used as end of extopt array */
#define EXTOPTS_END { 0, 0, 0, 0, 0, 0, {0} }

/* Macro for extopt struct initialization depending on option argument
 * type */
#define EXTOPT_NO_ARG(FLAG_ADDR)                \
    .has_arg = no_argument,                     \
        .arg_name = NULL,                       \
        .arg_type = EXTOPT_ARGTYPE_NO_ARG,      \
        .arg.flag_addr = FLAG_ADDR
#define EXTOPT_ARG_INT(NAME, ADDR)               \
    .has_arg = required_argument,                \
        .arg_name = NAME,                        \
        .arg_type = EXTOPT_ARGTYPE_INT,          \
        .arg.addr = ADDR
#define EXTOPT_ARG_STR(NAME, ADDR)               \
    .has_arg = required_argument,                \
        .arg_name = NAME,                        \
        .arg_type = EXTOPT_ARGTYPE_STR,          \
        .arg.addr = ADDR
#define EXTOPT_ARG_CHAR(NAME, ADDR)              \
    .has_arg = required_argument,                \
        .arg_name = NAME,                        \
        .arg_type = EXTOPT_ARGTYPE_CHAR,         \
        .arg.addr = ADDR
#define EXTOPT_ARG_SPECIAL(NAME, SETTER_FUNC)    \
    .has_arg = required_argument,                \
        .arg_name = NAME,                        \
        .arg_type = EXTOPT_ARGTYPE_SPECIAL,      \
        .arg.setter = SETTER_FUNC


void extopts_usage(struct extopt *opts);
int get_extopt(int argc, char *argv[], struct extopt *opts);

inline static char opt_is_end(struct extopt opt)
{
    return opt.name_long  == 0 &&
           opt.name_short == 0 &&
           opt.has_arg    == 0 &&
           opt.arg_name   == 0 &&
           opt.desc       == 0 &&
           opt.arg_type   == 0 &&
           opt.arg.addr   == 0;
}

#endif /* __EXTOPTS_H */
