#include <stdlib.h>
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
 * Default integer types setter.
 */
int default_setter_int(struct extopt *opt, const char *arg)
{
    int ret = 0;
    char *endptr;

    switch(opt->arg_type) {
    case EXTOPT_ARGTYPE_INT:
        *(int *)opt->arg.addr = strtol(arg, &endptr, 0);
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LINT:
        *(long int *)opt->arg.addr = strtol(arg, &endptr, 0);
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LLINT:
        *(long long int *)opt->arg.addr = strtoll(arg, &endptr, 0);
        if (*endptr && *arg)
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
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_ULINT:
        *(unsigned long int *)opt->arg.addr = strtoul(arg, &endptr, 0);
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_ULLINT:
        *(unsigned long long int *)opt->arg.addr = strtoull(arg, &endptr, 0);
        if (*endptr && *arg)
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
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_DOUBLE:
        *(double *)opt->arg.addr = strtod(arg, &endptr);
        if (*endptr && *arg)
            ret = 1;
        break;
    case EXTOPT_ARGTYPE_LDOUBLE:
        *(long double *)opt->arg.addr = strtold(arg, &endptr);
        if (*endptr && *arg)
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
 * Find needed extopt
 */
struct extopt *find_opt(char *opt_str, struct extopt *opts)
{
    int i = 0;
    struct extopt *opt = 0;
    char shortopt_str[] = "-X";

    while (1) {
        if (opt_is_end(opts[i]))
            break;

        shortopt_str[1] = opts[i].name_short;
        if (!strncmp(opt_str, shortopt_str, 2) ||
            !strcmp(opt_str, opts[i].name_long))
            opt = &opts[i];

        i++;
    }

    return opt;
}

/*
 * Parse command line arguments.
 */
int get_extopts(int argc, char *argv[], struct extopt *opts)
{
    int ret = 0;
    char wait_optarg = 0;
    struct extopt *opt;
    int i;
    char *optkey;
    char *optarg;
    char *rest[255];
    int rest_size = 0;


    if (validate_extopts(opts)) {
        ret = -1;
        goto err;
    }

    empty_noargers(opts);

    for (i = 0; i < argc; i++) {
        if (wait_optarg) {
            optarg = argv[i];

            ret = default_setter(opt, optarg);
            if (ret) {
                fprintf(stderr, "Error: parsing '%s' argument of parameter "
                        "'%s' (type %s) has failed\n",
                        optarg, optkey, get_argtype_name(opt->arg_type));
                ret = -1;
                goto err;
            }
            wait_optarg = 0;
        } else {
            optkey = argv[i];
            
            opt = find_opt(optkey, opts);
            if (opt)
                wait_optarg = opt->has_arg;
            else
                rest[rest_size++] = argv[i];
        }
	}
    
err:

    return ret;
}
