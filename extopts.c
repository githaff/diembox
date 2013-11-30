#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

#include "extopts.h"


/*
 * Parse command line arguments.
 */
int get_extopt(int argc, char *argv[], struct extopt *opts)
{
    int i, j;
    int k;
    int num_of_opts;
    struct option *longopts;
    char optstring[64];
    
    i = 0; 
    while (1) {
        if (opt_is_end(opts[i]))
            break;

        i++;
    }
    num_of_opts = i;

    /* Compose option structs from extopt */
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

    while (1) {
        int index;
        int index_short;
        int index_long  = -1;

		index_short = getopt_long(argc, argv, optstring,
                                  longopts, &index_long);

        if (index_long > -1)
            index = index_long;
        else if (index_short > 0) {
            for (i = 0; i < num_of_opts; i++) {
                if (index_short == opts[i].name_short)
                    index = i;
            }
            if (index < 0)
                return 1;
        } else
			return 1;

        switch (opts[index].arg_type) {
        case EXTOPT_ARGTYPE_STR:
            *(char **)opts[index].arg.addr = optarg;
            break;
        case EXTOPT_ARGTYPE_INT:
            *(int *)opts[index].arg.addr = strtol(optarg, NULL, 0);
            break;
        case EXTOPT_ARGTYPE_CHAR:
            *(char *)opts[index].arg.addr = optarg[0];
            break;
        case EXTOPT_ARGTYPE_SPECIAL:
            opts[index].arg.setter(&opts[index], optarg);
            break;
        case EXTOPT_ARGTYPE_NO_ARG:
            *opts[index].arg.flag_addr = 1;
            break;
        }
	}

    return 0;
}
