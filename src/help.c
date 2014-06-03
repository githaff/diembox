#include <stdio.h>
#include <extopts/extmods.h>

#include "main-info.h"


int help_module(int argc, char *argv[])
{
	struct extmod *module;
	int ret = 0;

	if (argc > 1)
		module = extmod_find(argv[1]);
	else {
		embox_usage();
		goto end;
	}

	if (module)
		extmod_print_desc(module);
	else {
		fprintf(stderr, "Error: module %s is not found\n", argv[1]);
		ret = 1;
	}

end:
	return ret;
}

EXTMOD_DECL(help, help_module, NULL,
			"Print help",
			"Usage: embox-help [command]\n"
			"Print help on specified command. If no command is specified print\n"
			"common help.")
