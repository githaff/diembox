#ifndef __TOBIN_H
#define __TOBIN_H

#include "common.h"

#define INTVAL_SET(V, VAL)						\
	switch (V.type) {							\
	case S8  : V.s8  = VAL; break;				\
	case S16 : V.s16 = VAL; break;				\
	case S32 : V.s32 = VAL; break;				\
	case S64 : V.s64 = VAL; break;				\
	default : break;							\
	}
#define INTVAL_OP_BIN(V_OUT, V1, V2, OP)					\
	{														\
		V_OUT.type = intval_type;							\
		switch (intval_type) {								\
		case S8  : V_OUT.s8  = (V1.s8  OP V2.s8);  break; 	\
		case S16 : V_OUT.s16 = (V1.s16 OP V2.s16); break;	\
		case S32 : V_OUT.s32 = (V1.s32 OP V2.s32); break;	\
		case S64 : V_OUT.s64 = (V1.s64 OP V2.s64); break;	\
		default  :											\
			V_OUT.s64 = 0;									\
			dbg_msg("invalid default intval type"); 		\
			break;											\
		}													\
	}
#define INTVAL_OP_UNO(V_OUT, V1, OP)				\
	{												\
		V_OUT.type = intval_type;					\
		switch (intval_type) {						\
		case S8  : V_OUT.s8  = (OP V1.s8);  break;	\
		case S16 : V_OUT.s16 = (OP V1.s16); break;	\
		case S32 : V_OUT.s32 = (OP V1.s32); break;	\
		case S64 : V_OUT.s64 = (OP V1.s64); break;	\
		default  :									\
			V_OUT.s64 = 0;							\
			dbg_msg("invalid default intval type");	\
			break;									\
		}											\
	}
#define S_OP_BIN(S_OUT, S1, S2, OP) INTVAL_OP_BIN(S_OUT->val, S1->val, S2->val, OP)
#define S_OP_UNO(S_OUT, S1, OP) INTVAL_OP_UNO(S_OUT->val, S1->val, OP)

struct intval {
	enum intval_type { INVAL = 0, S8, S16, S32, S64 } type;
	union {
		s8_t  s8;
		s16_t s16;
		s32_t s32;
		s64_t s64;
	};
};

extern enum intval_type intval_type;
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
