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
 * Extended option description structure.
 */
struct extopt {
    /* Equivalent of 'struct option' */
    const char *name_long;
    int has_arg;
    int *flag;
    int val;

    /* Added fields */
    char name_short;
    const char *arg_name;
    char *desc;
};


#define EXTOPTS_END { 0, 0, 0, 0, 0, 0, 0 }


void print_usage(struct extopt *opts);


#endif /* __EXTOPTS_H */
