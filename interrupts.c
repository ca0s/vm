#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "interrupts.h"
#include "stack.h"
#include "virtual.h"

extern t_regs regs;
int_func int_vect[NUM_INTS];
extern uint8_t *mem;

uint64_t int_showstate ()
{
	int i;
	uint64_t p;
	const static char *reg_names[] = { "rip", "rsp", "rbp", "rer", "rco", "rdo",
									   "r0", "r1", "r2", "r3", "r4", "r5", "r6",
									   "r7", "r8", "r9", "rp" };
	
	printf ("Registers:\n");
	for (i=0; i<NUM_REG; i++) 
		printf ("%s\t%p\n", reg_names[i], (void *)regs.a[i]);

	printf ("Stack:");
	for(p = regs.f.rsp, i = 0; p <= STACK_BEGIN; p++, i++) {
		if(i % 8 == 0) {
			printf ("\n%p\t", (void *)p);
		}
		printf("\t0x%02X", mem[p]);
	}
	printf("\n");
	return 0;
}

uint64_t int_exit ()
{
	printf ("Exit: status code %p\n", (void *)regs.f.r0);
	if (mem) munmap (mem, MEMORY);
	exit (regs.f.r0);
}

void init_int_vect ()
{
	int_vect[INT_SHOWSTATE]		=	int_showstate;
	int_vect[INT_EXIT]			=	int_exit;
	return;
}
