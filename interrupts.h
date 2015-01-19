#include <stdint.h>

#ifndef _INTERRUPTS_
#define _INTERRUPTS_

#define NUM_INTS	2
#define INT_SHOWSTATE	0
#define INT_EXIT		1

typedef uint64_t (*int_func)(void);

void init_int_vect (void);

#endif
