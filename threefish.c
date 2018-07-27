#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "threefish.h"

/*
Threefish-256 tweakable block cipher example.
*/

/* Under key at k and tweak at t, encrypt 32 bytes of plaintext at p and store it at c. */
void threefish(unsigned char *c, const unsigned char *p, const unsigned char *k, const unsigned char *t)
{
	// Number of rounds
	const int rounds_N = 72;
	
	// Counter for rounds
	int d;
	
	// Counter for s
	int s=0;
	
	// Permutations table
	const int permutations[] = {0, 3, 2, 1};
	
	// Mix table
	const int mix_R[8][2] = {
		{14, 16},
		{52, 57},
		{23, 40},
		{5, 37},
		{25, 33},
		{46, 12},
		{58, 22},
		{32, 32}
		};

	// Key words
  	uint64_t k_words[5];
  	memcpy(k_words,k,32);
  	k_words[4] = 0x1BD11BDAA9FC1A22 ^ k_words[0] ^ k_words[1] ^ k_words[2] ^ k_words[3];
	
	// Tweak words
  	uint64_t t_words[3];
  	memcpy(t_words, t, 16);
  	t_words[2] = t_words[0]^t_words[1];
	
	// State words
  	uint64_t v_words[4];
  	memcpy(v_words, p, 32);
		
	debug_print("KEY 00: %" PRIx64 " %" PRIx64 " %" PRIx64 " %" PRIx64 "\n", 
				k_words[0], k_words[1], k_words[2], k_words[3]);
		
	for(d=0; d<rounds_N; d++)
	{
		// Add sub-key
		if(d%4==0) {
			uint64_t k0 = k_words[s%5];
			uint64_t k1 = (k_words[(s+1)%5] + t_words[s%3]);
			uint64_t k2 = (k_words[(s+2)%5] + t_words[(s+1)%3]);
			uint64_t k3 = (k_words[(s+3)%5] + s) ;
			++s;

			v_words[0] = (v_words[0] + k0);
			v_words[1] = (v_words[1] + k1);
			v_words[2] = (v_words[2] + k2);
			v_words[3] = (v_words[3] + k3);
			
			debug_print("KEY %02d: %16" PRIx64 " %16" PRIx64 " %16" PRIx64 " %16" PRIx64 "\n", 
				s, k0, k1, k2, k3);
		}
		
		// Mix
		{
			v_words[0] = (v_words[0]+v_words[1]);
			v_words[1] = _rotl(v_words[1], mix_R[d%8][0]) ^ v_words[0];
			v_words[2] = (v_words[2]+v_words[3]);
			v_words[3] = _rotl(v_words[3], mix_R[d%8][1]) ^ v_words[2];
		}
		
		// Permute
		{
			uint64_t *tmp = malloc(sizeof(v_words));
			memcpy(tmp, v_words, sizeof(v_words));
			
			_swap(&tmp[0], &v_words[permutations[0]]);
			_swap(&tmp[1], &v_words[permutations[1]]);
			_swap(&tmp[2], &v_words[permutations[2]]);
			_swap(&tmp[3], &v_words[permutations[3]]);
			
			free(tmp);
		}
		
	}
	
	debug_print("Output: %" PRIx64 " %" PRIx64 " %" PRIx64 " %" PRIx64 "\n", 
		v_words[0], v_words[1], v_words[2], v_words[3]);
	
	memcpy(c, v_words, 32);
}

uint64_t _rotl(const uint64_t value, int shift)
{
    return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

void _swap(uint64_t *a, uint64_t *b)
{
	uint64_t tmp = *a;
	*a = *b;
	*b = tmp;
}
