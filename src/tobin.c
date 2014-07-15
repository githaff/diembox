#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"
#include "tobin.h"


enum intval_type default_intval_type = U32;
enum intval_type intval_type;
enum output_type { OUTPUT_NORM = 0, OUTPUT_COMMON, OUTPUT_DIFF };

int opts_diff;
int opts_common;
int opts_help;
char opts_type[255];

struct extopt tobin_opts[] = {
	{
		EXTOPT_NO_ARG(&opts_diff),
		.name_long = "diff",
		.name_short = 'd',
		.desc = "highlight difference between selected expressions",
	}, {
		EXTOPT_NO_ARG(&opts_common),
		.name_long = "common",
		.name_short = 'c',
		.desc = "highlight common part between selected expressions",
	}, {
		.name_long = "type",
		.name_short = 't',
		EXTOPT_ARG_STR_ALLOC("TYPE", &opts_type),
		.desc = "internal data type (u32 is default)",
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
	if (!strlen(str))
		return default_intval_type;

	if (!strcmp(str, "u8"))
		return U8;
	else if (!strcmp(str, "u16"))
		return U16;
	else if (!strcmp(str, "u32"))
		return U32;
	else if (!strcmp(str, "u64"))
		return U64;

	err_msg("unknown intval type %s. Look --help for more info\n", str);

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

char *byte_str(u8_t byte, u8_t byte_hl)
{
	static char str[255];
	int sh;
	int i;
	int pos = 0;

	for (i = 0, sh = 7; i < 9; i++, sh--) {
		if (i == 4) {
			str[pos++] = ' ';
			sh++;
			continue;
		}

		if (BIT(byte_hl, sh)) {
			pos += sprintf(str + pos, COLOR_TEXT("%d"), BIT(byte, sh));
		} else
			str[pos++] = BIT(byte, sh) ? '1' : '0';
	}

	str[pos] = 0;

	return str;
}

void print_8(u8_t val, u8_t hl)
{
	puts("7       0");
	printf("%s\n", byte_str(val, hl));
}

void print_16(u16_t val, u16_t hl)
{
	u8_t *bytes = (u8_t*)&val;
	u8_t *bytes_hl = (u8_t*)&hl;

	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1], bytes_hl[1]));
	printf("%s\n", byte_str(bytes[0], bytes_hl[0]));
}

void print_32(u32_t val, u32_t hl)
{
	u8_t *bytes = (u8_t*)&val;
	u8_t *bytes_hl = (u8_t*)&hl;

	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3], bytes_hl[3]));
	printf("%s\n", byte_str(bytes[2], bytes_hl[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1], bytes_hl[1]));
	printf("%s\n", byte_str(bytes[0], bytes_hl[0]));
}

void print_64(u64_t val, u64_t hl)
{
	u8_t *bytes = (u8_t*)&val;
	u8_t *bytes_hl = (u8_t*)&hl;

	puts("63     56  55     48");
	printf("%s  ", byte_str(bytes[7], bytes_hl[7]));
	printf("%s\n", byte_str(bytes[6], bytes_hl[6]));
	puts("47     40  39     32");
	printf("%s  ", byte_str(bytes[5], bytes_hl[5]));
	printf("%s\n", byte_str(bytes[4], bytes_hl[4]));
	puts("31     24  23     16");
	printf("%s  ", byte_str(bytes[3], bytes_hl[3]));
	printf("%s\n", byte_str(bytes[2], bytes_hl[2]));
	puts("15      8  7       0");
	printf("%s  ", byte_str(bytes[1], bytes_hl[1]));
	printf("%s\n", byte_str(bytes[0], bytes_hl[0]));
}

/*
 * Print intval regarding its type.
 * hl - mask for bits needed to be highlighted
 */
void print_intval(struct intval val, struct intval hl)
{
	switch (val.type) {
	case U8  : printf("Dec: %d\n",  val.u8);  break;
	case U16 : printf("Dec: %d\n",  val.u16); break;
	case U32 : printf("Dec: %d\n",  val.u32); break;
	case U64 : printf("Dec: %ld\n", val.u64); break;
	default:
		err_msg("invalid intval\n");
		return;
	}

	switch (val.type) {
	case U8  : printf("Hex: 0x%02x\n",   val.u8);  break;
	case U16 : printf("Hex: 0x%04x\n",   val.u16); break;
	case U32 : printf("Hex: 0x%08x\n",   val.u32); break;
	case U64 : printf("Hex: 0x%016lx\n", val.u64); break;
	default: return;
	}

	printf("Bin:\n");

	switch (val.type) {
	case U8  : print_8(val.u8,   hl.u8);   break;
	case U16 : print_16(val.u16, hl.u16); break;
	case U32 : print_32(val.u32, hl.u32); break;
	case U64 : print_64(val.u64, hl.u64); break;
	default: return;
	}
}


void print_result(struct intval *res, int size, enum output_type type)
{
	struct intval tmp;
	struct intval diff;
	struct intval hl;
	int i, j;

	diff.u64 = 0;
	for (i = 0; i < size; i++) {
		for (j = 0; j < i; j++) {
			INTVAL_OP_BIN(tmp, res[i], res[j], ^);
			diff.u64 |= tmp.u64;
		}
	}

	switch (type) {
	case OUTPUT_COMMON :
		hl.u64 = ~diff.u64;
		break;
	case OUTPUT_DIFF :
		hl.u64 = diff.u64;
		break;
	case OUTPUT_NORM :
	default :
		hl.u64 = 0;
		break;
	}

	for (i = 0; i < size; i++) {
		if (i)
			printf("\n");

		print_intval(res[i], hl);
	}
}


int tobin_main(int argc, char *argv[])
{
	struct intval *result = NULL;
	enum output_type print_type;
	int ret = 0;
	int i;

	ret = extopts_get(&argc, argv, tobin_opts);
	if (opts_help) {
		tobin_help();
		goto end;
	}

	result = calloc(argc, sizeof(struct intval));
	for (i = 0; i < argc; i++) {
		intval_type = parse_intval_type(opts_type);
		if (intval_type == INVAL) {
			ret = 1;
			goto end;
		}

		result[i] = eval(argv[i]);
		if (result[i].type == INVAL) {
			ret = 1;
			goto end;
		}
	}

	print_type = OUTPUT_NORM;
	if (opts_common)
		print_type = OUTPUT_COMMON;
	if (opts_diff)
		print_type = OUTPUT_DIFF;

	print_result(result, argc, print_type);

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
			"  u8  - unsigned 8-bit\n"
			"  u16 - unsigned 16-bit\n"
			"  u32 - unsigned 32-bit\n"
			"  u64 - unsigned 64-bit\n"
			"Data type for each used value can be specified in () right before the value\n"
			"itself.\n"
			"\n"
			"Examples:\n"
			"$ embox-tobin 0xee488f120\n"
			"$ embox-tobin \"4 + 8 << (2 - 1)\"\n"
			"$ embox-tobin 0x52ea \"(0x18 << 5) + 0x8937ffee\"")
