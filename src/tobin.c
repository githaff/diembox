#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"
#include "tobin.h"


enum intval_type default_initval_type;
enum output_type { OUTPUT_NORM = 0, OUTPUT_DIFF };

int opts_diff;
int opts_help;
const char *opts_type = "s32";

struct extopt tobin_opts[] = {
	{
		EXTOPT_NO_ARG(&opts_diff),
		.name_long = "diff",
		.name_short = 'd',
		.desc = "show difference between selected expressions",
	}, {
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
	case S8  : printf("Dec: %d\n",  val.s8);  break;
	case U8  : printf("Dec: %d\n",  val.u8);  break;
	case S16 : printf("Dec: %d\n",  val.s16); break;
	case U16 : printf("Dec: %d\n",  val.u16); break;
	case S32 : printf("Dec: %d\n",  val.s32); break;
	case U32 : printf("Dec: %d\n",  val.u32); break;
	case S64 : printf("Dec: %ld\n", val.s64); break;
	case U64 : printf("Dec: %ld\n", val.u64); break;
	default:
		err_msg("invalid intval\n");
		return;
	}

	switch (val.type) {
	case S8  : case U8  : printf("Hex: 0x%02x\n",   val.u8);  break;
	case S16 : case U16 : printf("Hex: 0x%04x\n",   val.u16); break;
	case S32 : case U32 : printf("Hex: 0x%08x\n",   val.u32); break;
	case S64 : case U64 : printf("Hex: 0x%016lx\n", val.u64); break;
	default: return;
	}

	printf("Bin:\n");

	switch (val.type) {
	case S8  : case U8  : print_8(val.u8);   break;
	case S16 : case U16 : print_16(val.u16); break;
	case S32 : case U32 : print_32(val.u32); break;
	case S64 : case U64 : print_64(val.u64); break;
	default: return;
	}
}


void print_result(struct intval *res, int size, enum output_type type)
{
	int i;

	switch (type) {
	case OUTPUT_NORM :
		for (i = 0; i < size; i++) {
			if (i)
				printf("\n");

			print_intval(res[i]);
		}
		break;
	case OUTPUT_DIFF :
		printf("DIFF\n");
		break;
	}
}


int tobin_main(int argc, char *argv[])
{
	struct intval *result = NULL;
	int ret = 0;
	int i;

	ret = extopts_get(&argc, argv, tobin_opts);
	if (opts_help) {
		tobin_help();
		goto end;
	}
	default_initval_type = parse_intval_type(opts_type);

	result = calloc(argc, sizeof(struct intval));
	for (i = 0; i < argc; i++) {
		result[i] = eval(argv[i]);
		if (result[i].type == INVAL) {
			ret = 1;
			goto end;
		}
	}

	if (opts_diff)
		print_result(result, argc, OUTPUT_DIFF);
	else
		print_result(result, argc, OUTPUT_NORM);

end:
	if (result)
		free(result);

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
