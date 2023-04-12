

// from http://www.burtleburtle.net/bob/hash/doobs.html Bernstein's hash
unsigned int bernstein(unsigned char *key, unsigned int len)
{
  unsigned int hash = 0;//level;
  for (int i=0; i<len; ++i) hash = 33*hash + key[i];
  return hash;
}
// from http://www.burtleburtle.net/bob/hash/doobs.html One-at-a-Time Hash

unsigned int one_at_a_time(char *key, unsigned int len)
{
  unsigned int   hash, i;
  for (hash=0, i=0; i<len; ++i)
  {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return (hash & mask);
} 

