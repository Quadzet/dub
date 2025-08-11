#include "lexer.h"
#include "assert.h"

static int i = 0;
static struct t_vector *tokens;

char *expr_type_strs[] = {
	"GROUP",
	"UNARY",
	"BINARY",
	"LITERAL"
};


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


char *stmt_type_strs[] = {
	"EXPR",
	"PRINT"
};

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

char *literal_expr_to_str(struct expr *e)
{
	if (e == NULL || e->op->lexeme == NULL) {
		printf("Null expr in literal_expr_to_str");
		return NULL;
	}

	int  len  = strlen(e->op->lexeme);
	char *ret = malloc(len + 1);
	if (ret == NULL) {
		printf("Error allocating lexeme expr string");
		return NULL;
	}
	char *ptr = ret;

	memcpy(ptr, e->op->lexeme, len);
	ptr += len;
	*ptr = '\0';

	return ret;
}

char *unary_expr_to_str(struct expr *e, char *expr_str)
{
	if (!e || e->op->lexeme == NULL) {
		printf("Null expr or lexeme in unary_expr_to_str");
		return NULL;
	}
	if (expr_str == NULL) {
		printf("Null expr string in unary_expr_to_str");
		return NULL;
	}

	int  expr_len  = strlen(expr_str);
	int  op_len    = strlen(e->op->lexeme);
	char *ret      = malloc(expr_len + op_len + 4);
	if (ret == NULL) {
		printf("Error allocating unary expr string");
		return NULL;
	}

	char *ptr      = ret;

	*ptr++ = '(';
	memcpy(ptr, e->op->lexeme, op_len);
	ptr += op_len;
	*ptr++ = ' ';
	memcpy(ptr, expr_str, expr_len);
	ptr += expr_len;
	*ptr++ = ')';
	*ptr = '\0';

	return ret;
}

char *binary_expr_to_str(struct expr *e, char *expr_str_l, char *expr_str_r)
{
	if (e == NULL || e->op->lexeme == NULL) {
		printf("Null expr or lexeme in binary_expr_to_str");
		return NULL;
	}
	if (expr_str_l == NULL && expr_str_r == NULL) {
		printf("Null expr string in binary_expr_to_str");
		return NULL;
	}

	int  expr_len_l  = strlen(expr_str_l);
	int  expr_len_r  = strlen(expr_str_r);
	int  op_len      = strlen(e->op->lexeme);
	char *ret        = malloc(expr_len_l + expr_len_r + op_len + 5);
	if (ret == NULL) {
		printf("Error allocating binary expr string");
		return NULL;
	}
	char *ptr = ret;

	*ptr++ = '(';
	memcpy(ptr, e->op->lexeme, op_len);
	ptr += op_len;
	*ptr++ = ' ';
	memcpy(ptr, expr_str_l, expr_len_l);
	ptr += expr_len_l;
	*ptr++ = ' ';
	memcpy(ptr, expr_str_r, expr_len_r);
	ptr += expr_len_r;
	*ptr++ = ')';
	*ptr = '\0';

	return ret;
}

char *group_expr_to_str(struct expr *e, char *expr_str)
{
	if (e == NULL) {
		printf("Null expr in group_expr_to_str");
		return NULL;
	}
	if (expr_str == NULL) {
		printf("Null expr string in group_expr_to_str");
		return NULL;
	}

	char *op = "group";
	int  expr_len  = strlen(expr_str);
	int  op_len    = strlen(op);
	char *ret      = malloc(op_len + expr_len + 4);
	if (ret == NULL) {
		printf("Error allocating group expr string");
		return NULL;
	}
	char *ptr      = ret;

	*ptr++ = '(';
	memcpy(ptr, op, op_len);
	ptr += op_len;
	*ptr++ = ' ';
	memcpy(ptr, expr_str, expr_len);
	ptr += expr_len;
	*ptr++ = ')';
	*ptr = '\0';

	return ret;
}

