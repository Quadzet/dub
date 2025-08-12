#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "logging.h"

#define MAXLINE 10000

static int i = 0;
static int line = 1;

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

char *token_strs[] = {
	"NULL",

	"LEFT_PAREN",
	"RIGHT_PAREN",
	"LEFT_BRACE",
	"RIGHT_BRACE",
	"LEFT_BRACKET",
	"RIGHT_BRACKET",
	"COMMA",
	"DOT",
	"MINUS",
	"PLUS",
	"SEMICOLON",
	"COLON",
	"SLASH",
	"STAR",

	"BANG",
	"BANG_EQUAL",
	"EQUAL",
	"EQUAL_EQUAL",
	"GREATER",
	"GREATER_EQUAL",
	"LESS",
	"LESS_EQUAL",

	"IDENTIFIER",
	"STRING",
	"NUMBER",

	"AND",
	"CLASS",
	"ELSE",
	"FALSE",
	"FUN",
	"FOR",
	"IF",
	"NIL",
	"OR",
	"PRINT",
	"RETURN",
	"SUPER",
	"THIS",
	"TRUE",
	"VAR",
	"WHILE",

	"END"
};

char *keywords[] = {
	"and",
	"class",
	"else",
	"false",
	"for",
	"fun",
	"if",
	"nil",
	"or",
	"print",
	"return",
	"super",
	"this",
	"true",
	"var",
	"while"
};

int keyword_tokens[] = {
	AND,
	CLASS,
	ELSE,
	FALSE,
	FOR,
	FUN,
	IF,
	NIL,
	OR,
	PRINT,
	RETURN,
	SUPER,
	THIS,
	TRUE,
	VAR,
	WHILE
};

int hash(char *key)
{
	int i, hash = 0;
	char c;
	for (i = 0; (c = key[i]) != '\0'; i++)
		hash += c;
	return (hash * 75948) % 100;
}

#define HASH_SIZE 100
struct hash_node
{
	struct hash_node *next;
	char *key;
	char *val;
};

int contains(struct hash_node *set[], char *key)
{
	int ix = hash(key);
	struct hash_node *n = set[ix];
	while (n) {
		if (strcmp(key, n->key) == 0)
			return 1;
		n = n->next;
			
	}
	return 0;
}

int add(struct hash_node *set[], char *key, char *val)
{
	int ix = hash(key);

	struct hash_node *n = set[ix];
	while (n) {
		if (strcmp(key, n->key) == 0) {
			free(n->val);
			n->val = strdup(val);
			return 1;
		}
		n = n->next;
	}

	struct hash_node *new_n = malloc(sizeof(struct hash_node));
	if (!new_n)
		return 0;
	new_n->key = strdup(key);
	new_n->val = strdup(val);
	new_n->next = set[ix];
	set[ix] = new_n;

	return 1;
}

enum val_type {
	V_INT, V_DOUBLE, V_STRING, V_NIL, V_BOOL
};

union val {
	int ival;
	double dval;
	char *sval;
	int null;
	int boolean;
};

struct token
{
	enum token_type type;
	char *lexeme;
	int line;
	union val value;
	enum val_type value_type;
};

char *token_to_str(struct token *t)
{
	char *repr = malloc(256);
	if (!repr) 
		return NULL;

	int n_chars = 0;
	switch (t->value_type) {
	case V_INT:
		n_chars = sprintf(repr, "[%u] %s: [type %u] [value %d]",
			t->line, t->lexeme, t->type, t->value.ival);
		break;
	case V_DOUBLE:
		n_chars = sprintf(repr, "[%u] %s: [type %u] [value %f]",
			t->line, t->lexeme, t->type, t->value.dval);
		break;
	case V_STRING:
		n_chars = sprintf(repr, "[%u] %s: [type %u] [value %s]",
			t->line, t->lexeme, t->type, t->value.sval);
		break;
	case V_NIL:
		n_chars = sprintf(repr, "[%u] %s: [type %u] [value %d]",
			t->line, t->lexeme, t->type, t->value.null);
		break;
	case V_BOOL:
		n_chars = sprintf(repr, "[%u] %s: [type %u] [value %d]",
			t->line, t->lexeme, t->type, t->value.boolean);
		break;
	default:
		n_chars = sprintf(repr, "[%u] %s: [type %u]",
			t->line, t->lexeme, t->type);
		break;
	}

	if (n_chars < 0) {
		log_message(ERROR, "sprintf error in token_to_str");
		free(repr);
		return NULL;
	}
	return repr;
}

