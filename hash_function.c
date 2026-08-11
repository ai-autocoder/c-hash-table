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

struct User *get(char *key){
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
		if (strcmp(e->key, key) == 0) { e->user = user; return; }
	}

	struct Entry *entry = calloc(1, sizeof(struct Entry));
	if (entry == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	entry->key = strdup(key);
	if (entry->key == NULL) { free(entry); fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
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
			if (scanf("%49s", buf) != 1) { free(new_user); break; }
			new_user->name = strdup(buf);
			printf("Enter surname: ");
			if (scanf("%49s", buf) != 1) { free(new_user->name); free(new_user); break; }
			new_user->surname = strdup(buf);
			printf("Enter age: ");
			if (scanf("%d", &new_user->age) != 1) { free(new_user->name); free(new_user->surname); free(new_user); break; }
			new_user->email = strdup(email);
			insert(email, new_user);
			printf("Inserted %s with value %u\n", email, hash);
		}
	} while(1);

	return EXIT_SUCCESS;
}