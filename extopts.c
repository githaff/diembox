#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "extopts.h"


/*
 * Check option for validity.
 * It is automatically preformed before parsing command line
 * arguments.
 */
int check_extopt(struct extopt *opt)
{
    int ret = 0;
    char opt_name_reserved[] = "<unnamed>";
    char *opt_name;

    if (opt->name_long && *opt->name_long)
        opt_name = opt->name_long;
    else if (opt->name_short) {
        opt_name_reserved[0] = opt->name_short;
        opt_name_reserved[1] = 0;
    } else
        opt_name = opt_name_reserved;

    if (!opt->name_long && !opt->name_short) {
        fprintf(stderr, "Error: '%s' extopt doesn't have any name.\n", opt_name);
        ret = 1;
    }

    if (!opt->arg.addr) {
        fprintf(stderr, "Error: '%s' extopt has unspecified argument value\n", opt_name);
        ret = 1;
    }

    if (opt->has_arg != no_argument && opt->has_arg != required_argument) {
        fprintf(stderr, "Error: '%s' extopt has invalid number of arguments (%d)\n",
               opt_name, opt->has_arg);
        ret = 1;
    }

    return ret;
}

int validate_extopts(struct extopt *opts)
{
    int ret = 0;
    int i;

    i = 0;
    while (1) {
        if (opt_is_end(opts[i]))
            break;
        if (check_extopt(&opts[i]))
            ret = 1;

        i++;
    }

    return ret;
}

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
            fprintf(stderr, "Error: optional arguments are not supported in %s\n",__func__);
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
 * Default integer types setter.
 */
int default_setter_int(struct extopt *opt, const char *arg)
{
    int ret = 0;
    char *endptr;

    switch(opt->arg_type) {
    case EXTOPT_ARGTYPE_INT:
        *(int *)opt->arg.addr = strtol(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LINT:
        *(long int *)opt->arg.addr = strtol(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LLINT:
        *(long long int *)opt->arg.addr = strtoll(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    default:
        ret = 1;
    }

    return ret;
}

/*
 * Default unsigned integer types setter.
 */
int default_setter_uint(struct extopt *opt, const char *arg)
{
    int ret = 0;
    char *endptr;

    switch(opt->arg_type) {
    case EXTOPT_ARGTYPE_UINT:
        *(unsigned int *)opt->arg.addr = strtoul(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_ULINT:
        *(unsigned long int *)opt->arg.addr = strtoul(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_ULLINT:
        *(unsigned long long int *)opt->arg.addr = strtoull(arg, &endptr, 0);
        if (*endptr && !*arg)
            ret = 1;
        break;
    default:
        ret = 1;
    }

    return ret;
}

/*
 * Default floating-point types setter.
 */
int default_setter_float(struct extopt *opt, const char *arg)
{
    int ret = 0;
    char *endptr;

    switch(opt->arg_type) {
    case EXTOPT_ARGTYPE_FLOAT:
        *(float *)opt->arg.addr = strtof(arg, &endptr);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_DOUBLE:
        *(double *)opt->arg.addr = strtod(arg, &endptr);
        if (*endptr && !*arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LDOUBLE:
        *(long double *)opt->arg.addr = strtold(arg, &endptr);
        if (*endptr && !*arg)
            ret = 1;
        break;
    default:
        ret = 1;
    }

    return ret;
}

/*
 * Default argument parser. Applied for all standart argument types.
 */
int default_setter(struct extopt *opt, const char *arg)
{
    int ret = 0;

    switch (opt->arg_type) {
    case EXTOPT_ARGTYPE_SPECIAL:
        opt->arg.setter(opt, arg);
        break;
    case EXTOPT_ARGTYPE_NO_ARG:
        *opt->arg.flag_addr = 1;
        break;
    case EXTOPT_ARGTYPE_STR:
        *opt->arg.const_str = arg;
        break;
    case EXTOPT_ARGTYPE_STR_ALLOC:
        strcpy(opt->arg.addr, arg);
        break;
    case EXTOPT_ARGTYPE_INT:
    case EXTOPT_ARGTYPE_LINT:
    case EXTOPT_ARGTYPE_LLINT:
        ret = default_setter_int(opt, arg);
        break;
    case EXTOPT_ARGTYPE_UINT:
    case EXTOPT_ARGTYPE_ULINT:
    case EXTOPT_ARGTYPE_ULLINT:
        ret = default_setter_uint(opt, arg);
        break;
    case EXTOPT_ARGTYPE_FLOAT:
    case EXTOPT_ARGTYPE_DOUBLE:
    case EXTOPT_ARGTYPE_LDOUBLE:
        ret = default_setter_float(opt, arg);
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
    case EXTOPT_ARGTYPE_NO_ARG:        return "NO_ARG";
    case EXTOPT_ARGTYPE_STR:           return "STR";
    case EXTOPT_ARGTYPE_STR_ALLOC:     return "STR";
    case EXTOPT_ARGTYPE_INT:           return "INT";
    case EXTOPT_ARGTYPE_LINT:          return "LINT";
    case EXTOPT_ARGTYPE_LLINT:         return "LLINT";
    case EXTOPT_ARGTYPE_UINT:          return "INT";
    case EXTOPT_ARGTYPE_ULINT:         return "ULINT";
    case EXTOPT_ARGTYPE_ULLINT:        return "ULLINT";
    case EXTOPT_ARGTYPE_FLOAT:         return "FLOAT";
    case EXTOPT_ARGTYPE_DOUBLE:        return "DOUBLE";
    case EXTOPT_ARGTYPE_LDOUBLE:       return "LDOUBLE";
    case EXTOPT_ARGTYPE_CHAR:          return "CHAR";
    case EXTOPT_ARGTYPE_SPECIAL:       return "SPECIAL";
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
            *opts[i].arg.flag_addr = 0;

        i++;
    }
}

/*
 * Parse command line arguments.
 */
int get_extopts(int argc, char *argv[], struct extopt *opts)
{
    int ret = 0;
    struct option *longopts = 0;
    char optstring[64];

    if (validate_extopts(opts)) {
        ret = 1;
        goto err;
    }

    empty_noargers(opts);

    /* Compose option structs from extopt */
    longopts = compose_longopts(opts, optstring);

    while (1) {
        int index;
        int index_short;
        int index_long  = -1;

		index_short = getopt_long(argc, argv, optstring,
                                  longopts, &index_long);


        if (index_short == -1 && index_long == -1)
            break;
        if (index_long > -1)
            index = index_long;
        else if (index_short > 0) {
            index = find_short(opts, index_short);
            if (index < 0) {
                ret = 1;
                goto err;
            }
        }
        else {
            ret = 1;
            goto err;
        }

        ret = default_setter(&opts[index], optarg);
        if (ret) {
            if (opts[index].name_long)
                fprintf(stderr, "Error: parsing '%s' argument of parameter "
                       "'%s' (type %s) has failed\n",
                       optarg, opts[index].name_long,
                       get_argtype_name(opts[index].arg_type));
            else
                fprintf(stderr, "Error: parsing '%s' argument of parameter "
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
