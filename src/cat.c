#include <stdio.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>


int cat_main(int argc, char *argv[])
{
	puts(":3");

	return 0;
}

EXTMOD_DECL(cat, cat_main, NULL,
			"Cat",
			"Cat doesn't need your help. He is purfectly fine.")
