#ifndef AST_H
#define AST_H

#include "lexer.h"

extern char *expr_type_strs[4];

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
	struct token *op;
	struct expr *right;
};

extern char *stmt_type_strs[];

enum stmt_type
{
	STMT_EXPR,
	STMT_PRINT
};

struct stmt
{
	enum stmt_type type;
	struct expr *e;
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
struct stmt *print_stmt();
struct stmt *statement();

void free_expr(struct expr *e);
struct stmt **ast(struct t_vector *in_tokens);

#endif
