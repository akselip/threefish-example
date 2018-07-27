#include <inttypes.h>

#define DEBUG	0

#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

uint64_t _rotl(const uint64_t value, int shift);
void _swap(uint64_t *a, uint64_t *b);

void threefish(unsigned char *c, const unsigned char *p, const unsigned char *k, const unsigned char *t);
