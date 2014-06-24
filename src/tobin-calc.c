#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "tobin.h"


struct opdesc ops[] = {
	OPDESC(NEG,     1, "~"),
	OPDESC(MULT,    2, "*"),
	OPDESC(DIV,     2, "/"),
	OPDESC(REST,    2, "%"),
	OPDESC(PLUS,    3, "+"),
	OPDESC(MINUS,   3, "-"),
	OPDESC(SHIFT_L, 4, "<<"),
	OPDESC(SHIFT_R, 4, ">>"),
	OPDESC(AND,     5, "&"),
	OPDESC(XOR,     6, "^"),
	OPDESC(OR,      7, "|"),
	OPDESC(PAR_L,   9, "("),
	OPDESC(PAR_R,   9, ")"),
};

int prior(struct symbol *s)
{
	if (s && s->type == OPERATOR)
		return ops[s->op].prior;
	else {
		err_msg("priority for invalid symbol requested\n");
		return 0;
	}
}

void symbol_queue_add(struct symbol_queue *queue, struct symbol *s)
{
	if (!queue->last)
		queue->last = queue->first = s;
	else {
		queue->last->next = s;
		queue->last = s;
	}
	s->next = NULL;
}

void symbol_stack_push(struct symbol_stack *stack, struct symbol *s)
{
	if (!stack->top) {
		stack->top = stack->bottom = s;
		s->next = NULL;
	}
	else {
		s->next = stack->top;
		stack->top = s;
	}
}

struct symbol *symbol_stack_pull(struct symbol_stack *stack)
{
	struct symbol *s;

	s = stack->top;
	if (stack->top) {
		stack->top = stack->top->next;
	}

	if (!stack->top)
		stack->bottom = NULL;

	return s;
}

void print_symbol(struct symbol *s)
{
	if (s->type == INTVAL)
		/* TODO: fix for other integer types */
		dbg_msg("%d", s->val.s32);
	else if (s->type == OPERATOR)
		dbg_msg("%s", ops[s->op].str);
}

void print_token(char *token)
{
	dbg_msg("Token: '%s'\n", token);
}

void print_queue(struct symbol_queue *out)
{
	struct symbol *s;

	dbg_msg("Out:  ");
	s = out->first;
	while (s) {
		dbg_msg(" ");
		print_symbol(s);
		s = s->next;
	}
	dbg_msg("\n");
}

void print_stack(struct symbol_stack *stack)
{
	struct symbol *s;
	struct symbol *s_print;

	dbg_msg("Stack:");
	s_print = stack->bottom;
	if (s_print) {
		fflush(stdout);
		while (s_print != stack->top) {
			s = stack->top;
			fflush(stdout);
			while (s->next != s_print) {
				s = s->next;
			}
			dbg_msg(" ");
			print_symbol(s_print);
			s_print = s;
		}
		dbg_msg(" ");
		print_symbol(s_print);
	}
	dbg_msg("\n");
}


void print_state(char *token, struct symbol_queue *out,
	struct symbol_stack *stack)
{

	print_token(token);

	dbg_msg(" ");
	print_queue(out);

	dbg_msg(" ");
	print_stack(stack);
}

struct symbol_queue *symbol_queue_create()
{
	return calloc(1, sizeof(struct symbol_queue));
}

struct symbol_stack *symbol_stack_create()
{
	return calloc(1, sizeof(struct symbol_stack));
}

char *strdup(char *str)
{
	char *str_new;
	str_new = (char *)calloc(strlen(str) + 1, sizeof(char));
	strcpy(str_new, str);
	return str_new;
}

int char_in_ops(char symb)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ops); i++)
	{
		if (symb == ops[i].str[0])
			return 1;
	}

	return 0;
}

int char_in_set(char symb, char *set)
{
	while (*set) {
		if (symb == *set)
			return 1;
		set++;
	}

	return 0;
}

