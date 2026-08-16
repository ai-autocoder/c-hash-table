#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABLE_SIZE 16

struct User{
	char *name;
	char *surname;
	char *email;
	int age;
};
struct Entry{
	char *key;
	struct User *user;
	struct Entry *next;
};
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

int main(){
	do {
		char email[100];
		printf("Enter the email to look up: ");
		if(scanf("%99s", email) == -1) break; // Exit on EOF
		unsigned hash = hash_function(email, TABLE_SIZE);
		printf("The hash is: %u\n", hash);

		struct User *user = get(email);
		if(user != NULL){
			printf("The user for %s is: %s %s\n", user->email, user->name, user->surname);
		} else {
			printf("%s not found in the hash table.\n", email);
			// Create a new user or handle the case where the user is not found
			printf("Creating a new user for %s.\n", email);

			struct User *new_user = calloc(1, sizeof(struct User));
			if (new_user == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }

			char buf[50];

			printf("Enter name: ");
			if (scanf("%49s", buf) != 1) { free_user(new_user); break; }
			new_user->name = xstrdup(buf);

			printf("Enter surname: ");
			if (scanf("%49s", buf) != 1) { free_user(new_user); break; }
			new_user->surname = xstrdup(buf);

			printf("Enter age: ");
			if (scanf("%d", &new_user->age) != 1) { free_user(new_user); break; }
			new_user->email = xstrdup(email);

			insert(email, new_user);   /* the table owns new_user from here on — do NOT free it */
			printf("Inserted %s with value %u\n", email, hash);
		}
	} while(1);

	free_table();
	return EXIT_SUCCESS;
}