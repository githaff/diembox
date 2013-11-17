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
    OPTS_END
};




int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

    printf("Options\n");
    print_usage(embox_opts);

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
