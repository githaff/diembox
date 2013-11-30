#include <stdio.h>
#include <stdlib.h>
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

int  opts_help;
int   opts_some_num;
char *opts_some_str;
char opts_some_str_alloc[64];
long int   opts_spec_num;
int opts_set(struct extopt *opt, char *arg)
{
    (void)opt;
    
    printf("Opt set '%s'\n", arg);
    opts_spec_num = strtol(arg, NULL, 10);

    return 0;
}


struct extopt embox_opts[] = {
    {
        .name_long = "help",
        .name_short = 'h',
        EXTOPT_NO_ARG(&opts_help),
        .desc = "print this help",
    }, {
        .name_long = "some-num",
        .name_short = 'n',
        EXTOPT_ARG_INT("NUM", &opts_some_num),
        .desc = "specify some number",
    }, {
        .name_long = "some-str",
        EXTOPT_ARG_STR("STR", &opts_some_str),
        .desc = "specify some string",
    }, {
        .name_long = "some-str-all",
        .name_short = 's',
        EXTOPT_ARG_STR_ALLOC("SSTR", opts_some_str_alloc),
        .desc = "specify some string",
    }, {
        .name_long = "some-spec",
        .name_short = 'c',
        EXTOPT_ARG_SPECIAL("SPEC", opts_set),
        .desc = "specify some string",
    },
    EXTOPTS_END
};

int parse_arguments(int argc, char *argv[])
{
    printf(":: ==== BEFORE ====\n");
    printf(":: help = %d\n", opts_help);
    printf(":: num = %d\n", opts_some_num);
    printf(":: str = %s\n", opts_some_str);
    printf(":: str_alloc = %s\n", opts_some_str_alloc);
    printf(":: spec = %ld\n", opts_spec_num);

    get_extopts(argc, argv, embox_opts);

    printf(":: ==== AFTER ====\n");
    printf(":: help = %d\n", opts_help);
    printf(":: num = %d\n", opts_some_num);
    printf(":: str = %s\n", opts_some_str);
    printf(":: str_alloc = %s\n", opts_some_str_alloc);
    printf(":: spec = %ld\n", opts_spec_num);

    return 0;
}


int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

    printf("Options\n");
    extopts_usage(embox_opts);
    
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
