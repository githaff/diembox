#ifndef __TOBIN_H
#define __TOBIN_H

#include "common.h"


struct intval {
	enum intval_type { INVAL = 0, U8, U16, U32, U64 } type;
	union {
		u8_t  u8;
		u16_t u16;
		u32_t u32;
		u64_t u64;
	};
};

extern enum intval_type default_intval_type;

enum operator { PLUS, MINUS, MULT, DIV, REST, SHIFT_L, SHIFT_R, NEG,
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

struct symbol *symbol_create();
struct symbol_queue *symbol_queue_create();
struct symbol_stack *symbol_stack_create();

void symbol_destroy(struct symbol *s);
void symbol_queue_destroy(struct symbol_queue *queue);
void symbol_stack_destroy(struct symbol_stack *stack);


#endif /* __TOBIN_H */
