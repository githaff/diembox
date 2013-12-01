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
void embox_usage(struct extopt *opts)
{
	printf("Usage: %s [OPTIONS] COMMAND [COMMAND_OPTIONS]\n"
		   "Embedded developer toolbox.\n"
           "Targeted mainly at aiding embedded low-level programming (but of course\n"
           "can be used by anyone) and implements basic useful tools which perfectly\n"
           "may be replaced by other different tools. But it is simply convenient\n"
           "to hold it all in one console utility.\n"
		   "\n"
		   "Options:\n", embox_name);
    extopts_usage(opts);
}

int  opts_help;

struct extopt embox_opts[] = {
    {
        .name_long = "help",
        .name_short = 'h',
        EXTOPT_NO_ARG(&opts_help),
        .desc = "print this help",
    },
    EXTOPTS_END
};


int main(int argc, char *argv[])
{
	int ret = 0;

    if (get_extopts(argc, argv, embox_opts)) {
        printf("Error: parsing command line arguments failed\n");
        ret = 1;
        goto err;
    }

    if (opts_help) {
        embox_usage(embox_opts);
        goto end;
    }

err:
end:

	return ret;
}
