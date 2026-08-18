# c-hash-table

A hash table implemented from scratch in C, with separate chaining for collisions.
Written to learn manual memory management and the data structures behind in-memory
caches.

Fixed 16 buckets, `char *` keys, `struct User` values. The table owns every key and
every value it is given: `insert` copies the key, and `free_table` frees the entries,
the keys, the `User` structs, and the strings inside them.

## API

```c
unsigned      hash_function(const char *key, int hash_table_size);
void          insert(const char *key, struct User *user);   /* table takes ownership */
struct User  *get(const char *key);                          /* NULL if absent */
void          free_table(void);
```

`insert` prepends to the bucket on a collision. Inserting a key that is already present
replaces the value in place and frees the old one, so the chain does not grow and the
replaced record does not leak. Allocation failure is handled in one place rather than
checked at every call site: the program prints to `stderr` and exits.

## Build

```sh
# interactive program: look up an email, and create the record if it isn't there
gcc -Wall -Wextra -g -o main hashtable.c main.c && ./main

# test suite
gcc -Wall -Wextra -g -o test_hashtable hashtable.c test_hashtable.c && ./test_hashtable
```

Both compile with no warnings under `-Wall -Wextra`.

## Test

`test_hashtable.c` exercises the collision path. `"a"` (97) and `"q"` (113) both land in
bucket 1 of a 16-bucket table, so the whole suite is about what happens when two keys
share a bucket. It asserts that:

1. `"a"` and `"q"` really do hash to the same bucket — the collision is verified, not assumed
2. each key returns its own record, so the chain walk and the `strcmp` are both doing work
3. the bucket holds exactly 2 entries — the second insert prepended instead of overwriting
4. a third key that also hashes to bucket 1 but was never inserted returns `NULL`
5. re-inserting `"a"` replaces in place: the chain is still 2 long and the neighbour survives

Assertion 4 is the load-bearing one. A `get` that walks the chain correctly but whose
not-found path returns the head of the bucket instead of `NULL` passes assertions 1, 2
and 3; only 4 catches it.

`assert` aborts, so a broken table exits non-zero.

### Memory

Both binaries run clean under Valgrind:

```sh
valgrind --leak-check=full --error-exitcode=1 ./test_hashtable
valgrind --leak-check=full ./main          # short run, then Ctrl-D
```

```
==58856== total heap usage: 17 allocs, 17 frees, 4,286 bytes allocated
==58856== All heap blocks were freed -- no leaks are possible
==58856== ERROR SUMMARY: 0 errors from 0 contexts
```

## Known limitations

Scope decisions, not oversights:

- **Additive hashing.** The hash sums the bytes of the key and takes the modulo of the
  table size. Correct, but the distribution is poor: anagrams collide and short keys
  cluster in the low buckets. djb2 is the upgrade.
- **No load factor and no resize.** The table is 16 buckets, always. Past ~16 keys the
  chains just get longer and lookups tend toward a linear scan.
- **No `delete`.** Entries can be inserted, replaced, and looked up; individual removal
  is not implemented. `free_table` tears down everything at once.
- **One table per process.** The buckets are a single file-scope array rather than a
  `struct HashTable` handle passed to each function, so there is no way to have two.
