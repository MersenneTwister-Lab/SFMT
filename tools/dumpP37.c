#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "sfmt_p1_st.h"

int main(int argc, char *argv[]) {
    int cnt;
    int i, j;
    uint32_t ran;
    sfmt_t sfmt;
    FILE *fp;

    if (argc < 3) {
	printf("dumpX78 count filename\n");
	return -1;
    }
    cnt = (int)strtol(argv[1], NULL, 10);
    fp = fopen(argv[2], "w");
    if (fp == NULL) {
	printf("can't open %s for write\n", argv[2]);
	return -1;
    }
    memset(&sfmt, 0, sizeof(sfmt));
    sfmt.sfmt[0][0] = 1;
    sfmt.idx = N * 4;
    for (i = 0; i < cnt; i++) {
	ran = gen_rand32(&sfmt);
	for (j = 0; j < 4; j++) {
	    putc(ran & 0xff, fp);
	    ran = ran >> 8;
	}
    }
    fclose(fp);
    return 0;
}
