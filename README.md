# c-hash-table

A hash table implemented from scratch in C. Written to learn manual memory
management and the data structures behind in-memory caches.

## Status

Hash function implemented. Table, insert, and lookup in progress.

## Build

```sh
gcc -Wall -Wextra -g -o hash_function hash_function.c
./hash_function
```

## Notes

The hash function sums character values and takes the modulo of the table
size. Simple and correct, but distribution is poor — anagrams collide and
short keys cluster in low buckets. Planning to move to djb2 once insert and
lookup work.