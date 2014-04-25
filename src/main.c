#include <stdio.h>
#include <extopts/extopts.h>


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
	struct extmod *module;
	int index;

	if (extopts_get(&argc, argv, opts))
		return 1;
	if (opts_help)
		embox_usage();
	else
		printf("U-u-u-u\n");

	return 0;
}