int strcmp_part(char **whole_orig, char *part)
{
	char *whole = *whole_orig;

	while (*part) {
		if (*whole != *part)
			return 1;

		part++;
		whole++;
	}

	*whole_orig = whole;

	return 0;
}

enum intval_type extract_intstr(char **str_orig, char *buf)
{
	char *str = *str_orig;
	enum intval_type type = default_initval_type;
	int len = 0;

	if (*str == '-') {
		*buf = *str;
		buf++;
		str++;
	}

	while (*str) {
		/* Correct intstr termination - operation*/
		if (char_in_ops(*str))
			break;
		/* Correct intstr termination - whitespace*/
		if (char_in_set(*str, " "))
			break;

		/* Incorrect integer */
		if (!char_in_set(*str, "1234567890xabcdefABCDEF"))
			return INVAL;

		*buf = *str;
		str++;
		buf++;
		len++;
	}
	*buf = 0;

	if (!len)
		return INVAL;
	else
		*str_orig = str;

	return type;
}

int read_val(char *valstr, struct symbol *s)
{
	int ret = 1;
	char *endptr;

	if (s->type != INTVAL)
		return 0;

	switch (s->val.type) {
	case S8  : s->val.s8  = strtoll(valstr, &endptr, 0); break;
	case U8  : s->val.u8  = strtoll(valstr, &endptr, 0); break;
	case S16 : s->val.s16 = strtoll(valstr, &endptr, 0); break;
	case U16 : s->val.u16 = strtoll(valstr, &endptr, 0); break;
	case S32 : s->val.s32 = strtoll(valstr, &endptr, 0); break;
	case U32 : s->val.u32 = strtoll(valstr, &endptr, 0); break;
	case S64 : s->val.s64 = strtoll(valstr, &endptr, 0); break;
	case U64 : s->val.u64 = strtoll(valstr, &endptr, 0); break;
	default : break;
	}

	if (*endptr)
		ret = 0;

	return ret;
}

struct symbol *symbol_extract(char **str_orig)
{
	char *str = *str_orig;
	struct symbol *s;
	char buf[256];
	enum intval_type type;
	int i;

	while (*str == ' ')
		str++;

	if (*str == 0)
		return 0;

	s = calloc(1, sizeof(struct symbol));
	s->type = NONE;

	type = extract_intstr(&str, buf);
	if (type) {
		s->type = INTVAL;
		s->val.type = type;
		if (!read_val(buf, s)) {
			err_msg("invalid integer symbol '%s'\n", buf);
			s->type = NONE;
		}
		goto ret_symbol;
	}

	if (s->type == NONE) {
		for (i = 0; i < ARRAY_SIZE(ops); i++)
		{
			if (!strcmp_part(&str, ops[i].str)) {
				s->type = OPERATOR;
				s->op = ops[i].type;
				goto ret_symbol;
			}
		}
	}

	s->type = NONE;
	return s;

ret_symbol:
	*str_orig = str;

	return s;
}

struct symbol_queue *expr_parse(char *str)
{
	struct symbol_queue *out;
	struct symbol_stack *stack;
	struct symbol *s;

	out   = symbol_queue_create();
	stack = symbol_stack_create();

	printf("STR=%s\n", str);

	s = symbol_extract(&str);
	while (s) {
		if (s->type == NONE) {
			err_msg("incorrect symbol at '%s'\n", str);
			exit(1);
		}

		if (s->type == INTVAL)
			symbol_queue_add(out, s);
		else if (s->type == OPERATOR) {
			struct symbol *top;

			switch (s->op) {
			case PAR_L :
				symbol_stack_push(stack, s);
				break;
			case PAR_R :
				while (stack->top) {
					top = symbol_stack_pull(stack);
					if (!top) {
						err_msg("no opening parentheses found\n");
						exit(1);
					}

					if (top->op == PAR_L)
						break;
					else
						symbol_queue_add(out, top);
				}
				break;
			default :
				while (stack->top && prior(s) >= prior(stack->top)) {

					top = symbol_stack_pull(stack);
					symbol_queue_add(out, top);
				}
				symbol_stack_push(stack, s);
				break;
			}
		}

		s = symbol_extract(&str);
	}

