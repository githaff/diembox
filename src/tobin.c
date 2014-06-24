#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"
#include "tobin.h"


enum intval_type default_initval_type;

int opts_help;
const char *opts_type = "s32";

struct extopt tobin_opts[] = {
/*
  Options TODO:
  -d, --diff          - show xor-difference between two first expression results
 */
	{
		.name_long = "type",
		.name_short = 't',
		EXTOPT_ARG_STR("TYPE", &opts_type),
		.desc = "internal data type (s32 is default)",
	},
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_END
};

void tobin_help(void)
{
	extmod_print_desc(extmod);
	printf("Options:\n");
	extmod_print_opts(extmod);
}

enum intval_type parse_intval_type(const char *str)
{
	if (!strcmp(str, "s8"))
		return S8;
	else if (!strcmp(str, "u8"))
		return U8;
	else if (!strcmp(str, "s16"))
		return S16;
	else if (!strcmp(str, "u16"))
		return U16;
	else if (!strcmp(str, "s32"))
		return S32;
	else if (!strcmp(str, "u32"))
		return U32;
	else if (!strcmp(str, "s64"))
		return S64;
	else if (!strcmp(str, "u64"))
		return U64;
	return INVAL;
}

struct intval eval(char *exp)
{
	struct intval result;
	struct symbol_queue *rpn;

	rpn = expr_parse(exp);
	if (!rpn)
		goto err;
	result = rpn_eval(rpn);

	symbol_queue_destroy(rpn);

	return result;

err:
	result.type = INVAL;
	return result;
}

char *byte_str(u8_t byte)
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

void print_8(u8_t val)
{
	puts("7       0");
	printf("%s\n", byte_str(val));
}

void print_16(u16_t val)
{
	u8_t *bytes = (u8_t*)&val;

	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

void print_32(u32_t val)
{
	printf("val=%d\n", val);
	u8_t *bytes = (u8_t*)&val;

	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3]));
	printf("%s\n", byte_str(bytes[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

void print_64(u64_t val)
{
	u8_t *bytes = (u8_t*)&val;

	puts("63     56  55     48");
	printf("%s  ", byte_str(bytes[7]));
	printf("%s\n", byte_str(bytes[6]));
	puts("47     40  39     32");
	printf("%s  ", byte_str(bytes[5]));
	printf("%s\n", byte_str(bytes[4]));
	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3]));
	printf("%s\n", byte_str(bytes[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1]));
	printf("%s\n", byte_str(bytes[0]));
}

void print_intval(struct intval val)
{
	switch (val.type) {
	case S8 :
	case U8 :
		print_8(val.u8);
		break;
	case S16 :
	case U16 :
		print_16(val.u16);
		break;
	case S32 :
	case U32 :
		print_32(val.u32);
		break;
	case S64 :
	case U64 :
		print_64(val.u64);
		break;
	default:
		err_msg("invalid intval\n");
	}
}


int tobin_main(int argc, char *argv[])
{
	struct intval result;
	int ret = 0;
	int i;

	ret = extopts_get(&argc, argv, tobin_opts);
	if (opts_help) {
		tobin_help();
		goto end;
	}
	default_initval_type = parse_intval_type(opts_type);

	for (i = 0; i < argc; i++) {
		result = eval(argv[i]);
		if (result.type == INVAL) {
			ret = 1;
			goto end;
		}
		print_intval(result);
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
			"$ embox-tobin 0x52ea \"(0x18 << 5) + 0x8937ffee\"")
