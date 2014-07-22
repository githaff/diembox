#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"
#include "tobin.h"
#include "config.h"


enum intval_type intval_type;
enum output_type { OUTPUT_NORM = 0, OUTPUT_COMMON, OUTPUT_DIFF };

bool opts_diff;
bool opts_common;
bool opts_help;
bool opts_version;
bool opts_list;
int opts_type;

const int horiz_print_distance = 4;

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
		EXTOPT_NO_ARG(&opts_list),
		.name_long = "list",
		.name_short = 'l',
		.desc = "print results in for of a list (vertically one after another)",
	}, {
		.name_short = 'b',
		EXTOPT_ARG_INT("SIZE", &opts_type),
		.desc = "size of internal data type in bits. "
		"8, 16, 32, 64 are supported. 32 is default.",
	},
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_VERSION(&opts_version),
	EXTOPTS_END
};

void tobin_help(void)
{
	extmod_print_desc(extmod);
	printf("Options:\n");
	extmod_print_opts(extmod);
}

void tobin_version(void)
{
	printf("Tobin utility from embox toolset %s\n", EMBOX_VERSION_FULL);
}

enum intval_type get_intval_type(int size)
{
	switch (size) {
	case 0  : return DEFAULT_INTVAL_TYPE;
	case 8  : return S8;
	case 16 : return S16;
	case 32 : return S32;
	case 64 : return S64;
	default :
		err_msg("unknown intval type %d. Look --help for more info\n", size);
		break;
	}

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

int strlen_col(char *str_orig)
{
	int len;
	int i;

	len = strlen(str_orig);

	for (i = 0; colors[i]; i++) {
		char *str = str_orig;
		int color_len;

		color_len = strlen(colors[i]);
		str = strstr(str, colors[i]);
		while (str) {
			len -= color_len;
			str += color_len;
			str = strstr(str, colors[i]);
		}
	}

	return len;
}

void get_print_size(enum intval_type type, int *w_out, int *h_out)
{
	int w, h;

	switch (type) {
	case S8  : w = 9;  h = 5; break;
	case S16 : w = 20; h = 5; break;
	case S32 : w = 20; h = 7; break;
	case S64 : w = 20; h = 11; break;
	default  : w = 0;  h = 0; break;
	}

	if (w_out)
		*w_out = w;
	if (h_out)
		*h_out = h;
}

char *byte_str(u8_t byte, u8_t byte_hl)
{
	static char str[1024];
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

char *linestr_8(u8_t *bytes, u8_t *bytes_hl, int line)
{
	static char str[1024];
	s8_t val = *bytes;
	s8_t hl = *bytes_hl;

	switch (line) {
	case 0 : sprintf(str, "Dec: %d",     val);		break;
	case 1 : sprintf(str, "Hex: 0x%02x", val);		break;
	case 2 : sprintf(str, "Bin:");					break;
	case 3 : sprintf(str, "7       0");				break;
	case 4 : sprintf(str, "%s", byte_str(val, hl));	break;
	}

	return str;
}

char *linestr_16(u8_t *bytes, u8_t *bytes_hl, int line)
{
	static char str[1024];
	s16_t val = *(s16_t*)bytes;

	switch (line) {
	case 0 : sprintf(str, "Dec: %d",     val);		break;
	case 1 : sprintf(str, "Hex: 0x%04x", val);		break;
	case 2 : sprintf(str, "Bin:");					break;
		break;
	case 3 : sprintf(str, "15      8  7       0");	break;
	case 4 :
		sprintf(str, "%s  ", byte_str(bytes[1], bytes_hl[1]));
		strcat(str, byte_str(bytes[0], bytes_hl[0]));
		break;
	}

	return str;
}

char *linestr_32(u8_t *bytes, u8_t *bytes_hl, int line)
{
	static char str[1024];
	s32_t val = *(s32_t*)bytes;

	switch (line) {
	case 0 : sprintf(str, "Dec: %d",     val);		break;
	case 1 : sprintf(str, "Hex: 0x%08x", val);		break;
	case 2 : sprintf(str, "Bin:");					break;
	case 3 : sprintf(str, "31     24  23     16");	break;
	case 4 :
		sprintf(str, "%s  ", byte_str(bytes[3], bytes_hl[3]));
		strcat(str, byte_str(bytes[2], bytes_hl[2]));
		break;
	case 5 : sprintf(str, "15      8  7       0");	break;
	case 6 :
		sprintf(str, "%s  ", byte_str(bytes[1], bytes_hl[1]));
		strcat(str, byte_str(bytes[0], bytes_hl[0]));
		break;
	}

	return str;
}

char *linestr_64(u8_t *bytes, u8_t *bytes_hl, int line)
{
	static char str[1024];
	s64_t val = *(s64_t*)bytes;

	switch (line) {
	case 0 : sprintf(str, "Dec: %ld",     val);		break;
	case 1 : sprintf(str, "Hex: 0x%016lx", val);		break;
	case 2 : sprintf(str, "Bin:");					break;
	case 3 : sprintf(str, "63     56  55     48");	break;
	case 4 :
		sprintf(str, "%s  ", byte_str(bytes[7], bytes_hl[7]));
		strcat(str, byte_str(bytes[6], bytes_hl[6]));
		break;
	case 5 : sprintf(str, "47     40  39     32");	break;
	case 6 :
		sprintf(str, "%s  ", byte_str(bytes[5], bytes_hl[5]));
		strcat(str, byte_str(bytes[4], bytes_hl[4]));
		break;
	case 7 : sprintf(str, "31     24  23     16");	break;
	case 8 :
		sprintf(str, "%s  ", byte_str(bytes[3], bytes_hl[3]));
		strcat(str, byte_str(bytes[2], bytes_hl[2]));
		break;
	case 9  : sprintf(str, "15      8  7       0");	break;
	case 10 :
		sprintf(str, "%s  ", byte_str(bytes[1], bytes_hl[1]));
		strcat(str, byte_str(bytes[0], bytes_hl[0]));
		break;
	}

	return str;
}

char *linestr(struct intval val, struct intval hl, int line)
{
	switch (val.type) {
	case S8  : return linestr_8((u8_t*)&val.s64,  (u8_t*)&hl.s64, line);
	case S16 : return linestr_16((u8_t*)&val.s64, (u8_t*)&hl.s64, line);
	case S32 : return linestr_32((u8_t*)&val.s64, (u8_t*)&hl.s64, line);
	case S64 : return linestr_64((u8_t*)&val.s64, (u8_t*)&hl.s64, line);
	default: return NULL;
	}
}

/*
 * Print intval regarding its type.
 * hl - mask for bits needed to be highlighted
 */
void print_intval(struct intval val, struct intval hl)
{
	int h;
	int i;

	get_print_size(val.type, NULL, &h);
	for (i = 0 ; i < h; i++)
		puts(linestr(val, hl, i));
}

void print_vert(struct intval *res, int size, struct intval hl)
{
	int i;

	for (i = 0; i < size; i++) {
		if (i)
			printf("\n");

		print_intval(res[i], hl);
	}
}

void print_horiz(struct intval *res, int size, struct intval hl)
{
	enum intval_type max_type = INTVAL;
	int total_width;
	int max_height = 0;
	int idx;
	int i, j;

	total_width = 0;
	for (i = 0; i < size; i++) {
		int w, h;
		get_print_size(res[i].type, &w, &h);
		total_width += w;
		if (res[i].type > max_type)
			max_type = res[i].type;
		if (h > max_height)
			max_height = h;
	}

	for (idx = 0; idx < max_height; idx++) {
		int rest_spaces = 0;

		for (i = 0; i < size; i++) {
			int line;
			int w, h;

			get_print_size(res[i].type, &w, &h);

			if (idx < 3)
				line = idx;
			else
				line = h + idx - max_height;

			if (idx >= 3 && line < 3) {
				rest_spaces += w + horiz_print_distance;
			} else {
				char *str;
				for (j = 0; j < rest_spaces; j++)
					printf(" ");

				str = linestr(res[i], hl, line);
				rest_spaces = w - strlen_col(str) + horiz_print_distance;
				printf("%s", str);
			}
		}

		printf("\n");
	}
}

void print_result(struct intval *res, int size, enum output_type type)
{
	struct intval tmp;
	struct intval diff;
	struct intval hl;
	struct winsize w;
	int total_width;
	int i, j;

	diff.s64 = 0;
	enum intval_type min_type = INVAL;
	for (i = 0; i < size; i++) {
		for (j = 0; j < i; j++) {
			min_type = res[i].type < res[j].type ? res[i].type : res[j].type;
			INTVAL_OP_BIN(tmp, res[i], res[j], ^);
			diff.s64 |= tmp.s64;
		}
	}

	switch (min_type) {
	case S8  : diff.s64 &= 0x00000000000000ff; 	break;
	case S16 : diff.s64 &= 0x000000000000ffff;	break;
	case S32 : diff.s64 &= 0x00000000ffffffff;	break;
	case S64 : diff.s64 &= 0xffffffffffffffff;	break;
	default : break;
	}

	switch (type) {
	case OUTPUT_COMMON :
		hl.s64 = ~diff.s64;
		break;
	case OUTPUT_DIFF :
		hl.s64 = diff.s64;
		break;
	case OUTPUT_NORM :
	default :
		hl.s64 = 0;
		break;
	}

	total_width = 0;
	for (i = 0; i < size; i++) {
		int w;
		get_print_size(res[i].type, &w, NULL);
		total_width += w;
	}
	total_width += (size - 1) * horiz_print_distance;

	ioctl(0, TIOCGWINSZ, &w);

	if (opts_list || size == 1 || total_width > w.ws_col)
		print_vert(res, size, hl);
	else
		print_horiz(res, size, hl);
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
	if (opts_version) {
		tobin_version();
		goto end;
	}

	result = calloc(argc, sizeof(struct intval));
	for (i = 0; i < argc; i++) {
		intval_type = get_intval_type(opts_type);
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
			"Supported types: 8, 16, 32, 64 bit.\n"
			"Data type for each used value can be specified in () right before the value\n"
			"itself.\n"
			"\n"
			"Examples:\n"
			"embox tobin 0xee488f10\n"
			"embox tobin 0x00000000ee488f10\n"
			"embox tobin 0x00001fde 0x1242 0xdeadbeef\n"
			"embox tobin --diff 0x23 0xee48f120 0x4fd1\n"
			"embox tobin \"4 + 8 << (2 - 1)\"\n"
			"embox tobin 0x52ea \"(0x18 << 5) + 0x8937ffee\"")
