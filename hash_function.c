#include <stdio.h>
#include <string.h>

int hash_function(const char *value, int hash_table_size){
    int sum = 0;
    size_t len = strlen(value);
    for(size_t i = 0; i < len; i++){
        sum += (unsigned char)value[i];
    }
    return sum % hash_table_size;
}

int main(){

	char name[] = "hello world!";
	int hash_table_size = 10;
	int hash = 0;

	hash = hash_function(name,hash_table_size);
	printf("The hash is: %d\n", hash);

	return 0;
}