#ifndef HASHTABLE_H
#define HASHTABLE_H

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

unsigned hash_function(const char *key, int hash_table_size);

struct User *get(const char *key);
void insert(const char *key, struct User *user);
void free_table(void);

/* used by main.c when it builds a User to hand to insert(), so they can't be static */
char *xstrdup(const char *s);
void free_user(struct User *user);

#endif /* HASHTABLE_H */
