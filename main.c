#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "tobin.h"
#include "module.h"
#include "usage.h"


char ltools_name[] = "ltools";
int ltools_name_len = sizeof(ltools_name);
void ltools_usage(void)
{
	printf("Usage: ltools [OPTIONS] COMMAND [COMMAND_OPTIONS]\n"
		   "Embedded developer toolbox.\n"
           "Targeted mainly at aiding embedded low-level programming (but of course\n"
           "can be used by anyone) and implements basic useful tools which perfectly\n"
           "may be replaced by other different tools. But it is simply convenient\n"
           "to hold it all in one console utility.\n"
		   "\n"
		   "Options:\n"
		   "  -h|--help   Print this help\n"
           "\n"
           "Commands:\n");
}

struct module_opt ltools_opts[] = {
    {
        .name_long = "my-arg",
        .name_short = 'h',
        .has_arg = optional_argument,
        .arg_name = "MY_ARG",
        .desc = "print this  blocks are copied only when modified.  If this is not possible the copy",
    }, {
        .name_long = "required-arg",
        .name_short = 's',
        .has_arg = required_argument,
        .arg_name = NULL,
        .desc = NULL,
    }, {
        .name_long = NULL,
        .name_short = 'o',
        .has_arg = no_argument,
        .arg_name = "",
        .desc = "print this help\nare\nequivalent. The second style allows repeated references to the  same  argument ",
    }, {
        .name_long = "version",
        .name_short = 0,
        .has_arg = no_argument,
        .arg_name = "",
        .desc = "print this help are equivalent. The second style allows repeated references to the  same  argument The  C99  standard does not include the style using '$', which comes from the Single data blocks are copied only when modified.\nIf this is not possible the copy",
    },
    OPTS_END
};


int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

    printf("Options\n");
    print_usage(ltools_opts);

    return 0;

	toolname = basename(argv[0]);

	if (!strncmp(toolname, ltools_name, ltools_name_len)) {
        ltools_usage();
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
