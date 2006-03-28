/* Simple and Fast MT 2006/3/27 
 * SFMT抽象基底クラス
 * 結局C++の機能を使うことにした。
 * またやめるかもしれないけど。
 */
#ifndef __SFMT_BASE_H__
#define __SFMT_BASE_H__

#include <stdint.h>
#include <stdio.h>

class SFMT_BASE {
public:
    virtual static void setup_param(unsigned int p1, unsigned int p2,
				    unsigned int p3, unsigned int p4, 
				    unsigned int p5, unsigned int p6,
				    unsigned int p7, unsigned int p8,
				    unsigned int p9) = 0;
    virtual static unsigned int get_rnd_maxdegree(void);
    virtual static unsigned int get_rnd_mexp(void);
    virtual static void print_param(FILE *fp);
    virtual static void print_param2(FILE *fp);
    virtual static void read_random_param(FILE *fp);

    virtual void init_gen_rand(sfmt_t *sfmt, uint32_t seed);
    virtual uint32_t gen_rand32(sfmt_t *sfmt);
    virtual uint64_t gen_rand64(sfmt_t *sfmt);
    virtual uint64_t gen_rand128(sfmt_t *sfmt, uint64_t *hi, uint64_t *low);
    virtual uint32_t gen_rand128sp(sfmt_t *sfmt, uint32_t arrary[4],
				   uint32_t mode);
    virtual void add_rnd(sfmt_t *dist, sfmt_t *src);
};
#endif
