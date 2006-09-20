#include <stdio.h>
#include <inttypes.h>

int main(void) {
    unsigned char cf[4];
    unsigned char cd[8];
    float *pf;
    double *pd;
    uint32_t *p32;
    uint64_t *p64;
    int i;

    pf = (float *)cf;
    pd = (double *)cd;
    *pf = -1.0;
    *pd = -1.0;
    p32 = (uint32_t *)cf;
    p64 = (uint64_t *)cd;

    for (i = 0; i < 4; i++) {
	printf("%02x ", cf[i]);
    }
    printf("\n");
    for (i = 0; i < 8; i++) {
	printf("%02x ", cd[i]);
    }
    printf("\n");
    printf("float %f(%08x)\n", *pf, *p32);
    printf("double %f(%016llx)\n", *pd, *p64);

    return 0;
}
