#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashtable.h"

/* the one and only table. defined here, not in the header: a header is #included into
   every .o, and a definition in a header would give the linker several of them. */
struct Entry *table[TABLE_SIZE];

unsigned hash_function(const char *key, int hash_table_size){
    unsigned sum = 0;
    size_t len = strlen(key);
    for(size_t i = 0; i < len; i++){
        sum += (unsigned char)key[i];
    }
    return sum % hash_table_size;
}

/* strdup that can't fail: on OOM it reports and exits, so callers never NULL-check.
   the "x" prefix is the usual C convention for these (cf. GNU's xmalloc). */
char *xstrdup(const char *s){
	char *copy = strdup(s);
	if (copy == NULL) { fprintf(stderr, "Out of memory\n"); exit(EXIT_FAILURE); }
	return copy;
}

/* frees the struct *and* the strings its members point at.
   safe on a partially filled User: calloc zeroed it, and free(NULL) is a no-op. */
void free_user(struct User *user){
	if (user == NULL) return;
	free(user->name);
	free(user->surname);
	free(user->email);
	free(user);
}

/* the table owns every key and every User handed to insert() */
void free_table(void){
	for (int i = 0; i < TABLE_SIZE; i++){
		struct Entry *e = table[i];
		while (e != NULL){
			struct Entry *next = e->next;   /* stash it: reading e->next after free(e) is UB */
			free(e->key);
			free_user(e->user);
			free(e);
			e = next;
		}
		table[i] = NULL;
	}
}

struct User *get(const char *key){
	unsigned hash = hash_function(key, TABLE_SIZE);
	for (struct Entry *e = table[hash]; e != NULL; e = e->next){
		if(strcmp(e->key, key) == 0){
			return e->user;
		}
	}
	return NULL; // Not found
}

void insert(const char *key, struct User *user){
	unsigned hash = hash_function(key, TABLE_SIZE);

	/* key already present? update in place, don't allocate a second copy */
	for (struct Entry *e = table[hash]; e != NULL; e = e->next){
		if (strcmp(e->key, key) == 0) {
			if (e->user != user) free_user(e->user);   /* we own the old one; don't leak it */
			e->user = user;
			return;
		}
	}

	struct Entry *entry = calloc(1, sizeof(struct Entry));
	if (entry == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	entry->key = xstrdup(key);
	entry->user = user;

	entry->next = table[hash];   /* old head becomes our second element */
	table[hash] = entry;         /* we become the new head */
}
