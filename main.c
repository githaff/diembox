#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "tobin.h"
#include "module.h"
#include "extopts.h"


char embox_name[] = "embox";
int embox_name_len = sizeof(embox_name);
void embox_usage(void)
{
	printf("Usage: %s [OPTIONS] COMMAND [COMMAND_OPTIONS]\n"
		   "Embedded developer toolbox.\n"
           "Targeted mainly at aiding embedded low-level programming (but of course\n"
           "can be used by anyone) and implements basic useful tools which perfectly\n"
           "may be replaced by other different tools. But it is simply convenient\n"
           "to hold it all in one console utility.\n"
		   "\n"
		   "Options:\n"
		   "  -h|--help   print this help\n"
           "\n"
           "Commands:\n", embox_name);
}

struct extopt embox_opts[] = {
    {
        .name_long = "help",
        .name_short = 'h',
        .has_arg = no_argument,
        .arg_name = NULL,
        .desc = "print this help",
    },
    EXTOPTS_END
};

struct extopt_orig embox_opts_orig[] = {
    {
        .name = "some-req",
        .has_arg = required_argument,
        .flag = 0,
        .val = 'r',
        .name_short = 'r',
        .arg_name = "REQ_ARG",
        .desc = "take some required argument",
    }, {
        .name = "help",
        .has_arg = no_argument,
        .flag = 0,
        .val = 0,
        .name_short = 'h',
        .arg_name = NULL,
        .desc = "print this help",
    }, {
        .name = "some-opt",
        .has_arg = optional_argument,
        .flag = 0,
        .val = 'o',
        .name_short = 'o',
        .arg_name = "OPT_ARG",
        .desc = "take some optional argument",
    },
    
    EXTOPTS_ORIG_END
};


int parse_arguments(int argc, char *argv[])
{
	int c;

	while (1) {
        int option_index = 0;

		c = get_extopt_orig(argc, argv, "hr:o::",
                            embox_opts_orig, &option_index);
		if (c == -1)
			break;
		
		switch (c) {
		case '?':
            printf("Some error\n");
			return -EINVAL;
		case 'h':
            printf("Found option 'h'\n");
			break;
		case 'r':
            printf("Found option 'r' with param %s\n", optarg);
			break;
		case 'o':
            printf("Found option 'o' with param %s\n", optarg);
			break;
		}
	}
	
	return 0;  
}



int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

    printf("Options\n");
    extopts_usage(embox_opts);

    printf("Options\n");
    extopts_usage_orig(embox_opts_orig);

    parse_arguments(argc, argv);
    
    return 0;

	toolname = basename(argv[0]);

	if (!strncmp(toolname, embox_name, embox_name_len)) {
        embox_usage();
		return ret;
    }
	else if (!strncmp(toolname, tobin_name, tobin_name_len))
		ret = do_tobin(argc, argv);
	else {
		printf("Error: unknown toolname '%s'\n", toolname);
		ret = -EINVAL;
	}

	return ret;
}
