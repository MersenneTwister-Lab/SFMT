#ifndef DSFMT_H
#define DSFMT_H

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdio.h>

#if !defined(DSFMT_BIG_ENDIAN)
#  if defined(__BYTE_ORDER) && defined(__BIG_ENDIAN)
#    if __BYTE_ORDER == __BIG_ENDIAN
#      define DSFMT_BIG_ENDIAN 1
#    endif
#  elif defined(_BYTE_ORDER) && defined(_BIG_ENDIAN)
#    if _BYTE_ORDER == _BIG_ENDIAN
#      define DSFMT_BIG_ENDIAN 1
#    endif
#  elif defined(__BYTE_ORDER__) && defined(__BIG_ENDIAN__)
#    if __BYTE_ORDER__ == __BIG_ENDIAN__
#      define DSFMT_BIG_ENDIAN 1
#    endif
#  elif defined(BYTE_ORDER) && defined(BIG_ENDIAN)
#    if BYTE_ORDER == BIG_ENDIAN
#      define DSFMT_BIG_ENDIAN 1
#    endif
#  elif defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) \
    || defined(__BIG_ENDIAN__) || defined(BIG_ENDIAN)
#      define DSFMT_BIG_ENDIAN 1
#  endif
#endif

class DSFMT {
public:
    static void setup_param(uint32_t array[], int *index);
    static void set_pcv(uint64_t pcv[2]);
    static void read_random_param(FILE *fp);
    static void print_param(FILE *fp);
    static unsigned int get_rnd_maxdegree(void);
    static unsigned int get_rnd_mexp(void);

    static int pos1;
    static int pos2;
    static int pos3;
    static int sl1;
    static int sl2;
    static int sl3;
    static int sr1;
    static int sr2;
    static int sr3;
    static uint64_t msk1;
    static uint64_t msk2;
    static uint64_t msk3;
    static uint64_t msk4;
    static uint64_t msk5;
    static uint64_t msk6;
    static uint64_t high_const;
    static uint64_t fix[2];
    static uint64_t pcv[2];

    void init_gen_rand(uint64_t seed, uint64_t high = high_const);
    uint64_t gen_rand104sp(uint64_t arrary[2], int mode);
    void gen_rand104spar(uint64_t array[][2], int size);
    void get_lung(uint64_t lung[2]);
    void add(const DSFMT& src);
    void fill_rnd(uint64_t high = high_const);
    void fill_rnd_all(int p);
    void d_p();
    void mask_status();
    void set_const();
    int period_certification(bool no_fix = false);
    double gen_rand();
    DSFMT();
    DSFMT(uint64_t seed);
    DSFMT(const DSFMT& src);
    ~DSFMT();
    DSFMT& operator=(const DSFMT &src);

private:
    void initial_mask(uint64_t high);
#if defined(DSFMT_BIG_ENDIAN)
    int idxof(int i) { return i ^ 1; };
#else
    int idxof(int i) { return i; };
#endif
    void next_state();
    uint64_t (*status)[2];
    int idx;
};

#endif
