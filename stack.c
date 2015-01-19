#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "virtual.h"
#include "opcodes.h"
#include "stack.h"

extern t_regs regs;
extern int64_t offset;
extern uint8_t *mem;

int64_t pop (uint8_t size)
{
	int64_t ret;
	switch (size) {
		case 4:
			//ret = (uint64_t) ((uint32_t) mem[regs.f.rsp]);
			ret = fetch_32(regs.f.rsp);
			break;
		default:
			size = 8;
		case 8:
			//ret = (uint64_t) mem[regs.f.rsp];
			ret = fetch_64(regs.f.rsp);
			break;
	}
	if (regs.f.rsp + size < MEMORY)
		regs.f.rsp += size;
	return ret;
}

void push (uint64_t what, uint8_t size) 
{
	if (regs.f.rsp - size < 0)
		return;
	switch (size) {
		case 4:
			regs.f.rsp -= 4;
			//*((uint32_t *) &mem[regs.f.rsp]) = (uint32_t) what;
			write_32(regs.f.rsp, what);
			break;
		default:
		case 8:
			regs.f.rsp -= 8;
			//*((uint64_t *) &mem[regs.f.rsp]) = what;
			write_64(regs.f.rsp, what);
			break;
	}
	return;
}
