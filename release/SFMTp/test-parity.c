#include <stdlib.h>
#include "sfmt.h"

int main(int argc, char* argv[]) {
    int c;
    
    if (argc >= 2) {
	c = strtol(argv[1], NULL, 10);
	init_gen_rand(c);
    }
    return 0;
}
