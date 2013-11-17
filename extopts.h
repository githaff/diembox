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
enum extarg_type {
    /* Will use default string transforms if needed */
    EXTARG_STR,
    EXTARG_INT,
    EXTARG_CHAR,
    /* Will call function from pointer in @arg of extopt structure */
    EXTARG_SPECIAL,
};

/*
 * Extended option description structure.
 */
struct extopt {
    char *name_long;
    char name_short;
    int has_arg;
    char *arg_name;
    char *desc;
    
    /* Returned argument */
    enum extarg_type arg_type;
    union {
        void *addr;
        void (*setter)(struct extopt *opt, char *arg);
    } arg;
};



/*
 * Extended option description structure.
 * Getopt-compatible verstion.
 */
struct extopt_orig {
    /* Equivalent of 'struct option' */
    char *name;
    int has_arg;
    int *flag;
    int val;

    /* Added fields */
    char name_short;
    char *arg_name;
    char *desc;
};


#define EXTOPTS_END { 0, 0, 0, 0, 0, 0, {0} }
#define EXTOPTS_ORIG_END { 0, 0, 0, 0, 0, 0, 0 }


void extopts_usage(struct extopt *opts);
void extopts_usage_orig(struct extopt_orig *opts);


int get_extopt(int argc, char *argv[], struct extopt *opts);
int get_extopt_orig(int argc, char *const argv[],
                    const char *optstring,
                    struct extopt_orig *opts_orig, int *longindex);


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

inline static char opt_orig_is_end(struct extopt_orig opt)
{
    return opt.name       == 0 &&
           opt.has_arg    == 0 &&
           opt.flag       == 0 &&
           opt.val        == 0 &&
           opt.name_short == 0 &&
           opt.arg_name   == 0 &&
           opt.desc       == 0;
}


#endif /* __EXTOPTS_H */
