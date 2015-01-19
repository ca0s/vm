#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "opcodes.h"
#include "virtual.h"
#include "stack.h"
#include "interrupts.h"

extern t_regs regs;
extern int64_t offset;
extern uint8_t *mem;
extern int_func int_vect[NUM_INTS];

void init_opcode_table(t_opcode opcodes[])
{
	opcodes[OPCODE_RET].opcode_0 	= opcode_ret;
	opcodes[OPCODE_CALL].opcode_1 	= opcode_call;
	opcodes[OPCODE_INT].opcode_1	= opcode_int;
	opcodes[OPCODE_JMP].opcode_1	= opcode_jmp;
	opcodes[OPCODE_JE].opcode_1		= opcode_je;
	opcodes[OPCODE_Z].opcode_1		= opcode_z;
	opcodes[OPCODE_MV].opcode_2		= opcode_mv;
	opcodes[OPCODE_ADD].opcode_2	= opcode_add;
	opcodes[OPCODE_SUB].opcode_2	= opcode_sub;
	opcodes[OPCODE_MUL].opcode_2	= opcode_mul;
	opcodes[OPCODE_DIV].opcode_2	= opcode_div;
	opcodes[OPCODE_XOR].opcode_2	= opcode_xor;
	opcodes[OPCODE_PUSH].opcode_1	= opcode_push;
	opcodes[OPCODE_POP].opcode_1	= opcode_pop;
	opcodes[OPCODE_NOP].opcode_0	= opcode_nop;
	opcodes[OPCODE_AND].opcode_2	= opcode_and;
	opcodes[OPCODE_CMP].opcode_2	= opcode_cmp;
	return;
}

int64_t opcode_ret (uint8_t flags)
{
	uint64_t ret = pop(8);
	return ret;
}

int64_t opcode_call (uint8_t flags, uint64_t addr)
{
	push (regs.f.rip + offset - CODE_BEGIN, 8);
	return resolv_content (flags, addr, 1);
}

int64_t opcode_int (uint8_t flags, uint64_t which)
{
	return int_vect[which]();
}

int64_t opcode_jmp (uint8_t flags, uint64_t addr)
{
	return resolv_content (flags, addr, 1);
}

int64_t opcode_je (uint8_t flags, uint64_t addr)
{
	if (regs.f.rer == 0)
		return resolv_content (flags, addr, 1);
	else return 0;
}

int64_t opcode_z (uint8_t flags, uint64_t reg)
{
	regs.a[reg] = 0;
	return 0;
}

int64_t opcode_mv (uint8_t flags, uint64_t to, uint64_t from)
{
	if ((flags & MASK_A1M) && (flags & MASK_A1D)) 
		regs.a[to] = resolv_content (flags, from, 2);
	else
		mem[resolv_content (flags, to, 1)] = resolv_content (flags, from, 2);
	return 0;
}

int64_t opcode_add (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] += resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_sub (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] -= resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_mul (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] *= resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_div (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] /= resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_xor (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] ^= resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_push (uint8_t flags, uint64_t what)
{
	uint64_t value = resolv_content (flags, what, 1);
	push (value, ((flags & MASK_A1T)?8:4));
	return 0;
}

int64_t opcode_pop (uint8_t flags, uint64_t reg)
{
	regs.a[reg] = pop (((flags & MASK_A1T)?8:4));
	return 0;
}

int64_t	opcode_nop (uint8_t flags)
{
	return 0;
}

int64_t opcode_and (uint8_t flags, uint64_t reg, uint64_t what)
{
	regs.a[reg] &= resolv_content (flags, what, 2);
	return 0;
}

int64_t opcode_cmp (uint8_t flags, uint64_t w1, uint64_t w2)
{
	if ((flags & MASK_A1M) && (flags & MASK_A1D)) 
		regs.f.rer = regs.a[w1] != resolv_content (flags, w2, 2);
	else
		regs.f.rer = mem[resolv_content (flags, w1, 1)] != resolv_content (flags, w2, 2);
	return 0;
}