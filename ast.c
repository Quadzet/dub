#include "lexer.c"

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
	struct token operator;
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
	if (assert(e != NULL && e->operator.literal != NULL, "Null expr in literal_expr_to_str")) return NULL;

	int  len  = strlen(e->operator.literal);
	char *ret = malloc(len + 3);
	if (assert(ret != NULL, "Error allocating literal expr string")) return NULL;
	char *ptr = ret;

	*ptr++ = '(';
	memcpy(ptr, e->operator.literal, len);
	ptr += len;
	*ptr++ = ')';
	*ptr = '\0';

	return ret;
}

char *unary_expr_to_str(struct expr *e, char *expr_str)
{
	if (assert(e && e->operator.literal != NULL, "Null expr or literal in unary_expr_to_str")) return NULL;
	if (assert(expr_str != NULL, "Null expr string in unary_expr_to_str")) return NULL;

	int  expr_len  = strlen(expr_str);
	int  op_len    = strlen(e->operator.literal);
	char *ret      = malloc(expr_len + op_len + 4);
	if (assert(ret != NULL, "Error allocating unary expr string"))
		return NULL;

	char *ptr      = ret;

	*ptr++ = '(';
	memcpy(ptr, e->operator.literal, op_len);
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
	if (assert(e != NULL && e->operator.literal != NULL, "Null expr or literal in binary_expr_to_str")) return NULL;
	if (assert(expr_str_l != NULL && expr_str_r != NULL, "Null expr string in binary_expr_to_str")) return NULL;

	int  expr_len_l  = strlen(expr_str_l);
	int  expr_len_r  = strlen(expr_str_r);
	int  op_len      = strlen(e->operator.literal);
	char *ret        = malloc(expr_len_l + expr_len_r + op_len + 5);
	if (assert(ret != NULL, "Error allocating binary expr string")) return NULL;
	char *ptr = ret;

	*ptr++ = '(';
	memcpy(ptr, e->operator.literal, op_len);
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
