//#include "./hash/hash.h"
#include "hashmap.h"
uint32 bernstein(unsigned char *key, uint32 len)
{
  uint32 hash = 0; // level;
  for (int i = 0; i < len; ++i)
    hash = 33 * hash + key[i];
  return hash;
}

_entry *new_entry(void *key, void *value)
{
  _entry *pair = _new(_entry);
  pair->key = key;
  pair->value = value;
  pair->next = NULL;
  pair->not_empty = TRUE;
  return pair;
}

// break hashmap.h:66 if v==302
void hashmap_double(_hashmap *map)
{
  uint32 cap = map->max_capacity << 1;
  _init_new_arr(new_entrys, _entry, cap, 0);

  _entry *cur_entrys = map->pairs;
  for (int i = 0; i < map->max_capacity; i++)
  {
    _entry *cur = &cur_entrys[i];
    if (!cur->not_empty) // if empty ,continue
      continue;
    // else copy bucket
    uint32 pos = _get_position(cap, map->hash(cur->key));
    new_entrys[pos].key = cur->key;
    new_entrys[pos].value = cur->value;
    new_entrys[pos].not_empty = TRUE;
    // copy each node
    cur = cur->next;
    while (cur)
    {
      _entry *next = cur->next;

      _entry *pp = new_entrys + _get_position(cap, map->hash(cur->key));

      if (!pp->not_empty) // copy current node to new bucket.
      {
        pp->key = cur->key;
        pp->value = cur->value;
        pp->not_empty = TRUE;
       // printf("(free %d %d)", _get_position(cap, map->hash(cur->key)), pp->value);
        free(cur);
      }
      else // if the bucket is not empty ,then append this pair to it's tail.
      {
        while (pp->next)
          pp = pp->next;

        pp->next = cur;
        cur->next = NULL;
      }
      cur = next;
    }
  }
  free(map->pairs);
  map->max_capacity = cap;
  map->pairs = new_entrys;
}

int hashmap_get(_hashmap *map, void *key, void *value)
{
  uint32 hash = map->hash(key);
  _entry *curr = &(map->pairs[_get_position(map->max_capacity, hash)]);
#ifdef DEBUG
  printf("get [%d] from bucket %d ", key, _get_position(map->max_capacity, hash));
#endif

  if (curr->not_empty)
  {
    do
    {
      if (!map->compare(key, curr->key)){
        //memcpy(value,&curr->value,map->element_size);
        *(uint64 *)value=(uint64)curr->value;
        return 1;
      }
        
      curr = curr->next;
    } while (curr);
  }
#ifdef DEBUG
  printf("NULL");
#endif
  return 0;
}

void hashmap_put(_hashmap *map, void *key, void *value)
{
  uint32 hash = map->hash(key);
  uint32 max = map->max_capacity;
#ifdef DEBUG
  printf("put [%d,%d] into bucket %d ", (uint64)key, (uint64)value, _get_position(max, hash));
#endif
  _entry *curr = &(map->pairs[_get_position(max, hash)]);
  if (!curr->not_empty)
  {
    curr->key = key;
    curr->value = value;
    curr->not_empty = TRUE;
    map->cur_capacity++;
    return;
  }
  do
  {
    if (!map->compare(key, curr->key))
    {
      curr->value = value;
      break;
    }
    if (!curr->next)
    {
      curr->next = new_entry(key, value);
      map->cur_capacity++;
      if (map->cur_capacity > max)
        hashmap_double(map);
      break;
    }
    curr = curr->next;
  } while (curr);
}

uint32 str_hash(void *a)
{
  char *str = (char *)a;
  return bernstein(str, strlen(str));
}

_hashmap *new_hashmap(uint32 size)
{
  uint32 capacity = max(((uint32)1) << (get_highest_bit(size - 1)), MIN_HASH_CAPACITY);
  _hashmap *map = _new(_hashmap);
  _init_new_arr(pairs, _entry, capacity, 0);

  map->max_capacity = capacity;
  map->cur_capacity = 0;
  map->pairs = pairs;

  return map;
}
