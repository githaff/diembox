#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "config.h"
#include "main-info.h"


char embox_name[] = "embox";

void embox_usage(void)
{
	printf("Usage: %s [OPTIONS] COMMAND [COMMAND_OPTIONS]\n"
		   "Embedded developer toolbox.\n"
		   "Targeted mainly at aiding embedded low-level programming (but of course\n"
		   "can be used by anyone) and implements basic useful tools which perfectly\n"
		   "may be replaced by other different tools. But it is simply convenient\n"
		   "to hold it all in one console utility.\n"
		   "\n", embox_name);
	printf("Commands:\n");
	extmods_usage_list();
	printf("\n");
	printf("Options:\n");
	extopts_usage(embox_opts);
}

void embox_version(void)
{
	printf("Embox utilities toolset %s\n", EMBOX_VERSION_FULL);
}

int opts_help;
int opts_version;

struct extopt embox_opts[] = {
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

	if (!strcmp(execname, embox_name)) {
		if (argc > 1)
			module = extmod_find(argv[1]);
	}
	else {
		module = extmod_find(execname);
		if (!module) {
			char buf[1024];
			sprintf(buf, "%s.%s", embox_name, execname);
			module = extmod_find(buf);
		}
	}

	if (module)
		ret = extmod_exec(argc, argv, module);
	else {
		ret = extopts_get(&argc, argv, embox_opts);
		if (ret)
			goto err;

		if (opts_help) {
			embox_usage();
			goto end;
		}
		if (opts_version) {
			embox_version();
			goto end;
		}
	}

end:
	return ret;

err:
	return ret;
}
