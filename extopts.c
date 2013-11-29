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

    (void)argc;
    (void)argv;
    
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
        int my;
        
		index = getopt_long(argc, argv, optstring,
                        longopts, &my);
		if (index == -1)
			break;
        else if (index == '?') {
			return 1;
        }

        printf("?==> %d (%c)\n", index, index);
        
        if (opts[index].has_arg == no_argument) {
            *(char *)opts[index].arg.addr = 1;
            continue;
        }
            
        switch (opts[index].arg_type) {
        case EXTARG_STR:
            printf(":::: str - 0\n");
            *(char **)opts[index].arg.addr = optarg;
            printf(":::: str - 1\n");
            break;
        case EXTARG_INT:
            *(int *)opts[index].arg.addr = strtol(optarg, NULL, 0);
            break;
        case EXTARG_CHAR:
            *(char *)opts[index].arg.addr = optarg[0];
            break;
        case EXTARG_SPECIAL:
            printf(":::: - spec\n");
            opts[index].arg.setter(&opts[index], optarg);
            printf(":::: - spec - 0\n");
            break;
        }
	}

    return 0;
}
