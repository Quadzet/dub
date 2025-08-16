#include <string.h>
#include <stdlib.h>
#include "lexer.h"

struct eval
{
	enum val_type type;
	union val value;
};

#define HASH_SIZE 100
int hash(char *key)
{
	int i, hash = 0;
	char c;
	for (i = 0; (c = key[i]) != '\0'; i++)
		hash += c;
	return (hash * 75948) % 100;
}

struct hash_node
{
	struct hash_node *next;
	char *key;
	struct eval *val;
};

int contains(struct hash_node *map[], char *key)
{
	int ix = hash(key);
	struct hash_node *n = map[ix];
	while (n) {
		if (strcmp(key, n->key) == 0)
			return 1;
		n = n->next;
	}
	return 0;
}

struct eval *get(struct hash_node *map[], char *key)
{
	int ix = hash(key);
	struct hash_node *n = map[ix];
	while (n) {
		if (strcmp(key, n->key) == 0)
			return n->val;
		n = n->next;
	}
	return NULL;
}

int add(struct hash_node *map[],
	char             *key,
	struct eval      *val)
{
	int ix = hash(key);

	struct hash_node *n = map[ix];
	while (n) {
		if (strcmp(key, n->key) == 0) {
			n->val = val;
			return 1;
		}
		n = n->next;
	}

	struct hash_node *new_n = malloc(sizeof(struct hash_node));
	if (!new_n)
		return 0;
	new_n->key  = strdup(key);
	new_n->val  = val;
	new_n->next = map[ix];
	map[ix] = new_n;

	return 1;
}

void free_hash_map(struct hash_node *map[])
{
	for (int i = 0; i < HASH_SIZE; i++) {
		struct hash_node *n = map[i];
		while (n) {
			struct hash_node *next = n->next;
			free(n->key);
			free(n->val);
			free(n);
			n = next;
		}
		map[i] = NULL;
	}
}



