#include <iostream>
#include <NTL/vec_GF2.h>
#include "sfmt-cls.h"

NTL_CLIENT;

void print_vec(vec_GF2& vec) {
    int i;
    int len;

    len = vec.length();
    for (i = 0; i < len; i++) {
	cout << vec[i];
    }
    cout << endl;
}

int main(void) {
    SFMT sfmt;
    int i;
    vec_GF2 vec;

    setup_param(3, 25, 9, 22, 1, 20, 24, 11, 14, 6, 29, 14, 3);
    sfmt.reseed(1234);
    for (i = 1; i <= 128; i++) {
	sfmt.gen_rand(vec, i);
	print_vec(vec);
    }
    return 0;
}
