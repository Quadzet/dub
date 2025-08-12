#ifndef logging_h
#define logging_h


extern char *log_level_strs[];

enum LOG_LEVEL {
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR
};

extern enum LOG_LEVEL LOG_LVL;

void log_message(enum LOG_LEVEL lvl, char *msg, ...);

#endif