#define T_VEC_SIZE 1<<10
struct t_vector
{
	struct token *array;
	int capacity;
	int size;
};

void free_t_vector(struct t_vector *vec)
{
	if (!vec) return;
	free(vec->array);
	free(vec);
}

int push_back(struct t_vector *t_vec, struct token t)
{
	if (t_vec->size > t_vec->capacity) {
		log_message(ERROR, "Invalid size for pushback. Size: %d, capacity: %d", t_vec->size, t_vec->capacity);
		return 1;
	} else if (t_vec->size == t_vec->capacity) {
		struct token *new_array = malloc(sizeof(struct token) * t_vec->capacity * 2);
		if (!new_array) return 1;

		memcpy(new_array, t_vec->array, t_vec->size * sizeof(struct token));
		free(t_vec->array);
		t_vec->array = new_array;
		t_vec->capacity *= 2;
	}
	t_vec->array[t_vec->size]= t;
	t_vec->size++;
	return 0;
}

char match(char *buffer, int *i, char expect)
{
	char peek = buffer[*i];
	if (peek == expect) {
		++*i;
		return 1;
	}
	else
		return 0;
}

int is_keyword(char *word)
{
	int count = sizeof(keywords) / sizeof(keywords[0]);
	for (int i = 0; i < count; i++)
		if (!strcmp(keywords[i], word))
			return keyword_tokens[i];
	return 0;
};

struct token identifier(char *buffer, int *i, int *line)
{
	int start = *i - 1;
	while (isalnum(buffer[*i]) || buffer[*i] == '_')
		++*i;

	int len = *i - start;
	char *identifier_str = malloc(len + 1);
	memcpy(identifier_str, buffer + start, len);
	identifier_str[len] = '\0';

	int type = is_keyword(identifier_str);
	union val v;
	enum val_type vtype;

	if (!type) {
		type = IDENTIFIER;
		v.sval = identifier_str;
		vtype = V_STRING;
	} else {
		switch (type) {
		case TRUE:
			v.boolean = 1;
			vtype = V_BOOL;
			break;
		case FALSE:
			v.boolean = 0;
			vtype = V_BOOL;
			break;
		case NIL:
			v.null = 0;
			vtype = V_NIL;
			break;
		default:
			v.sval = identifier_str;
			vtype = V_STRING;
			break;
		}
	}

	struct token t = { type, identifier_str, *line, v, vtype };
	return t;
}

struct token number(char *buffer, int *i, int *line)
{
	int start = *i - 1;
	while (isdigit(buffer[*i]))
		++*i;
	if (buffer[*i] == '.' && isdigit(buffer[(*i) + 1]))
		++*i;
	while (isdigit(buffer[*i]))
		++*i;
	
	int len = *i - start;
	char *num_str = malloc(len + 1);
	if (!num_str) {
		log_message(ERROR, "Failed to allocate memory for number");
		struct token t = { 0 };
		return t;
	}
	memcpy(num_str, buffer + start, len);
	num_str[len] = '\0';

	char *dummy_ptr;
	union val v;
	v.dval = strtod(num_str, &dummy_ptr);
	struct token t = { NUMBER, num_str, *line, v, V_DOUBLE};
	return t;
}

struct token string(char *buffer, int *i, int *line)
{
	int start = *i - 1;
	int start_line = *line;
	while (buffer[*i] != '"' && buffer[*i] != '\0') {
		if (buffer[*i] == '\n')
			++*line;
		++*i;
	}

	if (buffer[*i] == '\0') {
		log_message(ERROR, "[%d] Unterminated string", *line);
		struct token t = { 0 };
		return t;
	}

	++*i; // skip end "

	int literal_len = *i - start - 2;
	// my string
	char *literal = malloc(literal_len + 1);
	if (!literal) {
		log_message(ERROR, "Failed to allocate memory for string literal");
		struct token t = { 0 };
		return t;
	}
	memcpy(literal, buffer + start + 1, literal_len);
	literal[literal_len] = '\0';

