#include <stdio.h>
#include <extopts/extmods.h>

#include "common.h"
#include "config.h"
#include "main-info.h"


bool opts_help;
bool opts_version;

struct extopt help_opts[] = {
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_VERSION(&opts_version),
	EXTOPTS_END
};


void help_usage(void)
{
	extmod_print_desc(extmod);
}

void help_version(void)
{
	printf("Embox utilities toolset %s\n", EMBOX_VERSION_FULL);
}

int help_module(int argc, char *argv[])
{
	struct extmod *module;
	int ret = 0;

	if (argc <= 1) {
		embox_usage();
		goto end;
	}

	module = extmod_extract(&argc, argv);

	if (module)
		extmod_print_desc(module);
	else {
		ret = extopts_get(&argc, argv, help_opts);
		if (ret)
			goto end;

		if (opts_help) {
			help_usage();
			goto end;
		}
		if (opts_version) {
			help_version();
			goto end;
		}

		err_msg("module %s is not found\n", argv[0]);
		ret = 1;
		goto end;
	}

end:
	return ret;
}

EXTMOD_DECL(help, help_module, NULL,
			"Print help",
			"Usage: embox help [COMMAND]\n"
			"Print help on specified command. If no command is specified print\n"
			"common embox help.")
