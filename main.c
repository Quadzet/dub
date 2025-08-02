#include <stdio.h>
#include <stdlib.h>
#include "lexer.c"

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
