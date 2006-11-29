#include <stdio.h>

#define SIZE 1000

int count1bit(FILE *fp);
int read_table(FILE *fp, int table[], int count);
int count32bit(FILE *fp);
void make_table(void);
static int CNT[256];

int main(int argc, char *argv[]) {
    FILE *fp;
    int rc;

    if (argc < 2) {
	printf("count1bit filename\n");
	return -1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
	printf("can't open file:%s\n", argv[1]);
	return -1;
    }
    make_table();
    rc = count1bit(fp);
    return rc;
}

void make_table(void) {
    int i, j;
    int bit;
    int mask;

    for (i = 0; i < 256; i++) {
	mask = 1;
	bit = 0;
	for (j = 0; j < 8; j++) {
	    if (i & mask) {
		bit++;
	    }
	    mask = mask << 1;
	}
	CNT[i] = bit;
    }
}

int count1bit(FILE *fp) {
    int rc;
    int cnt;
    int sum;
    int i;
    static int table[SIZE];

    rc = read_table(fp, table, SIZE);
    if (rc != 0) {
	return rc;
    }
    sum = 0;
    for (i = 0; i < SIZE; i++) {
	sum += table[i];
    }
    //printf("%d, %d\n", 0, sum);
    printf("%d, %.4f\n", 0, (double)sum / (SIZE * 32));
    cnt = 0;
    while (!feof(fp)) {
	rc = count32bit(fp);
	if (rc < 0) {
	    break;
	}
	//printf("DEBUG:sum:%d, pre:%d, rc:%d\n", sum, table[cnt % SIZE], rc);
	sum = sum - table[cnt % SIZE] + rc;
	table[cnt % SIZE] = rc;
	cnt++;
	//printf("%d, %d\n", cnt, sum);
	printf("%d, %.4f\n", cnt, (double)sum / (SIZE * 32));
    }
    return 0;
}

int read_table(FILE *fp, int table[], int size) {
    int i;
    int cnt;

    for (i = 0; i < size; i++) {
	cnt = count32bit(fp);
	if (cnt < 0) {
	    return cnt;
	}
	table[i] = cnt;
    }
    return 0;
}

int count32bit(FILE *fp) {
    int i;
    int r;
    int sum = 0;

    for (i = 0; i < 4; i++) {
	r = getc(fp);
	if (r < 0) {
	    return r;
	}
	sum += CNT[r];
    }
    return sum;
}
