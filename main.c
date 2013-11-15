#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "tobin.h"


int main(int argc, char *argv[])
{
	int ret = 0;
	char *toolname;

	toolname = basename(argv[0]);

	if (!strncmp(toolname, tobin_name, tobin_name_len))
		ret = do_tobin(argc, argv);
	else {
		printf("Error: unknown toolname '%s'\n", toolname);
		ret = -EINVAL;
	}

	return ret;
}
