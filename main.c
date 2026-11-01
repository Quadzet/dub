#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include "ast.h"
#include "logging.h"
#include <sysexits.h>


static void error(int code, char *msg)
{
	log_message(ERROR, "Error %d: %s", code, msg);
}

/*
static void error_no_msg(int code)
{
	error(code, "No message");
}
*/

int run(char *buffer)
{
	struct t_vector *tokens = parse_buffer(buffer);
	struct stmt **stmts = ast(tokens);
	if (!panic_mode_triggered)
		return interpret(stmts);
	else {
		log_message(INFO, "Encountered syntax errors - skipping evaluation");
		return EX_DATAERR;
	}

	free_t_vector(tokens);
	free_stmts(stmts);

	printf("\n");
	return EX_OK;
}

int run_file(char *path)
{
	log_message(DEBUG, "Opening file...");
	int ex_code;
	FILE *fp = fopen(path, "r");
		return ex_code;
	}
	int file_size = ftell(fp);
	if ((ex_code = fseek(fp, 0, SEEK_SET)) != 0) {
		error(ex_code, "fseek set error");
		return ex_code;
	}
	log_message(DEBUG, "Allocating buffer...");

	char *buffer =  malloc(file_size + 1);
	if (buffer) {
		fread(buffer, 1, file_size, fp);
		*(buffer + file_size) = '\0';
	}
	fclose(fp);

	if (!buffer) {
		error(MALLOC_ERROR, "Error allocating file buffer");
		return EX_SOFTWARE;
	}

	ex_code = run(buffer);

	free(buffer);

	return ex_code;
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
	if (argc > 3) {
		log_message(ERROR, "Usage: LANGNAME [-D] [file.dub]");
		return 64;
	} else if (argc == 3) {
		if (!strcmp("-D", argv[1])) {
			LOG_LVL = DEBUG;
			log_message(DEBUG, "Scanning file %s...", argv[2]);
			return run_file(argv[2]);
		} else {
			log_message(ERROR, "Usage: LANGNAME [-D] [file.dub]");
			return EX_USAGE;
		}
	} else if (argc == 2) {
		log_message(DEBUG, "Scanning file %s...", argv[1]);
		return run_file(argv[1]);
	} else if (argc == 1) {
		log_message(DEBUG, "Running prompt...");
		run_prompt();
	}
}
