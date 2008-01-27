/* Simple and Fast MT 2006/1/21 */
#ifndef MT19937BLK_H
#define MT19937BLK_H
#include "string.h"
#include <inttypes.h>

void mt_init(uint32_t seed);
uint32_t mt_gen(void);
void mt_fill(uint32_t array[], int size);
#endif