	// "my string"
	int lexeme_len = literal_len + 2;
	char *lexeme = malloc(lexeme_len + 1);
	if (!lexeme) {
		free(literal);
		log_message(ERROR, "Failed to allocate memory for string lexeme");
		struct token t = { 0 };
		return t;
	}
	lexeme[0] = '"';
	memcpy(lexeme + 1, buffer + start + 1, literal_len);
	lexeme[lexeme_len - 1] = '"';
	lexeme[lexeme_len] = '\0';

	union val v;
	v.sval = literal;
	struct token t = { STRING, lexeme, start_line, v, V_STRING };
	return t;
}

int scan_token(char *buffer, struct token *t)
{
	char c;
	char err_str[256];
	while (1) {
		c = buffer[i++];
		switch (c)
		{
		case '\n':
			line++; break;
		case '\r':
		case '\t':
		case ' ':
			break;

		case '(': *t = (struct token) { LEFT_PAREN, "(", line }; return line;
		case ')': *t = (struct token) { RIGHT_PAREN, ")", line }; return line;
		case '{': *t = (struct token) { LEFT_BRACE, "{", line }; return line;
		case '}': *t = (struct token) { RIGHT_BRACE, "}", line }; return line;
		case '[': *t = (struct token) { LEFT_BRACKET, "[", line }; return line;
		case ']': *t = (struct token) { RIGHT_BRACKET, "]", line }; return line;
		case ',': *t = (struct token) { COMMA, ",", line }; return line;
		case '.': *t = (struct token) { DOT, ".", line }; return line;
		case '-': *t = (struct token) { MINUS, "-", line }; return line;
		case '+': *t = (struct token) { PLUS, "+", line }; return line;
		case ';': *t = (struct token) { SEMICOLON, ";", line }; return line;
		case ':': *t = (struct token) { COLON, ":", line }; return line;
		case '*': *t = (struct token) { STAR, "*", line }; return line;

		case '/':
			if (match(buffer, &i, '/')) {
				while (buffer[i++] != '\n' && buffer[i] != '\0')
					;
			} else {
				*t = (struct token) { SLASH, "/", line };
				return line;
			}
			break;
		case '!':
			*t = match(buffer, &i, '=')
				? (struct token) { BANG_EQUAL, "", line }
				: (struct token) { BANG, "!", line };
			return line;
		case '=':
			*t = match(buffer, &i, '=')
				? (struct token) { EQUAL_EQUAL, "==", line }
				: (struct token) { EQUAL, "=", line };
			return line;
		case '<':
			*t = match(buffer, &i, '=')
				? (struct token) { LESS_EQUAL, "<=", line }
				: (struct token) { LESS, "<", line };
			return line;

		case '>':
			*t = match(buffer, &i, '=')
				? (struct token) { GREATER_EQUAL, ">=", line }
				: (struct token) { GREATER, ">", line };
			return line;


		case '"':
			*t = string(buffer, &i, &line);
			return line;

		case '\0':
			return 0;
		default:
			if (isdigit(c)) {
				*t = number(buffer, &i, &line);
				return line;
			} else if (isalpha(c)) {
				*t = identifier(buffer, &i, &line);
				return line;
			} else {
				log_message(ERROR, "[%d] Unexpected character: %c, ASCI code: %d",
					line, c, c);
				return 0;
			}
			break;
		}
	}
	return 0;
}

struct t_vector *parse_buffer(char *buffer)
{
	struct token *tmp = malloc(T_VEC_SIZE);
	struct t_vector *t_vec = malloc(sizeof(struct t_vector));
	t_vec->array = tmp;
	t_vec->size = 0;
	t_vec->capacity = T_VEC_SIZE;
	
	log_message(DEBUG, "Parsing buffer...");
	char c;
	i = 0;
	line = 1;
	struct token t;
	while (scan_token(buffer, &t)) {
		log_message(DEBUG, "Scanned token %s, as %s.", token_strs[t.type], t.lexeme);
		int err = push_back(t_vec, t);
		if (err != 0) {
			log_message(ERROR, "Error while parsing file, aborting.");
			break;
		}
	}
	log_message(DEBUG, "Parsing complete\n");

	return t_vec;
}

