#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "well19937a.h"

int main(int argc, char *argv[]) {
    int cnt;
    int i, j;
    uint32_t ran;
    uint32_t well[624];
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
    memset(well, 0, sizeof(well));
    well[180] = 1;
    //well[0] = 1;
    InitWELLRNG19937a(well);
    for (i = 0; i < cnt; i++) {
	ran = (*WELLRNG19937a)();
	for (j = 0; j < 4; j++) {
	    putc(ran & 0xff, fp);
	    ran = ran >> 8;
	}
    }
    fclose(fp);
    return 0;
}
