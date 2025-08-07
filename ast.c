#include "lexer.h"

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

int assert(int check, char *msg)
{
	if (!check) {
		printf("%s\n", msg);
		return 1;
	}
	return 0;
}

char *literal_expr_to_str(struct expr *e)
{
	if (assert(e != NULL && e->op->lexeme != NULL, "Null expr in literal_expr_to_str")) return NULL;

	int  len  = strlen(e->op->lexeme);
	char *ret = malloc(len + 1);
	if (assert(ret != NULL, "Error allocating lexeme expr string")) return NULL;
	char *ptr = ret;

	memcpy(ptr, e->op->lexeme, len);
	ptr += len;
	*ptr = '\0';

	return ret;
}

char *unary_expr_to_str(struct expr *e, char *expr_str)
{
	if (assert(e && e->op->lexeme != NULL, "Null expr or lexeme in unary_expr_to_str")) return NULL;
	if (assert(expr_str != NULL, "Null expr string in unary_expr_to_str")) return NULL;

	int  expr_len  = strlen(expr_str);
	int  op_len    = strlen(e->op->lexeme);
	char *ret      = malloc(expr_len + op_len + 4);
	if (assert(ret != NULL, "Error allocating unary expr string"))
		return NULL;

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
	if (assert(e != NULL && e->op->lexeme != NULL, "Null expr or lexeme in binary_expr_to_str")) return NULL;
	if (assert(expr_str_l != NULL && expr_str_r != NULL, "Null expr string in binary_expr_to_str")) return NULL;

	int  expr_len_l  = strlen(expr_str_l);
	int  expr_len_r  = strlen(expr_str_r);
	int  op_len      = strlen(e->op->lexeme);
	char *ret        = malloc(expr_len_l + expr_len_r + op_len + 5);
	if (assert(ret != NULL, "Error allocating binary expr string")) return NULL;
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
	if (assert(e != NULL, "Null expr in group_expr_to_str")) return NULL;
	if (assert(expr_str != NULL, "Null expr string in group_expr_to_str")) return NULL;

	char *op = "group";
	int  expr_len  = strlen(expr_str);
	int  op_len    = strlen(op);
	char *ret      = malloc(op_len + expr_len + 4);
	if (assert(ret != NULL, "Error allocating group expr string"))
		return NULL;
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
	if (assert(e != NULL, "Null expr in expr_to_str")) return NULL;
	char *ret;
	if (e->type == LITERAL)
		return literal_expr_to_str(e);
	else if (e->type == UNARY) {
		char *expr_str = expr_to_str(e->right);
		if (assert(expr_str != NULL, "Null expr string in unary expression")) {
			free(expr_str);
			return NULL;
		}
		ret = unary_expr_to_str(e, expr_str);
		free(expr_str);
	} else if (e->type == BINARY) {
		char *expr_str_l = expr_to_str(e->left);
		char *expr_str_r = expr_to_str(e->right);
		if (assert(expr_str_l != NULL && expr_str_r != NULL, "Null expr string in binary expression")) {
			free(expr_str_l);
			free(expr_str_r);
			return NULL;
		}
		ret = binary_expr_to_str(e, expr_str_l, expr_str_r);
		free(expr_str_l);
		free(expr_str_r);
	} else if (e->type == GROUP) {
		char *expr_str = expr_to_str(e->right);
		if (assert(expr_str != NULL, "Null expr string in group expression")) {
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
		printf("Literal expression, lexeme: %s\n", op->lexeme);
		ret->type = LITERAL;
		ret->left = NULL;
		ret->op = get();
		ret->right = NULL;
		return ret;
	} else if (check(LEFT_PAREN)) {
		printf("Group expression, lexeme: %s\n", op->lexeme);
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
		tmp->op  = op;
		tmp->right     = right;
		e = tmp;
	}
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
		tmp->op  = op;
		tmp->right     = right;
		e = tmp;
	}
	return e;
}

struct expr *comparison()
{
	struct expr *e = term();

	while (check(GREATER) || check(GREATER_EQUAL) || check(LESS) || check(LESS_EQUAL)) {
		struct token *op = get();
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
		tmp->op  = op;
		tmp->right     = right;
		e = tmp;
	}
	return e;
}

struct expr *equality()
{
	struct expr *e = comparison();

	while (check(EQUAL_EQUAL) || check(BANG_EQUAL)) {
		struct token *op = get();
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
	return e;
}

struct expr *expression()
{
	return equality();
}

void free_expr(struct expr *e)
{
	if (!e) return;
	free_expr(e->left);
	free_expr(e->right);
	free(e);
}

struct expr *ast(struct t_vector *in_tokens)
{
	printf("Generating Abstract Syntax Tree\n");
	tokens = in_tokens;
	i = -1;
	struct expr* e = expression();
	if (!e) {
		printf("Failed to generate Abstract Syntax Tree\n");
		return NULL;
	}
	char *repr = expr_to_str(e);
	if (!repr) {
		printf("Failed to generate a string representation of the Abstract Syntax Tree\n");
		return e;
	}

	printf("Abstract Syntax Tree complete. View string representation below:\n\n");
	printf("%s\n", repr);
	printf("\nEnd of string representation.\n\n");

	free(repr);
	return e;
}


