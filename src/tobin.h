#ifndef __TOBIN_H
#define __TOBIN_H

#include "common.h"

#define INTVAL_SET(V, VAL)						\
	switch (V.type) {							\
	case U8  : V.u8  = VAL; break;				\
	case U16 : V.u16 = VAL; break;				\
	case U32 : V.u32 = VAL; break;				\
	case U64 : V.u64 = VAL; break;				\
	default : break;							\
	}
#define INTVAL_OP_BIN(V_OUT, V1, V2, OP)					\
	{														\
		V_OUT.type = intval_type;							\
		switch (intval_type) {								\
		case U8  : V_OUT.u8  = (V1.u8  OP V2.u8);  break; 	\
		case U16 : V_OUT.u16 = (V1.u16 OP V2.u16); break;	\
		case U32 : V_OUT.u32 = (V1.u32 OP V2.u32); break;	\
		case U64 : V_OUT.u64 = (V1.u64 OP V2.u64); break;	\
		default  :											\
			V_OUT.u64 = 0;									\
			dbg_msg("invalid default intval type"); 		\
			break;											\
		}													\
	}
#define INTVAL_OP_UNO(V_OUT, V1, OP)				\
	{												\
		V_OUT.type = intval_type;					\
		switch (intval_type) {						\
		case U8  : V_OUT.u8  = (OP V1.u8);  break;	\
		case U16 : V_OUT.u16 = (OP V1.u16); break;	\
		case U32 : V_OUT.u32 = (OP V1.u32); break;	\
		case U64 : V_OUT.u64 = (OP V1.u64); break;	\
		default  :									\
			V_OUT.u64 = 0;							\
			dbg_msg("invalid default intval type");	\
			break;									\
		}											\
	}
#define S_OP_BIN(S_OUT, S1, S2, OP) INTVAL_OP_BIN(S_OUT->val, S1->val, S2->val, OP)
#define S_OP_UNO(S_OUT, S1, OP) INTVAL_OP_UNO(S_OUT->val, S1->val, OP)

struct intval {
	enum intval_type { INVAL = 0, U8, U16, U32, U64 } type;
	union {
		u8_t  u8;
		u16_t u16;
		u32_t u32;
		u64_t u64;
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
