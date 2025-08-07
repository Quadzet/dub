#include "ast.h"

struct eval
{
	enum val_type type;
	union val value;
};

void eval_to_str(struct eval *v)
{
    switch (v->type) {
        case V_INT:
            printf("[INT]: %d\n", v->value.ival);
            break;
        case V_DOUBLE:
            printf("[DOUBLE]: %f\n", v->value.dval);
            break;
        case V_STRING:
            printf("[STRING]: %s\n", v->value.sval);
            break;
        case V_BOOL:
            printf("[BOOL]: %s\n", v->value.boolean ? "true" : "false");
            break;
        case V_NIL:
            printf("[NIL]: nil\n");
            break;
        default:
            printf("[UNKNOWN]: <unknown type>\n");
            break;
	}
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
		printf("Unhandled value type in truthy\n");
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

	printf("Evaluating binary expr ");
	switch (e->op->type) {
	case SLASH:
		printf("division");
		if (left.type == V_INT && right.type == V_INT) {
			if (right.value.ival == 0) {
				printf("Division by zero\n");
				v.type = V_NIL;
				return v;
			}
			v.value.ival = left.value.ival / right.value.ival;
			v.type = V_INT;
		} else if (left.type == V_INT && right.type == V_DOUBLE) {
			if (right.value.dval == 0) {
				printf("Division by zero\n");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.ival / right.value.dval;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_INT) {
			if (right.value.ival == 0) {
				printf("Division by zero\n");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.dval / right.value.ival;
			v.type = V_DOUBLE;
		} else if (left.type == V_DOUBLE && right.type == V_DOUBLE) {
			if (right.value.dval == 0) {
				printf("Division by zero\n");
				v.type = V_NIL;
				return v;
			}
			v.value.dval = left.value.dval / right.value.dval;
			v.type = V_DOUBLE;
		} else {
			printf("Invalid types for division\n");
			v.type = V_NIL;
		}
		break;
	case STAR:
		printf("multiplication");
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
			printf("Invalid types for multiplication\n");
			v.type = V_NIL;
		}
		break;
	case MINUS:
		printf("subtraction");
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
			printf("Invalid types for subtraction\n");
			v.type = V_NIL;
		}
		break;
	case PLUS:
		printf("addition");
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
		} else {
			printf("Invalid types for addition\n");
			v.type = V_NIL;
		}

		break;
	case GREATER:
		printf("greater comparison");
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
			printf("Invalid types for comparison\n");
			v.type = V_NIL;
		}

		break;
	case GREATER_EQUAL:
		printf("greater or equal comparison");
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
			printf("Invalid types for comparison\n");
			v.type = V_NIL;
		}
		break;
	case LESS:
		printf("less than comparison");
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
			printf("Invalid types for comparison\n");
			v.type = V_NIL;
		}
		break;
	case LESS_EQUAL:
		printf("less than or equal comparison");
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
			printf("Invalid types for comparison\n");
			v.type = V_NIL;
		}
		break;
	case EQUAL_EQUAL:
		printf("equality comparison");
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
			printf("Invalid types for comparison\n");
			v.type = V_NIL;
		}
	default:
		printf("Unexpected op type in eval_binary\n");
		v.type = V_NIL;
		break;
	}
	printf("\nReturning value: ");
	eval_to_str(&v);
	printf("\n");
	return v;
}

struct eval evaluate(struct expr *e)
{
	printf("Evaluating expr:\n");
	char *repr = expr_to_str(e);
	if (!repr)
		printf("<error in string representation>\n\n");
	else
		printf("%s\n\n", repr);

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
		printf("Unhandled expr type in evaluate\n");
		struct eval v;
		return v;
	}
}
