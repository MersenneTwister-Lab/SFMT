#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "sfmt-st.h"

void check32(void);

void check32(void) {
    int i;
    uint32_t ini[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};
    sfmt_t sfmt;

    printf("%s\n32 bit generated randoms\n", get_idstring());
    printf("init_gen_rand__________\n");
    /* 32 bit generation */
    init_gen_rand(&sfmt, 1234);
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32(&sfmt));
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
    printf("\n");
    printf("init_by_array__________\n");
    init_by_array(&sfmt, ini, 4);
    for (i = 0; i < 1000; i++) {
	printf("%10u ", gen_rand32(&sfmt));
	if (i % 5 == 4) {
	    printf("\n");
	}
    }
}

int main(void) {

    check32();
    return 0;
}
