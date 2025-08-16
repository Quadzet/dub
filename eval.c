#include "ast.h"
#include "logging.h"
#include "state.h"


char *eval_to_str(struct eval *v, int include_type)
{
	char tmp[256];
	
	switch (v->type) {
	case V_INT:
		sprintf(tmp, "%s%d", include_type ? "[INT]:" : "", v->value.ival);
		break;
	case V_DOUBLE:
		sprintf(tmp, "%s%f", include_type ? "[DOUBLE]: " : "", v->value.dval);
		break;
	case V_STRING:
		sprintf(tmp, "%s%s", include_type ? "[STRING]: " : "", v->value.sval);
		break;
	case V_BOOL:
		sprintf(tmp, "%s%s", (include_type ? "[BOOL]: " : ""),
			(v->value.boolean ? "true" : "false"));
		break;
	case V_NIL:
		strcpy(tmp, "[NIL]: nil");
		break;
	default:
		strcpy(tmp, "[UNKNOWN]: <unknown type>");
		break;
	}

	int len = strlen(tmp);
	char *ret = malloc(len + 1);
	if (!ret) return NULL;

	strcpy(ret, tmp);

	return ret;
}

int truthy(struct eval *v)
{
	switch (v->type) {
	case V_INT:
		return v->value.ival != 0;
	case V_DOUBLE:
		return v->value.dval != 0;
	case V_BOOL:
		return v->value.boolean != 0;
	case V_NIL:
		return 0;
	case V_STRING:
		return v->value.sval != NULL;
	default:
		log_message(ERROR, "Unhandled value type in truthy");
		return 0;
	}
}

struct eval evaluate(struct expr *e);

struct eval eval_literal(struct expr *e)
{
	struct eval v;
	char *expr = expr_to_str(e);

	v.type = e->op->value_type;
	v.value = e->op->value;
	return v;
}


struct eval eval_group(struct expr *e)
{
	struct eval v = evaluate(e->right);
	return v;
}

struct eval eval_unary(struct expr *e)
{
	struct eval v = evaluate(e->right);
	switch (e->op->type) {
	case MINUS:
		if (v.type == V_INT)
			v.value.ival = -v.value.ival;
		else if (v.type == V_DOUBLE)
			v.value.dval = -v.value.dval;
		break;
	case BANG:
		if (v.type == V_BOOL) {
			v.value.boolean = !truthy(&v);
			v.type = V_BOOL;
		}
		break;
	default:
		break;
	}
	return v;
}

struct eval eval_binary(struct expr *e)
{
	struct eval v;
	struct eval left = evaluate(e->left);
	struct eval right = evaluate(e->right);

	log_message(DEBUG, "Evaluating binary expr ");
	switch (e->op->type) {
	case SLASH:
		log_message(DEBUG, "division");
		if (left.type == V_INT && right.type == V_INT) {
			if (right.value.ival == 0) {
				log_message(ERROR, "Division by zero");
				v.type = V_NIL;
				return v;
			}
			v.value.ival = left.value.ival / right.value.ival;
			v.type = V_INT;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			if (right.value.dval == 0) {
				log_message(ERROR, "Division by zero");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.ival / right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			if (right.value.ival == 0) {
				log_message(ERROR, "Division by zero");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.dval / right.value.ival;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			if (right.value.dval == 0) {
				log_message(ERROR, "Division by zero");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.dval / right.value.dval;
			v.type = V_DOUBLE;
		} else {
			log_message(ERROR, "Invalid types for division");
			v.type = V_NIL;
		}
		break;
	case STAR:
		log_message(DEBUG, "multiplication");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.ival = left.value.ival * right.value.ival;
			v.type = V_INT;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.dval = left.value.ival * right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.dval = left.value.dval * right.value.ival;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.dval = left.value.dval * right.value.dval;
			v.type = V_DOUBLE;
		} else {
			log_message(ERROR, "Invalid types for multiplication");
			v.type = V_NIL;
		}
		break;
	case MINUS:
		log_message(DEBUG, "subtraction");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.ival = left.value.ival - right.value.ival;
			v.type = V_INT;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.dval = left.value.ival - right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.dval = left.value.dval - right.value.ival;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.dval = left.value.dval - right.value.dval;
			v.type = V_DOUBLE;
		} else {
			log_message(ERROR, "Invalid types for subtraction");
			v.type = V_NIL;
		}
		break;
	case PLUS:
		log_message(DEBUG, "addition");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.ival = left.value.ival + right.value.ival;
			v.type = V_INT;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.dval = left.value.ival + right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.dval = left.value.dval + right.value.ival;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.dval = left.value.dval + right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_STRING && right.type == V_STRING) {
			int len1 = strlen(left.value.sval);
			int len2 = strlen(right.value.sval);
			char *res = malloc(len1 + len2 + 1);
			memcpy(res, left.value.sval, len1);
			memcpy(res + len1, right.value.sval, len2);
			res[len1 + len2] = '\0';

			v.value.sval = res;
			v.type = V_STRING;
		} else {
			log_message(ERROR, "Invalid types for addition");
			v.type = V_NIL;
		}

		break;
	case GREATER:
		log_message(DEBUG, "greater comparison");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.boolean = left.value.ival > right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.boolean = left.value.ival > right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.boolean = left.value.dval > right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.boolean = left.value.dval > right.value.dval;
			v.type = V_BOOL;
		} else {
			log_message(ERROR, "Invalid types for comparison");
			v.type = V_NIL;
		}

