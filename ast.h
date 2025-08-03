#ifndef AST_H
#define AST_H

#include "lexer.h"

enum expr_type
{
	GROUP,
	UNARY,
	BINARY,
	LITERAL,
};

struct expr
{
	enum expr_type type;
	struct expr *left;
	struct token *operator;
	struct expr *right;
};



int assert(int check, char *msg);
char *literal_expr_to_str(struct expr *e);
char *unary_expr_to_str(struct expr *e, char *expr_str);
char *binary_expr_to_str(struct expr *e, char *expr_str_l, char *expr_str_r);
char *group_expr_to_str(struct expr *e, char *expr_str);
char *expr_to_str(struct expr *e);



struct expr *primary();
struct expr *unary();
struct expr *factor();
struct expr *term();
struct expr *comparison();
struct expr *equality();
struct expr *expression();

void free_expr(struct expr *e);
void ast(struct t_vector *in_tokens);

#endif
