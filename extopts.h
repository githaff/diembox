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
    char *name_long;
    char name_short;
    int has_arg;
    char *arg_name;
    char *desc;
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


#define EXTOPTS_END { 0, 0, 0, 0, 0 }
#define EXTOPTS_ORIG_END { 0, 0, 0, 0, 0, 0, 0 }


void extopts_usage(struct extopt *opts);
void extopts_usage_orig(struct extopt_orig *opts);


#endif /* __EXTOPTS_H */