		break;
	case GREATER_EQUAL:
		log_message(DEBUG, "greater or equal comparison");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.boolean = left.value.ival >= right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.boolean = left.value.ival >= right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.boolean = left.value.dval >= right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.boolean = left.value.dval >= right.value.dval;
			v.type = V_BOOL;
		} else {
			log_message(ERROR, "Invalid types for comparison");
			v.type = V_NIL;
		}
		break;
	case LESS:
		log_message(DEBUG, "less than comparison");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.boolean = left.value.ival < right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.boolean = left.value.ival < right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.boolean = left.value.dval < right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.boolean = left.value.dval < right.value.dval;
			v.type = V_BOOL;
		} else {
			log_message(ERROR, "Invalid types for comparison");
			v.type = V_NIL;
		}
		break;
	case LESS_EQUAL:
		log_message(DEBUG, "less than or equal comparison");
		if (left.type == V_INT && right.type == V_INT) {
			v.value.boolean = left.value.ival <= right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.boolean = left.value.ival <= right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.boolean = left.value.dval <= right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.boolean = left.value.dval <= right.value.dval;
			v.type = V_BOOL;
		} else {
			log_message(ERROR, "Invalid types for comparison");
			v.type = V_NIL;
		}
		break;
	case EQUAL_EQUAL:
		log_message(DEBUG, "equality comparison");
		if (left.type == V_BOOL && right.type == V_BOOL) {
			v.value.boolean = left.value.boolean == right.value.boolean;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_INT) {
			v.value.boolean = left.value.ival == right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_STRING && right.type == V_STRING) {
			v.value.boolean = !strcmp(left.value.sval, right.value.sval);
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			v.value.boolean = left.value.dval == right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			v.value.boolean = left.value.ival == right.value.dval;
			v.type = V_BOOL;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			v.value.boolean = left.value.dval == right.value.ival;
			v.type = V_BOOL;
		} else if (left.type == V_NIL && right.type == V_NIL) {
			v.value.boolean = left.value.null == right.value.null;
			v.type = V_BOOL;
		} else {
			log_message(ERROR, "Invalid types for comparison");
			v.type = V_NIL;
		}
	default:
		log_message(ERROR, "Unexpected op type in eval_binary");
		v.type = V_NIL;
		break;
	}
	log_message(DEBUG, "Returning value: %s", eval_to_str(&v, 1));
	return v;
}

struct eval evaluate(struct expr *e)
{
	log_message(DEBUG, "Evaluating expr: %s", expr_to_str(e));

	switch (e->type) {
	case LITERAL:
		return eval_literal(e);
	case GROUP:
		return eval_group(e);
	case UNARY:
		return eval_unary(e);
	case BINARY:
		return eval_binary(e);
	default:
		log_message(ERROR, "Unhandled expr type in evaluate");
		struct eval v;
		return v;
	}
}

void execute(struct stmt *st)
{
	struct eval v = evaluate(st->e);
	log_message(DEBUG, "Evaluation complete, result: %s", eval_to_str(&v, 1));
	switch (st->type) {
	case STMT_PRINT:
		printf("%s", eval_to_str(&v, 0));
		break;
	case STMT_EXPR:
		break;
	default:
		log_message(ERROR, "Unhandled statement type");
		return;
	}
}

void interpret(struct stmt **stmts)
{
	int i = 0;
	if (!stmts[i])
		log_message(ERROR, "stmt %d: null stmt", i + 1);
	while (stmts[i]) {
		if (!stmts[i]->e)
			log_message(ERROR, "stmt %d: null eval", i + 1);
		execute(stmts[i]);
		free_expr(stmts[i]->e);
		i++;
	}
}






