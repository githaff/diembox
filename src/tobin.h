#ifndef __TOBIN_H
#define __TOBIN_H

#include "common.h"


struct intval {
	enum intval_type { INVAL = 0, S8, U8, S16, U16, S32, U32, S64, U64 } type;
	union {
		s8_t  s8;
		u8_t  u8;
		s16_t s16;
		u16_t u16;
		s32_t s32;
		u32_t u32;
		s64_t s64;
		u64_t u64;
	};
};

extern enum intval_type default_initval_type;

enum operator { PLUS, MINUS, MULT, DIV, REST, SHIFT_L, SHIFT_R,
				AND, XOR, OR, PAR_L, PAR_R };

struct opdesc {
	enum operator type;
	int prior;
	char *str;
};

#define OPDESC(id, prior, str) [id] = { id, prior, str }
extern struct opdesc ops[];

struct symbol {
	enum { NONE = 0, INTVAL, OPERATOR } type;
	union {
		struct intval val;
		enum operator op;
	};
	struct symbol *next;
};

struct symbol_queue {
	struct symbol *first;
	struct symbol *last;
};

struct symbol_stack {
	struct symbol *top;
	struct symbol *bottom;
};


struct symbol_queue *expr_parse(char *str_orig);
void print_queue(struct symbol_queue *out);
struct intval rpn_eval(struct symbol_queue *rpn);


#endif /* __TOBIN_H */
