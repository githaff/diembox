#include <stdio.h>
#include <stdlib.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"


int opts_help;

struct extopt tobin_opts[] = {
/*
  Options TODO:
  -o TYPE, --out TYPE - output value datatype
  -d, --diff          - show xor-difference between two first expression results
 */
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_END
};

void tobin_help(void)
{
	extmod_print_desc(extmod);
	printf("Options:\n");
	extmod_print_opts(extmod);
}

int eval(char *exp)
{
	return strtol(exp, NULL, 0);
}

char *byte_str(u8 byte)
{
	static char str[] = "0000 0000";

	str[0] = (byte >> 7) & 0x1 ? '1' : '0';
	str[1] = (byte >> 6) & 0x1 ? '1' : '0';
	str[2] = (byte >> 5) & 0x1 ? '1' : '0';
	str[3] = (byte >> 4) & 0x1 ? '1' : '0';
	str[5] = (byte >> 3) & 0x1 ? '1' : '0';
	str[6] = (byte >> 2) & 0x1 ? '1' : '0';
	str[7] = (byte >> 1) & 0x1 ? '1' : '0';
	str[8] = byte & 0x1 ? '1' : '0';

	return str;
}

void print_8(int val)
{
	puts("7       0");
	printf("%s\n", byte_str(val));
}

void print_16(int val)
{
	u8 *bytes = (u8*)&val;

	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

void print_32(int val)
{
	printf("val=%d\n", val);
	u8 *bytes = (u8*)&val;

	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3]));
	printf("%s\n", byte_str(bytes[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

void print_64(int val)
{
	u8 *bytes = (u8*)&val;

	puts("63     56  55     48");
	printf("%s  ", byte_str(bytes[7]));
	printf("%s\n", byte_str(bytes[6]));
	puts("47      40  39       32");
	printf("%s  ", byte_str(bytes[5]));
	printf("%s\n", byte_str(bytes[4]));
	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3]));
	printf("%s\n", byte_str(bytes[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

int tobin_main(int argc, char *argv[])
{
	int ret = 0;
	int result;
	int i;

	for (i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

	ret = extopts_get(&argc, argv, tobin_opts);
	if (opts_help) {
		tobin_help();
		goto end;
	}

	for (i = 0; i < argc; i++) {
		result = eval(argv[i]);
		print_32(result);
		if (i > 0)
			printf("\n");
	}

end:
	return ret;
}

EXTMOD_DECL(tobin, tobin_main, tobin_opts,
			"Binary calculator",
			"Usage: embox-tobin [OPTION]... EXPRESSION\n"
			"This utility implements basic binary calculator functions with central ability\n"
			"to output result of expression in readable binary form.\n"
			"\n"
			"Options:\n"
			"[[OPTIONS]]\n"
			"It works only with integer data types.\n"
			"Supported operators: +, -, /, %, <<, >>, (, )\n"
			"Supported types:\n"
			"  s8  - signed   8-bit\n"
			"  u8  - unsigned 8-bit\n"
			"  s16 - signed   16-bit\n"
			"  u16 - unsigned 16-bit\n"
			"  s32 - signed   32-bit\n"
			"  u32 - unsigned 32-bit\n"
			"  s64 - signed   64-bit\n"
			"  u64 - unsigned 64-bit\n"
			"Data type for each used value can be specified in () right before the value\n"
			"itself.\n"
			"\n"
			"Examples:\n"
			"$ embox-tobin 0xee488f120\n"
			"$ embox-tobin \"4 + 8 << (2 - 1)\"\n"
			"$ embox-tobin (s16)0x52ea \"(s32)((u16)0x18 << 5) + 0x8937ffee\"")
