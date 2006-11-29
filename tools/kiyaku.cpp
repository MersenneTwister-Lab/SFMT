/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include "util.h"

NTL_CLIENT;

void readFile(GF2X& poly, FILE *fp);

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    clock_t clo;
    bool b;

    if (argc != 2) {
	printf("usage:%s filename %d\n", argv[0], argc);
	exit(1);
    }
    printf("filename:%s\n", argv[1]);
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    readFile(f, fp);
    fclose(fp);
    printf("deg poly = %ld\n", deg(f));
    clo = clock();
    b =IterIrredTest(f); 
    clo = clock() - clo;
    if(b) {
	printf("kiyaku\n");
    } else {
	printf("not kiyaku\n");
    }
    printf("time: %lfs\n", (double)clo / (double)CLOCKS_PER_SEC);
    return 0;
}

void readFile(GF2X& poly, FILE *fp) {
    char c;
    unsigned int j = 0;

    while ((c = getc(fp)) != EOF) {
	if (c < ' ') {
	    continue;
	} else if (c == '1') {
	    SetCoeff(poly, j, 1);
	    j++;
	} else if (c == '0') {
	    SetCoeff(poly, j, 0);
	    j++;
	} else {
	    break;
	}
    }
}
