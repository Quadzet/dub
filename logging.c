#include <stdio.h>
#include <stdarg.h>

char *log_level_strs[] = {
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR"
};

enum LOG_LEVEL {
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR
};

enum LOG_LEVEL LOG_LVL = INFO;

void log_message(enum LOG_LEVEL lvl, char *msg, ...)
{
	if (lvl >= LOG_LVL) {
		va_list args;
		va_start(args, msg);

		printf("[%s]: ", log_level_strs[lvl]);
		vprintf(msg, args);
		printf("\n");

		va_end(args);
	}
}
