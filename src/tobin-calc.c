#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "tobin.h"


struct opdesc ops[] = {
	OPDESC(MULT,    1, "*"),
	OPDESC(DIV,     1, "/"),
	OPDESC(REST,    1, "%"),
	OPDESC(PLUS,    2, "+"),
	OPDESC(MINUS,   2, "-"),
	OPDESC(SHIFT_L, 3, "<<"),
	OPDESC(SHIFT_R, 3, ">>"),
	OPDESC(AND,     4, "&"),
	OPDESC(XOR,     5, "^"),
	OPDESC(OR,      6, "|"),
	OPDESC(PAR_L,   9, "("),
	OPDESC(PAR_R,   9, ")"),
};


int prior(struct symbol *s)
{
	if (s && s->type == OPERATOR)
		return ops[s->op].prior;
	else {
		printf("Error: prioritet for invalid symbol requested\n");
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
		printf("%d", s->val.s32);
	else if (s->type == OPERATOR)
		printf("%s", ops[s->op].str);
}

void print_token(char *token)
{
	printf("Token: '%s'\n", token);
}

void print_queue(struct symbol_queue *out)
{
	struct symbol *s;

	printf("Out:  ");
	s = out->first;
	while (s) {
		printf(" ");
		print_symbol(s);
		s = s->next;
	}
	printf("\n");
}

void print_stack(struct symbol_stack *stack)
{
	struct symbol *s;
	struct symbol *s_print;

	printf("Stack:");
	s_print = stack->bottom;
	if (s_print) {
		fflush(stdout);
		while (s_print != stack->top) {
			s = stack->top;
			fflush(stdout);
			while (s->next != s_print) {
				s = s->next;
			}
			printf(" ");
			print_symbol(s_print);
			s_print = s;
		}
		printf(" ");
		print_symbol(s_print);
	}
	printf("\n");
}


void print_state(char *token, struct symbol_queue *out,
	struct symbol_stack *stack)
{

	print_token(token);

	printf(" ");
	print_queue(out);

	printf(" ");
	print_stack(stack);
}

struct symbol *symbol_create(char *token)
{
	int i;
	struct symbol *s;

	s = calloc(1, sizeof(struct symbol));

	for (i = 0; i < ARRAY_SIZE(ops); i++)
	{
		if (!strcmp(ops[i].str, token)) {
			s->type = OPERATOR;
			s->op = ops[i].type;
			break;
		}
	}

	if (s->type == NONE) {
		s->type = INTVAL;
		/* TODO: parse type specification if presen */
		s->val.type = S32;
		s->val.s32 = strtol(token, NULL, 0);
	}

	return s;
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


struct symbol_queue *expr_parse(char *str_orig)
{
	char *token;
	char *str;
	struct symbol_queue *out;
	struct symbol_stack *stack;
	struct symbol *s;

	out   = symbol_queue_create();
	stack = symbol_stack_create();

	str = strdup(str_orig);

	printf("Initial string: %s\n", str);
	token = strtok(str, " ");
	while (token) {
		s = symbol_create(token);

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
						printf("Error: no opening parentheses found\n");
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

		token = strtok(NULL, " ");
	}

	s = symbol_stack_pull(stack);
	while (s) {
		symbol_queue_add(out, s);
		s = symbol_stack_pull(stack);
	}

	free(str);
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

int rpn_eval(struct symbol_queue *rpn)
{
	struct symbol *s, *s_new;
	struct symbol *s1, *s2;
	struct symbol_stack *stack;
	int ret;

	stack = symbol_stack_create();

	s = rpn->first;
	while (s) {
		if (s->type == INTVAL) {
			s_new = symbol_clone(s);
			symbol_stack_push(stack, s_new);
		} else if (s->type == OPERATOR) {
			int val;

			s2 = symbol_stack_pull(stack);
			s1 = symbol_stack_pull(stack);
			if (!s1 || !s2) {
				printf("Error: corrupted RPN evaluation stack\n");
				exit(1);
			}

			switch (s->op) {
			case MULT :
				val = s1->val.s32 * s2->val.s32;
				break;
			case DIV :
				val = s1->val.s32 / s2->val.s32;
				break;
			case REST :
				val = s1->val.s32 % s2->val.s32;
				break;
			case PLUS :
				val = s1->val.s32 + s2->val.s32;
				break;
			case MINUS :
				val = s1->val.s32 - s2->val.s32;
				break;
			case SHIFT_L :
				val = s1->val.s32 << s2->val.s32;
				break;
			case SHIFT_R :
				val = s1->val.s32 >> s2->val.s32;
				break;
			case AND :
				val = s1->val.s32 & s2->val.s32;
				break;
			case XOR :
				val = s1->val.s32 ^ s2->val.s32;
				break;
			case OR :
				val = s1->val.s32 | s2->val.s32;
				break;
			default :
				val = -1;
				break;
			}

			s1->type = INTVAL;
			s1->val.s32 = val;
			symbol_stack_push(stack, s1);
		}

		s = s->next;
		rpn->first = s;
	}

	if (stack->top != stack->bottom) {
		printf("Error: corrupted stack at finish stage\n");
		print_stack(stack);
		exit(1);
	}

	s = symbol_stack_pull(stack);
	ret = s->val.s32;

	free(stack);

	return ret;
}
