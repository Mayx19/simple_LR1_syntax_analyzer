

// from http://www.burtleburtle.net/bob/hash/doobs.html Bernstein's hash
unsigned int bernstein(unsigned char *key, unsigned int len);
// from http://www.burtleburtle.net/bob/hash/doobs.html One-at-a-Time Hash

unsigned int one_at_a_time(char *key, unsigned int len);