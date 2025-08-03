#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 10000
#define T_VEC_SIZE (1<<10)
#define HASH_SIZE 100

enum errors {
	MALLOC_ERROR,
	PUSH_BACK_ERROR,
	INVALID_CHAR_ERROR,
	UNTERMINATED_STRING_ERROR,
	SPRINTF_ERROR,
};

enum token_type {
	// single char
	LEFT_PAREN = 1, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	LEFT_BRACKET, RIGHT_BRACKET,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, COLON,
	SLASH, STAR, 

	// comparisons, one or two char
	BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

	// literals
	IDENTIFIER, STRING, NUMBER,

	// keywords
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	END,
};

struct hash_node
{
	struct hash_node *next;
	char *key;
	char *val;
};

struct token
{
	enum token_type type;
	char *lexeme;
	char *literal;
	// literal ..?
	int line;
};

struct t_vector
{
	struct token *array;
	int capacity;
	int size;
};

int hash(char *key);
int contains(struct hash_node *set[], char *key);
int add(struct hash_node *set[], char *key, char *val);
char *token_to_str(struct token *t);
int push_back(struct t_vector *t_vec, struct token t);

char match(char *buffer, int *i, char expect);
int is_keyword(char *word);
struct token identifier(char *buffer, int *i, int *line);
struct token number(char *buffer, int *i, int *line);
struct token string(char *buffer, int *i, int *line);
int scan_token(char *buffer, struct token *t);
struct t_vector *parse_buffer(char *buffer);

#endif
