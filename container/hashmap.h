//#include "./hash/hash.h"
#include "tool.h"
#define _get_position(capacity, hash) ((capacity)-1 & hash)
#define MIN_HASH_CAPACITY 32
#define hput(map, k, v) hashmap_put(map, (void *)(long)(k), (void *)(long)(v))
#define hget(map, k, v) hashmap_get(map, (void *)(long)(k), (void *)(long)(v))

struct _pair
{
  void *key;
  void *value;
} typedef _pair;

struct _entry
{
  uint8 not_empty;
  void *key;
  void *value;
  struct _entry *next;
} typedef _entry;

struct _hashmap
{
  _entry *pairs;
  uint32 max_capacity;
  uint32 cur_capacity;
  uint32 element_size;
  uint32 (*hash)(void *a);
  uint32 (*compare)(void *a, void *b);
} typedef _hashmap;

uint32 bernstein(unsigned char *key, uint32 len);

_entry *new_entry(void *key, void *value);

// break hashmap.h:66 if v==302
void hashmap_double(_hashmap *map);

int hashmap_get(_hashmap *map, void *key, void *value);

void hashmap_put(_hashmap *map, void *key, void *value);

_hashmap *new_hashmap(uint32 size);

uint32 str_hash(void *a);