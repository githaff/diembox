#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>


struct extopt opts[];

char embox_name[] = "embox";
int embox_name_len = sizeof(embox_name);
void embox_usage()
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

int opts_help;

struct extopt opts[] = {
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_END
};

int main(int argc, char *argv[])
{
	int ret = 0;
	struct extmod *module = 0;
	char *execname;

	execname = basename(argv[0]);

	if (!strncmp(execname, embox_name, embox_name_len)) {
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
		ret = extopts_get(&argc, argv, opts);
		if (ret)
			goto err;

		if (opts_help) {
			embox_usage(opts);
			goto end;
		}
	}

end:
	return ret;

err:
	return ret;
}
