#include <stdio.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>


int opts_help;

struct extopt cat_opts[] = {
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_END
};

void cat_help(void)
{
	puts("Cat doesn't need your help. He is purfectly fine.");
}

int cat_main(int argc, char *argv[])
{
	int ret = 0;

	ret = extopts_get(&argc, argv, cat_opts);
	if (opts_help) {
		cat_help();
		goto end;
	}

	puts(":3");

end:
	return ret;
}

EXTMOD_DECL(cat, cat_main, cat_opts,
			"Cat",
			"Usage: embox-cat [OPTION]...\n"
			"Embedded developer's cat.")
