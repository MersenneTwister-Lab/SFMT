#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "pmt32-st.h"

int main(int argc, char *argv[]) {
    int cnt;
    int i, j;
    uint32_t ran;
    pmt32_t pmt32;
    FILE *fp;

    if (argc < 3) {
	printf("dumpPMT count filename\n");
	return -1;
    }
    cnt = (int)strtol(argv[1], NULL, 10);
    fp = fopen(argv[2], "w");
    if (fp == NULL) {
	printf("can't open %s for write\n", argv[2]);
	return -1;
    }
    memset(&pmt32, 0, sizeof(pmt32));
    pmt32.gx[0] = 1;
    pmt32.idx = 1000;
    for (i = 0; i < cnt; i++) {
	ran = gen_rand_int32(&pmt32);
	for (j = 0; j < 4; j++) {
	    putc(ran & 0xff, fp);
	    ran = ran >> 8;
	}
    }
    fclose(fp);
    return 0;
}
