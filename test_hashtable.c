/* Collision test. "a" (97) and "q" (113) both land in bucket 1 of a 16-bucket table,
   so every assertion here is about what happens when two keys share a bucket.
   assert() aborts on failure, so a broken table exits non-zero. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

/* the test needs to see inside the table to count chain links; nothing else does,
   so this stays out of hashtable.h. extern = "the storage lives in another .o". */
extern struct Entry *table[TABLE_SIZE];

#define BUCKET 1u

static unsigned chain_length(unsigned bucket){
	unsigned n = 0;
	for (struct Entry *e = table[bucket]; e != NULL; e = e->next) n++;
	return n;
}

static struct User *make_user(const char *name, const char *surname, const char *email, int age){
	struct User *u = calloc(1, sizeof(struct User));
	if (u == NULL) { fprintf(stderr, "Out of memory\n"); exit(EXIT_FAILURE); }
	u->name = xstrdup(name);
	u->surname = xstrdup(surname);
	u->email = xstrdup(email);
	u->age = age;
	return u;
}

int main(void){
	/* 1. the collision is real, not assumed */
	assert(hash_function("a", TABLE_SIZE) == hash_function("q", TABLE_SIZE));
	assert(hash_function("a", TABLE_SIZE) == BUCKET);
	printf("1 ok  \"a\" and \"q\" both hash to bucket %u\n", BUCKET);

	struct User *ada   = make_user("Ada", "Lovelace", "a", 36);
	struct User *grace = make_user("Grace", "Hopper", "q", 45);
	insert("a", ada);
	insert("q", grace);

	/* 2. chain walk + strcmp: each key gets its own user back, not its neighbour's */
	assert(get("a") == ada);
	assert(get("q") == grace);
	assert(strcmp(get("a")->name, "Ada") == 0);
	assert(strcmp(get("q")->name, "Grace") == 0);
	printf("2 ok  each key returns its own user\n");

	/* 3. the second insert prepended; it did not overwrite the bucket head */
	assert(chain_length(BUCKET) == 2);
	printf("3 ok  bucket %u holds exactly 2 entries\n", BUCKET);

	/* 4. the load-bearing one. "A0" (65+48=113) lands in bucket 1 too, but was never
	   inserted, so a lookup has to walk the whole chain and then give up. without this
	   the suite still passes if get()'s not-found path returns table[hash]->user
	   instead of NULL. */
	assert(hash_function("A0", TABLE_SIZE) == BUCKET);
	assert(get("A0") == NULL);
	printf("4 ok  uninserted colliding key \"A0\" returns NULL\n");

	/* 5. update path: replace in place, don't duplicate the key or leak the old user.
	   the free itself is invisible from in here — valgrind is what proves it. */
	struct User *ada2 = make_user("Ada", "Byron", "a", 37);
	insert("a", ada2);
	assert(get("a") == ada2);
	assert(chain_length(BUCKET) == 2);   /* replaced, not appended */
	assert(get("q") == grace);           /* the neighbour survived the update */
	printf("5 ok  re-insert replaced in place, chain still 2\n");

	free_table();                        /* clean baseline for valgrind */
	assert(chain_length(BUCKET) == 0);

	printf("\nall 5 assertions passed\n");
	return EXIT_SUCCESS;
}
