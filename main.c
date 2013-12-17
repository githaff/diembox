#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "tobin.h"
#include "module.h"
#include "extopts.h"
#include "extmods.h"


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
    struct extmod *module;
    int index;

    index = get_extopts(argc, argv, embox_opts);
    if (index < 0) {
        ret = 1;
        goto err;
    }

    if (opts_help) {
        embox_usage(embox_opts);
        goto end;
    }

    module = extmod_find("tobin");
    if (!module) {
        fprintf(stderr, "Error: no such command. See --help for details\n");
        ret = 1;
        goto err;
    }
    ret = module_exec(argc, argv, module);
    if (ret)
        goto err;

err:
end:

	return ret;
}
