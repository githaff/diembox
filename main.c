#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "tobin.h"


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


int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

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
