#ifndef DSFMT_H
#define DSFMT_H

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdio.h>

class DSFMT {
public:
    static void setup_param(uint32_t array[], int *index);
    static void read_random_param(FILE *fp);
    static void print_param(FILE *fp);
    static unsigned int get_rnd_maxdegree(void);
    static unsigned int get_rnd_mexp(void);
    void init_gen_rand(uint64_t seed);
    uint64_t gen_rand104sp(uint64_t arrary[2], int mode);
    void gen_rand104spar(uint64_t array[][2], int size);
    void get_lung(uint64_t lung[2]);
    void add(const DSFMT& src);
    void fill_rnd();
    void d_p();
    DSFMT(uint64_t seed);
    DSFMT(const DSFMT& src);
    ~DSFMT();
    DSFMT& operator=(const DSFMT &src);

private:
    void next_state();
    uint64_t (*status)[2];
    int idx;
};

#endif
