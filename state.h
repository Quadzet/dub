#ifndef state_h
#define state_h

#include <string.h>
#include <stdlib.h>
#include "lexer.h"

struct eval
{
	enum val_type type;
	union val value;
};

struct hash_node
{
	struct hash_node *next;
	char *key;
	char *val;
};

int hash(char *key);
int contains(struct hash_node *set[], char *key);
int add(struct hash_node *set[], char *key, char *val);
struct eval *get(struct hash_node *map[], char *key);
void free_hash_map(struct hash_node *map[]);

#endif
