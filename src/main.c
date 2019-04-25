#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "config.h"
#include "main-info.h"


char diembox_name[] = "diembox";

void diembox_usage(void)
{
	printf("Usage: %s [OPTIONS] COMMAND [COMMAND_OPTIONS]\n"
		   "DiEM toolbox.\n"
		   "A set of console helper utilities.\n"
		   "\n", diembox_name);
	printf("Commands:\n");
	extmods_usage_list();
	printf("\n");
	printf("Options:\n");
	extopts_usage(diembox_opts);
}

void diembox_version(void)
{
	printf("Embox utilities toolset %s\n", EMBOX_VERSION_FULL);
}

bool opts_help;
bool opts_version;

struct extopt diembox_opts[] = {
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_VERSION(&opts_version),
	EXTOPTS_END
};


int main(int argc, char *argv[])
{
	int ret = 0;
	struct extmod *module = 0;
	char *execname;

	execname = basename(argv[0]);

	module = extmod_extract(&argc, argv);

	if (module)
		ret = extmod_exec(argc, argv, module);
	else {
		if (strcmp(execname, diembox_name)) {
			fprintf(stderr, "%s: module not found\n", execname);
			ret = 1;
			goto err;
		}

		ret = extopts_get(&argc, argv, diembox_opts);
		if (ret)
			goto err;

		if (opts_help) {
			diembox_usage();
			goto end;
		}
		if (opts_version) {
			diembox_version();
			goto end;
		}
	}

end:
err:
	return ret;
}
