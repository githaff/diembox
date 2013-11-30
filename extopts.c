#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "extopts.h"


/*
 * Count number of extopts in array.
 */
int count_extopts(struct extopt *opts)
{
    int i;
    
    i = 0; 
    while (1) {
        if (opt_is_end(opts[i]))
            break;
        i++;
    }

    return i;
}

/*
 * Compose array of longopts corresponding to specified extopts.
 * Composed optstring will be written to 'optstring' parameter.
 * Returned array must be freed after use.
 */
struct option *compose_longopts(struct extopt *opts, char *optstring)
{
    int i, j;
    int k;
    int num_of_opts;
    struct option *longopts;

    num_of_opts = count_extopts(opts);

    k = 0;
    longopts = (struct option *)calloc(num_of_opts + 1, sizeof(struct option));
    for (i = 0, j = 0; i < num_of_opts; i++, j++) {
        if (opts[i].has_arg == optional_argument) {
            printf("Error: optional arguments are not supported in %s\n",__func__);
            j--;
            continue;
        }

        if (opts[i].name_short) {
            optstring[k++] = opts[i].name_short;
            if (opts[i].has_arg == required_argument)
                optstring[k++] = ':';
        }
        longopts[j].name = opts[i].name_long;
        longopts[j].has_arg = opts[i].has_arg;
        longopts[j].flag = NULL;
        longopts[j].val = j;
    }
    optstring[k] = 0;

    return longopts;
}

/*
 * Find index of extopt with specified name_short.
 * Returns -1 if such extopt wasn't found.
 */
int find_short(struct extopt *opts, int name_short)
{
    int i = 0;

    while (1) {
        if (opt_is_end(opts[i]))
            break;
        if (opts[i].name_short == name_short)
            break;

        i++;
    }

    if (opt_is_end(opts[i]))
        return -1;
    else
        return i;
}


/*
 * Default argument parser. Applied for all standart argument types.
 */
int default_setter(struct extopt *opt, char *arg)
{
    int ret = 0;
    char *endptr;

    switch (opt->arg_type) {
    case EXTOPT_ARGTYPE_SPECIAL:
        opt->arg.setter(opt, arg);
        break;
    case EXTOPT_ARGTYPE_NO_ARG:
        *opt->arg.flag_addr = 1;
        break;
    case EXTOPT_ARGTYPE_STR:
        *(char **)opt->arg.addr = arg;
        break;
    case EXTOPT_ARGTYPE_INT:
        *(int *)opt->arg.addr = strtol(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_CHAR:
        *(char *)opt->arg.addr = arg[0];
        break;
    }

    return ret;
}

/*
 * Get name of extopt arg type.
 */
char *get_argtype_name(enum extopt_argtype argtype)
{
    switch(argtype) {
    case EXTOPT_ARGTYPE_NO_ARG:  return "NO_ARG";
    case EXTOPT_ARGTYPE_STR:     return "STR";
    case EXTOPT_ARGTYPE_INT:     return "INT";
    case EXTOPT_ARGTYPE_CHAR:    return "CHAR";
    case EXTOPT_ARGTYPE_SPECIAL: return "SPECIAL";
    }

    return "UNKNOWN";
}

/*
 * Clean flags of all no-argumented parameters.
 */
void empty_noargers(struct extopt *opts)
{
    int i = 0;

    while (1) {
        if (opt_is_end(opts[i]))
            break;

        if (opts[i].arg_type == EXTOPT_ARGTYPE_NO_ARG)
            *opts[i].arg.flag_addr = 1;

        i++;
    }
}

/*
 * Parse command line arguments.
 */
int get_extopts(int argc, char *argv[], struct extopt *opts)
{
    int ret;
    struct option *longopts;
    char optstring[64];

    empty_noargers(opts);

    /* Compose option structs from extopt */
    longopts = compose_longopts(opts, optstring);

    while (1) {
        int index;
        int index_short;
        int index_long  = -1;

		index_short = getopt_long(argc, argv, optstring,
                                  longopts, &index_long);

        if (index_long > -1)
            index = index_long;
        else if (index_short > 0) {
            index = find_short(opts, index_short);
        } else {
            ret = 1;
            goto err;
        }

        if (index < 0) {
            ret = 1;
            goto err;
        }

        ret = default_setter(&opts[index], optarg);
        if (ret) {
            if (opts[index].name_long)
                printf("Error: parsing '%s' argument of parameter "
                       "'%s' (type %s) has failed\n",
                       optarg, opts[index].name_long,
                       get_argtype_name(opts[index].arg_type));
            else
                printf("Error: parsing '%s' argument of parameter "
                       "'%c' (type %s) has failed\n",
                       optarg, opts[index].name_short,
                       get_argtype_name(opts[index].arg_type));
            goto err;
        }
	}

err:
    if (longopts)
        free(longopts);

    return ret;
}
