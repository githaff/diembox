#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"


char *capts_positive[] = {
	"Murr...",
	"Purr...",
	"Nyaa"
};

char *capts_negative[] = {
	"Hissss",
	"*Wagging tail*",
	"Miaaauuu",
};

char *capts_neutral[] = {
	"z-z-z-Z",
	"*no interest shown*"
};

int opts_help;
int opts_pet;
int opts_kick;
int opts_mouse;
int opts_nothing;
int opts_call;

struct extopt cat_opts[] = {
	{
		EXTOPT_NO_ARG(&opts_pet),
		.name_long = "pet",
		.name_short = 'p',
		.desc = "pet that fluffy mass"
	},
	{
		EXTOPT_NO_ARG(&opts_kick),
		.name_long = "kick",
		.name_short = 'k',
		.desc = "kick poor thing"
	},
	{
		EXTOPT_NO_ARG(&opts_mouse),
		.name_long = "mouse",
		.name_short = 'm',
		.desc = "play with mouse"
	},
	{
		EXTOPT_NO_ARG(&opts_nothing),
		.name_long = "nothing",
		.name_short = 'n',
		.desc = "do nothing"
	},
	{
		EXTOPT_NO_ARG(&opts_call),
		.name_long = "call",
		.name_short = 'c',
		.desc = "call that beast"
	},
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_END
};

void cat_help(void)
{
	extmod_print_desc(extmod);
	printf("\n");
	printf("Options:\n");
	extmod_print_opts(extmod);
}

void print_positive(void)
{
	int ind;
	ind = rand() % ARRAY_SIZE(capts_positive);
	puts(capts_positive[ind]);
}

void print_negative(void)
{
	int ind;
	ind = rand() % ARRAY_SIZE(capts_negative);
	puts(capts_negative[ind]);
}

void print_neutral(void)
{
	int ind;
	ind = rand() % ARRAY_SIZE(capts_neutral);
	puts(capts_neutral[ind]);
}

char decide(double percents)
{
	double throw;
	throw = 100.0 * (rand() / (double)RAND_MAX);
	return throw < percents ? 1 : 0;
}

int cat_module(int argc, char *argv[])
{
	int ret = 0;

	srand(time(NULL));

	ret = extopts_get(&argc, argv, cat_opts);
	if (opts_help) {
		cat_help();
		goto end;
	}

	if (opts_pet) {
		if (decide(90))
			print_positive();
		else if (decide(50))
			print_negative();
		else
			print_neutral();
		goto end;
	}

	if (opts_kick) {
		if (decide(90))
			print_negative();
		else if (decide(50))
			print_positive();
		else
			print_neutral();
		goto end;
	}

	if (opts_mouse) {
		print_neutral();
		goto end;
	}

	if (opts_nothing) {
		print_neutral();
		goto end;
	}

	if (opts_call) {
		print_neutral();
		goto end;
	}

end:
	return ret;
}

EXTMOD_DECL(cat, cat_module, cat_opts,
			"Cat",
			"Usage: embox-cat [OPTION]...\n"
			"Embedded developer's cat.")
