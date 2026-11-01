#ifndef INTERPRET_H
#define INTERPRET_H

#include "ast.h"
#include "logging.h"
#include "state.h"

char *eval_to_str(struct eval *v);
int truthy(struct eval *v);

struct eval eval_literal(struct expr *e);
struct eval eval_group(struct expr *e);
struct eval eval_unary(struct expr *e);
struct eval eval_binary(struct expr *e);
struct eval evaluate(struct expr *e);

int execute(struct stmt *st);
int interpret(struct stmt **stmts);

#endif