	s = symbol_stack_pull(stack);
	while (s) {
		symbol_queue_add(out, s);
		s = symbol_stack_pull(stack);
	}

	free(stack);

	return out;
}

struct symbol *symbol_clone(struct symbol *s)
{
	struct symbol *s_new;

	s_new = malloc(sizeof(struct symbol));
	memcpy(s_new, s, sizeof(struct symbol));

	return s_new;
}

#define S_INT(S)								\
	((S->val.type == S8)  ? S->val.s8  :		\
	 (S->val.type == U8)  ? S->val.u8  :		\
	 (S->val.type == S16) ? S->val.s16 :		\
	 (S->val.type == U16) ? S->val.u16 :		\
	 (S->val.type == S32) ? S->val.s32 :		\
	 (S->val.type == U32) ? S->val.u32 :		\
	 (S->val.type == S64) ? S->val.s64 :		\
	 (S->val.type == U64) ? S->val.u64 : 0)
#define S_OP_BIN(S1, S2, OP)					\
	S1->val.type = default_initval_type;		\
	S1->val.s64 = ((S_INT(S2)) OP (S_INT(S1)))
#define S_OP_UNO(S1, OP)				\
	S1->val.type = default_initval_type;		\
	S1->val.s64 = (OP (S_INT(S1)))
#define S_OP_TYP(S1, TYPE, TYPE_ID)				\
	S1->val.type = TYPE_ID;						\
	S1->val.TYPE = (TYPE##_t)(S_INT(S1))

struct intval rpn_eval(struct symbol_queue *rpn)
{
	struct symbol *s, *s_new;
	struct symbol *s1, *s2;
	struct symbol_stack *stack;
	struct intval intval;

	stack = symbol_stack_create();

	s = rpn->first;
	while (s) {
		if (s->type == INTVAL) {
			s_new = symbol_clone(s);
			symbol_stack_push(stack, s_new);
		} else if (s->type == OPERATOR) {
			s1 = symbol_stack_pull(stack);
			if (!s1) {
				err_msg("corrupted RPN evaluation stack\n");
				exit(1);
			}

			s1->type = INTVAL;
			switch (s->op) {
			case NEG : S_OP_UNO(s1, ~);  break;
				/* One operand operators */
			default :
				s2 = symbol_stack_pull(stack);
				if (!s2) {
					err_msg("corrupted RPN evaluation stack\n");
					exit(1);
				}
				switch (s->op) {
					/* Two operand operators */
				case MULT    : S_OP_BIN(s1, s2, *);  break;
				case DIV     : S_OP_BIN(s1, s2, /);  break;
				case REST    : S_OP_BIN(s1, s2, %);  break;
				case PLUS    : S_OP_BIN(s1, s2, +);  break;
				case MINUS   : S_OP_BIN(s1, s2, -);  break;
				case SHIFT_L : S_OP_BIN(s1, s2, <<); break;
				case SHIFT_R : S_OP_BIN(s1, s2, >>); break;
				case AND     : S_OP_BIN(s1, s2, &);  break;
				case XOR     : S_OP_BIN(s1, s2, ^);  break;
				case OR      : S_OP_BIN(s1, s2, |);  break;
				default      : s->val.s32 = -1;      break;
				}
			}

			symbol_stack_push(stack, s1);
		}

		s = s->next;
		rpn->first = s;
	}

	if (stack->top != stack->bottom) {
		err_msg("corrupted stack at finish stage\n");
		print_stack(stack);
		exit(1);
	}

	s = symbol_stack_pull(stack);
	if (s->type != INTVAL) {
		err_msg("RPN evaluation result is not intval\n");
		exit(1);
	}
	intval = s->val;

	free(stack);

	return intval;
}
