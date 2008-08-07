#include <stdio.h>
#include <stdint.h>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT;

int non_reducible(GF2X& fpoly, int degree);
void berlekampMassey(GF2X& minpoly, unsigned int maxdegree, vec_GF2& vec);
void printBinary(FILE *fp, GF2X& poly);
int32_t gauss_plus(mat_GF2& mat);
void LCM(GF2X& lcm, const GF2X& x, const GF2X& y);
void readFile(GF2X& poly, FILE *fp, bool skip = false);
unsigned int get_uint(char *line, int radix);
uint64_t get_uint64(char *line, int radix);
static inline uint64_t vec_to_uint64(vec_GF2& vec);
static inline void uint64_to_vec(vec_GF2& vec, uint64_t u, int size);
static inline void vec_to_uint128(uint64_t u[2], vec_GF2& vec, int size);
static inline void uint128_to_vec(vec_GF2& vec, uint64_t u[2], int size);

static inline uint64_t vec_to_uint64(vec_GF2& vec) {
    int i;
    uint64_t u = 0;
    
    for (i = 0; i < vec.length(); i++) {
	u <<= 1;
	if (IsOne(vec.get(i))) {
	    u = u | 1;
	}
    }
    return u;
}

static inline void uint64_to_vec(vec_GF2& vec, uint64_t u, int size) {
    int i;

    vec.SetLength(size);
    for (i = size - 1; i >= 0; i--) {
	vec.put(i, u & 1);
	u >>= 1;
    }
}

static inline void vec_to_uint128(uint64_t u[2], vec_GF2& vec, int size) {
    int i;

    u[0] = 0;
    u[1] = 0;
    for (i = 0; (i < vec.length()) && (i < size); i++) {
	u[0] <<= 1;
	if (IsOne(vec.get(i))) {
	    u[0] = u[0] | 1;
	}
    }
    for (; (i < vec.length()) && (i < size * 2); i++) {
	u[1] <<= 1;
	if (IsOne(vec.get(i))) {
	    u[1] = u[1] | 1;
	}
    }
}

static inline void uint128_to_vec(vec_GF2& vec, uint64_t u[2], int size) {
    int i;
    uint64_t w;

    vec.SetLength(size * 2);
    w = u[1];
    for (i = size * 2 - 1; i >= size; i--) {
	vec.put(i, w & 1);
	w >>= 1;
    }
    w = u[0];
    for (; i >= 0; i--) {
	vec.put(i, w & 1);
	w >>= 1;
    }
}
