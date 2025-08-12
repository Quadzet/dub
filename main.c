#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include "logging.h"

static void error(int code, char *msg)
{
	log_message(ERROR, "Error %d: %s", code, msg);
}

static void error_no_msg(int code)
{
	error(code, "No message");
}

void run(char *buffer)
{
	struct t_vector *tokens = parse_buffer(buffer);
	struct stmt **stmts = ast(tokens);

	int i = 0;
	if (!stmts[i])
		log_message(ERROR, "stmt %d: null stmt", i + 1);
	while (stmts[i]) {
		if (!stmts[i]->e)
			log_message(ERROR, "stmt %d: null eval", i + 1);
		fflush(stdout);
		struct eval v = evaluate(stmts[i]->e);
		log_message(DEBUG, "Evaluation complete, result: %s", eval_to_str(&v));
		free_expr(stmts[i]->e);
		i++;
	}

	// intermediate repr...
	// compilation...
	// free_eval(v);

	printf("\n");
}

void run_file(char *path)
{
	log_message(DEBUG, "Opening file...");
	int err_code;
	FILE *fp = fopen(path, "r");
	if ((err_code = fseek(fp, 0, SEEK_END)) != 0)
		error(err_code, "fseek end error");
	int file_size = ftell(fp);
	if ((err_code = fseek(fp, 0, SEEK_SET)) != 0)
		error(err_code, "fseek set error");
	log_message(DEBUG, "Allocating buffer...");
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
		log_message(DEBUG, "Scanning file %s...", argv[1]);
		run_file(argv[1]);
	} else if (argc == 1) {
		log_message(DEBUG, "Running prompt...");
		run_prompt();
	}
}
