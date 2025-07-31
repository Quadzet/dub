#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 10000

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

void error(int code, char *msg)
{
	printf("Error %d: %s\r\n", code, msg);
}

void error_no_msg(int code)
{
	error(code, "No message");
}

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

// struct hash_node **get_reserved_words()
// {
// 	struct hash_node *map[100] = malloc(sizeof(struct hash_node) * 100);
//
// }

struct literal
{
	// union/variant..?
};

struct token
{
	enum token_type type;
	char *lexeme;
	char *literal;
	// literal ..?
	int line;
};

char *token_to_str(struct token *t)
{
	char *repr = malloc(256);
	if (!repr) 
		return NULL;

	int n_chars = sprintf(repr, "[%u] %s: %s [type %u]",
		  t->line, t->lexeme, t->literal, t->type);
	if (n_chars < 0) {
		error(SPRINTF_ERROR, "sprintf error in token_to_str");
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

int push_back(struct t_vector *t_vec, struct token t)
{
	if (t_vec->size > t_vec->capacity) {
		char err[256];
		sprintf(err, "Invalid size for pushback. Size: %d, capacity: %d\r\n",
			t_vec->size, t_vec->capacity);
		error(PUSH_BACK_ERROR, err);
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
	if (!type)
		type = IDENTIFIER;

	struct token t = { type, identifier_str, identifier_str, *line };
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
		error(MALLOC_ERROR, "Failed to allocate memory for number");
		struct token t = { 0 };
		return t;
	}
	memcpy(num_str, buffer + start, len);
	num_str[len] = '\0';

	struct token t = { NUMBER, num_str, num_str, *line };
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
		char err_str[256];
		snprintf(
			err_str, sizeof(err_str),
			"[%d] Unterminated string",
			*line);
		error(UNTERMINATED_STRING_ERROR, err_str);
		struct token t = { 0 };
		return t;
	}

	++*i; // skip end "

	int literal_len = *i - start - 2;
	// my string
	char *literal = malloc(literal_len + 1);
	if (!literal) {
		error(MALLOC_ERROR, "Failed to allocate memory for string literal");
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
		error(MALLOC_ERROR, "Failed to allocate memory for string lexeme");
		struct token t = { 0 };
		return t;
	}
	lexeme[0] = '"';
	memcpy(lexeme + 1, buffer + start + 1, literal_len);
	lexeme[lexeme_len - 1] = '"';
	lexeme[lexeme_len] = '\0';

	struct token t = { STRING, lexeme, literal, start_line };
	return t;
}

int scan_token(char *buffer, struct token *t)
{
	static int i = 0;
	// printf("i is now %d\r\n", i);
	static int line = 1;
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

		case '(': *t = (struct token) { LEFT_PAREN, "(", "", line }; return line;
		case ')': *t = (struct token) { RIGHT_PAREN, ")", "", line }; return line;
		case '{': *t = (struct token) { LEFT_BRACE, "{", "", line }; return line;
		case '}': *t = (struct token) { RIGHT_BRACE, "}", "", line }; return line;
		case '[': *t = (struct token) { LEFT_BRACKET, "[", "", line }; return line;
		case ']': *t = (struct token) { RIGHT_BRACKET, "]", "", line }; return line;
		case ',': *t = (struct token) { COMMA, ",", "", line }; return line;
		case '.': *t = (struct token) { DOT, ".", "", line }; return line;
		case '-': *t = (struct token) { MINUS, "-", "", line }; return line;
		case '+': *t = (struct token) { PLUS, "+", "", line }; return line;
		case ';': *t = (struct token) { SEMICOLON, ";", "", line }; return line;
		case ':': *t = (struct token) { COLON, ":", "", line }; return line;
		case '*': *t = (struct token) { STAR, "*", "", line }; return line;

		case '/':
			if (match(buffer, &i, '/')) {
				while (buffer[i++] != '\n' && buffer[i] != '\0')
					;
			} else {
				*t = (struct token) { SLASH, "/", "", line };
				return line;
			}
			break;
		case '!':
			*t = match(buffer, &i, '=')
				? (struct token) { BANG_EQUAL, "!=", "", line }
				: (struct token) { BANG, "!", "", line };
			return line;
		case '=':
			*t = match(buffer, &i, '=')
				? (struct token) { EQUAL_EQUAL, "==", "", line }
				: (struct token) { EQUAL, "=", "", line };
			return line;
		case '<':
			*t = match(buffer, &i, '=')
				? (struct token) { LESS_EQUAL, "<=", "", line }
				: (struct token) { LESS, "<", "", line };
			return line;

		case '>':
			*t = match(buffer, &i, '=')
				? (struct token) { GREATER_EQUAL, ">=", "", line }
				: (struct token) { GREATER, ">", "", line };
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
				snprintf(
					err_str, sizeof(err_str),
					"[%d] Unexpected character: %c, ASCI code: %d",
					line, c, c);
				error(INVALID_CHAR_ERROR, err_str);
				return 0;
			}
			break;
		}
	}
	return 0;
}

struct t_vector parse_buffer(char *buffer)
{
	struct token tmp[T_VEC_SIZE];
	struct t_vector t_vec = { tmp, T_VEC_SIZE, 0 };
	
	char c;
	int i = 0;
	struct token t;
	while (scan_token(buffer, &t)) {
		printf("Scanned token %s, as %s.\r\n", token_strs[t.type], t.lexeme);
		int err = push_back(&t_vec, t);
		if (err != 0) {
			printf("Error while parsing file, aborting.");
			break;
		}
	}
	return t_vec;
}

void run(char *buffer)
{
	parse_buffer(buffer);
	// lexical analysis...
	// intermediate repr...
	// compilation...
}

void run_file(char *path)
{
	printf("Opening file...\r\n");
	int err_code;
	FILE *fp = fopen(path, "r");
	if ((err_code = fseek(fp, 0, SEEK_END)) != 0)
		error(err_code, "fseek end error");
	int file_size = ftell(fp);
	if ((err_code = fseek(fp, 0, SEEK_SET)) != 0)
		error(err_code, "fseek set error");
	printf("Allocating buffer...\r\n");
	char *buffer =  malloc(file_size + 1);
	if (buffer) {
		fread(buffer, 1, file_size, fp);
		*(buffer + file_size) = '\0';
	}
	fclose(fp);

	if (!buffer) {
		error(MALLOC_ERROR, "Error allocating file buffer");
		return;
	}

	printf("Parsing buffer...\r\n");
	run(buffer);
}

int read_line(char *s)
{
	int c, i;
	for (i = 0; i < MAXLINE && (c = getchar()) != EOF && c != '\n'; i++)
		s[i] = c;
	if (c == '\n') {
		s[i] = c;
		i++;
	}
	s[i] = '\0';
	return i;
}

void run_prompt()
{
	int c;
	char line[MAXLINE];
	for(;;) {
		printf(" >");
		int len = read_line(line);
		if (!len)
			return;
		run(line);
	}
}


int main(int argc, char *argv[])
{
	// Tokenize statements(tuple) in a list

	if (argc > 2) {
		printf("Usage: LANGNAME [file.c]");
		return 64;
	} else if (argc == 2) {
		printf("Scanning file %s...\r\n", argv[1]);
		run_file(argv[1]);
	} else if (argc == 1) {
		printf("Running prompt...\r\n");
		run_prompt();
	}
}