char *expr_to_str(struct expr *e)
{
	if (e == NULL) {
		printf("Null expr in expr_to_str\n");
		return NULL;
	}
	char *ret;
	if (e->type == LITERAL)
		return literal_expr_to_str(e);
	else if (e->type == UNARY) {
		char *expr_str = expr_to_str(e->right);
		if (expr_str == NULL) {
			printf("Null expr string in unary expression");
			free(expr_str);
			return NULL;
		}
		ret = unary_expr_to_str(e, expr_str);
		free(expr_str);
	} else if (e->type == BINARY) {
		char *expr_str_l = expr_to_str(e->left);
		char *expr_str_r = expr_to_str(e->right);
		if (expr_str_l == NULL || expr_str_r == NULL) {
			printf("Null expr string in binary expression");
			free(expr_str_l);
			free(expr_str_r);
			return NULL;
		}
		ret = binary_expr_to_str(e, expr_str_l, expr_str_r);
		free(expr_str_l);
		free(expr_str_r);
	} else if (e->type == GROUP) {
		char *expr_str = expr_to_str(e->right);
		if (expr_str == NULL) {
			printf("Null expr string in group expression");
			free(expr_str);
			return NULL;
		}
		ret = group_expr_to_str(e, expr_str);
		free(expr_str);
	} else
		ret = NULL;

	return ret;
}

static void error(struct token *t, char *msg)
{
	if (t->type == END) {
		printf("Error on line %d at end: %s\n", t->line, msg);
	} else {
		printf("Error on line %d at %s: %s\n", t->line, t->lexeme, msg);
	}
}

static struct token *peek()
{
	if (i + 1 >= tokens->size) {
		static struct token end_token = { END, "", -1 };
		return &end_token;
	}
	return &tokens->array[i+1];
}

static struct token *get()
{
	if (i + 1 >= tokens->size) {
		static struct token end_token = { END, "", -1 };
		return &end_token;
	}
	return &tokens->array[++i];
}

static int check(enum token_type type)
{
	return peek()->type == type;
}

static struct token *consume(enum token_type type)
{
	if (check(type)) {
		return get();
	} else {
		char err[256];
		sprintf(err, "Unexpected token: %s", peek()->lexeme);
		error(peek(), err);
		return NULL;
	}
}


struct expr *expression();

struct expr *primary()
{
	struct expr *ret = malloc(sizeof(struct expr));
	if (!ret) return NULL;
	struct token *op   = peek();
	if (check(FALSE) || check(TRUE) || check(NUMBER) || check(STRING) || check(NIL)) {
		ret->type = LITERAL;
		ret->left = NULL;
		ret->op = get();
		ret->right = NULL;
		printf("Literal expression: %s\n", expr_to_str(ret));
		return ret;
	} else if (check(LEFT_PAREN)) {
		get();
		struct expr *e = expression();
		if (!e) {
			free(ret);
			return NULL;
		}
		if (!consume(RIGHT_PAREN)) {
			error(peek(), "Expected closing parenthesis");
			free(e);
			return NULL;
		}

		ret->type = GROUP;
		ret->left = NULL;
		ret->op = NULL;
		ret->right = e;
		printf("Group expression: %s\n", expr_to_str(ret));
		return ret;
	} else {
		error(peek(), "Expected expression");
		free(ret);
		return NULL;
	}
}

struct expr *unary()
{
	while (check(BANG) || check(MINUS)) {
		struct token *op   = get();
		printf("Unary operation, lexeme: %s\n", op->lexeme);
		struct expr *right = unary();
		if (!right) return NULL;

		struct expr *e     = malloc(sizeof(struct expr));
		if (!e) {
			free(right);
			return NULL;
		}
		e->type      = UNARY;
		e->left      = NULL;
		e->op  = op;
		e->right     = right;
		return e;
	}
	return primary();
}

struct expr *factor()
{
	struct expr *e = unary();
	if (!e) return NULL;

	while (check(SLASH) || check(STAR)) {
		struct token *op   = get();
		printf("Factor operation, lexeme: %s\n", op->lexeme);
		struct expr *right = unary();
		if (!right) {
			free(e);
			return NULL;
		}

		struct expr *tmp   = malloc(sizeof(struct expr));
		if (!tmp) {
			free(e);
			free(right);
			return NULL;
		}
		tmp->type      = BINARY;
		tmp->left      = e;
		tmp->op        = op;
		tmp->right     = right;
		e = tmp;
	}
	printf("Factor expression: %s\n", expr_to_str(e));
	return e;
}

struct expr *term()
{
	struct expr *e = factor();
	if (!e) return NULL;

