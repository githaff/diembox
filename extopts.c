#include <stdlib.h>
#include <getopt.h>

#include "extopts.h"


/*
 * Getopt-compatible version of extopts parser. Behaves like
 * getopt_long. Only options structure differ.
 */
int get_extopt_orig(int argc, char * const argv[],
                    const char *optstring,
                    struct extopt_orig *opts_orig, int *longindex)
{
    int i;
    int num_of_opts;
    struct option *longopts;
    int ret;

    i = 0; 
    while (1) {
        if (opt_orig_is_end(opts_orig[i]))
            break;

        i++;
    }
    num_of_opts = i;
    
    /* Transform extopt_orig to 'struct option' */
    longopts = (struct option *)calloc(num_of_opts + 1, sizeof(struct option));
    for (i = 0; i < num_of_opts; i++) {
        longopts[i].name = opts_orig[i].name;
        longopts[i].has_arg = opts_orig[i].has_arg;
        longopts[i].flag = opts_orig[i].flag;
        longopts[i].val = opts_orig[i].val;
    }

    ret = getopt_long(argc, argv, optstring, longopts, longindex);

    free(longopts);

    return ret;
}
