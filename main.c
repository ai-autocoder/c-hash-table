#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

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