	while (check(MINUS) || check(PLUS)) {
		struct token *op = get();
		printf("Term operation, lexeme: %s\n", op->lexeme);
		struct expr *right = factor();
		if (!right) {
			free(e);
			return NULL;
		}

		struct expr *tmp = malloc(sizeof(struct expr));
		if (!tmp) {
			free(e);
			free(right);
			return NULL;
		}
		tmp->type      = BINARY;
		tmp->left      = e;
		tmp->op        = op;
		tmp->right     = right;
		e = tmp;
	}
	return e;
	printf("Term expression: %s\n", expr_to_str(e));
}

struct expr *comparison()
{
	struct expr *e = term();

	while (check(GREATER) || check(GREATER_EQUAL) || check(LESS) || check(LESS_EQUAL)) {
		struct token *op = get();
		printf("Comparison operation, lexeme: %s\n", op->lexeme);
		struct expr *right = term();
		if (!right) {
			free(e);
			return NULL;
		}

		struct expr *tmp = malloc(sizeof(struct expr));
		if (!tmp) {
			free(e);
			free(right);
			return NULL;
		}
		tmp->type      = BINARY;
		tmp->left      = e;
		tmp->op        = op;
		tmp->right     = right;
		e = tmp;
	}
	printf("Comparison expression: %s\n", expr_to_str(e));
	return e;
}

struct expr *equality()
{
	struct expr *e = comparison();

	while (check(EQUAL_EQUAL) || check(BANG_EQUAL)) {
		struct token *op = get();
		printf("Equality operation, lexeme: %s\n", op->lexeme);
		struct expr *right = comparison();
		if (!right) {
			free(e);
			return NULL;
		}

		struct expr *tmp = malloc(sizeof(struct expr));
		if (!tmp) {
			free(e);
			free(right);
			return NULL;
		}
		tmp->type      = BINARY;
		tmp->left      = e;
		tmp->op  = op;
		tmp->right     = right;
		e = tmp;
	}
	printf("Equality expression: %s\n", expr_to_str(e));
	return e;
}

struct expr *expression()
{
	return equality();
}

struct stmt *print_stmt()
{
	struct stmt *st = malloc(sizeof(struct stmt));
	st->type = STMT_PRINT;
	st->e = expression();
	consume(SEMICOLON);
	return st;
}

struct stmt *statement()
{
	if (check(PRINT))
		return print_stmt();

	printf("Normal statement, generating expr\n");
	struct stmt *st = malloc(sizeof(struct stmt));
	st->type = STMT_EXPR;
	st->e = expression();
	printf("Expression: %s\n", expr_to_str(st->e));
	printf("Consuming semicolon\n");
	consume(SEMICOLON);
	return st;
}


void free_expr(struct expr *e)
{
	if (!e) return;
	free_expr(e->left);
	free_expr(e->right);
	free(e);
}

struct stmt **ast(struct t_vector *in_tokens)
{
	int array_size = 64;
	struct stmt **stmts = calloc(array_size, sizeof(struct stmt));

	printf("Generating Abstract Syntax Tree\n");
	tokens = in_tokens;
	i = -1;

	int st_ix = 0;
	while (peek()->type != END) {
		if (st_ix == array_size - 1) {
			struct stmt **new_stmts = malloc(sizeof(struct stmt) * array_size * 2);
			memcpy(new_stmts, stmts, array_size * sizeof(struct stmt*));
			array_size *= 2;
			stmts = new_stmts;
		}

		printf("Generating statement number %d\n", st_ix + 1);
		struct stmt* st = statement();
		if (!st) {
			printf("Failed to generate Abstract Syntax Tree\n");
			return NULL;
		} else {
			printf("Generated statement number %d\n", st_ix + 1);
			printf("Expr: %s\n\n", expr_to_str(st->e));
		}
		stmts[st_ix++] = st;
	}

	printf("Abstract Syntax Tree complete. View string representation below:\n\n");
	for (int i = 0; i < st_ix; i++) {
		char *repr = expr_to_str(stmts[i]->e);
		if (!repr) {
			printf("Failed to generate a string representation of the Abstract Syntax Tree\n");
			return stmts;
		}
		printf("Statement %d: %s ;\n", i + 1, repr);
	}

	printf("\nEnd of string representation.\n\n");
	return stmts;
}


