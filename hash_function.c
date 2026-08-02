#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABLE_SIZE 16

struct Entry {
	char *key;
	int value;
};
struct Entry *table[TABLE_SIZE];

unsigned hash_function(const char *value, int hash_table_size){
    unsigned sum = 0;
    size_t len = strlen(value);
    for(size_t i = 0; i < len; i++){
        sum += (unsigned char)value[i];
    }
    return sum % hash_table_size;
}

int get(char *key){
	unsigned hash = hash_function(key, TABLE_SIZE);
	struct Entry *entry = table[hash];
	if(entry != NULL && strcmp(entry->key, key) == 0){
		return entry->value;
	}
	return -1; // Not found
}

void insert(char *key, int value){
	unsigned hash = hash_function(key, TABLE_SIZE);
	struct Entry *entry = calloc(1, sizeof(struct Entry));
	if (entry == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	entry->key = strdup(key);
	if (entry->key == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		free(entry);
		exit(EXIT_FAILURE);
	}
	entry->value = value;
	table[hash] = entry;
}

int main(){
	do {
		char name[100];
		printf("Enter the name to look up: ");
		if(scanf("%99s", name) == -1) break; // Exit on EOF
		unsigned hash = hash_function(name, TABLE_SIZE);
		printf("The hash is: %u\n", hash);

		int value = get(name);
		if(value != -1){
			printf("The value for %s is: %d\n", name, value);
		} else {
			printf("%s not found in the hash table.\n", name);
			insert(name, hash);
			printf("Inserted %s with value %u\n", name, hash);
		}
	} while(1);

	return EXIT_SUCCESS;
}